//Helpers SDRC_MissionHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for missions. 
*/

enum DC_EMissionPosFailReason
{
	NONE,
	POS_IN_WATER,
	PLAYER_TOO_CLOSE,
	MISSION_TOO_CLOSE,
	IN_NON_VALID_AREA
};

//------------------------------------------------------------------------------------------------
sealed class SDRC_MissionHelper
{
	private const int DC_LOCATION_SEACRH_ITERATIONS = 8;		//How many different spots to try for a mission before giving up
	private const int DC_LOCATION_SEACRH_RADIUS = 50;			//The start search radius for mission position 
	private const int DC_LOCATION_SEACRH_RADIUS_INC = 15;		//The increase of search radius for each failed iteration
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find a completely random position for a mission. "0 0 0" returned if nothing found.
	\param distanceToMission Distance to another mission. Two missions shall not be too close to each other. -1 will use the default from MissionFrame.
	\param distanceToPlayer Mission shall not spawn too close to a player. -1 will use the default from MissionFrame.
	*/	
	static vector FindMissionPos(float distanceToMission = -1, float distanceToPlayer = -1)
	{	
		vector pos = SDRC_Misc.GetRandomWorldPos();
		
		if (!IsValidMissionPos(pos, distanceToMission, distanceToPlayer))
		{
			return "0 0 0";
		}
		
		return pos;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Find a position near pos for a mission. "0 0 0" returned if nothing found.
	\param pos Position to start to search for mission position
	\param distanceToMission Distance to another mission. Two missions shall not be too close to each other. -1 will use the default from MissionFrame.
	\param distanceToPlayer Mission shall not spawn too close to a player. -1 will use the default from MissionFrame.
	*/	
	static vector FindMissionPos(vector pos, float distanceToMission = -1, float distanceToPlayer = -1, float size = 5)
	{	
		pos = FindWithIterate(pos, DC_LOCATION_SEACRH_RADIUS, size);
		
		return pos;
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Find a location where to spawn a mission. 
	\param locationTypes Array of EMapDescriptorType to look for a place
	\param size Size (radius) of the mission. This size should be the size of the objects to spawn - like a camp.
	*/	
	static vector FindMissionPos(array<EMapDescriptorType> locationTypes, float size = 5)
	{	
		//Find a random location
		vector pos = "0 0 0";
		
		IEntity location = null;
		array<IEntity> locations = {};
		SDRC_Locations.GetLocations(locations, locationTypes);		
		int searchRadius = DC_LOCATION_SEACRH_RADIUS;			//Find the position within DC_LOCATION_SEACRH_RADIUS from pos. In case of fail, we increase the area.
		
		if (locations.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_MissionHelper:FindMissionLocation] No locations found. Check the list of locationTypes in your mission." , LogLevel.ERROR);
			return "0 0 0";
		}

		location = locations.GetRandomElement();

		SDRC_Log.Add("[SDRC_MissionHelper:FindMissionPos] Searching near: " + location.GetName() + " " + location.GetOrigin(), LogLevel.DEBUG);
		
		pos = FindWithIterate(location.GetOrigin(), searchRadius, size);
		
		return pos;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Find the location close to the position 
	\param pos Position to start to search for mission position
	\param searchRadius The radius to search for the proper position.
	\param size Size (radius) of the mission. This size should be the size of the objects to spawn - like a camp.
	*/	
	static vector FindWithIterate(vector pos, float searchRadius, float size = 5)
	{
		bool positionFound = false;
		
		for (int i = 0; i < DC_LOCATION_SEACRH_ITERATIONS; i++)
		{
			//Find the position within searchRadius from pos.			
			if (SDRC_SpawnHelper.FindEmptyPos(pos, searchRadius, size))
			{			
				if (SDRC_MissionHelper.IsValidMissionPos(pos))
				{				
					positionFound = true;
					SDRC_Log.Add("[SDRC_MissionHelper:FindWithIterate] Position found: " + pos, LogLevel.DEBUG);
					break;
				}
			}
			else
			{	
				pos = SDRC_Misc.RandomizePos(pos, searchRadius/2);
				searchRadius = searchRadius + DC_LOCATION_SEACRH_RADIUS_INC;	//Increase the are with DC_LOCATION_SEACRH_RADIUS_INC
				SDRC_Log.Add("[SDRC_MissionHelper:FindWithIterate] Invalid position. Try " + (i + 1) + "/" + DC_LOCATION_SEACRH_ITERATIONS, LogLevel.SPAM);
			}
		}

		if (!positionFound)
		{
			return "0 0 0";
		}
			
		return pos;
	}	
		
	//------------------------------------------------------------------------------------------------
	/*!
	Find destination location where the mission ends.
	This is usable for missions where something is traveling to a destination.
	\param locationTypes Array of EMapDescriptorType to look for a place
	\param missionPos The mission position aka the starting point for mission
	\param distance Minimum distance for missionPos and destination
	*/		
	static vector FindMissionDestination(array<EMapDescriptorType> locationTypes, vector missionPos, int distance)
	{
		//IEntity location = null;
		vector pos = "0 0 0";

		bool positionFound = false;
		
		for (int i = 0; i < DC_LOCATION_SEACRH_ITERATIONS; i++)
		{					
			pos = SDRC_MissionHelper.FindMissionPos(locationTypes);
			if (pos == "0 0 0")
			{
				return "0 0 0";
			}
			
			if (!SDRC_Misc.IsPosNearPos(pos, missionPos, distance))	//Shall be distance meters from actual missionPos
			{
				positionFound = true;
				SDRC_Log.Add("[SDRC_MissionHelper:FindMissionDestination] Location found: " + pos, LogLevel.DEBUG);
				break;
			}
			else
			{						
				SDRC_Log.Add("[SDRC_MissionHelper:FindMissionDestination] Invalid mission position. Try " + (i + 1) + "/" + DC_LOCATION_SEACRH_ITERATIONS, LogLevel.SPAM);
			}
		}			
		
		if (!positionFound)
		{
			return "0 0 0";
		}

		return pos;
	}		
			
	//------------------------------------------------------------------------------------------------
	/*!
	Check if given pos is a valid position for mission. 

	\param distanceToMission Distance to another mission. Two missions shall not be too close to each other. -1 will use the default from MissionFrame.
	\param distanceToPlayer Mission shall not spawn too close to a player. -1 will use the default from MissionFrame.
	
	The position shall have ... 
	- no players nearby
	- no missions nearby
	- not in water	
	*/	
	static bool IsValidMissionPos(vector pos, float distanceToMission = -1, float distanceToPlayer = -1)
	{	
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		if (distanceToMission == -1)
			distanceToMission = baseGameMode.missionFrame.m_Config.minDistanceToMission;

		if (distanceToPlayer == -1)
			distanceToPlayer = baseGameMode.missionFrame.m_Config.minDistanceToPlayer;

		if (SDRC_Misc.IsPosInWater(pos))
		{
			SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(DC_EMissionPosFailReason, DC_EMissionPosFailReason.POS_IN_WATER), LogLevel.WARNING);
			return false;
		}
				
		if (SDRC_PlayerHelper.IsAnyPlayerCloseToPos(pos, distanceToPlayer))
		{
			SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(DC_EMissionPosFailReason, DC_EMissionPosFailReason.PLAYER_TOO_CLOSE), LogLevel.WARNING);
			return false;
		}

		if (IsAnyMissionCloseToPos(pos, distanceToMission))
		{
			SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(DC_EMissionPosFailReason, DC_EMissionPosFailReason.MISSION_TOO_CLOSE), LogLevel.WARNING);
			return false;
		}

