//Helpers SDRC_MissionHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for missions. 
*/

//------------------------------------------------------------------------------------------------
class SDRC_MissionHelper
{
	private const int DC_LOCATION_SEACRH_ITERATIONS = 5;		//How many different spots to try for a mission before giving up
	private const int DC_LOCATION_SEACRH_RADIUS_INC = 30;		//The increase of search radius for each failed iteration

	//------------------------------------------------------------------------------------------------
	/*!
	Select a mission position from the list of positions. "0 0 0" returned as default.
	\param positions List of positions
	*/	
	static vector SelectMissionPos(array<vector>positions, float size = 1, array<EMapDescriptorType> locationTypes = null, int posRandomization = -1)
	{
		vector pos = "0 0 0";
		
		//If multiple positions, pick a random one		
		if (!positions.IsEmpty())
		{
			pos = positions.GetRandomElement();
		}
		
		//We should not find with a size of 0 or smaller
		if (size <= 0)
		{
			size = 1;
		}
		
		if (pos == "0 0 0")
		{					
			pos = SDRC_MissionHelper.FindMissionPos(locationTypes, size, posRandomization);
		}
				
		return pos;
	}

		
	//------------------------------------------------------------------------------------------------
	/*!
	Find a completely random position for a mission. "0 0 0" returned if nothing found.
	\param distanceToMission Distance to another mission. Two missions shall not be too close to each other. -1 will use the default from MissionFrame.
	\param distanceToPlayer Mission shall not spawn too close to a player. -1 will use the default from MissionFrame.
	*/	
	static vector FindMissionPos(float distanceToMission = -1, float distanceToPlayer = -1)
	{	
		vector pos = SDRC_Misc.GetRandomWorldPos();
		
		if (SDRC_MissionPosHelper.IsValidMissionPos(pos, distanceToMission, distanceToPlayer) != SDRC_EMissionError.NONE)
		{
			return "0 0 0";
		}
		
		return pos;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Find a position near pos for a mission. "0 0 0" returned if nothing found.
	\param pos Position to start to search for mission position
	\param size Size (radius) of the mission. This size should be the size of the objects to spawn - like a camp.
	\param posRandomization Position randomization. 
	*/	
	static vector FindMissionPos(vector pos, float size, int posRandomization = -1)
	{	
		pos = FindWithIterate(pos, size, posRandomization);
		
		return pos;
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Find a location where to spawn a mission. 
	\param locationTypes Array of EMapDescriptorType to look for a place
	\param size Size (radius) of the mission. This size should be the size of the objects to spawn - like a camp.
	\param posRandomization Position randomization to avoid the same spot every time. -1 uses the value set in missionFrame settings.
	*/	
	static vector FindMissionPos(array<EMapDescriptorType> locationTypes, float size, int posRandomization = -1)
	{	
		//Find a random location
		vector pos = "0 0 0";
		
		SDRC_Location location = null;
		array<SDRC_Location> locations = {};
		
		if ( (locationTypes) && (!locationTypes.IsEmpty()) )
		{
			SDRC_Locations.GetLocationsCached(locations, locationTypes);
		}
		
		if (locations.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_MissionHelper:FindMissionLocation] locationTypes is empty. Selecting a random location.", LogLevel.WARNING);
			SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
			if (baseGameMode)
			{
				pos = SDRC_MissionHelper.FindMissionPos(baseGameMode.missionFrame.m_Config.minDistanceToMission, baseGameMode.missionFrame.m_Config.minDistanceToPlayer);
			}
		}
		else
		{		
			location = locations.GetRandomElement();
			pos = location.pos;//GetOrigin();
			
//			SDRC_Log.Add("[SDRC_MissionHelper:FindMissionPos] Searching near: " + location.GetName() + " " + location.GetOrigin(), LogLevel.DEBUG);			
			SDRC_Log.Add("[SDRC_MissionHelper:FindMissionPos] Searching near: " + location.displayName + " " + location.pos, LogLevel.DEBUG);			
		}

		pos = FindWithIterate(pos, size, posRandomization);
		
		return pos;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Find the location close to the position 
	\param pos Position to start to search for mission position
	\param size Size (radius) of the mission. This size should be the size of the objects to spawn - like a camp.
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
		
		for (int i = 0; i < DC_LOCATION_SEACRH_ITERATIONS; i++)
		{
			//Give the position some randomization so it's not always in the same spot.			
			pos = SDRC_Misc.RandomizePos(posOrig, posRandomization);
			
			//Find the position within posRandomization from pos.			
			if (SDRC_SpawnHelper.FindEmptyPos(pos, posRandomization, size))
			{			
				failReason = SDRC_MissionPosHelper.IsValidMissionPos(pos);
				if (failReason == SDRC_EMissionError.NONE)
				{				
					positionFound = true;
					SDRC_Log.Add("[SDRC_MissionHelper:FindWithIterate] Position found: " + pos, LogLevel.DEBUG);
					break;
				}
			}
			else
			{	
				searchRadius = searchRadius + DC_LOCATION_SEACRH_RADIUS_INC;	//Increase the are with DC_LOCATION_SEACRH_RADIUS_INC
				SDRC_Log.Add("[SDRC_MissionHelper:FindWithIterate] Invalid position. Try " + (i + 1) + "/" + DC_LOCATION_SEACRH_ITERATIONS, LogLevel.SPAM);
			}
		}

		if (!positionFound)
		{
			SDRC_Log.Add("[SDRC_MissionHelper:FindWithIterate] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, failReason), LogLevel.WARNING);
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
			pos = SDRC_MissionHelper.FindMissionPos(locationTypes, 1);
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
	Select a mission building
	*/	
	static IEntity FindMissionBuilding(vector pos, array<string>buildingFilter, float radius)
	{
		array<IEntity>buildings = {};
		SDRC_BuildingHelper.FindBuildings(buildings, buildingFilter, pos, radius);

		IEntity building = null;
		
		if (buildings.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_MissionHelper:FindMissionBuilding] Could not find suitable building near " + SDRC_Locations.CreateName(pos, "any") + " " + pos, LogLevel.ERROR);
			return null;
		}
		
		float distance = 1000;
		int idx = 0;
		
		//Find the closest building to position
		foreach (int i, IEntity bld : buildings)
		{
			float tempDistance = vector.DistanceXZ(bld.GetOrigin(), pos);
			if (tempDistance < distance)
			{
				distance = tempDistance;
				idx = i;				
			}
		}

		building = buildings[idx];
		/* OLD 
		building = buildings.GetRandomElement();
		*/

		vector bpos = building.GetOrigin();
				
/*		if (SDRC_MissionPosHelper.IsValidMissionPos(bpos) != SDRC_EMissionError.NONE)
		{
			return null;
		}
		else
		{
			SDRC_Log.Add("[SDRC_MissionHelper:FindMissionBuilding] Building selected: " + building.GetPrefabData().GetPrefabName() + " " + bpos, LogLevel.DEBUG);
		}
*/		
		SDRC_Log.Add("[SDRC_MissionHelper:FindMissionBuilding] Building selected: " + building.GetPrefabData().GetPrefabName() + " " + bpos, LogLevel.DEBUG);		
		
		return building;
	}		

	//------------------------------------------------------------------------------------------------
	/*!
	Spawn item in building (with loot)
	\param building Target building
	\param lootBox Container for loot
	\param addLoot If true, add loot
	\param loot list of items to spawn
	\param looChance chance for each item to appear in the lootBox
	*/	
	static IEntity SpawnItemInBuildingWithLoot(IEntity building, string lootBox, bool addLoot = false, array<string> loot = null, float lootChance = 0, SDRC_EDifficulty difficulty = SDRC_EDifficulty.IGNORE)
	{
		float rotation = SDRC_Misc.RandomFloat(0, 360);
		IEntity entity = SDRC_SpawnHelper.SpawnItemInBuilding(building, lootBox, rotation, 2.0, false);
		if (addLoot)
		{
			AddLoot(entity, loot, lootChance, difficulty);
		}
		
		return entity;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Add loot to mission storage. Handles difficulty.
	*/
	static void AddLoot(IEntity storage, array<string> itemNames, float itemChance = 1.0, SDRC_EDifficulty difficulty = SDRC_EDifficulty.IGNORE)
	{	
		//Handle difficulty options
		if ( (difficulty >= SDRC_EDifficulty.EASY) && (difficulty <= SDRC_EDifficulty.HARD) )
		{
			SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
			if (baseGameMode)
			{
				float lootChanceCoef = baseGameMode.missionFrame.m_Config.missionDifficulty.lootChanceCoef[difficulty];
				float lootCountCoef = baseGameMode.missionFrame.m_Config.missionDifficulty.lootCountCoef[difficulty];
				
				itemChance = Math.Clamp(itemChance * lootChanceCoef, 0, 1.0);
				
				int itemCount = Math.Round(itemNames.Count() * lootCountCoef);
				itemCount = Math.ClampInt(itemCount, 1, 10000);
				
				//Spawn always one item so that players know the mission loot worked
				if (itemCount == 1)
				{
					itemChance = 1.0;
				}
				
				//If more items are expected, add them to the list
				int diff = itemCount - itemNames.Count();
				if (diff > 0)
				{
					for (int i = 0; i < diff; i++)
					{
						itemNames.Insert(itemNames.GetRandomElement());
					}
				}
			}						
		}
		
		SDRC_LootHelper.SpawnItemsToStorage(storage, itemNames, itemChance);
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
	Spawn a randomly selected mission AI group to given position making sure it's empty. 
	NOTE: Position is not exact
	*/
	static SCR_AIGroup SpawnMissionAIGroupRandom(array<string> groups, vector pos, string faction)
	{
		if (groups.IsEmpty())
		{
			return null;
		}
		
		string groupToSpawn = groups.GetRandomElement();
		SCR_AIGroup group = SpawnMissionAIGroup(groupToSpawn, pos, faction);
		return group;
	}	
		
	//------------------------------------------------------------------------------------------------
	/*!
	Counts the amount of missions for the map
	\param count
	\param mul
	*/	
	static int GetMissionCountForWorld(int count, float mul)	
	{
		if (count == -1)
		{			
			count = (SDRC_Misc.GetWorldSize() * mul) / 1000;
			SDRC_Log.Add("[SDRC_MissionHelper:GetMissionCountForWorld] Count = (Worldsize) " + SDRC_Misc.GetWorldSize() + " * " + mul + " / 1000 = " + count, LogLevel.DEBUG);			
		}
		
		return count;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Select mission index randomly from given list.
	Returns -1 in case of an error - for example empty list.
	*/
	static int SelectMissionIndex(array<ref int>confList, int missionSubIdx)	
	{
		int idx = -1;

		if (confList.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_MissionHelper:SelectMissionIndex] Mission list is empty.", LogLevel.ERROR);
			return -1;
		}
		
		if (missionSubIdx == -1)
		{
			//Pick a configuration for mission
			idx = confList.GetRandomElement();
		}
		else
		{
			//TBD: This needs to search through suid
			
			//TBD: This probably should be moved to SDRC_Mission. We don't have access to subIdx's that are available.
			if (missionSubIdx > -1 && missionSubIdx < confList.Count())
			{
				idx = missionSubIdx;
/*				foreach (int i, int conf : confList)
				{
					if (conf.subIdx == missionSubIdx)
					{
						idx = i;
						break;
					}
				}*/
			}
			else
			{
				SDRC_Log.Add("[SDRC_MissionHelper:SelectMissionIndex] Incorrect mission index: " + missionSubIdx, LogLevel.ERROR);
				return -1;
			}			
		}
		
		SDRC_Log.Add("[SDRC_MissionHelper:SelectMissionIndex] Mission index: " + idx, LogLevel.DEBUG);
				
		return idx;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Create info to show to players.
	*/	
	static string CreateInfo(string msg, string location = "", string destination = "", SDRC_EDifficulty difficulty = SDRC_EDifficulty.NORMAL)
	{
		msg = SCR_StringHelper.ReplaceMultiple(msg, {"%l"}, location);
		msg = SCR_StringHelper.ReplaceMultiple(msg, {"%d"}, destination);
		msg = SCR_StringHelper.ReplaceMultiple(msg, {"%x"}, SCR_Enum.GetEnumName(SDRC_EDifficulty, difficulty));
		
		SDRC_Log.Add("[SDRC_MissionHelper:CreateInfo] Message created: " + msg, LogLevel.SPAM);
		return msg;
	}	

	
	//------------------------------------------------------------------------------------------------
	/*! 
	Recalculate loot spawn chance according to difficulty
	*/	
/*	static float GetLootChance(float chance, SDRC_EDifficulty difficulty = SDRC_EDifficulty.NORMAL)
	{
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (m_BaseGameMode)
		{
			float coef = m_BaseGameMode.missionFrame.m_Config.missionDifficulty.lootChanceCoef[difficulty];
			chance = chance * coef;
		}		
		return chance;		
	}	*/	

	//------------------------------------------------------------------------------------------------
	/*!
	Select the mission type and respect the mission limits
	\param dynamic To request for a dynamic or static mission
	*/	
	static SDRC_EMissionType SelectMissionType(bool dynamic = true)
	{
		SDRC_EMissionType missionType = SDRC_EMissionType.NONE;
		int cnt = 0;
		
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (m_BaseGameMode)
		{
			for (int i = 0; i < 10; i++)
			{	
				SDRC_EMissionType missionTypeSelected;
				
				if (dynamic)
				{
					missionTypeSelected = m_BaseGameMode.missionFrame.m_Config.missionDynamic.missionTypeArray.GetRandomElement();
				}
				else
				{
					missionTypeSelected = m_BaseGameMode.missionFrame.m_Config.missionStatic.missionTypeArray.GetRandomElement();
				}
				
				cnt = CountMissionsOfType(missionTypeSelected);
				
				//If no limit set OR if count is less than limit set, accept the missionType
				if ( (m_BaseGameMode.missionFrame.m_Config.missionLimit[missionTypeSelected] == -1) ||
				     (cnt < m_BaseGameMode.missionFrame.m_Config.missionLimit[missionTypeSelected]) )
				{
					missionType = missionTypeSelected;
					break;
				}
				
				SDRC_Log.Add("[SDRC_MissionHelper:SelectMissionType] Trying again.. ", LogLevel.SPAM);
			}
		}
		
		if (missionType == SDRC_EMissionType.NONE)
		{ 
			SDRC_Log.Add("[SDRC_MissionHelper:SelectMissionType] Missiontype limit reached. Trying again later. Type NONE selected.", LogLevel.WARNING);
		}
		else
		{ 
			SDRC_Log.Add("[SDRC_MissionHelper:SelectMissionType] There are now " + (cnt + 1) + " missions of type " + SCR_Enum.GetEnumName(SDRC_EMissionType, missionType), LogLevel.DEBUG);
		}
		
		return missionType;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Return the amount of missions of given type
	\param missionType The mission type to count
	*/	
	static int CountMissionsOfType(SDRC_EMissionType missionType)
	{
		int cnt = 0;
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (m_BaseGameMode)
		{
			foreach(SDRC_Mission mission : m_BaseGameMode.missionFrame.m_MissionList)
			{
				if (mission.GetType() == missionType)
				{
					cnt++;
				}
			}
			
				
		}
		return cnt;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	DEBUG: Test mission positions on map. Only for debugging.
	This will create a map marker for each position tested.
	*/	
	static void DebugTestMissionPos()
	{	
		#ifndef SDRC_RELEASE
			vector pos;
	
			for (int i = 0; i < 300; i++)
			{		
				#ifndef SDRC_RELEASE
					//SDRC_HelloHelper.Hello();
				#endif
			
				pos = FindMissionPos();
			
				if (pos != "0 0 0")
				{
					SDRC_MapMarkerHelper.CreateMapMarker(pos, SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, "DUMMY_");
				}
			}		
		#endif
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
}