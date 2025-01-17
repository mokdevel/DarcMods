//Helpers SCR_DC_AIHelper.c

enum DC_EWaypointRndType
{
	RANDOM,			//Use one from the list below randomly
	SCATTERED,		//Completely random waypoints without any logic
	RADIUS,			//AI follow a path that is close to a circle with a radius. There is some additional randomization to avoid a perfect circle.
	SLOTS 			//AI goes from a slot to slot. NOTE: This will not work unless the map has slots (the S/M/L letters on map) defined.
};

enum DC_EWaypointMoveType
{
	RANDOM,			//Pick a random one. Mission should define how to handle randomness. Often a reasonable RANDOM is a selection between MOVECYCLE and PATROLCYCLE.
	MOVE,			//Creates move waypoints. AI will stop once waypoints are visited.
	PATROL,			//Same as MOVE but with patrol speed.
	MOVECYCLE,		//Creates move waypoints in cycke. AI will restart the cycle once all waypoints are visited.
	PATROLCYCLE		//Same as MOVECYCLE but with patrol speed.
};

sealed class SCR_DC_AIHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn a group
	*/
	static SCR_AIGroup SpawnGroup(string groupToSpawn, IEntity spawnLocation)
	{
//		SCR_AIGroup group = SpawnGroup(groupToSpawn, IEntity.Cast(spawnLocation).GetOrigin());
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
				string aiBaseGroup = "{9E45E4F7AE1CB45A}Prefabs/Groups/Campaign/Group_USSR.et";
				
				switch (faction)
				{
					case "USSR":
						aiBaseGroup = "{9E45E4F7AE1CB45A}Prefabs/Groups/Campaign/Group_USSR.et";
						break;
					case "US":
						aiBaseGroup = "{412A7767B11310C5}Prefabs/Groups/Campaign/Group_US.et";
					default:
						//USSR
				}
			
				resource = Resource.Load(aiBaseGroup);
				if (resource.IsValid())
				{
					EntitySpawnParams params = EntitySpawnParams();
					params.TransformMode = ETransformMode.WORLD;
					params.Transform[3] = spawnPosition;
						
					group = SCR_AIGroup.Cast(GetGame().SpawnEntityPrefab(resource, null, params));
					
					group.SetNewLeader(aiAgent);
					group.AddAgent(aiAgent);
					
					SCR_DC_Log.Add("[SCR_DC_AIHelper:SpawnGroup] Spawned single unit (" + groupToSpawn + ") to " + faction + " faction.", LogLevel.DEBUG);				
				}			
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
		
		if (!resource.IsValid())
		{
			SCR_DC_Log.Add(("[SCR_DC_AIHelper:SpawnGroup] Unable to load resource for group to spawn: " + groupToSpawn), LogLevel.ERROR);
			return null;
		}
		
		return group;		
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
	Delete a group
	*/
	static void GroupDelete(AIGroup group)
	{
		array<AIAgent> groupMembers  = new array<AIAgent>;
		group.GetAgents(groupMembers);
		
		foreach(AIAgent groupMember : groupMembers)
		{
			RemoveAIAgent(groupMember);
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Spawn an AIagent 
	This is used for spawning an individual character rather than a group.
	*/
	static AIAgent SpawnAIAgent(string aiAgentPrefab, vector pos)
	{
		Resource resource = null;
		AIAgent aiAgent = null;
		
		resource = Resource.Load(aiAgentPrefab);
		
		EntitySpawnParams params = EntitySpawnParams();
		
		//Spawn the resource exactly to pos
		vector transform[4];
		SCR_DC_SpawnHelper.GetTransformFromPosAndRot(transform, pos, 0);
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
	Find AIagent faction
	Returns the faction ID for an AIAgent. Return empty in case it's not found.
	*/
	static string GetAIAgentFactionKey(AIAgent aiAgent)
	{
		string faction = "";
		
		SCR_CallsignCharacterComponent callsignComp = SCR_CallsignCharacterComponent.Cast(aiAgent.GetControlledEntity().FindComponent(SCR_CallsignCharacterComponent));
		FactionAffiliationComponent factionComp = FactionAffiliationComponent.Cast(aiAgent.GetControlledEntity().FindComponent(FactionAffiliationComponent));
		
		if (factionComp)
		{
			faction = factionComp.GetAffiliatedFaction().GetFactionKey();
		}
		
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

		if (groups.Count() == 0)
		{
			SCR_DC_Log.Add("[SCR_DC_AIHelper:AreAllGroupsDead] All groups killed. Mission has ended.", LogLevel.SPAM);
			return true;
		}
		
		return false;				
	}		
		
	//------------------------------------------------------------------------------------------------
	/*!
	Creates a list of waypoints for a group
	NOTE: Currently only supports creation of cycle waypoints. 
	TBD: Non-cycle waypoint list creation for a group
	\param group
	\param count Amount of waypoints to create
	\param rndtype Type of waypoint creation
	\param range Range (radius) for randomization from given position.
	\param emptyspot True if found position needs to be on clear area.	
	*/
	static bool CreateWaypoints(SCR_AIGroup group, int count, DC_EWaypointMoveType wptype = DC_EWaypointMoveType.MOVECYCLE, DC_EWaypointRndType rndtype = DC_EWaypointRndType.SCATTERED, float range = 100, bool emptyspot = false)
	{
		array<AIWaypoint> waypoints = {};
		AIWaypointCycle wpcycle = null;
		
		if (wptype == DC_EWaypointMoveType.MOVECYCLE || wptype == DC_EWaypointMoveType.PATROLCYCLE)
		{
			wpcycle = AIWaypointCycle.Cast(CreateWaypointEntity(wptype));
			
			//Cycle created, change type to something normal
			switch (wptype)
			{
				case DC_EWaypointMoveType.MOVECYCLE:
					wptype = DC_EWaypointMoveType.MOVE;
					break;
				case DC_EWaypointMoveType.PATROLCYCLE:
					wptype = DC_EWaypointMoveType.PATROL;
					break;
				default:
					SCR_DC_Log.Add("[SCR_DC_AIHelper:CreateWaypoints] Incorrect wptype", LogLevel.ERROR);
			}
			SCR_DC_Log.Add("[SCR_DC_AIHelper:CreateWaypoints] Creating cycle", LogLevel.DEBUG);
		}
		
		RemoveWaypoints(group);		
		waypoints = GetPatrolWaypoints(group.GetOrigin(), count, wptype, rndtype, range, emptyspot);
		
		if (waypoints.Count() == 0)
		{
			SCR_DC_Log.Add("[SCR_DC_AIHelper:CreateWaypoints] Could not create waypoint to group: " + group, LogLevel.WARNING);
			return false;
		}
		
		//Create cycle waypoints
		if (wpcycle != null)
		{				
			if (waypoints.Count() > 0)
			{
				wpcycle.SetWaypoints(waypoints);
				group.AddWaypoint(wpcycle);
				
				SCR_DC_Log.Add("[SCR_DC_AIHelper:CreateWaypoints] Adding waypoints: " + waypoints.Count(), LogLevel.DEBUG);
				return true;
			}
			else
			{
				SCR_DC_Log.Add("[SCR_DC_AIHelper:CreateWaypoints] No waypoints added to group: " + group, LogLevel.WARNING);
				return false;
			}		
		}
		//Create normal waypoints
		else
		{
			foreach (AIWaypoint wpc : waypoints)
			{
				group.AddWaypoint(wpc);
			}
			
			SCR_DC_Log.Add("[SCR_DC_AIHelper:CreateWaypoints] Adding waypoints: " + waypoints.Count(), LogLevel.DEBUG);
			return true;			
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	static void RemoveWaypoints(AIGroup group)
	{
		array<AIWaypoint> waypoints = {};
		group.GetWaypoints(waypoints);
		
		foreach(AIWaypoint waypoint : waypoints)
		{
			group.RemoveWaypoint(waypoint);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Creates an array waypoints in a given pattern
	\param position
	\param count How many waypoints to create
	\param wptype See DC_EWaypointMoveType definition for more information
	\param rndtype See DC_EWaypointRndType definition for more information
	\param range Range (radius) for randomization from given position.
	\param emptyspot True if found position needs to be on clear area.
	*/
	static array<AIWaypoint> GetPatrolWaypoints(vector position, int count, DC_EWaypointMoveType wptype = DC_EWaypointMoveType.PATROL, DC_EWaypointRndType rndtype = DC_EWaypointRndType.SCATTERED, float range = 100, bool emptyspot = false )
	{
		array<AIWaypoint> waypoints = {};
		
		//Random 
		if (rndtype == DC_EWaypointRndType.SCATTERED)
		{		
			for (int i = 0; i < count; i++)
			{
				//Add some additional randomization
				float rndRange = Math.RandomInt(0, rndRange/3); 
				
				AIWaypoint waypoint = GetWaypoint(position, wptype, (range + rndRange), emptyspot);
				if (waypoint != null)
				{
					waypoints.Insert(waypoint);
				}
			}		
		}

		//Circular
		if (rndtype == DC_EWaypointRndType.RADIUS)
		{		
			for (int i = 0; i < count; i++)
			{				
				vector vec = SCR_DC_Misc.GetCoordinatesOnCircle(position, range, i*(360/count));
				
				AIWaypoint waypoint = GetWaypoint(vec, wptype, (range/4), emptyspot);
				if (waypoint != null)
				{
					waypoints.Insert(waypoint);
				}
			}		
		}

		//Slots
		if (rndtype == DC_EWaypointRndType.SLOTS)
		{		
			array<IEntity> slots = {};
			SCR_DC_Locations.GetLocationSlots(slots, position, range);
			
			for (int i = 0; i < count; i++)
			{
				IEntity slot = slots.GetRandomElement();				

				AIWaypoint waypoint = GetWaypoint(slot.GetOrigin(), wptype, 3, emptyspot);
				if (waypoint != null)
				{
					waypoints.Insert(waypoint);
				}
			}		
		}
						
		return waypoints;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Creates a waypoint
	\param position
	\param wptype
	\param range Range (radius) for randomization from given position.
	\param emptyspot True if found position needs to be on clear area.
	*/
	static AIWaypoint GetWaypoint(vector position, DC_EWaypointMoveType wptype, float range = 100, bool emptyspot = false )
	{	
		vector wpPos;
		float emptyRange = 30;
		
		wpPos = SCR_DC_Misc.RandomizePos(position, range);
		
		vector wpPosFixed;

		if (emptyspot)
		{
			SCR_WorldTools().FindEmptyTerrainPosition(wpPosFixed, wpPos, emptyRange, 1);
			wpPos = wpPosFixed;
		}
		
		AIWaypoint waypoint = CreateWaypointEntity(wptype);
		waypoint.SetOrigin(wpPos);
		return waypoint;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Creates a waypoint entity
	\param wptype
	*/
	
	static AIWaypoint CreateWaypointEntity(DC_EWaypointMoveType wptype)
	{
		private string wpPrefab = "";
		
		switch (wptype)
		{
			case DC_EWaypointMoveType.MOVE:
				wpPrefab = "{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et";
				break;
			case DC_EWaypointMoveType.PATROL:
				wpPrefab = "{22A875E30470BD4F}Prefabs/AI/Waypoints/AIWaypoint_Patrol.et";
				break;
			case DC_EWaypointMoveType.MOVECYCLE:
			case DC_EWaypointMoveType.PATROLCYCLE:
				wpPrefab = "{35BD6541CBB8AC08}Prefabs/AI/Waypoints/AIWaypoint_Cycle.et";
				break;
			default: 
				wpPrefab = "";
				break;		
		}
		
		Resource resource = Resource.Load(wpPrefab);
		if (!resource)
			return null;
		
		AIWaypoint wp = AIWaypoint.Cast(GetGame().SpawnEntityPrefab(resource));
		if (!wp)
			return null;
		
		return wp;
	}	
	
}