		if (IsPosInNonValidArea(pos))
		{
			SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(DC_EMissionPosFailReason, DC_EMissionPosFailReason.IN_NON_VALID_AREA), LogLevel.WARNING);
			return false;
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
		
		foreach (int i, SDRC_Mission mission: baseGameMode.missionFrame.m_MissionList)
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
	
		foreach (SDRC_NonValidArea nonValidArea : baseGameMode.missionFrame.m_aNonValidAreas)
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
	DEBUG: Test mission positions on map. Only for debugging.
	This will create a map marker for each position tested.
	*/	
	static void DebugTestMissionPos()
	{	
		if (!SDRC_Conf.RELEASE)
		{
			vector pos;
	
			for (int i = 0; i < 400; i++)
			{		
				pos = SDRC_MissionHelper.FindMissionPos();
				if (pos != "0 0 0")
				{
					SDRC_MapMarkerHelper.CreateMapMarker(pos, DC_EMissionIcon.REDCROSS, "DUMMY_");	//TBD: Create some other debug marker
				}
			}		
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	DEBUG: Delete the test mission positions on map. Only for debugging.
	*/	
	static void DeleteDebugTestMissionPos()
	{	
		if (!SDRC_Conf.RELEASE)
		{
			SDRC_MapMarkerHelper.DeleteMarker("DUMMY_");
		}
	}
					
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn a mission AI group to given position making sure it's empty. 
	NOTE: Position is not exact
	*/
	static SCR_AIGroup SpawnMissionAIGroup(string groupToSpawn, vector pos, string faction)
	{
		SDRC_SpawnHelper.FindEmptyPos(pos, 100, 4);
		
		string groupName = SDRC_EnemyHelper.SelectEnemy(groupToSpawn, faction);
		
		SCR_AIGroup group = SDRC_AIHelper.SpawnGroup(groupName, pos, faction);
		
		return group;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Select mission index randomly from given list.
	Returns -1 in case of an error - for example empty list.
	*/
	static int SelectMissionIndex(array<ref int>confList)	
	{
		int idx = -1;

		if (confList.Count() == 0)
		{
			return -1;
		}
				
		//Pick a configuration for mission
		idx = confList.GetRandomElement();

		SDRC_Log.Add("[SDRC_MissionHelper:SelectMissionIndex] Mission index: " + idx, LogLevel.DEBUG);
				
		return idx;
	}
}