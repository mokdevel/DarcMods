//Helpers SCR_DC_MissionHelper.c

sealed class SCR_DC_MissionHelper
{
	private const int DC_LOCATION_SEACRH_ITERATIONS = 5;	//How many different spots to try for a mission before giving up
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find an empty spot where to spawn a mission. 
	*/	
	static vector FindMissionPos(float distanceToMission = -1, float distanceToPlayer = -1)
	{	
		vector pos = SCR_DC_Misc.GetRandomWorldPos();
		if (!IsValidMissionPos(pos, distanceToMission, distanceToPlayer))
		{
			return "0 0 0";
		}
		
		return pos;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find a location where to spawn a mission. 
	*/	
	static IEntity FindMissionLocation(array<EMapDescriptorType> locationTypes)
	{	
		//Find a random location
		vector pos = "0 0 0";
		bool positionFound = false;
		
		IEntity location = null;
		array<IEntity> locations = {};
		SCR_DC_Locations.GetLocations(locations, locationTypes);		
		
		for (int i = 0; i < DC_LOCATION_SEACRH_ITERATIONS; i++)
		{
			location = locations.GetRandomElement();
			pos = location.GetOrigin();
			
			if (SCR_DC_MissionHelper.IsValidMissionPos(pos))
			{				
				//Find an empty position at mission pos
				pos = SCR_DC_SpawnHelper.FindEmptyPos(pos, 200, 10);
				positionFound = true;
			
				SCR_DC_Log.Add("[SCR_DC_Mission_Occupation] Location for spawn " + SCR_StringHelper.Translate(location.GetName()) + " " + location.GetOrigin(), LogLevel.DEBUG);
				break;
			}
			else
			{						
				SCR_DC_Log.Add("[SCR_DC_Mission_Occupation] Invalid mission position. Try " + (i + 1) + "/" + DC_LOCATION_SEACRH_ITERATIONS, LogLevel.SPAM);
			}
		}

		if (!positionFound)
		{
			return null;
		}

		location.SetOrigin(pos);
						
		return location;
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Check if given pos is a valid position for mission. 
	
	The position shall have ... 
	- no players nearby
	- no missions nearby
	- not in water	
	*/	
	static bool IsValidMissionPos(vector pos, float distanceToMission = -1, float distanceToPlayer = -1)
	{
		float waterHeight = GetGame().GetWorld().GetOceanHeight(pos[0], pos[2]);
	
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		if (distanceToMission == -1)
			distanceToMission = baseGameMode.missionFrame.m_Config.minDistanceToMission;

		if (distanceToPlayer == -1)
			distanceToPlayer = baseGameMode.missionFrame.m_Config.minDistanceToPlayer;
				
		if (waterHeight != 0)
		{			
			return false;
		}
		
		if (SCR_DC_PlayerHelper.IsAnyPlayerCloseToPos(pos, distanceToPlayer))
		{
			return false;
		}

		if (IsAnyMissionCloseToPos(pos, distanceToMission))
		{
			return false;
		}

		if (IsPosInNonValidArea(pos))
		{
			return false;
		}
										
		if (!SCR_DC_Core.RELEASE)
		{
			SCR_DC_MapMarkersUI.AddMarker("DUMMY_", pos, "", DC_EMarkerType.DEBUG);
		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Check if any mission is close to given position. Returns true if another mission is within radius
	*/	
	static bool IsAnyMissionCloseToPos(vector positionToCheck, int radiusToCheck = 1000)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		foreach(int i, SCR_DC_Mission mission: baseGameMode.missionFrame.m_MissionList)
		{
			float distance = vector.Distance(mission.GetPos(), positionToCheck);
		
			if (distance < radiusToCheck)
			{
				return true;
				break;
			}
		}

		return false;
	}		

	//------------------------------------------------------------------------------------------------
	/*!
	Check if pos is within nonValidArea
	*/	
	static bool IsPosInNonValidArea(vector pos)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	
//		foreach (SCR_DC_NonValidArea nonValidArea : baseGameMode.missionFrame.m_Config.nonValidAreas)
		foreach (SCR_DC_NonValidArea nonValidArea : baseGameMode.missionFrame.m_NonValidAreas)
		{
			float distance = vector.Distance(pos, nonValidArea.pos);
		
			if (distance < nonValidArea.radius)
			{
				return true;
				break;
			}
		}

		return false;
	}			

	//------------------------------------------------------------------------------------------------
	/*!
	Test mission positions on map. Only for debugging.
	*/	
	static void DebugTestMissionPos(float distanceToMission = -1, float distanceToPlayer = -1)
	{	
		vector pos;

		for (int i = 0; i < 500; i++)
		{		
			SCR_DC_MissionHelper.FindMissionPos();
		}		
	}
				
	//------------------------------------------------------------------------------------------------
	static SCR_AIGroup SpawnMissionAIGroup(string groupToSpawn, vector pos)
	{
		vector posFixed = SCR_DC_SpawnHelper.FindEmptyPos(pos, 100, 8);
		
		SCR_AIGroup group = SCR_DC_AIHelper.SpawnGroup(groupToSpawn, posFixed);
		
		return group;
	}	

	//------------------------------------------------------------------------------------------------
	//Create waypoint for AI m_Groups
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
			
			SCR_DC_Log.Add("[SCR_DC_MissionHelper:CreateMissionAIWaypoints] Adding " + rndCount + " waypoints (" + SCR_Enum.GetEnumName(DC_EWaypointRndType, waypointRndType) + ", " + SCR_Enum.GetEnumName(DC_EWaypointMoveType, waypointMoveType) + ")", LogLevel.DEBUG);

			SCR_DC_AIHelper.CreateWaypoints(group, rndCount, waypointMoveType, waypointRndType, rndRange, true);
		}
	}			
}