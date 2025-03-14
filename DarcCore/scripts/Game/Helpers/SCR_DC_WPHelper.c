//Helpers SCR_DC_WPHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for waypoint related things
*/

enum DC_EWaypointGenerationType
{
	NONE,
	RANDOM,			//Use one from the list below randomly
	SCATTERED,		//Completely random waypoints without any logic
	RADIUS,			//AI follow a path that is close to a circle with a radius. There is some additional randomization to avoid a perfect circle.
	ROUTE,			//
	LOITER,			//Currently makes a DEFEND waypoint for the location
	SLOTS 			//AI goes from a slot to slot. NOTE: This will not work unless the map has slots (the S/M/L letters on map) defined.
};

enum DC_EWaypointMoveType
{
	NONE,
	RANDOM,			//Pick a random one. Mission should define how to handle randomness. Often a reasonable RANDOM is a selection between MOVECYCLE and PATROLCYCLE.
	MOVE,			//Creates move waypoints. AI will stop once waypoints are visited.
	PATROL,			//Same as MOVE but with patrol speed.
	MOVECYCLE,		//Creates move waypoints in cycke. AI will restart the cycle once all waypoints are visited.
	PATROLCYCLE,	//Same as MOVECYCLE but with patrol speed.
	LOITER			//If LOITER is chosen for generation, this needs to be in movetype. This is enforced in CreateMissionAIWaypoints
};

