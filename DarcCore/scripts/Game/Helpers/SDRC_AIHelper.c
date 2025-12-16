//Helpers SDRC_AIHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various AI actions
*/

sealed class SDRC_AIHelper
{			
	static private ref array<SCR_FactionManager> m_aFactionManagers = {};
		
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn an AIagent 
	This is used for spawning an individual character rather than a group.
	
		AIAgent aiAgent = SDRC_AIHelper.SpawnAIAgent("{6058AB54781A0C52}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_AMG.et", position);
		SDRC_AIHelper.GroupAddAI(aiAgent);
	
	*/
	static AIAgent SpawnAIAgent(ResourceName resourceName, vector pos, string faction, bool snap = true)
	{
		Resource resource = null;
		AIAgent aiAgent = null;
		
		ResourceName name = SDRC_EnemyHelper.SelectEnemy(resourceName, faction);
		resource = Resource.Load(name);
		
		if (!resource)
		{
			return null;
		}
		
		EntitySpawnParams params = EntitySpawnParams();
		
		//Spawn the resource exactly to pos
		vector transform[4];
		SDRC_Math.GetTransformFromPosAndRot(transform, pos, 0, snap);
        params.TransformMode = ETransformMode.WORLD;			
        params.Transform = transform;
		IEntity entity = SDRC_SpawnHelper.SpawnEntityPrefabPersistence(resource, GetGame().GetWorld(), params);		
		
		SCR_ChimeraCharacter characterEnt = SCR_ChimeraCharacter.Cast(entity);
		
		if (!characterEnt)
		{
			return null;
		}
				
		CharacterControllerComponent characterController = characterEnt.GetCharacterController();			
		AIControlComponent aiControlComponent = characterController.GetAIControlComponent();
		aiControlComponent.ActivateAI();
			
		aiAgent = aiControlComponent.GetControlAIAgent();			
		
		return aiAgent;
	}			

	//------------------------------------------------------------------------------------------------
	/*!
	Spawn a group of AIs from a prefab. 
	This also supports character spawning.
	
		AIGroup group = SDRC_AIHelper.SpawnGroup("{6058AB54781A0C52}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_AMG.et", position);
	*/
	static SCR_AIGroup SpawnGroup(string groupToSpawn, IEntity spawnLocation, string faction)
	{
		SCR_AIGroup group = SpawnGroup(groupToSpawn, spawnLocation.GetOrigin(), faction);
		return group;
	}	
	
