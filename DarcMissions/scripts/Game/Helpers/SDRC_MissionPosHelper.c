//Helpers SDRC_MissionPosHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for position check for valid areas. 
*/

//------------------------------------------------------------------------------------------------
class SDRC_MissionPosHelper
{
	private const int DC_LOCATION_SEARCH_ITERATIONS = 8;		//How many different spots to try for a mission before giving up
	private const int DC_LOCATION_SEARCH_RADIUS_INC = 30;		//The increase of search radius for each failed iteration

	//------------------------------------------------------------------------------------------------
	/*!
	Select a mission position from the list of positions. "0 0 0" returned as default.
	
	\param positions List of positions
	\param size Size the mission needs to spawn
	\param locationTypes Location types needed
	\param posRandomization Randomization for the found position
	\return Location not found if "0 0 0" returned.
	*/	
	static vector SelectMissionPos(array<vector>positions, float size = 1, array<EMapDescriptorType> locationTypes = null, int posRandomization = -1)
	{
		vector pos = "0 0 0";
		
		//If multiple positions, pick a random one from the list.
		if (!positions.IsEmpty())
		{
			pos = positions.GetRandomElement();
		}
		
		//We should not find with a size of 0 or smaller
		if (size <= 0)
		{
			size = 1;
		}

		//If we had an empty positions list, let's search with locationTypes
		if (pos == "0 0 0")
		{			
			if (locationTypes)
			{		
				pos = SDRC_MissionPosHelper.FindMissionPosWithLocationTypes(locationTypes, size, posRandomization);
			}
		}
				
		return pos;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Select a mission position from the list of positions. "0 0 0" returned as default.
	*/	
	static vector SelectMissionPosFromPairs(out vector destination, array<vector>positions, float size = 1, array<EMapDescriptorType> locationTypes = null, int posRandomization = -1)
	{
		//Fill from-to positions for later use
		array<vector> posFrom = {};
		array<vector> posTo = {};
		vector pos = "0 0 0";
		
		if (positions.IsEmpty())
		{
			posFrom.Insert("0 0 0");
			posTo.Insert("0 0 0");
		}
		else
		{
			int count = positions.Count();
			if (count & 1) // odd number
			{
				positions.Insert("0 0 0");
				SDRC_Log.Add("[SelectMissionPosFromPairs] The count of pos shall be even (from-to pairs). Adding one zero position as a fix. Please check your json.", LogLevel.WARNING);
			}
			
			//Copy values
			count = (positions.Count() / 2); //Pairs
			int index = SDRC_Misc.RandomInt(0, count - 1);
			
			posFrom.Insert(positions[index*2 + 0]);
			posTo.Insert(positions[index*2 + 1]);			
		}
		
		if (posFrom[0] == "0 0 0")
		{
			pos = SDRC_MissionPosHelper.SelectMissionPos(posFrom, size, locationTypes, posRandomization);
		}
		else
		{
			pos = posFrom[0];
		}
		
		if (posTo[0] == "0 0 0")
		{
			destination = SDRC_MissionPosHelper.SelectMissionPos(posFrom, size, locationTypes, posRandomization);
		}
		else
		{
			destination = posTo[0];
		}
		
		return pos;
	}	
		
	//------------------------------------------------------------------------------------------------
	/*!
	Find a completely random position for a mission. "0 0 0" returned if nothing found.
	\param distanceToMission Distance to another mission. Two missions shall not be too close to each other. -1 will use the default from MissionFrame.
	\param distanceToPlayer Mission shall not spawn too close to a player. -1 will use the default from MissionFrame.
	\return Location not found if "0 0 0" returned.
	*/	
	static vector FindMissionPosWithDistances(float distanceToMission = -1, float minDistanceToPlayer = -1)
	{	
		vector pos = SDRC_Misc.GetRandomWorldPos();
		
		if (SDRC_MissionPosHelper.IsValidMissionPos(pos, true) != SDRC_EMissionError.NONE)
		{
			pos = "0 0 0";
		}
		
		return pos;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Find a position near pos for a mission. "0 0 0" returned if nothing found.
	\param pos Position to start to search for mission position
	\param size Size (radius) of the mission. This size should be the size of the objects to spawn - like a camp.
	\param posRandomization Position randomization. 
	\return Location not found if "0 0 0" returned.
	*/	
	static vector FindMissionPosNearPos(vector pos, float size, int posRandomization = -1)
	{	
		pos = FindWithIterate(pos, size, posRandomization);
		
		return pos;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Find a position around pos for a mission. "0 0 0" returned if nothing found.
	\param pos Position to start to search for mission position
	\param radius Radius to search around pos.
	*/	
	static vector FindMissionPosAroundPos(vector pos, float radius = 300)
	{	
		//Find spawn position
		float randomAngle = SDRC_Misc.RandomInt(0, 360);
		vector newPos = vector.Zero;
		
		for (int i = 0; i < 12; i++)
		{
			vector testPos = SDRC_Misc.GetCoordinatesOnCircle(pos, radius + (i*30), i*(360/12), randomAngle);
			if (!SDRC_PlayerHelper.IsAnyPlayerCloseToPos(testPos, 200))
			{
				//Check that the suggested position is not in water nor nonValidArea
				if (!SDRC_Misc.IsPosInWater(testPos) && !SDRC_Misc.IsPosUnderMap(testPos) && !SDRC_MissionPosHelper.IsPosInNonValidArea(testPos))
				{
					newPos = testPos;
					break;
				}
			}
		}
		
		return newPos;
	}
			
	//------------------------------------------------------------------------------------------------
	/*!
	Find a location where to spawn a mission. 
	\param locationTypes Array of EMapDescriptorType to look for a place
	\param size Size (radius) of the mission. This size should be the size of the objects to spawn - like a camp.
	\param posRandomization Position randomization to avoid the same spot every time. -1 uses the value set in missionFrame settings.
	\return Location not found if "0 0 0" returned.
	*/	
	static vector FindMissionPosWithLocationTypes(array<EMapDescriptorType> locationTypes, float size, int posRandomization = -1)
	{	
		//Find a random location
		vector pos = "0 0 0";
		
		SDRC_Location location = null;
		array<SDRC_Location> locations = {};
		
		if ( (locationTypes) && (!locationTypes.IsEmpty()) )
		{
			SDRC_Locations.GetLocationsCached(locations, locationTypes);
		}

		//If no locations define, we select a random position
		if (locations.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_MissionHelper:FindMissionLocation] locationTypes is empty. Selecting a random location.", LogLevel.WARNING);
		}

		//Search a few times		
		for (int i = 0; i < DC_LOCATION_SEARCH_ITERATIONS; i++)
		{				
			if (locations.IsEmpty())
			{
				SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
				if (baseGameMode)
				{
					pos = SDRC_MissionPosHelper.FindMissionPosWithDistances(baseGameMode.missionFrame.m_Config.minDistanceToMission, baseGameMode.missionFrame.m_Config.minDistanceToPlayer);
				}
			}
			else
			{	
				location = locations.GetRandomElement();
				pos = location.pos;//GetOrigin();
				
	//			SDRC_Log.Add("[SDRC_MissionHelper:FindMissionPos] Searching near: " + location.GetName() + " " + location.GetOrigin(), LogLevel.DEBUG);			
				SDRC_Log.Add("[SDRC_MissionHelper:FindMissionPos] Searching near: " + location.displayName + " " + location.pos, LogLevel.DEBUG);			
			}
	
			vector newPos = FindWithIterate(pos, size, posRandomization);
			if (newPos != "0 0 0")
			{
				//Found a suitable position. Return.
				pos = newPos;
				break;
			}
		}
		
		return pos;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Find the location close to the position 
	\param pos Position to start to search for mission position
	\param size Size (radius) of the mission. This size should be the size of the objects to spawn - like a camp.
	\param posRandomization TBD
	\return Location not found if "0 0 0" returned.
	*/	
	static vector FindWithIterate(vector pos, float size, int posRandomization = -1)
	{
		bool positionFound = false;		
		vector posOrig = pos;
		int searchRadius = 0;
		
		//We should not find with a size of 0 or smaller
		if (size <= 0)
		{
			size = 1;
		}		
		
		//Define searchRadius
		if (posRandomization == -1)
		{
			//Use the default missionRandomPos as a starting point
			SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
			posRandomization = baseGameMode.missionFrame.m_Config.missionRandomPos;
			searchRadius = baseGameMode.missionFrame.m_Config.missionRandomPos;
		}
				
		//Use the provided posRandomization as a starting point
		searchRadius = posRandomization;
		SDRC_EMissionError failReason = SDRC_EMissionError.LOCATION_NOT_FOUND;
		
		for (int i = 0; i < DC_LOCATION_SEARCH_ITERATIONS; i++)
		{
			//Give the position some randomization so it's not always in the same spot.			
			pos = SDRC_Misc.RandomizePos(posOrig, posRandomization);
			
			//Find the position within posRandomization from pos.			
			if (SDRC_SpawnHelper.FindEmptyPos(pos, posRandomization, size))
			{			
				failReason = SDRC_MissionPosHelper.IsValidMissionPos(pos, true);
				if (failReason == SDRC_EMissionError.NONE)
				{				
					positionFound = true;
					SDRC_Log.Add("[SDRC_MissionHelper:FindWithIterate] Position found: " + pos, LogLevel.DEBUG);
					break;
				}
			}
			else
			{	
				searchRadius = searchRadius + DC_LOCATION_SEARCH_RADIUS_INC;	//Increase the are with DC_LOCATION_SEARCH_RADIUS_INC
				SDRC_Log.Add("[SDRC_MissionHelper:FindWithIterate] Invalid position. Try " + (i + 1) + "/" + DC_LOCATION_SEARCH_ITERATIONS, LogLevel.SPAM);
			}
		}

		if (!positionFound)
		{
			SDRC_Log.Add("[SDRC_MissionHelper:FindWithIterate] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, failReason), LogLevel.SPAM);
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
		
		for (int i = 0; i < DC_LOCATION_SEARCH_ITERATIONS; i++)
		{					
			pos = SDRC_MissionPosHelper.FindMissionPosWithLocationTypes(locationTypes, 1);
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
				SDRC_Log.Add("[SDRC_MissionHelper:FindMissionDestination] Invalid mission position. Try " + (i + 1) + "/" + DC_LOCATION_SEARCH_ITERATIONS, LogLevel.SPAM);
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
	\param onlyBasicChecks Check only basic things
	
	Checks are done for positions:
	- Basic checks for all:
		- not in water	
		- not under map
	- Test for Static missions + Dynamic missions
		- not in a nonValidArea
		- no players nearby
	- Full checks for Dynamic missions
		- no players far away
		- no missions nearby
	*/	
	static SDRC_EMissionError IsValidMissionPos(vector pos, bool onlyBasicChecks = false, bool isRequested = false, float distanceToMission = -1, float distanceToPlayer = -1)
	{	
		//SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] OnlyBasicChecks: " +  onlyBasicChecks, LogLevel.DEBUG);
		
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		// --- Basic checks ---
		//These are for all missions
		if (SDRC_Misc.IsPosInWater(pos))
		{
			SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.POSITION_IN_WATER), LogLevel.SPAM);
			return SDRC_EMissionError.POSITION_IN_WATER;
		}
				
		if (SDRC_Misc.IsPosUnderMap(pos))
		{
			SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.POSITION_UNDER_MAP), LogLevel.SPAM);
			return SDRC_EMissionError.POSITION_UNDER_MAP;
		}

		//Only position in water and under map is tested for Requested missions
		if (isRequested)
		{
			//No more tests for requested missions
			return SDRC_EMissionError.NONE;
		}
		
		//Tests to be included for Static missions (+all)
		if (IsPosInNonValidArea(pos))
		{
			SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.IN_NON_VALID_AREA), LogLevel.SPAM);
			return SDRC_EMissionError.IN_NON_VALID_AREA;
		}

		if (SDRC_PlayerHelper.PlayerCount() > 0)
		{
			//Check that players are not too close
			if (SDRC_PlayerHelper.IsAnyPlayerCloseToPos(pos, distanceToPlayer))
			{
				SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.PLAYER_TOO_CLOSE), LogLevel.SPAM);
				return SDRC_EMissionError.PLAYER_TOO_CLOSE;
			}
		}		
				
