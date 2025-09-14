//Mission SDRC_Mission_Roadblock.c
//
//An example of a mission file with simple set. No real functionality.
//Do a "replace all" for the word "Roadblock" with your mission name.

//------------------------------------------------------------------------------------------------
/*!

*/

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Roadblock : SDRC_Mission
{
	private ref SDRC_RoadblockJsonApi m_RoadblockJsonApi = new SDRC_RoadblockJsonApi();	
	private ref SDRC_RoadblockConfig m_Config = new SDRC_RoadblockConfig();	
	private ref SDRC_Camp m_DC_Roadblock = new SDRC_Camp;
	
	private int m_iSpawnIndex = 0;						//Counter for the item to spawn
	private float m_fSpawnRotation = 0;					//Rotation of the camp for random locations.
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Roadblock(DC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Load config
		m_RoadblockJsonApi.Load();
		m_Config = m_RoadblockJsonApi.conf;
		
		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		if (GetSubIdx() == -1)
		{
			SetState(DC_EMissionState.FAILED, DC_EMissionError.WRONG_SUBIDX);
			return;
		}
		m_DC_Roadblock = m_Config.roadblocks[GetSubIdx()];
		HandleRequestGeneralVariables(m_DC_Roadblock.general, request);
		
		//For requested missions we want have it as close as possible in the requested place.
		int randomPos = -1;		
		if (IsRequested())
		{
			randomPos = 0;
		}
		
		//Find a location for the mission
		vector pos = m_DC_Roadblock.general.pos[0];
		if (pos == "0 0 0")
		{
			pos = SDRC_MissionHelper.FindMissionPos(m_DC_Roadblock.locationTypes, m_DC_Roadblock.emptySize, randomPos);
		}
		
		//If we found a position, let's search more closely
		if (pos != "0 0 0")
		{		
			//Add randomization so that it's not always in the same place
			pos = SDRC_Misc.RandomizePos(pos, 150);
				
			//Find nearest road
			SDRC_RoadPos roadPos = new SDRC_RoadPos();				
			vector posOnRoad = SDRC_RoadHelper.FindClosestRoadposToPos(roadPos, pos);
			
			if (roadPos.roadPts.Count() < 2)	//We need two points for a road. Having only one point would be a bug on the map.
			{
				pos = "0 0 0";
				SDRC_Log.Add("[SDRC_Mission_Roadblock] No roadpoints found.", LogLevel.ERROR);
			}
			else
			{
				int roadPointIndex = 0;
				
				if (!IsRequested())
				{
					roadPointIndex = Math.RandomInt(0, roadPos.roadPts.Count() - 2);
				}
				else			
				{
					roadPointIndex = roadPos.posOnRoadIndex;
				}
	
				//If the closest point is the last point, go one point backwards.
				if (roadPointIndex >= roadPos.roadPts.Count() - 1)
				{
					roadPointIndex--;
					SDRC_Log.Add("[SDRC_Mission_Roadblock] Roadpoint index reduced", LogLevel.SPAM);
				}
				
				//SDRC_Log.Add("[SDRC_Mission_Roadblock] Roadpoints found: " + roadPos.roadPts.Count() + " idx: " + roadPointIndex, LogLevel.SPAM);
				
				pos = roadPos.roadPts[roadPointIndex];
				posOnRoad = roadPos.roadPts[roadPointIndex + 1];
				
				SDRC_DebugHelper.AddDebugPos(pos, ARGB(40, 192, 192, 192), 2, "NONE", 20);			//Gray
				SDRC_DebugHelper.AddDebugPos(posOnRoad, ARGB(40, 128, 128, 128), 2, "NONE", 20);	//Gray
								
				//Find the road direction. Roadblocks shall be aligned to road. 
				vector direction = vector.Direction(pos, posOnRoad);
				m_fSpawnRotation = SDRC_Misc.VectorToAngle(direction);
			}
		}
		
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SetState(DC_EMissionState.FAILED, DC_EMissionError.LOCATION_NOT_FOUND);
			return;
		}	
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(GetPos(), m_DC_Roadblock.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_DC_Roadblock.general);		
/*		SetMarker(m_Config.showMarker, m_DC_Roadblock.general.markerIcon, m_DC_Roadblock.general.markerType);
		SetHint(m_Config.showHint, m_DC_Roadblock.general.title, m_DC_Roadblock.general.info);
		SetMessages(m_Config.showMessage, m_DC_Roadblock.general.winMessage, m_DC_Roadblock.general.loseMessage);		
		SetWinCondition(m_DC_Roadblock.general.winCondition);*/

		SDRC_SpawnHelper.SetStructuresToOrigo(m_DC_Roadblock.campItems);
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == DC_EMissionState.SPAWN)
		{
			MissionSpawn();
			GetGame().GetCallqueue().CallLater(MissionRun, 2*1000);		//Spawn stuff every two seconds
			return;
		}

		if (GetState() == DC_EMissionState.END)
		{
			MissionEnd();
			SetState(DC_EMissionState.EXIT);
		}	
				
		if (GetState() == DC_EMissionState.ACTIVE)
		{			
			if (!IsActive())
			{
				SetState(DC_EMissionState.END);
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
		
		if (ready)
		{
			SetState(DC_EMissionState.ACTIVE);
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	override void DoWin()
	{	
		SDRC_CampHelper.AddLoot(m_DC_Roadblock);
		super.DoWin();
	}	
}
	
//------------------------------------------------------------------------------------------------
class SDRC_RoadblockConfig : SDRC_MissionConfig
{
	//Mission specific
	bool disableArsenal;								//Disable arsenal for vehicles so that no other items are found	
	ref array<ref SDRC_Camp> roadblocks = {};		//List of roadblocks - uses the same structure as for occupations	
}

//------------------------------------------------------------------------------------------------
class SDRC_RoadblockJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Roadblock.json";
	ref SDRC_RoadblockConfig conf = new SDRC_RoadblockConfig();
		
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_MISSIONCONFIG_FILE);
		
		if (!loadContext)
		{
			SetDefaults();
			Save("");
			return;
		}

		loadContext.ReadValue("", conf);
	}	
	
	//------------------------------------------------------------------------------------------------
	void Save(string data)
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen(DC_MISSIONCONFIG_FILE);
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}	
		
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		//Default		
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		conf.missionList = {0,1,2,3};
		//Mission specific		
		//----------------------------------------------------
		conf.roadblocks.Insert(Roadblock0());				
		conf.roadblocks.Insert(Roadblock1());				
		conf.roadblocks.Insert(Roadblock2());				
		conf.roadblocks.Insert(Roadblock3());				
	};
	
	//----------------------------------------------------
	SDRC_Camp Roadblock0()
	{
		SDRC_Camp roadblock = new SDRC_Camp();
		roadblock.general.Set(
			0, "index 0: Roadblock",
			{"0 0 0"},
			"any",
			"Roadblock near %l",
			"Look out for trouble.",
			DC_EMissionWinCondition.AI_KILL_75,
			"Roadblock cleared.",
			"Road was kept safe as planned.", 
			"",
			"DARC_MISSION", DC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP,
			0		
		);
		roadblock.Set(
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
			{1, 2},
			{0, 20},
			DC_EWaypointGenerationType.LOITER,//RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_LAUNCHER", "G_LIGHT", "G_LIGHT"
			},
			50, 1.0,
			6
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
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
		roadblock.general.Set(
			1, "index 1: Roadblock",
			{"0 0 0"},
			"any",
			"Roadblock seen close to %l",
			"Be careful.",
			DC_EMissionWinCondition.AI_KILL_ALL,
			"These blocks can not stop you.",
			"Scared of the enemy? %l is not a place for you.", 
			"",
			"DARC_MISSION", DC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP,
			0		
		);
		roadblock.Set(
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
			{1, 2},
			{0, 10},
			DC_EWaypointGenerationType.SCATTERED,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_LAUNCHER", "G_HEAVY", "G_LIGHT"
			},
			50, 1.0,
			6
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", "WEAPON_RIFLE", "WEAPON_RIFLE",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"UTIL_MAGAZINE", "UTIL_MAGAZINE"
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
		roadblock.general.Set(
			2, "index 2: Roadblock",
			{"0 0 0"},
			"any",
			"Road is closed near %l",
			"Are your ready to pay the toll?",
			DC_EMissionWinCondition.AI_KILL_75,
			"Road cleared.",
			"The road toll was too much for you.", 
			"",
			"DARC_MISSION", DC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP,
			0		
		);		
		roadblock.Set(
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
			{2, 3},
			{0, 10},
			DC_EWaypointGenerationType.LOITER,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_LIGHT", "G_LIGHT", "G_LIGHT", "C_SNIPER", 
			},
			50, 1.0,
			6
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_LAUNCHER", "WEAPON_RIFLE", "WEAPON_RIFLE",
				"UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO"
			};
		loot.Set(0.6, lootItems);
		roadblock.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
		    "10.866 1 140.452",
		    "0 2.385 0"
		);
		roadblock.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{365E4BF1FFFD8B30}PrefabsEditable/Auto/Structures/Walls/BarbedWire/E_BarbedTape_Long.et",
		    "6.01 0 148.186",
		    "0 -106.133 0"
		);
		roadblock.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{365E4BF1FFFD8B30}PrefabsEditable/Auto/Structures/Walls/BarbedWire/E_BarbedTape_Long.et",
		    "14.12 0 145.299",
		    "0 -45.299 0"
		);
		roadblock.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{365E4BF1FFFD8B30}PrefabsEditable/Auto/Structures/Walls/BarbedWire/E_BarbedTape_Long.et",
		    "13.278 0.732 134.665",
		    "0 -106.133 0"
		);
		roadblock.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{365E4BF1FFFD8B30}PrefabsEditable/Auto/Structures/Walls/BarbedWire/E_BarbedTape_Long.et",
		    "9.088 0 134.691",
		    "0 -76.67 0"
		);
		roadblock.campItems.Insert(item_4);
		
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{365E4BF1FFFD8B30}PrefabsEditable/Auto/Structures/Walls/BarbedWire/E_BarbedTape_Long.et",
		    "5.972 0 137.246",
		    "0 -18.935 0"
		);
		roadblock.campItems.Insert(item_5);
		
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{365E4BF1FFFD8B30}PrefabsEditable/Auto/Structures/Walls/BarbedWire/E_BarbedTape_Long.et",
		    "9.607 0 148.603",
		    "0 -90 0"
		);
		roadblock.campItems.Insert(item_6);
		
		ref SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
		    "{6A735BB1318B2E76}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_bunker_plastic_camonet.et",
		    "10.187 1 140.88"
		);
		roadblock.campItems.Insert(item_7);
		
		ref SDRC_Structure item_8 = new SDRC_Structure();
		item_8.Set(
		    "{A76FBE3B139F227A}PrefabsEditable/Auto/Structures/Signs/Military/E_SignCheckpoint_01_stop.et",
		    "6.063 1 143.004"
		);
		roadblock.campItems.Insert(item_8);
		
		ref SDRC_Structure item_9 = new SDRC_Structure();
		item_9.Set(
		    "{A76FBE3B139F227A}PrefabsEditable/Auto/Structures/Signs/Military/E_SignCheckpoint_01_stop.et",
		    "12.532 1 148.443",
		    "0 -9.375 0"
		);
		roadblock.campItems.Insert(item_9);		

		return roadblock;				
	}
	
	//----------------------------------------------------
	SDRC_Camp Roadblock3()
	{
		ref SDRC_Camp roadblock = new SDRC_Camp();
		roadblock.general.Set(
			3, "index 3: Roadblock with gates",
			{"0 0 0"},
			"any",
			"Road near %l is guarded",
			"Prepare for a mandatory stop.",
			DC_EMissionWinCondition.AI_KILL_75,
			"Gates do not stop you.",
			"Guards has left road near %l.", 
			"",
			"DARC_MISSION", DC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP,
			0		
		);		
		roadblock.Set(
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
			{2, 3},
			{0, 10},
			DC_EWaypointGenerationType.LOITER,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_LIGHT", "G_LIGHT", "G_HEAVY", "G_ADMIN",
			},
			50, 1.0,
			10
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", "WEAPON_RIFLE",
				"UTIL_MAGAZINE", "UTIL_MAGAZINE", 
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO",
			};
		loot.Set(0.6, lootItems);
		roadblock.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
		    "4.106 1 164.03",
		    "0 32.729 0"
		);
		roadblock.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{07F043DD6AACCD0B}Prefabs/Structures/Infrastructure/Barriers/BarGate_01/BarGate_01.et",
		    "8.797 0 157.636"
		);
		roadblock.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{18822FB3F0D2ED00}Prefabs/Structures/Walls/BarbedWire/BarbedTape_01/BarbedTape_01_Long.et",
		    "14.847 0 160.266",
		    "0 30.736 0"
		);
		roadblock.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{18822FB3F0D2ED00}Prefabs/Structures/Walls/BarbedWire/BarbedTape_01/BarbedTape_01_Long.et",
		    "14.936 0 173.246",
		    "0 160.061 0"
		);
		roadblock.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{18822FB3F0D2ED00}Prefabs/Structures/Walls/BarbedWire/BarbedTape_01/BarbedTape_01_Long.et",
		    "15.391 0 166.746",
		    "0 -175.406 0"
		);
		roadblock.campItems.Insert(item_4);
		
		// SignLarge group
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{2FE7AA0EBA71CDFB}Prefabs/Structures/Signs/Large/SignLarge_01_MilitaryBase.et",
		    "13.018 1 180.409"
		);
		roadblock.campItems.Insert(item_5);
		
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{2FE7AA0EBA71CDFB}Prefabs/Structures/Signs/Large/SignLarge_01_MilitaryBase.et",
		    "12.97 1 154.555",
		    "0 -180 0"
		);
		roadblock.campItems.Insert(item_6);
		
		ref SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
		    "{07F043DD6AACCD0B}Prefabs/Structures/Infrastructure/Barriers/BarGate_01/BarGate_01.et",
		    "9.13 0 177.086"
		);
		roadblock.campItems.Insert(item_7);
		
		
		// TransmitterTower
		ref SDRC_Structure item_8 = new SDRC_Structure();
		item_8.Set(
		    "{6A004A8F0571D456}Prefabs/Structures/Infrastructure/Towers/TransmitterTower_01/TransmitterTower_01_small.et",
		    "2.711 1 174.351"
		);
		roadblock.campItems.Insert(item_8);
		
		// GuardBox
		ref SDRC_Structure item_9 = new SDRC_Structure();
		item_9.Set(
		    "{9498ADA13BCDB218}Prefabs/Structures/Military/Houses/GuardBox_01/GuardBox_01_Base.et",
		    "3.929 0 166.778",
		    "0 -90 0"
		);
		roadblock.campItems.Insert(item_9);
		
		// TrashBin
		ref SDRC_Structure item_10 = new SDRC_Structure();
		item_10.Set(
		    "{E7117284012B39A4}Prefabs/Props/Garbage/Bins/TrashBin_02_patched.et",
		    "4.341 1 168.549"
		);
		roadblock.campItems.Insert(item_10);	
		
		return roadblock;				
	}
}