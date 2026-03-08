//Helpers SDRC_WPHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for waypoint related things
*/

enum SDRC_EWaypointGenerationType
{
	NONE,
	RANDOM,			//Use one of these randomly: SCATTERED (1), RADIUS (3), LOITER (1)
	SCATTERED,		//Completely random waypoints without any logic
	RADIUS,			//AI follow a path that is close to a circle with a radius. There is some additional randomization to avoid a perfect circle.
	ROUTE,			//AI follow a route from A to B. The route is created with waypoints along the road.
	LOITER,			//Currently makes a DEFEND waypoint for the location
	SLOTS, 			//AI goes from a slot to slot. NOTE: This will not work unless the map has slots (the S/M/L letters on map) defined.
};

enum SDRC_EWaypointMoveType
{
	NONE,
	RANDOM,			//Pick a random one from MOVECYCLE (1) and PATROLCYCLE (3).
	MOVE,			//Creates move waypoints. AI will stop once waypoints are visited.
	PATROL,			//Same as MOVE but with patrol speed.
	MOVECYCLE,		//Creates move waypoints in cycke. AI will restart the cycle once all waypoints are visited.
	PATROLCYCLE,	//Same as MOVECYCLE but with patrol speed.
	LOITER,			//If LOITER is chosen for generation, this needs to be in movetype. This is enforced in CreateMissionAIWaypoints
	GETOUT,
};