sealed class SCR_DC_WPHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Create waypoints for an AI groups
	This will randomize the type, speed and count of waypoints to create.
	\param group
	\param wpGenType Type for waypoint generation
	\param posFrom Starting point for waypoint. Note that AI may be spawned to another spot and will move first here.
	\param posTo End point for waypoint. This is only used for ROUTE
	\param wpMoveType Type for movement 
	\param wpRangeLow, wpRangeHigh The radius limits to create waypoints for a group. Note used for ROUTE
	*/
	static void CreateMissionAIWaypoints(SCR_AIGroup group, DC_EWaypointGenerationType wpGenType, vector posFrom, vector posTo, DC_EWaypointMoveType wpMoveType = DC_EWaypointMoveType.MOVECYCLE, int wpRangeLow = 50, int wpRangeHigh = 200)
	{		
		if (group)
		{
			array<AIWaypoint> waypoints = {};
			AIWaypointCycle wpcycle = null;
			
			RemoveWaypoints(group);
		
			int rndCount = Math.RandomInt(6, 14);
			int rndRange = Math.RandomInt(wpRangeLow, wpRangeHigh);
			
			//Select the waypoint generation type. Randomize if requested.
			if (wpGenType == DC_EWaypointGenerationType.RANDOM)
			{
				array<DC_EWaypointGenerationType> waypointGenTypeArray = {DC_EWaypointGenerationType.SCATTERED, DC_EWaypointGenerationType.RADIUS, DC_EWaypointGenerationType.RADIUS, DC_EWaypointGenerationType.RADIUS, DC_EWaypointGenerationType.LOITER};
				wpGenType = waypointGenTypeArray.GetRandomElement();
			}

			//Select the waypoint movement type. Randomize if requested.
			if (wpMoveType == DC_EWaypointMoveType.RANDOM)
			{
				array<DC_EWaypointMoveType> waypointMoveTypeArray = {DC_EWaypointMoveType.MOVECYCLE, DC_EWaypointMoveType.PATROLCYCLE, DC_EWaypointMoveType.PATROLCYCLE, DC_EWaypointMoveType.PATROLCYCLE};
				wpMoveType = waypointMoveTypeArray.GetRandomElement();
			}

			//If the generation type is LOITER, movement needs to be LOITER too.
			if (wpGenType == DC_EWaypointGenerationType.LOITER)
			{
				wpMoveType = DC_EWaypointMoveType.LOITER;
			}
			
			//Create a cycle wp if needed
			if (wpMoveType == DC_EWaypointMoveType.MOVECYCLE || wpMoveType == DC_EWaypointMoveType.PATROLCYCLE)
			{
				wpcycle = AIWaypointCycle.Cast(CreateWaypointEntity(wpMoveType));
				
				//Cycle created, change type to something normal
				switch (wpMoveType)
				{
					case DC_EWaypointMoveType.MOVECYCLE:
						wpMoveType = DC_EWaypointMoveType.MOVE;
						break;
					case DC_EWaypointMoveType.PATROLCYCLE:
						wpMoveType = DC_EWaypointMoveType.PATROL;
						break;
					default:
						SCR_DC_Log.Add("[SCR_DC_WPHelper:CreateMissionAIWaypoints] Incorrect wpMoveType", LogLevel.ERROR);
				}
				wpcycle.SetOrigin(group.GetOrigin());
				SCR_DC_Log.Add("[SCR_DC_WPHelper:CreateMissionAIWaypoints] Created cycle", LogLevel.SPAM);
			}			

			//Generate waypoints
			if (wpGenType == DC_EWaypointGenerationType.ROUTE)
			{
				GenerateWaypoints(waypoints, posFrom, posTo, 0, wpMoveType, wpGenType, 0, false);	//ROUTE does not use the rndCount and rndRange and spot is to be exact
			}
			else
			{
				GenerateWaypoints(waypoints, posFrom, posTo, rndCount, wpMoveType, wpGenType, rndRange, true);
			}
											

			//Add waypoints as a cycle
			if (wpcycle)
			{				
				if (waypoints.Count() > 0)
				{
					wpcycle.SetWaypoints(waypoints);
					group.AddWaypoint(wpcycle);
					
					SCR_DC_Log.Add("[SCR_DC_WPHelper:AddToCycleWP] Adding " + waypoints.Count() + " waypoints as a cycle", LogLevel.SPAM);
				}
				else
				{
					SCR_DC_Log.Add("[SCR_DC_WPHelper:AddToCycleWP] No waypoints added to group: " + group, LogLevel.WARNING);
				}		
			}
			//Create normal waypoints
			else
			{
				foreach (AIWaypoint wpc : waypoints)
				{
					group.AddWaypoint(wpc);
				}
				
				//Create a LOITER waypoint as the last one so that AIs don't just stand there.
				AIWaypoint wploiter = FindAndCreateWaypoint(waypoints[waypoints.Count() - 1].GetOrigin(), DC_EWaypointMoveType.LOITER);
				if(wploiter)
				{
					wploiter.SetOrigin(waypoints[waypoints.Count() - 1].GetOrigin());
					group.AddWaypoint(wploiter);
				}
				
				SCR_DC_Log.Add("[SCR_DC_WPHelper:CreateWaypoints] Adding non-cycle waypoints: " + waypoints.Count(), LogLevel.DEBUG);
			}						
			
			int realCount = waypoints.Count();
			
			SCR_DC_Log.Add("[SCR_DC_WPHelper:CreateMissionAIWaypoints] Added " + realCount + "/" + rndCount + " waypoints (" + SCR_Enum.GetEnumName(DC_EWaypointGenerationType, wpGenType) + ", " + SCR_Enum.GetEnumName(DC_EWaypointMoveType, wpMoveType) + ")", LogLevel.DEBUG);
		}
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Creates a single waypoint for a group
	\param group
	\param pos
	\param wptype 
	*/
	static bool CreateWaypoint(AIGroup group, vector pos, DC_EWaypointMoveType wptype = DC_EWaypointMoveType.MOVE)
	{
		AIWaypoint aiWayPoint;
		aiWayPoint = FindAndCreateWaypoint(pos, wptype);
		if(aiWayPoint)
		{
			group.AddWaypoint(aiWayPoint);
			return true;
		}
		return false;
	}		
			
	//------------------------------------------------------------------------------------------------
	/*!
	Remove all waypoints from a group
	\param group
	*/
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
	Get count of waypoints for a group
	TBD: If the group has cycle waypoint, it will return 1. Should return the count of waypoints in the cycle.
	\param group
	*/
	static int GetWaypointCount(AIGroup group)
	{
		array<AIWaypoint> waypoints = {};
		group.GetWaypoints(waypoints);
		
		return waypoints.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Creates an array waypoints in a given pattern
	\param posFrom Starting point for waypoint. Note that AI may be spawned to another spot and will move first here.
	\param posTo End point for waypoint. This is only used for ROUTE
	\param count How many waypoints to create
	\param moveType See DC_EWaypointMoveType definition for more information
	\param genType See DC_EWaypointGenerationType definition for more information
	\param range Range (radius) for randomization from given position.
	\param emptyspot True if found position needs to be on clear area.
	*/
	static void GenerateWaypoints(out array<AIWaypoint> waypoints, vector posFrom, vector posTo, int count, DC_EWaypointMoveType moveType = DC_EWaypointMoveType.PATROL, DC_EWaypointGenerationType genType = DC_EWaypointGenerationType.SCATTERED, float range = 0, bool emptyspot = false )
	{
		//Scattered as in completely random positions
		if (genType == DC_EWaypointGenerationType.SCATTERED)
		{		
			for (int i = 0; i < count; i++)
			{
				//Add some additional randomization
				float rndRange = 0;//Math.RandomInt(0, range/3); 
				
				AIWaypoint waypoint = FindAndCreateWaypoint(posFrom, moveType, (range + rndRange), emptyspot);
				if (waypoint != null)
				{
					waypoints.Insert(waypoint);
				}
			}		
		}

		//Circular
		if (genType == DC_EWaypointGenerationType.RADIUS)
		{		
			float startAngle = Math.RandomFloat(0, 360);
			
			for (int i = 0; i < count; i++)
			{				
				vector vec = SCR_DC_Misc.GetCoordinatesOnCircle(posFrom, range, i*(360/count), startAngle);
				
				AIWaypoint waypoint = FindAndCreateWaypoint(vec, moveType, (range/4), emptyspot);
				if (waypoint != null)
				{
					waypoints.Insert(waypoint);
				}
			}		
		}

		//Route
		if (genType == DC_EWaypointGenerationType.ROUTE)
		{			
			array<vector> routePts = {};
			SCR_DC_RoadHelper.CreateRoute(routePts, posFrom, posTo);
			
			//SCR_DC_RoadHelper.DebugDrawRoad(routePts);
			
			foreach(vector pt: routePts)
			{
				AIWaypoint waypoint = FindAndCreateWaypoint(pt, moveType, 0, emptyspot);
				if (waypoint != null)
				{
					waypoints.Insert(waypoint);
				}
			}		
		}

		//Loiter
		if (genType == DC_EWaypointGenerationType.LOITER)
		{			
			//TBD: Does not currently work one would expect
			AIWaypoint waypoint = FindAndCreateWaypoint(posFrom, moveType, 0, emptyspot);
			if (waypoint != null)
			{
				waypoints.Insert(waypoint);
			}
		}
				
		//Slots
		if (genType == DC_EWaypointGenerationType.SLOTS)
		{		
			array<IEntity> slots = {};
			SCR_DC_Locations.GetLocationSlots(slots, posFrom, range);
			
			for (int i = 0; i < count; i++)
			{
				IEntity slot = slots.GetRandomElement();				

				AIWaypoint waypoint = FindAndCreateWaypoint(slot.GetOrigin(), moveType, 3, emptyspot);
				if (waypoint != null)
				{
					waypoints.Insert(waypoint);
				}
			}		
		}

		if (waypoints.Count() == 0)
		{
			SCR_DC_Log.Add("[SCR_DC_WPHelper:GetWaypoints] Could not create waypoints.", LogLevel.WARNING);
		}		
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Creates a waypoint
	\param position
	\param wptype
	\param range Range (radius) for randomization from given position.
	\param emptyspot True if found position needs to be on clear area.
	*/
	static AIWaypoint FindAndCreateWaypoint(vector position, DC_EWaypointMoveType wptype, float range = 0, bool emptyspot = false )
	{	
		vector wpPos;
		float emptyRange = 30;	//Radius of queried area
		
		wpPos = SCR_DC_Misc.RandomizePos(position, range);
		
		vector wpPosFixed;

		if (emptyspot)
		{
			SCR_WorldTools().FindEmptyTerrainPosition(wpPosFixed, wpPos, emptyRange, 1, 1, TraceFlags.ENTS|TraceFlags.WORLD|TraceFlags.OCEAN);
			wpPos = wpPosFixed;
		}
		
		if (SCR_DC_Misc.IsPosInWater(wpPos))
		{
			SCR_DC_Log.Add("[SCR_DC_WPHelper:FindAndCreateWaypoint] Waypoint in water. Skipping.", LogLevel.DEBUG);			
			return null;
		}
		
		AIWaypoint waypoint = CreateWaypointEntity(wptype);
		if(waypoint)
		{
			waypoint.SetOrigin(wpPos);
		}
		
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
			case DC_EWaypointMoveType.LOITER:
				wpPrefab = "{FAD1D789EE291964}Prefabs/AI/Waypoints/AIWaypoint_Defend_Large.et";
//				wpPrefab = "{4ECD14650D82F5CA}Prefabs/AI/Waypoints/AIWaypoint_Loiter_CO.et";
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