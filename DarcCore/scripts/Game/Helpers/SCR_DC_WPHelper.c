//Helpers SCR_DC_WPHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for waypoint related things
*/

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

sealed class SCR_DC_WPHelper
{
	//------------------------------------------------------------------------------------------------
	//Create waypoints for an AI groups
	//This will randomize the type, speed and count of waypoints to create.
	
	static void CreateMissionAIWaypoints(SCR_AIGroup group, int wpRangeLow, int wpRangeHigh, DC_EWaypointMoveType wpMoveType = DC_EWaypointMoveType.MOVECYCLE, DC_EWaypointRndType wpType = DC_EWaypointRndType.SCATTERED)
	{		
		if (group)
		{
			int rndCount = Math.RandomInt(4, 11);
			int rndRange = Math.RandomInt(wpRangeLow, wpRangeHigh);
			
			//Select the waypoint formation
			DC_EWaypointRndType waypointRndType = wpType;
			if (waypointRndType == DC_EWaypointRndType.RANDOM)
			{
				array<DC_EWaypointRndType> waypointRndTypeArray = {DC_EWaypointRndType.SCATTERED, DC_EWaypointRndType.RADIUS, DC_EWaypointRndType.RADIUS}; //DC_EWaypointRndType.SLOTS
				waypointRndType = waypointRndTypeArray.GetRandomElement();
			}

			//Select the waypoint movement type
			DC_EWaypointMoveType waypointMoveType = wpMoveType;
			if (waypointMoveType == DC_EWaypointMoveType.RANDOM)
			{
				array<DC_EWaypointMoveType> waypointMoveTypeArray = {DC_EWaypointMoveType.MOVECYCLE, DC_EWaypointMoveType.PATROLCYCLE, DC_EWaypointMoveType.PATROLCYCLE, DC_EWaypointMoveType.PATROLCYCLE};
				waypointMoveType = waypointMoveTypeArray.GetRandomElement();
			}

			int realCount = CreateWaypoints(group, rndCount, waypointMoveType, waypointRndType, rndRange, true);
						
			SCR_DC_Log.Add("[SCR_DC_WPHelper:CreateMissionAIWaypoints] Added " + realCount + "/" + rndCount + " waypoints (" + SCR_Enum.GetEnumName(DC_EWaypointRndType, waypointRndType) + ", " + SCR_Enum.GetEnumName(DC_EWaypointMoveType, waypointMoveType) + ")", LogLevel.DEBUG);
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Creates a single waypoint for a group
	NOTE: Currently only supports creation of cycle waypoints. 
	TBD: Non-cycle waypoint list creation for a group
	
	\param group
	\param pos
	\param wptype 
	\param range Range (radius) for randomization from given position.
	\param emptyspot True if found position needs to be on clear area.	
	*/
	static bool CreateWaypoint(AIGroup group, vector pos, DC_EWaypointMoveType wptype = DC_EWaypointMoveType.MOVE, float range = 100, bool emptyspot = false)
	{
		RemoveWaypoints(group);		
		
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
	Creates a list of waypoints for a group around the spawn position.
	NOTE: Currently only supports creation of cycle waypoints. 
	TBD: Non-cycle waypoint list creation for a group
	
	\param group
	\param count Amount of waypoints to create
	\param wptype 
	\param rndtype Type of waypoint creation
	\param range Range (radius) for randomization from given position.
	\param emptyspot True if found position needs to be on clear area.	
	*/
	static int CreateWaypoints(SCR_AIGroup group, int count = 7, DC_EWaypointMoveType wptype = DC_EWaypointMoveType.MOVECYCLE, DC_EWaypointRndType rndtype = DC_EWaypointRndType.SCATTERED, float range = 100, bool emptyspot = false)
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
					SCR_DC_Log.Add("[SCR_DC_WPHelper:CreateWaypoints] Incorrect wptype", LogLevel.ERROR);
			}
			SCR_DC_Log.Add("[SCR_DC_WPHelper:CreateWaypoints] Creating cycle", LogLevel.SPAM);
		}
		
		RemoveWaypoints(group);		
		waypoints = GetPatrolWaypoints(group.GetOrigin(), count, wptype, rndtype, range, emptyspot);
		
		if (waypoints.Count() == 0)
		{
			SCR_DC_Log.Add("[SCR_DC_WPHelper:CreateWaypoints] Could not create waypoint to group: " + group, LogLevel.WARNING);
			return waypoints.Count();
		}
		
		//Create cycle waypoints
		if (wpcycle != null)
		{				
			if (waypoints.Count() > 0)
			{
				wpcycle.SetWaypoints(waypoints);
				group.AddWaypoint(wpcycle);
				
				SCR_DC_Log.Add("[SCR_DC_WPHelper:CreateWaypoints] Adding " + waypoints.Count() + " waypoints as a cycle", LogLevel.SPAM);
				return waypoints.Count();
			}
			else
			{
				SCR_DC_Log.Add("[SCR_DC_WPHelper:CreateWaypoints] No waypoints added to group: " + group, LogLevel.WARNING);
				return waypoints.Count();
			}		
		}
		//Create normal waypoints
		else
		{
			foreach (AIWaypoint wpc : waypoints)
			{
				group.AddWaypoint(wpc);
			}
			
			SCR_DC_Log.Add("[SCR_DC_WPHelper:CreateWaypoints] Adding non-cycle waypoints: " + waypoints.Count(), LogLevel.DEBUG);
			return waypoints.Count();			
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
	\param position
	\param count How many waypoints to create
	\param wptype See DC_EWaypointMoveType definition for more information
	\param rndtype See DC_EWaypointRndType definition for more information
	\param range Range (radius) for randomization from given position.
	\param emptyspot True if found position needs to be on clear area.
	*/
	static array<AIWaypoint> GetPatrolWaypoints(vector position, int count, DC_EWaypointMoveType wptype = DC_EWaypointMoveType.PATROL, DC_EWaypointRndType rndtype = DC_EWaypointRndType.SCATTERED, float range = 0, bool emptyspot = false )
	{
		array<AIWaypoint> waypoints = {};
		
		//Scattered as in completely random positions
		if (rndtype == DC_EWaypointRndType.SCATTERED)
		{		
			for (int i = 0; i < count; i++)
			{
				//Add some additional randomization
				float rndRange = Math.RandomInt(0, rndRange/3); 
				
				AIWaypoint waypoint = FindAndCreateWaypoint(position, wptype, (range + rndRange), emptyspot);
				if (waypoint != null)
				{
					waypoints.Insert(waypoint);
				}
			}		
		}

		//Circular
		if (rndtype == DC_EWaypointRndType.RADIUS)
		{		
			float startAngle = Math.RandomFloat(0, 360);
			
			for (int i = 0; i < count; i++)
			{				
				vector vec = SCR_DC_Misc.GetCoordinatesOnCircle(position, range, i*(360/count), startAngle);
				
				AIWaypoint waypoint = FindAndCreateWaypoint(vec, wptype, (range/4), emptyspot);
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

				AIWaypoint waypoint = FindAndCreateWaypoint(slot.GetOrigin(), wptype, 3, emptyspot);
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