//------------------------------------------------------------------------------------------------
sealed class SDRC_WPHelper
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
	static void CreateMissionAIWaypoints(SCR_AIGroup group, SDRC_EWaypointGenerationType wpGenType, vector posFrom, vector posTo, SDRC_EWaypointMoveType wpMoveType = SDRC_EWaypointMoveType.MOVECYCLE, int wpRangeLow = 50, int wpRangeHigh = 200)
	{		
		if (group)
		{
			array<AIWaypoint> waypoints = {};
			AIWaypointCycle wpcycle = null;
			
			RemoveWaypoints(group);
			
			int rndRange = SDRC_Misc.RandomInt(wpRangeLow, wpRangeHigh);
			int max = rndRange / 30;
			int rndCount = SDRC_Misc.RandomInt(3, 3 + max);

			SDRC_Log.Add("[SDRC_WPHelper:CreateMissionAIWaypoints] Random waypoint count: " + rndCount + " , Random range: " + rndRange, LogLevel.SPAM);
						
			//Select the waypoint generation type. Randomize if requested.
			if (wpGenType == SDRC_EWaypointGenerationType.RANDOM)
			{
				array<SDRC_EWaypointGenerationType> waypointGenTypeArray = {SDRC_EWaypointGenerationType.LOITER, 
																		  SDRC_EWaypointGenerationType.SCATTERED, SDRC_EWaypointGenerationType.SCATTERED, 
					                                                      SDRC_EWaypointGenerationType.RADIUS, SDRC_EWaypointGenerationType.RADIUS, SDRC_EWaypointGenerationType.RADIUS, SDRC_EWaypointGenerationType.RADIUS};
				wpGenType = waypointGenTypeArray.GetRandomElement();
			}

			//Select the waypoint movement type. Randomize if requested.
			if (wpMoveType == SDRC_EWaypointMoveType.RANDOM)
			{
				array<SDRC_EWaypointMoveType> waypointMoveTypeArray = {SDRC_EWaypointMoveType.MOVECYCLE, 
																	 SDRC_EWaypointMoveType.PATROLCYCLE, SDRC_EWaypointMoveType.PATROLCYCLE, SDRC_EWaypointMoveType.PATROLCYCLE, SDRC_EWaypointMoveType.PATROLCYCLE,
																	 SDRC_EWaypointMoveType.MOVE, SDRC_EWaypointMoveType.PATROL};
				wpMoveType = waypointMoveTypeArray.GetRandomElement();
			}

			//If the generation type is LOITER, movement needs to be LOITER too.
			if (wpGenType == SDRC_EWaypointGenerationType.LOITER)
			{
				wpMoveType = SDRC_EWaypointMoveType.LOITER;
			}
			
			//Create a cycle wp if needed
			if (wpMoveType == SDRC_EWaypointMoveType.MOVECYCLE || wpMoveType == SDRC_EWaypointMoveType.PATROLCYCLE)
			{
				wpcycle = AIWaypointCycle.Cast(CreateWaypointEntity(wpMoveType));
				
				//Cycle created, change type to something normal
				switch (wpMoveType)
				{
					case SDRC_EWaypointMoveType.MOVECYCLE:
						wpMoveType = SDRC_EWaypointMoveType.MOVE;
						break;
					case SDRC_EWaypointMoveType.PATROLCYCLE:
						wpMoveType = SDRC_EWaypointMoveType.PATROL;
						break;
					default:
						SDRC_Log.Add("[SDRC_WPHelper:CreateMissionAIWaypoints] Incorrect wpMoveType", LogLevel.ERROR);
				}
				wpcycle.SetOrigin(group.GetOrigin());
				SDRC_Log.Add("[SDRC_WPHelper:CreateMissionAIWaypoints] Created cycle", LogLevel.SPAM);
			}			

			//Generate waypoints
			if (wpGenType == SDRC_EWaypointGenerationType.ROUTE)
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
					
					SDRC_Log.Add("[SDRC_WPHelper:CreateMissionAIWaypoints] Adding " + waypoints.Count() + " waypoints as a cycle", LogLevel.SPAM);
				}
				else
				{
					SDRC_Log.Add("[SDRC_WPHelper:CreateMissionAIWaypoints] No waypoints added to group: " + group, LogLevel.WARNING);
				}		
			}
			//Create normal waypoints
			else
			{
				foreach (AIWaypoint wpc : waypoints)
				{
					group.AddWaypoint(wpc);
				}
				
				if (waypoints.Count() > 0)
				{
					//Create a LOITER waypoint as the last one so that AIs don't just stand there.
					AIWaypoint wploiter = FindAndCreateWaypoint(waypoints[waypoints.Count() - 1].GetOrigin(), SDRC_EWaypointMoveType.LOITER);
					if (wploiter)
					{
						wploiter.SetOrigin(waypoints[waypoints.Count() - 1].GetOrigin());
						group.AddWaypoint(wploiter);
					}
				}
								
				SDRC_Log.Add("[SDRC_WPHelper:CreateMissionAIWaypoints] Adding non-cycle waypoints: " + waypoints.Count(), LogLevel.SPAM);
			}						

			//If now waypoints defined, add a LOITER waypoint as a minimum.
			if (waypoints.Count() == 0)
			{
				SDRC_Log.Add("[SDRC_WPHelper:CreateMissionAIWaypoints] No waypoints added to group. Create a LOITER waypoint.", LogLevel.DEBUG);
				
				//Create a LOITER waypoint so that AIs don't just stand there.
				AIWaypoint wploiter = FindAndCreateWaypoint(posFrom, SDRC_EWaypointMoveType.LOITER);
				if (wploiter)
				{
					waypoints.Insert(wploiter);
					wploiter.SetOrigin(posFrom);
					group.AddWaypoint(wploiter);
				}
			}
			
			//Move the group to the first waypoint
			if (waypoints.Count() > 0)	//This check should be unnecessary as there should always be atleast one WP.
			{
				AIWaypoint wp = waypoints[0];
				
				group.SetOrigin(wp.GetOrigin());
			}
									
			int realCount = waypoints.Count();
			
			SDRC_Log.Add("[SDRC_WPHelper:CreateMissionAIWaypoints] Added " + realCount + " waypoints (" + SCR_Enum.GetEnumName(SDRC_EWaypointGenerationType, wpGenType) + ", " + SCR_Enum.GetEnumName(SDRC_EWaypointMoveType, wpMoveType) + ")", LogLevel.DEBUG);
		}
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Creates a single waypoint for a group
	\param group
	\param pos
	\param wptype 
	*/
	static bool CreateWaypoint(AIGroup group, vector pos, SDRC_EWaypointMoveType wptype = SDRC_EWaypointMoveType.MOVE)
	{
		if (!group)
		{
			return false;
		}
		
		AIWaypoint aiWayPoint;
		aiWayPoint = FindAndCreateWaypoint(pos, wptype);
		if (aiWayPoint)
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
		
		foreach (AIWaypoint waypoint : waypoints)
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
	\param moveType See SDRC_EWaypointMoveType definition for more information
	\param genType See SDRC_EWaypointGenerationType definition for more information
	\param range Range (radius) for randomization from given position.
	\param emptyspot True if found position needs to be on clear area.
	*/
	static void GenerateWaypoints(out array<AIWaypoint> waypoints, vector posFrom, vector posTo, int count, SDRC_EWaypointMoveType moveType = SDRC_EWaypointMoveType.PATROL, SDRC_EWaypointGenerationType genType = SDRC_EWaypointGenerationType.SCATTERED, float range = 0, bool emptyspot = false )
	{
		//Scattered as in completely random positions
		if (genType == SDRC_EWaypointGenerationType.SCATTERED)
		{		
			for (int i = 0; i < count; i++)
			{
				//Add some additional randomization
				float rndRange = SDRC_Misc.RandomInt(0, range/21); 
				
				AIWaypoint waypoint = FindAndCreateWaypoint(posFrom, moveType, (range + rndRange), emptyspot);
				if (waypoint != null)
				{
					waypoints.Insert(waypoint);
				}
			}		
		}

		//Circular
		if (genType == SDRC_EWaypointGenerationType.RADIUS)
		{		
			float startAngle = SDRC_Misc.RandomFloat(0, 360);
			
			for (int i = 0; i < count; i++)
			{				
				vector vec = SDRC_Misc.GetCoordinatesOnCircle(posFrom, range, i*(360/count), startAngle);
				
				AIWaypoint waypoint = FindAndCreateWaypoint(vec, moveType, (range/8), emptyspot);
				if (waypoint != null)
				{
					waypoints.Insert(waypoint);
				}
			}		
		}

		//Route
		if (genType == SDRC_EWaypointGenerationType.ROUTE)
		{			
			array<vector> routePts = {};
			SDRC_RoadHelper.CreateRoute(routePts, posFrom, posTo);
			
			foreach (vector pt : routePts)
			{
				AIWaypoint waypoint = FindAndCreateWaypoint(pt, moveType, 0, emptyspot);
				if (waypoint != null)
				{
					waypoints.Insert(waypoint);
				}
			}		
		}

		//Loiter
		if (genType == SDRC_EWaypointGenerationType.LOITER)
		{			
			AIWaypoint waypoint = FindAndCreateWaypoint(posFrom, moveType, 0, emptyspot);
			if (waypoint != null)
			{
				waypoints.Insert(waypoint);
			}
		}
				
		//Slots
		if (genType == SDRC_EWaypointGenerationType.SLOTS)
		{		
			array<IEntity> slots = {};
			SDRC_Locations.GetLocationSlots(slots, posFrom, range);
			
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
			SDRC_Log.Add("[SDRC_WPHelper:GenerateWaypoints] Could not create waypoints.", LogLevel.WARNING);
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
	static AIWaypoint FindAndCreateWaypoint(vector position, SDRC_EWaypointMoveType wptype, float range = 0, bool emptyspot = false )
	{	
		vector wpPos;
		const float EMPTY_RANGE = 30;	//Radius of queried area
		
		wpPos = SDRC_Misc.RandomizePos(position, range);
		
		vector wpPosFixed;

		if (emptyspot)
		{
			SCR_WorldTools().FindEmptyTerrainPosition(wpPosFixed, wpPos, EMPTY_RANGE, 1, 1, TraceFlags.ENTS|TraceFlags.WORLD|TraceFlags.OCEAN);
			wpPos = wpPosFixed;
		}
		
		if (SDRC_Misc.IsPosInWater(wpPos))
		{
			SDRC_Log.Add("[SDRC_WPHelper:FindAndCreateWaypoint] Waypoint in water. Skipping.", LogLevel.DEBUG);			
			return null;
		}
		
		AIWaypoint waypoint = CreateWaypointEntity(wptype);
		if (waypoint)
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
	static AIWaypoint CreateWaypointEntity(SDRC_EWaypointMoveType wptype)
	{
		private string wpPrefab = "";
		
		switch (wptype)
		{
			case SDRC_EWaypointMoveType.MOVE:
				wpPrefab = "{750A8D1695BD6998}Prefabs/AI/Waypoints/AIWaypoint_Move.et";
				break;
			case SDRC_EWaypointMoveType.PATROL:
				wpPrefab = "{22A875E30470BD4F}Prefabs/AI/Waypoints/AIWaypoint_Patrol.et";
				break;
			case SDRC_EWaypointMoveType.MOVECYCLE:
			case SDRC_EWaypointMoveType.PATROLCYCLE:
				wpPrefab = "{35BD6541CBB8AC08}Prefabs/AI/Waypoints/AIWaypoint_Cycle.et";
				break;
			case SDRC_EWaypointMoveType.LOITER:
				array<string> wpDefendList = {"{2FCBE5C76E285A7B}Prefabs/AI/Waypoints/AIWaypoint_DefendSmall.et", "{FAD1D789EE291964}Prefabs/AI/Waypoints/AIWaypoint_Defend_Large.et"};
				wpPrefab = wpDefendList.GetRandomElement();
//				wpPrefab = "{FAD1D789EE291964}Prefabs/AI/Waypoints/AIWaypoint_Defend_Large.et";
				break;
			case SDRC_EWaypointMoveType.GETOUT:
				wpPrefab = "{E5002E8CD9D1F4AF}Prefabs/AI/Waypoints/AIWaypoint_GetOutInstant.et";
//				wpPrefab = "{C40316EE26846CAB}Prefabs/AI/Waypoints/AIWaypoint_GetOut.et";
				break;
			default: 
				wpPrefab = "";
				break;		
		}
		
		Resource resource = Resource.Load(wpPrefab);
		if (!resource)
			return null;
		
		AIWaypoint wp = AIWaypoint.Cast(SDRC_SpawnHelper.SpawnEntityPrefabPersistence(resource));
		if (!wp)
			return null;
		
		return wp;
	}		
}