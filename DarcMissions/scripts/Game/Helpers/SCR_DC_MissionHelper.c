//Helpers SCR_DC_MissionHelper.c

sealed class SCR_DC_MissionHelper
{
	private const int DC_LOCATION_SEACRH_ITERATIONS = 5;	//How many different spots to try for a mission before giving up
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find a random position for a mission. "0 0 0" returned if nothing found.
	\param distanceToMission Distance to another mission. Two missions shall not be too close to each other. -1 will use the default from MissionFrame.
	\param distanceToPlayer Mission shall not spawn too close to a player. -1 will use the default from MissionFrame.
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
	\param locationTypes Array of EMapDescriptorType to look for a place
	\param size Size (radius) of the mission. This size should be the size of the objects to spawn - like a camp.
	*/	
	static IEntity FindMissionLocation(array<EMapDescriptorType> locationTypes, float size = 10)
	{	
		//Find a random location
		vector pos = "0 0 0";
		bool positionFound = false;
		
		IEntity location = null;
		array<IEntity> locations = {};
		SCR_DC_Locations.GetLocations(locations, locationTypes);		
		
		if(locations.Count() == 0)
		{
			SCR_DC_Log.Add("[SCR_DC_MissionHelper:FindMissionLocation] No locations found. Check the list of locationTypes in your mission." , LogLevel.ERROR);
			return null;
		}
		
		for (int i = 0; i < DC_LOCATION_SEACRH_ITERATIONS; i++)
		{
			location = locations.GetRandomElement();
			pos = location.GetOrigin();
			pos = SCR_DC_SpawnHelper.FindEmptyPos(pos, 100, size);	//Find the position within 100m from pos
			
			if (SCR_DC_MissionHelper.IsValidMissionPos(pos))
			{				
				positionFound = true;
				SCR_DC_Log.Add("[SCR_DC_MissionHelper:FindMissionLocation] Location found: " + location.GetName() + " " + location.GetOrigin(), LogLevel.DEBUG);
				break;
			}
			else
			{						
				SCR_DC_Log.Add("[SCR_DC_MissionHelper:FindMissionLocation] Invalid mission position. Try " + (i + 1) + "/" + DC_LOCATION_SEACRH_ITERATIONS, LogLevel.SPAM);
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
	Find destination location where the mission ends.
	This is usable for missions where something is traveling to a destination.
	\param locationTypes Array of EMapDescriptorType to look for a place
	\param missionPos The mission position aka the starting point for mission
	\param distance Minimum distance for missionPos and destination
	*/		
	static IEntity FindMissionDestination(array<EMapDescriptorType> locationTypes, vector missionPos, int distance)
	{
		IEntity location = null;
		array<IEntity> locations = {};

		bool positionFound = false;
		
		for (int i = 0; i < DC_LOCATION_SEACRH_ITERATIONS; i++)
		{					
			location = SCR_DC_MissionHelper.FindMissionLocation(locationTypes);
			if(!SCR_DC_Misc.IsPosNearPos(location.GetOrigin(), missionPos, distance))	//Shall be 300m from actual missionPos
			{
				positionFound = true;
				SCR_DC_Log.Add("[SCR_DC_MissionHelper:FindMissionLocation] Location found: " + location.GetName() + " " + location.GetOrigin(), LogLevel.DEBUG);
				break;
			}
			else
			{						
				SCR_DC_Log.Add("[SCR_DC_MissionHelper:FindMissionDestination] Invalid mission position. Try " + (i + 1) + "/" + DC_LOCATION_SEACRH_ITERATIONS, LogLevel.SPAM);
			}
		}			
		
		if (!positionFound)
		{
			return null;
		}

		return location;
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

		if (SCR_DC_Misc.IsPosInWater(pos))
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
	DEBUG: Test mission positions on map. Only for debugging.
	*/	
	static void DebugTestMissionPos()
	{	
		if (!SCR_DC_Core.RELEASE)
		{
			vector pos;
	
			for (int i = 0; i < 400; i++)
			{		
				pos = SCR_DC_MissionHelper.FindMissionPos();
				if (pos != "0 0 0")
				{
					SCR_DC_MapMarkerHelper.CreateMapMarker(pos, DC_EMissionIcon.DEBUG_SMALL, "DUMMY_");
				}
			}		
		}
	}

	static void DeleteDebugTestMissionPos()
	{	
		if (!SCR_DC_Core.RELEASE)
		{
			SCR_DC_MapMarkerHelper.DeleteMarker("DUMMY_");
		}
	}
					
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn a mission AI group to given position making sure it's empty. 
	NOTE: Position is not exact
	*/
	static SCR_AIGroup SpawnMissionAIGroup(string groupToSpawn, vector pos)
	{
		vector posFixed = SCR_DC_SpawnHelper.FindEmptyPos(pos, 100, 8);
		
		SCR_AIGroup group = SCR_DC_AIHelper.SpawnGroup(groupToSpawn, posFixed);
		
		return group;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Select mission index 
	- An empty list is an error -> return -1;
	*/
	static int SelectMissionIndex(array<ref int>confList)	
	{
		int idx = -1;

		if(confList.Count() == 0)
		{
			return -1;
		}
				
		//Pick a configuration for mission
		idx = confList.GetRandomElement();

		SCR_DC_Log.Add("[SCR_DC_MissionHelper:SelectMissionIndex] Mission index: " + idx, LogLevel.DEBUG);
				
		return idx;
	}
}