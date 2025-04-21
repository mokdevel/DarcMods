//Helpers SCR_DC_AIHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various AI actions
*/

sealed class SCR_DC_AIHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn an AIagent 
	This is used for spawning an individual character rather than a group.
	
		AIAgent aiAgent = SCR_DC_AIHelper.SpawnAIAgent("{6058AB54781A0C52}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_AMG.et", position);
		SCR_DC_AIHelper.GroupAddAI(aiAgent);
	
	*/
	static AIAgent SpawnAIAgent(string aiAgentPrefab, vector pos, bool snap = true)
	{
		Resource resource = null;
		AIAgent aiAgent = null;
		
		resource = Resource.Load(aiAgentPrefab);
		
		EntitySpawnParams params = EntitySpawnParams();
		
		//Spawn the resource exactly to pos
		vector transform[4];
		SCR_DC_SpawnHelper.GetTransformFromPosAndRot(transform, pos, 0, snap);
        params.TransformMode = ETransformMode.WORLD;			
        params.Transform = transform;
		IEntity entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);		
		
		SCR_ChimeraCharacter characterEnt = SCR_ChimeraCharacter.Cast(entity);
				
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
	
		AIGroup group = SCR_DC_AIHelper.SpawnGroup("{6058AB54781A0C52}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_AMG.et", position);
	*/
	static SCR_AIGroup SpawnGroup(string groupToSpawn, IEntity spawnLocation)
	{
		SCR_AIGroup group = SpawnGroup(groupToSpawn, spawnLocation.GetOrigin());
		return group;
	}	
	
	static SCR_AIGroup SpawnGroup(string groupToSpawn, vector spawnPosition)	
	{
		Resource resource = null;
		SCR_AIGroup group = null;
		
		if (groupToSpawn.Contains("Prefabs/Characters/"))
		{
			//Spawn an individual character
			AIAgent aiAgent = SpawnAIAgent(groupToSpawn, spawnPosition);

			//Add to proper group
			if (aiAgent)
			{
				string faction = GetAIAgentFactionKey(aiAgent);
				group = GroupCreate(faction, aiAgent.GetOrigin());
				if(group)
				{
					group.SetNewLeader(aiAgent);
					group.AddAgent(aiAgent);
				}
					
				SCR_DC_Log.Add("[SCR_DC_AIHelper:SpawnGroup] Spawned single unit (" + groupToSpawn + ") to " + faction + " faction.", LogLevel.DEBUG);				
			}
		}
		else
		{
			resource = Resource.Load(groupToSpawn);
			if (resource.IsValid())
			{
				EntitySpawnParams params = EntitySpawnParams();
				params.TransformMode = ETransformMode.WORLD;
				params.Transform[3] = spawnPosition;
				
				group = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(resource, null, params));
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
	static SCR_AIGroup SpawnAIInBuilding(IEntity building, string resourceName, EAISkill skill = EAISkill.REGULAR, float perceptionFactor = 1.0)
	{
		array<vector> floors = {};
		
		SCR_DC_Misc.FindBuildingFloors(floors, building);

		//Find the building size. The bigger X or Y value will be used as the radius
		vector sums = SCR_DC_SpawnHelper.FindEntitySize(building);
		//Pick the radius to be the bigger one from X/Y
		float radius = sums[0];
		if (sums[0] < sums[2])
		{
			radius = sums[2];
		}
	
		vector pos, floorpos;
	
		float empty_radius = 0.5;
	
		floorpos = floors.GetRandomElement();
		pos = SCR_DC_Misc.RandomizePos(floorpos, radius/6);
		pos = SCR_DC_SpawnHelper.FindEmptyPos(pos, radius/5, empty_radius);
		pos[1] = pos[1] + 0.2;			
//		SCR_DC_DebugHelper.AddDebugSphere(pos, Color.YELLOW, empty_radius);
		AIAgent aiAgent = SCR_DC_AIHelper.SpawnAIAgent(resourceName, pos, false);
		
		SetAISkill(aiAgent, skill, perceptionFactor);
		
		SCR_AIGroup group = SCR_DC_AIHelper.GroupAddAI(aiAgent);
	
		array<AIWaypoint> waypoints = {};
		AIWaypointCycle wpcycle = null;
		wpcycle = AIWaypointCycle.Cast(SCR_DC_WPHelper.CreateWaypointEntity(DC_EWaypointMoveType.PATROLCYCLE));
		wpcycle.SetOrigin(pos);
	
		AIWaypoint waypoint = SCR_DC_WPHelper.CreateWaypointEntity(DC_EWaypointMoveType.PATROL);
		if(waypoint)
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
	Set skill and perception for an AI
	
	See SCR_AICombatComponent for details
	*/
	static void SetAISkill(AIAgent aiAgent, EAISkill skill = EAISkill.REGULAR, float perceptionFactor = 1.0)
	{
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
	
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Set skill and perception for all AIs in the group
	
	See SCR_AICombatComponent for details
	*/
	static void SetAIGroupSkill(SCR_AIGroup group, EAISkill skill = EAISkill.REGULAR, float perceptionFactor = 1.0)
	{
		array<AIAgent> groupMembers  = new array<AIAgent>;
		
		if(group)
		{
			group.GetAgents(groupMembers);
			
			foreach(AIAgent groupMember : groupMembers)
			{
				SetAISkill(groupMember, skill, perceptionFactor);
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
			
			foreach(AIAgent agent : agents)
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
	static SCR_AIGroup GroupCreate(string faction, vector pos = "0 0 0")
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
				
			group = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(resource, null, params));		
		}	
		else
		{
			SCR_DC_Log.Add("[SCR_DC_AIHelper:GroupCreate] Unable to load group resource.", LogLevel.ERROR);
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
		
		if(group)
		{
			group.GetAgents(groupMembers);
			
			foreach(AIAgent groupMember : groupMembers)
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
		if(!group)
		{
			string faction = GetAIAgentFactionKey(aiAgent);
			group = GroupCreate(faction, aiAgent.GetOrigin());
		}
		
		group.AddAgent(aiAgent);
		
		return group;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find AIagent faction
	Returns the faction ID for an AIAgent. Return empty in case it's not found.
	*/
	static string GetAIAgentFactionKey(AIAgent aiAgent)
	{
		string faction = "";
		
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(aiAgent.GetControlledEntity());
		if (character)
		{
			faction = character.GetFaction().GetFactionKey();
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
		
		return faction;
	}		
			
	//------------------------------------------------------------------------------------------------
	/*!
	Delete an AI
	*/
	static void RemoveAIAgent(AIAgent ai)
	{
		IEntity aiEntity = ai.GetControlledEntity();
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(aiEntity.FindComponent(DamageManagerComponent));
				
		if (damageManager)
			damageManager.SetHealthScaled(0);
		
		SCR_DC_SpawnHelper.DespawnItem(aiEntity);
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
				SCR_DC_Log.Add("[SCR_DC_AIHelper:AreAllGroupsDead]: " + group.GetID() + " - " + group.GetOrigin(), LogLevel.SPAM);
				i++;
			}
			else
			{
				SCR_DC_Log.Add("[SCR_DC_AIHelper:AreAllGroupsDead] Group does not exist. Removing from table.", LogLevel.SPAM);
				groups.Remove(i);
			}
		}	

		if (groups.IsEmpty())
		{
			SCR_DC_Log.Add("[SCR_DC_AIHelper:AreAllGroupsDead] All groups killed. Mission has ended.", LogLevel.SPAM);
			return true;
		}
		
		return false;				
	}			
}