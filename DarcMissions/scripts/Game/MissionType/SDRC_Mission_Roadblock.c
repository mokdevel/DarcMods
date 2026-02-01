//Mission SDRC_Mission_Roadblock.c

//------------------------------------------------------------------------------------------------
/*!

*/

const string DC_MISSIONCONFIG_FILE_ROADBLOCK = "dc_missionConfig_Roadblock.json";
const int DC_MISSIONCONFIG_FILE_ROADBLOCK_JSONVER = 2;

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Roadblock : SDRC_Mission
{
	private ref SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_ROADBLOCK);	
	private ref SDRC_RoadblockConfig m_Config = new SDRC_RoadblockConfig();	
	private ref SDRC_Camp m_DC_Roadblock = new SDRC_Camp;
	
	private int m_iSpawnIndex = 0;						//Counter for the item to spawn
	private float m_fSpawnRotation = 0;					//Rotation of the camp for random locations.
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Roadblock(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Load config
		if (!m_JsonApi.Load(m_Config, SDRC_MissionConfig.Cast(m_Config), DC_MISSIONCONFIG_FILE_ROADBLOCK_JSONVER))
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.ERROR_LOADING_JSON);
			return;
		}
		m_Config.LoadMissionFiles(DC_MISSIONCONFIG_FILE_ROADBLOCK_JSONVER);
		
		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		int idx = m_Config.GetSubMissionIdx(GetSubIdx());
		if (idx == -1)
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.WRONG_SUBIDX);
			return;
		}
		m_DC_Roadblock = m_Config.subMissions[idx];
		HandleRequestGeneralVariables(m_DC_Roadblock.general, request);
		SetQrfConf(m_DC_Roadblock.qrf);
		
		//Find a location for the mission
		vector pos = "0 0 0";
		
		//For requested missions we want have it as close as possible in the requested place.
		if (IsRequested())
		{
			pos = request.general.pos[0];
		}
		else
		{				
			pos = SDRC_MissionHelper.SelectMissionPos(m_DC_Roadblock.general.pos, m_DC_Roadblock.general.size, m_DC_Roadblock.general.locationTypes, SDRC_Conf.POSITION_RANDOMIZATION);
		}
		
		//If pos has been set, we blindly accept it. Do basic checking for pos.
		if (SDRC_MissionPosHelper.IsValidMissionPos(pos, onlyBasicChecks: IsRequested()) != SDRC_EMissionError.NONE)
		{
			pos = "0 0 0";
		}
		
		//If we found a position, let's search more closely
		if (pos != "0 0 0")
		{		
			if (!IsRequested())
			{
				//Add randomization so that it's not always in the same place
				pos = SDRC_Misc.RandomizePos(pos, 150);
			}
				
			//Find nearest road
			SDRC_RoadPos roadPos = new SDRC_RoadPos();				
			vector posOnRoad = SDRC_RoadHelper.FindClosestRoadposToPos(roadPos, pos);
			
			if (roadPos.roadPts.Count() < 2)	//We need two points for a road. Having only one point would be a bug on the map.
			{
				pos = "0 0 0";
				SDRC_Log.Add("[SDRC_Mission_Roadblock] " +  GetId() + " : No roadpoints found.", LogLevel.ERROR);
			}
			else
			{
				int roadPointIndex = 0;
				
				if (!IsRequested())
				{
					roadPointIndex = SDRC_Misc.RandomInt(0, roadPos.roadPts.Count() - 2);
				}
				else			
				{
					roadPointIndex = roadPos.posOnRoadIndex;
				}
	
				//If the closest point is the last point, go one point backwards.
				if (roadPointIndex >= roadPos.roadPts.Count() - 1)
				{
					roadPointIndex--;
					SDRC_Log.Add("[SDRC_Mission_Roadblock] " +  GetId() + " : Roadpoint index reduced", LogLevel.SPAM);
				}
				
				//SDRC_Log.Add("[SDRC_Mission_Roadblock] " +  GetId() + " : Roadpoints found: " + roadPos.roadPts.Count() + " idx: " + roadPointIndex, LogLevel.SPAM);
				
				pos = roadPos.roadPts[roadPointIndex];
				posOnRoad = roadPos.roadPts[roadPointIndex + 1];
				
				SDRC_DebugHelper.AddDebugPos(pos, ARGB(40, 192, 192, 192), 2, "NONE", 20);			//Gray
				SDRC_DebugHelper.AddDebugPos(posOnRoad, ARGB(40, 128, 128, 128), 2, "NONE", 20);	//Gray
								
				//Find the road direction. Roadblocks shall be aligned to road. 
				vector direction = vector.Direction(pos, posOnRoad);
				m_fSpawnRotation = SDRC_Math.VectorToAngle(direction);
			}
		}
		
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.LOCATION_NOT_FOUND);
			return;
		}	
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(GetPos(), m_DC_Roadblock.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_DC_Roadblock.general);		

		SDRC_SpawnHelper.SetStructuresToOrigo(m_DC_Roadblock.campItems);
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == SDRC_EMissionState.SPAWN)
		{
			MissionSpawn();
			GetGame().GetCallqueue().CallLater(MissionRun, SDRC_Conf.SPAWN_ITEM_DELAY);		//Spawn stuff every two seconds
			return;
		}

		if (GetState() == SDRC_EMissionState.END)
		{
			MissionEnd();
			SetState(SDRC_EMissionState.EXIT);
		}	
				
		if (GetState() == SDRC_EMissionState.ACTIVE)
		{			
			if (!IsActive())
			{
				SetState(SDRC_EMissionState.END);
			}
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionCycleTime*1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		super.MissionEnd();	
	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		bool ready = false;
		
		ready = SDRC_CampHelper.Spawn(this, m_iSpawnIndex, m_DC_Roadblock, m_fSpawnRotation, m_Config.disableArsenal);
		m_iSpawnIndex++;			
		
		if ( (ready) && (GetState() != SDRC_EMissionState.FAILED) )
		{
			SetState(SDRC_EMissionState.ACTIVE);
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	override void DoWin()
	{	
		SDRC_CampHelper.AddCampLoot(m_DC_Roadblock, GetDifficulty());
		super.DoWin();
	}	
}
	
//------------------------------------------------------------------------------------------------
class SDRC_RoadblockConfig : SDRC_MissionConfig
{
	//Mission specific
	ref array<ref SDRC_Camp> subMissions = {};		//List of roadblocks - uses the same structure as for occupations	
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_RoadblockConfig data = SDRC_RoadblockConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------	
	override void LoadMissionFiles(int ver)
	{
		//Load mission files
		foreach (string missionFile : missionFiles)
		{
			SDRC_JsonApi2 jsonApi = new SDRC_JsonApi2(missionFile);
			SDRC_RoadblockConfig conf = new SDRC_RoadblockConfig();
			
			if (jsonApi.Load(conf, SDRC_MissionConfig.Cast(conf), ver, false))
			{
				foreach (SDRC_Camp subMission : conf.subMissions)
				{
					subMissions.Insert(subMission);
				}
				foreach (int idx : conf.missionList)
				{
					missionList.Insert(idx);
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	override void CreateMissionFiles()
	{
		super.CreateMissionFiles();
	}	
	
	//------------------------------------------------------------------------------------------------
	int GetSubMissionIdx(int subIdx)
	{
		int idx = -1;
		foreach (int i, SDRC_Camp subMission : subMissions)
		{
			if (subMission.general.subIdx == subIdx)
			{
				idx = i;
				break;
			}
		}
		return idx;
	}		
			
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		missionType = SDRC_EMissionType.ROADBLOCK;
		
		//Default		
		disableArsenal = true;
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		missionList = {0,1,2,3,4};
		//Mission specific		
		//----------------------------------------------------
		subMissions.Insert(Roadblock0());				
		subMissions.Insert(Roadblock1());				
		subMissions.Insert(Roadblock2());				
		subMissions.Insert(Roadblock3());				
		subMissions.Insert(Roadblock4());				
	};
	
	//----------------------------------------------------
	SDRC_Camp Roadblock0()
	{
		SDRC_Camp roadblock = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Roadblock near %l",
			"Look out for trouble.",
			"Roadblock cleared.",
			"Road was kept safe as planned.",);
		roadblock.general.Set(
			0, "index 0: Roadblock",
			{"0 0 0"}, 6,
			{
				EMapDescriptorType.MDT_NAME_TOWN,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_NAME_VALLEY,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_FORTRESS
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP,
			{SDRC_EDifficulty.RANDOM},
			0		
		);
		roadblock.ai.Set(
			{1, 2},
			{"G_LAUNCHER", "G_LIGHT", "G_LIGHT"},
			50, 1.0,
			{0, 20},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		ref SDRC_MissionConfigQrf qrf = new SDRC_MissionConfigQrf();		
		qrf.Set(
			{0}, SDRC_EMissionSuccess.WIN,
			1.0, {5, 20}
		);
		roadblock.qrf = qrf;
		
		ref SDRC_Loot loot = new SDRC_Loot();		
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_BAG", 
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
			};
		loot.Set(0.7, lootItems);
		roadblock.loot = loot;
				
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
		    "10.387 1 94.908"
		);
		roadblock.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{3F5EE4D69DBC478C}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_short_plastic.et",
		    "7.037 1 95.973"
		);
		roadblock.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{3F5EE4D69DBC478C}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_short_plastic.et",
		    "10.177 1 96.021"
		);
		roadblock.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{723870DBB19D30B0}Prefabs/Weapons/Tripods/Tripod_6T5_PKM.et",
		    "8.656 1 95.797"
		);
		roadblock.campItems.Insert(item_3);		
		
		return roadblock;
	};
	
	//----------------------------------------------------
	SDRC_Camp Roadblock1()
	{
		ref SDRC_Camp roadblock = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Roadblock seen close to %l",
			"Be careful.",
			"These blocks can not stop you.",
			"Scared of the enemy? %l is not a place for you.",);
		roadblock.general.Set(
			1, "index 1: Roadblock",
			{"0 0 0"}, 6,
			{
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_TOWN,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_FORTRESS
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP,
			{SDRC_EDifficulty.RANDOM},
			0		
		);
		roadblock.ai.Set(
			{1, 4},
			{"G_LAUNCHER", "G_HEAVY", "G_LIGHT"},
			30, 0.6,
			{0, 10},
			SDRC_EWaypointGenerationType.SCATTERED,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", "WEAPON_RIFLE", "WEAPON_RIFLE",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"UTIL_MAGAZINE", "UTIL_MAGAZINE",
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
			};
		loot.Set(0.9, lootItems);
		roadblock.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
		    "6.814 1 115.156",
		    "0 32.729 0"
		);
		roadblock.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "12.579 0 120.902",
		    "0 28.496 0"
		);
		roadblock.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "8.218 0 123.028",
		    "0 -31.966 0"
		);
		roadblock.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "11.18 0 107.827",
		    "0 9.997 0"
		);
		roadblock.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "5.713 0.227 120.863",
		    "0 14.494 0"
		);
		roadblock.campItems.Insert(item_4);
		
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "14.977 -0.169 120.709",
		    "0 -16.455 0"
		);
		roadblock.campItems.Insert(item_5);
		
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "7.873 0 108.275",
		    "0 -48.948 0"
		);
		roadblock.campItems.Insert(item_6);
		
		ref SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "4.596 0 108.679",
		    "0 54.582 0"
		);
		roadblock.campItems.Insert(item_7);
		
		ref SDRC_Structure item_8 = new SDRC_Structure();
		item_8.Set(
		    "{93E06E731212BD96}Prefabs/Structures/Walls/BarbedWire/BarbedTape_01/BarbedTape_01_Triple.et",
		    "14.151 0 114.891",
		    "0 -32.743 0"
		);
		roadblock.campItems.Insert(item_8);
		
		ref SDRC_Structure item_9 = new SDRC_Structure();
		item_9.Set(
		    "{93E06E731212BD96}Prefabs/Structures/Walls/BarbedWire/BarbedTape_01/BarbedTape_01_Triple.et",
		    "5.131 0 116.374",
		    "0 26.719 0"
		);
		roadblock.campItems.Insert(item_9);
		
		ref SDRC_Structure item_10 = new SDRC_Structure();
		item_10.Set(
		    "{DDF59362051B28BC}Prefabs/Props/Military/Fortification/BarbedTape_KnifeRest.et",
		    "9.49 1 117.729",
		    "0 86.019 0"
		);
		roadblock.campItems.Insert(item_10);
		
		ref SDRC_Structure item_11 = new SDRC_Structure();
		item_11.Set(
		    "{DDF59362051B28BC}Prefabs/Props/Military/Fortification/BarbedTape_KnifeRest.et",
		    "8.268 1 110.003",
		    "0 93.268 0"
		);
		roadblock.campItems.Insert(item_11);
		
		return roadblock;
	};
		
	//----------------------------------------------------
	SDRC_Camp Roadblock2()
	{
		ref SDRC_Camp roadblock = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Road is closed near %l",
			"Are your ready to pay the toll?",
			"Road cleared.",
			"The road toll was too much for you.");
		roadblock.general.Set(
			2, "index 2: Roadblock",
			{"0 0 0"}, 6,
			{
				EMapDescriptorType.MDT_NAME_CITY, EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_TOWN, EMapDescriptorType.MDT_NAME_TOWN, EMapDescriptorType.MDT_NAME_TOWN,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_FORTRESS
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP,
			{SDRC_EDifficulty.RANDOM},
			0		
		);		
		roadblock.ai.Set(
			{2, 4},
			{"G_LIGHT", "G_LIGHT", "G_LIGHT", "C_SNIPER", },
			50, 0.8,
			{0, 10},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_LAUNCHER", "WEAPON_RIFLE", "WEAPON_RIFLE",
				"UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO",
				"CLOTHING_HEADGEAR", "CLOTHING_HEADGEAR", 
				"CLOTHING_UNIFORM", "CLOTHING_UNIFORM",			
			};
		loot.Set(0.6, lootItems);
		roadblock.loot = loot;

		// Loot item ALWAYS first
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
		    "10.866 1 140.452",
		    "0 2.385 0"
		);
		roadblock.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{5608B9078BF2AC5A}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragonsteeth_01_old_V1.et",
		    "15.628 0 138.299"
		);
		roadblock.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{5608B9078BF2AC5A}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragonsteeth_01_old_V1.et",
		    "5.514 0 136.293",
		    "0 -11.07 0"
		);
		roadblock.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{5608B9078BF2AC5A}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragonsteeth_01_old_V1.et",
		    "3.632 0 141.811",
		    "0 24.752 0"
		);
		roadblock.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{5EF52FAE299A831A}Prefabs/Props/Military/Sandbags/Sandbag_01_short_base.et",
		    "13.68 1 135.766",
		    "0 -21.115 0"
		);
		roadblock.campItems.Insert(item_4);
		
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{5EF52FAE299A831A}Prefabs/Props/Military/Sandbags/Sandbag_01_short_base.et",
		    "8.664 1 134.464",
		    "0 6.278 0"
		);
		roadblock.campItems.Insert(item_5);
		
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{6A735BB1318B2E76}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_bunker_plastic_camonet.et",
		    "10.187 1 140.88"
		);
		roadblock.campItems.Insert(item_6);
		
		ref SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
		    "{A76FBE3B139F227A}PrefabsEditable/Auto/Structures/Signs/Military/E_SignCheckpoint_01_stop.et",
		    "6.063 1 147.099"
		);
		roadblock.campItems.Insert(item_7);
		
		ref SDRC_Structure item_8 = new SDRC_Structure();
		item_8.Set(
		    "{A76FBE3B139F227A}PrefabsEditable/Auto/Structures/Signs/Military/E_SignCheckpoint_01_stop.et",
		    "12.689 1 147.489",
		    "0 -9.375 0"
		);
		roadblock.campItems.Insert(item_8);
		
		ref SDRC_Structure item_9 = new SDRC_Structure();
		item_9.Set(
		    "{B6307C189CCCA0B9}Prefabs/Props/Military/Sandbags/Sandbag_01_round_high_plastic.et",
		    "14.815 1 145.385",
		    "0 37.532 0"
		);
		roadblock.campItems.Insert(item_9);
		
		ref SDRC_Structure item_10 = new SDRC_Structure();
		item_10.Set(
		    "{B6307C189CCCA0B9}Prefabs/Props/Military/Sandbags/Sandbag_01_round_high_plastic.et",
		    "6.093 1 145.387",
		    "0 -29.757 0"
		);
		roadblock.campItems.Insert(item_10);
		
		return roadblock;	
	}
	
	//----------------------------------------------------
	SDRC_Camp Roadblock3()
	{
		ref SDRC_Camp roadblock = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Road near %l is guarded",
			"Prepare for a mandatory stop.",
			"Gates do not stop you.",
			"Guards has left road near %l.",);
		roadblock.general.Set(
			3, "index 3: Roadblock with gates",
			{"0 0 0"}, 10,
			{
				EMapDescriptorType.MDT_NAME_CITY, EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_TOWN, EMapDescriptorType.MDT_NAME_TOWN, EMapDescriptorType.MDT_NAME_TOWN,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_FORTRESS
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP,
			{SDRC_EDifficulty.RANDOM},
			0		
		);		
		roadblock.ai.Set(
			{2, 4},
			{"G_LIGHT", "G_LIGHT", "G_HEAVY", "G_ADMIN",},
			70, 0.3,
			{0, 10},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", "WEAPON_RIFLE",
				"UTIL_MAGAZINE", "UTIL_MAGAZINE", 
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO",
				"GEAR_BAG", 			
			};
		loot.Set(0.6, lootItems);
		roadblock.loot = loot;

		// --------------------------------------------------
		
		// Loot item ALWAYS first
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
		    "4.106 1 164.03",
		    "0 32.729 0"
		);
		roadblock.campItems.Insert(item_0);
		
		// BarGate group ($grp StaticModelEntity)
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{07F043DD6AACCD0B}Prefabs/Structures/Infrastructure/Barriers/BarGate_01/BarGate_01.et",
		    "9.13 0 177.086"
		);
		roadblock.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{07F043DD6AACCD0B}Prefabs/Structures/Infrastructure/Barriers/BarGate_01/BarGate_01.et",
		    "8.797 0 157.636"
		);
		roadblock.campItems.Insert(item_2);
		
		// Military signs ($grp GenericEntity)
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{2FE7AA0EBA71CDFB}Prefabs/Structures/Signs/Large/SignLarge_01_MilitaryBase.et",
		    "13.018 1 180.409"
		);
		roadblock.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{2FE7AA0EBA71CDFB}Prefabs/Structures/Signs/Large/SignLarge_01_MilitaryBase.et",
		    "12.97 1 154.555",
		    "0 -180 0"
		);
		roadblock.campItems.Insert(item_4);
		
		// Transmitter Tower
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{6A004A8F0571D456}Prefabs/Structures/Infrastructure/Towers/TransmitterTower_01/TransmitterTower_01_small.et",
		    "2.711 1 174.351"
		);
		roadblock.campItems.Insert(item_5);
		
		// Dragon teeth group ($grp StaticModelEntity)
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "15.871 0 163.515",
		    "0 28.496 0"
		);
		roadblock.campItems.Insert(item_6);
		
		ref SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "15.682 0 167.103",
		    "0 -3.796 0"
		);
		roadblock.campItems.Insert(item_7);
		
		ref SDRC_Structure item_8 = new SDRC_Structure();
		item_8.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "16.06 0 171.102",
		    "0 46.018 0"
		);
		roadblock.campItems.Insert(item_8);
		
		ref SDRC_Structure item_9 = new SDRC_Structure();
		item_9.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "14.785 0 175.695",
		    "0 21.038 0"
		);
		roadblock.campItems.Insert(item_9);
		
		ref SDRC_Structure item_10 = new SDRC_Structure();
		item_10.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "14.358 0 159.652",
		    "0 54.995 0"
		);
		roadblock.campItems.Insert(item_10);
		
		
		// Guard Box
		ref SDRC_Structure item_11 = new SDRC_Structure();
		item_11.Set(
		    "{9498ADA13BCDB218}Prefabs/Structures/Military/Houses/GuardBox_01/GuardBox_01_Base.et",
		    "3.929 0 166.778",
		    "0 -90 0"
		);
		roadblock.campItems.Insert(item_11);
		
		// Trash Bin
		ref SDRC_Structure item_12 = new SDRC_Structure();
		item_12.Set(
		    "{E7117284012B39A4}Prefabs/Props/Garbage/Bins/TrashBin_02_patched.et",
		    "4.341 1 168.549"
		);
		roadblock.campItems.Insert(item_12);
		
		return roadblock;
	}
	
	//----------------------------------------------------
	SDRC_Camp Roadblock4()
	{
		ref SDRC_Camp roadblock = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Bus stop near %l",
			"Maybe their car needs fixing.",
			"The area is cleared.",
			"Car is fixed, guards left area near %l.",);
		roadblock.general.Set(
			4, "index 4: Burning bus stop",
			{"0 0 0"}, 20,
			{
				EMapDescriptorType.MDT_NAME_CITY, EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_TOWN, EMapDescriptorType.MDT_NAME_TOWN, EMapDescriptorType.MDT_NAME_TOWN,
				EMapDescriptorType.MDT_NAME_LOCAL,
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP,
			{SDRC_EDifficulty.RANDOM},
			0		
		);		
		roadblock.ai.Set(
			{2, 3},
			{"G_ADMIN", "G_HEAVY", },
			70, 0.3,
			{0, 10},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"UTIL_MAGAZINE", 
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"UTIL_AMMO", 
				"GEAR_BAG", "GEAR_BAG", 
				"CLOTHING_HEADGEAR", "CLOTHING_HEADGEAR", "CLOTHING_HEADGEAR", 
				"CLOTHING_UNIFORM", "CLOTHING_UNIFORM", "CLOTHING_UNIFORM", 
				"GEAR_HANDWEAR", 
				"UTIL_OPTIC", 
			};
		loot.Set(0.6, lootItems);
		roadblock.loot = loot;

		// --------------------------------------------------
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{128253A267BE9424}Prefabs/Vehicles/Wheeled/S105/S105_randomized.et",
		    "11.498 1 201.622",
		    "0 56.282 0"
		);
		roadblock.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{37439557BBDEC2F9}Prefabs/EffectsModuleEntities/SDRC_SmallSmoke.et",
		    "8.839 1.129 193.246"
		);
		roadblock.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{5D0248A228159B01}Prefabs/Structures/Debris/DebrisPile/DebrisPile_Concrete_01_Debris_V4.et",
		    "15.417 1 198.035",
		    "0 -25.479 0"
		);
		roadblock.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{736EECBB2B5FB5E7}Prefabs/Structures/Infrastructure/Bus/BusShed_E_01/BusShed_E_01_Base.et",
		    "4.526 0 199.825",
		    "0 90 0"
		);
		roadblock.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{85AA2163519C855D}Prefabs/Structures/Debris/DebrisPile/DebrisPile_Concrete_01_Debris_V2.et",
		    "11.411 1 199.69"
		);
		roadblock.campItems.Insert(item_4);
		
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{85AA2163519C855D}Prefabs/Structures/Debris/DebrisPile/DebrisPile_Concrete_01_Debris_V2.et",
		    "12.379 1 206.063",
		    "0 27.574 0"
		);
		roadblock.campItems.Insert(item_5);
		
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{97F01D47E387935A}Prefabs/Structures/Debris/DebrisPile/DebrisPile_Wood_01.et",
		    "8.553 1 193.821"
		);
		roadblock.campItems.Insert(item_6);
		
		ref SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
		    "{B9E11E1CC3A95088}Prefabs/Structures/Agriculture/Gates/GateCollectiveFarm_02/GateCollectiveFarm_02_SaintPhilippe.et",
		    "9.491 0 207.062"
		);
		roadblock.campItems.Insert(item_7);
		
		ref SDRC_Structure item_8 = new SDRC_Structure();
		item_8.Set(
		    "{F30731717AD0E0F2}Prefabs/Structures/Infrastructure/Landlines/PhoneBooth_E/PhoneBooth_USSR_01.et",
		    "3.854 0 203.36",
		    "0 -90 0"
		);
		roadblock.campItems.Insert(item_8);
		
		return roadblock;
	}	
}