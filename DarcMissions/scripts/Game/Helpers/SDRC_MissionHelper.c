//Helpers SDRC_MissionHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for missions. 
*/

//------------------------------------------------------------------------------------------------
class SDRC_MissionHelper
{
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
			SDRC_Log.Add("[SDRC_MissionHelper:FindMissionBuilding] Could not find suitable building near " + SDRC_Locations.CreateName(pos, "any") + " " + pos, LogLevel.DEBUG);
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
		vector bpos = building.GetOrigin();
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
	Spawn mission AI from class SDRC_MissionConfigAI
	*/	
	static int SpawnAIFromClassAI(SDRC_MissionConfigAi ai, SDRC_Mission mission, vector spawnPos, vector destinationPos)
	{
		int groupCount = 0;
		int aiCount = ai.GetCount(mission.GetDifficulty());

		for (int i = 0; i < aiCount; i++)
		{
			SCR_AIGroup group = SDRC_MissionHelper.SpawnMissionAIGroupRandom(ai.types, spawnPos, mission.GetFaction());
			if (group)
			{
				groupCount++;
				SDRC_AIHelper.SetAIGroupSettings(group, ai.GetSkill(mission.GetDifficulty()), ai.GetPerception(mission.GetDifficulty()));					
				mission.AddToGroupsList(group);
				
				//Set waypoint 
				SDRC_WPHelper.CreateMissionAIWaypoints(group, ai.waypointGenType, spawnPos, destinationPos, ai.waypointMoveType);
			}
		}
		
		return groupCount;
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn mission AI from class SDRC_MissionConfigAi
	*/	
	static int SpawnAIFromClassAIInVehicle(ResourceName vehicleName, SDRC_MissionConfigAi ai, SDRC_Mission mission, vector spawnPos, vector destinationPos)
	{
		//Spawn vehicle
		if (vehicleName[0] != "{")
		{
			vehicleName = SDRC_VehicleListHelper.FindVehicleItem(vehicleName, mission.GetFaction());
		}				
		
		ref IEntity vehicle = null;
		
		bool isChopper = false;
		if (SDRC_Resources.HasResourceTrait(vehicleName, EEditableEntityLabel.ENTITYTYPE_DARCCHOPPER))
		{
			//Chopper spawn
			isChopper = true;
			spawnPos[1] = SDRC_Misc.GetSurfaceYWithWater(spawnPos) + 30;
			vehicle = SDRC_SpawnHelper.SpawnItem(spawnPos, vehicleName, emptyPosRadius: -1, snap: false);
		}
		else
		{
			//Vehicle spawn
			vehicle = SDRC_SpawnHelper.SpawnItem(spawnPos, vehicleName);
		}
		
		//bool success = false;

		if (!vehicle)
		{			
			return -1;			
		}

		SDRC_Log.Add("[SDRC_MissionHelper:SpawnAiFromClassAiInVehicle] " +  mission.GetId() + " : Vehicle spawned: " + vehicle, LogLevel.DEBUG);										
		
		mission.AddToEntityList(vehicle);
		
		//Disable arsenal
		SDRC_VehicleHelper.EmptyStorage(vehicle);
		SDRC_VehicleHelper.DisableVehicleArsenal(vehicle, vehicleName, true);
		
		int groupCount = 0;
		
		if (!isChopper)
		{
			AICarMovementComponent vehicle_c = AICarMovementComponent.Cast(vehicle.FindComponent(AICarMovementComponent));
	        vehicle_c.SetCruiseSpeed(30);
			
			//Spawn mission AI
			groupCount = 0;
			int aiCount = ai.GetCount(mission.GetDifficulty());
			
			for (int i = 0; i < aiCount; i++)
			{		
				string groupToSpawn = ai.types.GetRandomElement();
				ResourceName aiType = SDRC_EnemyHelper.SelectEnemy(groupToSpawn, mission.GetFaction());
				
				SCR_AIGroup group = SDRC_AIHelper.GroupCreate(mission.GetFaction(), mission.GetPos());
				SDRC_VehicleHelper.SpawnGroupInVehicle(aiType, vehicle, group, mission.GetFaction());
				
				if (group)
				{			
					groupCount++;
					SDRC_AIHelper.SetAIGroupSettings(group, ai.GetSkill(mission.GetDifficulty()), ai.GetPerception(mission.GetDifficulty()));
					mission.AddToGroupsList(group);
					
					//Set waypoint 
					SDRC_WPHelper.CreateMissionAIWaypoints(group, ai.waypointGenType, spawnPos, destinationPos, ai.waypointMoveType);				
				}
			}
		}
		else
		{
			SDRC_ChopperComp vehicle_c = SDRC_ChopperComp.Cast(vehicle.FindComponent(SDRC_ChopperComp));
			
			if (!vehicle_c)
			{			
				delete vehicle;
				return false;			
			}
			
			vehicle_c.SetAutostart(false);
			vehicle_c.SetEnemySearchType(SDRC_EHeliEnemySearchType.PLAYER);
			vehicle_c.Setup(vehicle);
	
			//Spawn mission AI
			groupCount = 0;		
			int aiCount = ai.GetCount(mission.GetDifficulty());
			
			for (int i = 0; i < aiCount; i++)
			{		
				string groupToSpawn = ai.types.GetRandomElement();
				ResourceName aiType = SDRC_EnemyHelper.SelectEnemy(groupToSpawn, mission.GetFaction());
				
				SCR_AIGroup group = SDRC_AIHelper.GroupCreate(mission.GetFaction(), mission.GetPos());
				SDRC_VehicleHelper.SpawnGroupInVehicle(aiType, vehicle, group, mission.GetFaction());
				
				if (group)
				{			
					groupCount++;
					SDRC_AIHelper.SetAIGroupSettings(group, ai.GetSkill(mission.GetDifficulty()), ai.GetPerception(mission.GetDifficulty()));
					mission.AddToGroupsList(group);
				}
			}
			
			//Add our flight path
			int time = SDRC_Misc.RandomInt(300, mission.GetActiveTime() - 120);		//QRF chopper will fly away 2 mins before mission end
			SDRC_Log.Add("[SDRC_MissionHelper:SpawnAiFromClassAiInVehicle] Chopper will fly away after " + time + " seconds.", LogLevel.DEBUG);
			vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_SEARCH_DESTROY, destinationPos, time);
			vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY_AWAY);
			vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_DESPAWN);			
			
			vehicle_c.Ready(vehicle);
		}
		
		return groupCount;
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
			SDRC_Log.Add("[SDRC_MissionHelper:SelectMissionType] There are now " + (cnt + 1) + " missions of type " + SCR_Enum.GetEnumName(SDRC_EMissionType, missionType), LogLevel.SPAM);
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
			foreach (SDRC_Mission mission : m_BaseGameMode.missionFrame.m_MissionList)
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
			
				pos = SDRC_MissionPosHelper.FindMissionPosWithDistances();
			
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