		//If we only check for basic things, return		
		if (onlyBasicChecks)
		{
			//No more tests for Requested and Static missions 
			return SDRC_EMissionError.NONE;
		}
										
		// --- Full checks ---
		if (distanceToMission == -1)
			distanceToMission = baseGameMode.missionFrame.m_Config.minDistanceToMission;

		if (distanceToPlayer == -1)
			distanceToPlayer = baseGameMode.missionFrame.m_Config.minDistanceToPlayer;
		
		//If no players in game, ignore this
		if (SDRC_PlayerHelper.PlayerCount() > 0)
		{
			//Check that players are not too close
			if (SDRC_PlayerHelper.IsAnyPlayerCloseToPos(pos, distanceToPlayer))
			{
				SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.PLAYER_TOO_CLOSE), LogLevel.SPAM);
				return SDRC_EMissionError.PLAYER_TOO_CLOSE;
			}

			//Check that players are not too far. 
			//This check is not done for static missions.
			if (baseGameMode.missionFrame.m_Config.maxDistanceToPlayer > -1)
			{
				if (!SDRC_PlayerHelper.IsAnyPlayerCloseToPos(pos, baseGameMode.missionFrame.m_Config.maxDistanceToPlayer, distanceToPlayer))
				{
					SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.PLAYER_TOO_FAR), LogLevel.SPAM);
					return SDRC_EMissionError.PLAYER_TOO_FAR;
				}
			}
		}
		
		if (IsAnyMissionCloseToPos(pos, distanceToMission))
		{
			SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.MISSION_TOO_CLOSE), LogLevel.SPAM);
			return SDRC_EMissionError.MISSION_TOO_CLOSE;
		}

		return SDRC_EMissionError.NONE;
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
	
		foreach (SDRC_NonValidArea nonValidArea : baseGameMode.missionFrame.m_ConfigNonValidArea.m_NonValidAreas)
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
}