	static SCR_AIGroup SpawnGroup(string groupToSpawn, vector spawnPosition, string faction)	
	{
		if (groupToSpawn == "")
		{
			return null;
		}		
		
		Resource resource = null;
		SCR_AIGroup group = null;
		
		ResourceName name = SDRC_EnemyHelper.SelectEnemy(groupToSpawn, faction);
		
//		if ( (name.Contains("Prefabs/Characters/")) || (name.Contains("}Beasts/")) || (name.Contains("Prefabs/")))	//TBD: Fix these once halloween monsters are fixed.
		if ( (name.Contains("Prefabs/Characters/")) )
		{
			//Spawn an individual character
			AIAgent aiAgent = SpawnAIAgent(name, spawnPosition, faction, true);

			//Add to proper group
			if (aiAgent)
			{
				faction = GetAIAgentFactionKey(aiAgent);
				group = GroupCreate(faction, aiAgent.GetOrigin());
				if (group)
				{
					group.SetNewLeader(aiAgent);
					group.AddAgent(aiAgent);
				}
					
				SDRC_Log.Add("[SDRC_AIHelper:SpawnGroup] Spawned single unit (" + name + ") to " + faction + " faction.", LogLevel.DEBUG);
			}
		}
		else
		{
			resource = Resource.Load(name);
			if (resource.IsValid())
			{
				EntitySpawnParams params = EntitySpawnParams();
				params.TransformMode = ETransformMode.WORLD;
				params.Transform[3] = spawnPosition;
				
				group = SCR_AIGroup.Cast(SDRC_SpawnHelper.SpawnEntityPrefabPersistence(resource, null, params));
			}
		}	
		
		return group;		
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Spawn AI characters inside a building.
	
	We try to keep the AI inside the building by setting a cycle waypoint to spawn point. They will go out and fight,
	but go back inside (or close to that).
	
	The spawn point is chosen randomly on the floors. Sometimes the randomess sets them outside.
	
			|------------------------------------------| House size
		               |---------||---------|            Random spot is 1/6 of house size from the center
		          |----*----|                            Radius to search for a spot is 1/5 of house size
	*/
	
	static SCR_AIGroup SpawnAIInBuilding(IEntity building, string resourceName, string faction, EAISkill skill = EAISkill.REGULAR, float perceptionFactor = 1.0)
	{
		array<vector> floors = {};
		vector pos, floorpos;

		SDRC_BuildingHelper.FindBuildingFloors(floors, building);
				
		//Find the building size. The bigger X or Y value will be used as the radius
		vector sums = SDRC_SpawnHelper.FindEntitySize(building);
		//Pick the radius to be the bigger one from X/Y
		float radius = sums[0];
		if (sums[0] < sums[2])
		{
			radius = sums[2];
		}
	
		if (!floors.IsEmpty())
		{
			floorpos = floors.GetRandomElement();
		}
		else
		{
			floorpos = building.GetOrigin();
			ResourceName res = building.GetPrefabData().GetPrefabName();
			SDRC_Log.Add("[SDRC_AIHelper:SpawnAIInBuilding] No floors found from: " + res + " . Spawn will be interesting...", LogLevel.SPAM);
		}
		
		pos = SDRC_Misc.RandomizePos(floorpos, radius/6);
		pos[1] = pos[1] + 0.2;			
//		SDRC_DebugHelper.AddDebugSphere(pos, Color.YELLOW, 0.4);
		AIAgent aiAgent = SDRC_AIHelper.SpawnAIAgent(resourceName, pos, faction, false);
		
		SetAISettings(aiAgent, skill, perceptionFactor);
		
		SCR_AIGroup group = SDRC_AIHelper.GroupAddAI(aiAgent);
	
		//Using a patrol cycle should make the AI return in to the building
		array<AIWaypoint> waypoints = {};
		AIWaypointCycle wpcycle = null;
		wpcycle = AIWaypointCycle.Cast(SDRC_WPHelper.CreateWaypointEntity(SDRC_EWaypointMoveType.PATROLCYCLE));
		wpcycle.SetOrigin(pos);
	
		AIWaypoint waypoint = SDRC_WPHelper.CreateWaypointEntity(SDRC_EWaypointMoveType.PATROL);
		if (waypoint)
		{
			waypoint.SetOrigin(pos);
			waypoints.Insert(waypoint);
		}
		
		wpcycle.SetWaypoints(waypoints);
		group.AddWaypoint(wpcycle);
		
		return group;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Set settings for AI
	- skill and perception
	- persistence
	
	See SCR_AICombatComponent for details
	NOTE: The setting will affect only if the AI has been spawned -> needs to be delayed.
	*/
	static void SetAISettings(AIAgent aiAgent, EAISkill skill = EAISkill.REGULAR, float perceptionFactor = 1.0)
	{
		GetGame().GetCallqueue().CallLater(SetAISettingsDelayed, SDRC_Conf.AI_SETTING_DELAY, false, aiAgent, skill, perceptionFactor);
	}
		
	static void SetAISettingsDelayed(AIAgent aiAgent, EAISkill skill = EAISkill.REGULAR, float perceptionFactor = 1.0)
	{
		if (!aiAgent)
			return;
		
	    IEntity agentEntity = aiAgent.GetControlledEntity();
	
	    if (!agentEntity)
	        return;
	
	    SCR_AICombatComponent combatComponent = SCR_AICombatComponent.Cast(agentEntity.FindComponent(SCR_AICombatComponent));
	    if (combatComponent)
	    {
	        combatComponent.SetAISkill(skill);
	        combatComponent.SetPerceptionFactor(perceptionFactor);
//	        combatComponent.SetCombatType(combatType);
//	        combatComponent.SetHoldFire(holdFire);
	    }
		
		SDRC_SpawnHelper.SetPersistence(aiAgent, false);		
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Set settings for AI
	- skill and perception
	- persistence
		
	See SCR_AICombatComponent for details
	NOTE: The setting will affect only if the AI has been spawned -> needs to be delayed.	
	*/
	static void SetAIGroupSettings(SCR_AIGroup group, EAISkill skill = EAISkill.REGULAR, float perceptionFactor = 1.0)
	{
		GetGame().GetCallqueue().CallLater(SetAIGroupSettingsDelayed, SDRC_Conf.AI_SETTING_DELAY, false, group, skill, perceptionFactor);
	}
		
	static void SetAIGroupSettingsDelayed(SCR_AIGroup group, EAISkill skill = EAISkill.REGULAR, float perceptionFactor = 1.0)
	{
		array<AIAgent> groupMembers  = new array<AIAgent>;
		
		if (group)
		{
			group.GetAgents(groupMembers);
			
			foreach (AIAgent groupMember : groupMembers)
			{
				SetAISettingsDelayed(groupMember, skill, perceptionFactor);
			}
		}
	}
			
	//------------------------------------------------------------------------------------------------
	/*!
	Set movement speed for a group
	
	NOTE: The setting will affect only if the AI has been spawned -> needs to be delayed.
	*/
	static void SetAIGroupMovementType(SCR_AIGroup group, EMovementType movementType)
	{
		GetGame().GetCallqueue().CallLater(SetAIGroupMovementTypeDelayed, SDRC_Conf.AI_SETTING_DELAY, false, group, movementType);
	}
	
	static void SetAIGroupMovementTypeDelayed(SCR_AIGroup group, EMovementType movementType)
	{
		array<AIAgent> groupMembers  = new array<AIAgent>;
		
		if (group)
		{
			group.GetAgents(groupMembers);
			
			foreach (AIAgent groupMember : groupMembers)
			{
				if (movementType == EMovementType.IDLE)
				{
					SDRC_WPHelper.RemoveWaypoints(group);			
				}
					
				AICharacterMovementComponent m_MovementComponent = AICharacterMovementComponent.Cast(groupMember.GetControlledEntity().FindComponent(AICharacterMovementComponent));
				if (m_MovementComponent)
				{
					m_MovementComponent.SetMovementTypeWanted(movementType);
				}
			}
		}
	}	
		
	//------------------------------------------------------------------------------------------------
	/*!
	Enable/Disable AIs in the group
	
	NOTE: The setting will affect only if the AI has been spawned -> needs to be delayed.
	*/
	static void SetAIGroupEnable(SCR_AIGroup group, bool enable = true)
	{
		GetGame().GetCallqueue().CallLater(SetAIGroupEnableDelayed, SDRC_Conf.AI_SETTING_DELAY, false, group, enable);
	}
	
	static void SetAIGroupEnableDelayed(SCR_AIGroup group, bool enable)
	{
		array<AIAgent> groupMembers  = new array<AIAgent>;
		
		if (group)
		{
			group.GetAgents(groupMembers);
			
			foreach (AIAgent groupMember : groupMembers)
			{
				if (enable)
				{
					groupMember.ActivateAI();
				}
				else
				{
					groupMember.DeactivateAI();
				}
			}
		}
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find all groups
	*/
	static void GroupFindAll(out array<AIGroup> groups)
	{
		array<AIAgent> agents = {};
		
		SCR_AIWorld tmpAIWorld = SCR_AIWorld.Cast(GetGame().GetAIWorld());		
		if (tmpAIWorld)
		{
			tmpAIWorld.GetAIAgents(agents);
			
			groups.Clear();
			
			foreach (AIAgent agent : agents)
			{
				AIGroup group;
				group = agent.GetParentGroup();
				if (group != null)
				{
					groups.Insert(group);
				}
			}
			//Print(string.Format("[FindAllGroups] : %1", groups.Count()), LogLevel.NORMAL);				
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Create a group
	*/
	static SCR_AIGroup GroupCreate(string faction, vector pos)
	{
		string aiBaseGroup = "{F22EDFBEFC193357}Prefabs/Groups/Campaign/Group_FIA_Remnants.et";
		Resource resource = null;
		SCR_AIGroup group = null;
		
		switch (faction)
		{
			case "USSR":
				aiBaseGroup = "{9E45E4F7AE1CB45A}Prefabs/Groups/Campaign/Group_USSR.et";
				break;
			case "US":
				aiBaseGroup = "{412A7767B11310C5}Prefabs/Groups/Campaign/Group_US.et";
				break;
			case "CIV":
				aiBaseGroup = "{F22EDFBEFC193357}Prefabs/Groups/Campaign/Group_FIA_Remnants.et";
				break;
			default:
				//FIA
		}
	
		resource = Resource.Load(aiBaseGroup);
		if (resource.IsValid())
		{
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = pos;
				
			group = SCR_AIGroup.Cast(SDRC_SpawnHelper.SpawnEntityPrefabPersistence(resource, null, params));		
			group.SetFaction(SDRC_EnemyHelper.GetFactionWithName(faction));
		}	
		else
		{
			SDRC_Log.Add("[SDRC_AIHelper:GroupCreate] Unable to load group resource.", LogLevel.ERROR);
			return null;
		}
		
		return group;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Create a group and
	*/
	static SCR_AIGroup GroupCreate(string faction, vector pos, string aiBaseGroup)
	{
		Resource resource = null;
		SCR_AIGroup group = null;
	
		resource = Resource.Load(aiBaseGroup);
		if (resource.IsValid())
		{
			EntitySpawnParams params = EntitySpawnParams();
			params.TransformMode = ETransformMode.WORLD;
			params.Transform[3] = pos;
				
			group = SCR_AIGroup.Cast(SDRC_SpawnHelper.SpawnEntityPrefabPersistence(resource, null, params));		
			group.SetFaction(SDRC_EnemyHelper.GetFactionWithName(faction));
		}	
		else
		{
			SDRC_Log.Add("[SDRC_AIHelper:GroupCreate] Unable to load group resource.", LogLevel.ERROR);
			return null;
		}
		
		return group;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Delete a group
	*/
	static void GroupDelete(AIGroup group)
	{
		array<AIAgent> groupMembers  = new array<AIAgent>;
		
		if (group)
		{
			group.GetAgents(groupMembers);
			
			foreach (AIAgent groupMember : groupMembers)
			{
				RemoveAIAgent(groupMember);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Add an AI to a group. If the group does not exist, create a new one.
	*/
	static SCR_AIGroup GroupAddAI(AIAgent aiAgent, SCR_AIGroup group = null)
	{
		if (!group)
		{
			string faction = GetAIAgentFactionKey(aiAgent);
			group = GroupCreate(faction, aiAgent.GetOrigin());
		}
		
		group.AddAgent(aiAgent);
		
		return group;
	}	
	

	//------------------------------------------------------------------------------------------------
	/*!
	Get group IEntitySource
	*/
	static IEntitySource GroupGetEntitySource(ResourceName groupName)
	{
		Resource resource = Resource.Load(groupName);    
		IEntitySource entitySource = null;
		entitySource = SCR_BaseContainerTools.FindEntitySource(resource);
		
		return entitySource;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get group IEntitySource prefabs members
	*/
	static int GroupGetEntitySourceMembers(ResourceName groupName, out array<ResourceName> groupPrefabs)
	{
		IEntitySource entitySource = GroupGetEntitySource(groupName);
		
		int count = 0;
		
		if (entitySource)
		{
			vector pos = "0 500 0";
			SCR_AIGroup group = SDRC_AIHelper.GroupCreate("US", pos, groupName);
			
		    array<vector> groupOffsets = {}; //not needed
		    SCR_AIGroupClass.GetMembers(entitySource, groupPrefabs, groupOffsets);
		    count = groupPrefabs.Count();
			
			GetGame().GetCallqueue().CallLater(GroupDelete, SDRC_Conf.DESPAWN_ENTITY_USED_FOR_SIZE_DELAY, false, group);			
		}		
		
		return count;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find AIagent faction
	Returns the faction ID for an AIAgent.
	*/
	static FactionKey GetAIAgentFactionKey(AIAgent aiAgent)
	{
		FactionKey factionKey = "";

		//SDRC_Log.Add("[SDRC_AIHelper:GetAIAgentFactionKey] Checking: " + aiAgent, LogLevel.DEBUG);

		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(aiAgent.GetControlledEntity());
		if (character)
		{
			Faction faction = character.GetFaction();
			if (faction)
			{
				factionKey = character.GetFaction().GetFactionKey();
			}
			else
			{
				//Select the enemy faction from a list
				factionKey = SDRC_EnemyHelper.SelectEnemyFaction();
				
				IEntity ent = aiAgent.GetControlledEntity();
				ResourceName res = ent.GetPrefabData().GetPrefabName();
				SDRC_Log.Add("[SDRC_AIHelper:GetAIAgentFactionKey] Faction missing from game? FactionKey not found for : " + res, LogLevel.ERROR);
			}
		}

		/* 
		//Code that also works. Not sure what is the difference but leaving here for future				
		SCR_CallsignCharacterComponent callsignComp = SCR_CallsignCharacterComponent.Cast(aiAgent.GetControlledEntity().FindComponent(SCR_CallsignCharacterComponent));
		FactionAffiliationComponent factionComp = FactionAffiliationComponent.Cast(aiAgent.GetControlledEntity().FindComponent(FactionAffiliationComponent));
		
		if (factionComp)
		{
			faction = factionComp.GetAffiliatedFaction().GetFactionKey();
		}
		*/
		
		return factionKey;
	}		

	//------------------------------------------------------------------------------------------------
	/*!
	Delete an AI
	*/
	static void RemoveAIAgent(AIAgent ai)
	{
		if (ai)
		{		
			IEntity aiEntity = ai.GetControlledEntity();
			if (aiEntity)
			{
/*				#ifndef NEW_VERSION_WIP	
				DamageManagerComponent damageManager = DamageManagerComponent.Cast(aiEntity.FindComponent(DamageManagerComponent));
				
					//TBD: Do we need to set the damage before despawn?
					if (damageManager)
						damageManager.SetHealthScaled(0);
				#endif*/
				
				SDRC_SpawnHelper.DespawnItem(aiEntity);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Are all groups dead?
	Different scenarios:
	- The size of the groups table is 0
	- The group has been deleted by GM -> the table entry is null	
	*/
	static bool AreAllGroupsDead(array<SCR_AIGroup> groups)
	{
		int i = 0;				
		SCR_AIGroup group;
		
		//Clean up the group list
		while (i < groups.Count())
		{
			group = groups[i];
			if (group)
			{
				SDRC_Log.Add("[SDRC_AIHelper:AreAllGroupsDead]: " + group.GetID() + " - " + group.GetOrigin(), LogLevel.SPAM);
				i++;
			}
			else
			{
				SDRC_Log.Add("[SDRC_AIHelper:AreAllGroupsDead] Group does not exist. Removing from table.", LogLevel.SPAM);
				groups.Remove(i);
			}
		}	

		if (groups.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_AIHelper:AreAllGroupsDead] All groups killed. Mission has ended.", LogLevel.SPAM);
			return true;
		}
		
		return false;				
	}			
	
	//------------------------------------------------------------------------------------------------
	/*!
	Are all AIs in a group dead?
	*/
	static bool IsGroupDead(SCR_AIGroup group)
	{
		//Seems that if all members are dead, the group becomes null.
		if (group)
		{
			return false;
		}
		return true;
	}
}