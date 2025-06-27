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
	private ref SDRC_RoadblockConfig m_Config;
	
	protected ref SDRC_Occupation m_DC_Roadblock;		//Roadblock configuration in use
	
	private int m_iSpawnIndex = 0;						//Counter for the item to spawn
	private float m_fSpawnRotation = 0;					//Rotation of the camp for random locations.
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Roadblock(vector pos = "0 0 0")
	{
		SDRC_Log.Add("[SDRC_Mission_Roadblock] Constructor", LogLevel.SPAM);
				
		//Set some defaults
		SetType(DC_EMissionType.ROADBLOCK);

		//Load config
		m_RoadblockJsonApi.Load();
		m_Config = m_RoadblockJsonApi.conf;
		
		//Pick a configuration for mission
		int idx = SDRC_MissionHelper.SelectMissionIndex(m_Config.roadblockList);
		if (idx == -1)
		{
			SDRC_Log.Add("[SDRC_Mission_Roadblock] No roadblocks defined.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}
		m_DC_Roadblock = m_Config.roadblocks[idx];
		
		//If not a GM requested mission, use the default one.
		if (!IsRequested())
		{
			pos = m_DC_Roadblock.pos;
		}
		
		//Find a location for the mission
		if (pos == "0 0 0")
		{
			pos = SDRC_MissionHelper.FindMissionPos(m_DC_Roadblock.locationTypes, m_DC_Roadblock.emptySize);
			
			//Add randomization so that it's not always in the same place
			pos = SDRC_Misc.RandomizePos(pos, 150);
		}
			
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
		
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SDRC_Log.Add("[SDRC_Mission_Roadblock] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}	
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(GetPos(), m_DC_Roadblock.posName));
		SetTitle(m_DC_Roadblock.title + "" + GetPosName());
		SetInfo(m_DC_Roadblock.info);
		SetMarker(m_Config.showMarker, DC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP);
		SetShowHint(m_Config.showHint);

		SDRC_SpawnHelper.SetStructuresToOrigo(m_DC_Roadblock.campItems);
		
		SetState(DC_EMissionState.INIT);			
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == DC_EMissionState.INIT)
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
		
		ready = SDRC_OccupationHelper.Spawn(this, m_iSpawnIndex, m_DC_Roadblock, m_fSpawnRotation, m_Config.disableArsenal);
		m_iSpawnIndex++;			
		
		if (ready)
		{
			SetState(DC_EMissionState.ACTIVE);
		}
	}
}
	
//------------------------------------------------------------------------------------------------
class SDRC_RoadblockConfig : SDRC_MissionConfig
{
	//Mission specific
	vector pos;
	string posName;
	string title;
	string info;
	
	//Variables here
	bool disableArsenal;								//Disable arsenal for vehicles so that no other items are found	
	ref array<ref int> roadblockList = {};				//The indexes of roadblocks.
	ref array<ref SDRC_Occupation> roadblocks = {};		//List of roadblocks - uses the same structure as for occupations	
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
		array<string> lootItems = {};
		
		//Default		
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		conf.showMarker = true;
		//Mission specific		
		conf.roadblockList = {0,1,2};		
		
		//----------------------------------------------------
		SDRC_Occupation roadblock0 = new SDRC_Occupation();
		roadblock0.Set(
			"index 0: Roadblock",
			"0 0 0",
			"any",
			"Roadblock near ",
			"Look out for trouble.",
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
		conf.roadblocks.Insert(roadblock0);
		
		SDRC_Loot roadblock0loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
			};
		roadblock0loot.Set(0.7, lootItems);
		roadblock0.loot = roadblock0loot;
				
		SDRC_Structure rb0item0 = new SDRC_Structure;
		rb0item0.Set(
		    "{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
		    "10.387 1 94.908"
		);
		roadblock0.campItems.Insert(rb0item0);
		
		SDRC_Structure rb0item1 = new SDRC_Structure;
		rb0item1.Set(
		    "{3F5EE4D69DBC478C}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_short_plastic.et",
		    "7.037 1 95.973"
		);
		roadblock0.campItems.Insert(rb0item1);
		
		SDRC_Structure rb0item2 = new SDRC_Structure;
		rb0item2.Set(
		    "{3F5EE4D69DBC478C}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_short_plastic.et",
		    "10.177 1 96.021"
		);
		roadblock0.campItems.Insert(rb0item2);
		
		SDRC_Structure rb0item3 = new SDRC_Structure;
		rb0item3.Set(
		    "{723870DBB19D30B0}Prefabs/Weapons/Tripods/Tripod_6T5_PKM.et",
		    "8.656 1 95.797"
		);
		roadblock0.campItems.Insert(rb0item3);		
		
		//----------------------------------------------------
		SDRC_Occupation roadblock1 = new SDRC_Occupation();
		roadblock1.Set(
			"index 1: Roadblock",
			"0 0 0",
			"any",
			"Roadblock seen close to ",
			"Be careful.",
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
		conf.roadblocks.Insert(roadblock1);
		
		SDRC_Loot roadblock1loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE", "WEAPON_RIFLE", "WEAPON_RIFLE",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"UTIL_MAGAZINES", "UTIL_MAGAZINES"
			};
		roadblock1loot.Set(0.9, lootItems);
		roadblock1.loot = roadblock1loot;
		
		SDRC_Structure rb1item0 = new SDRC_Structure;
		rb1item0.Set(
		    "{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
		    "6.814 1 115.156",
		    "0 32.729 0"
		);
		roadblock1.campItems.Insert(rb1item0);
		
		SDRC_Structure rb1item1 = new SDRC_Structure;
		rb1item1.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "12.579 0 120.902",
		    "0 28.496 0"
		);
		roadblock1.campItems.Insert(rb1item1);
		
		SDRC_Structure rb1item2 = new SDRC_Structure;
		rb1item2.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "8.218 0 123.028",
		    "0 -31.966 0"
		);
		roadblock1.campItems.Insert(rb1item2);
		
		SDRC_Structure rb1item3 = new SDRC_Structure;
		rb1item3.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "11.18 0 107.827",
		    "0 9.997 0"
		);
		roadblock1.campItems.Insert(rb1item3);
		
		SDRC_Structure rb1item4 = new SDRC_Structure;
		rb1item4.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "5.713 0.227 120.863",
		    "0 14.494 0"
		);
		roadblock1.campItems.Insert(rb1item4);
		
		SDRC_Structure rb1item5 = new SDRC_Structure;
		rb1item5.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "14.977 -0.169 120.709",
		    "0 -16.455 0"
		);
		roadblock1.campItems.Insert(rb1item5);
		
		SDRC_Structure rb1item6 = new SDRC_Structure;
		rb1item6.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "7.873 0 108.275",
		    "0 -48.948 0"
		);
		roadblock1.campItems.Insert(rb1item6);
		
		SDRC_Structure rb1item7 = new SDRC_Structure;
		rb1item7.Set(
		    "{8B7B5323534404A7}Prefabs/Structures/Military/Fortifications/Dragontooth_01/Dragontooth_01_V2.et",
		    "4.596 0 108.679",
		    "0 54.582 0"
		);
		roadblock1.campItems.Insert(rb1item7);
		
		SDRC_Structure rb1item8 = new SDRC_Structure;
		rb1item8.Set(
		    "{93E06E731212BD96}Prefabs/Structures/Walls/BarbedWire/BarbedTape_01/BarbedTape_01_Triple.et",
		    "14.151 0 114.891",
		    "0 -32.743 0"
		);
		roadblock1.campItems.Insert(rb1item8);
		
		SDRC_Structure rb1item9 = new SDRC_Structure;
		rb1item9.Set(
		    "{93E06E731212BD96}Prefabs/Structures/Walls/BarbedWire/BarbedTape_01/BarbedTape_01_Triple.et",
		    "5.131 0 116.374",
		    "0 26.719 0"
		);
		roadblock1.campItems.Insert(rb1item9);
		
		SDRC_Structure rb1item10 = new SDRC_Structure;
		rb1item10.Set(
		    "{DDF59362051B28BC}Prefabs/Props/Military/Fortification/BarbedTape_KnifeRest.et",
		    "9.49 1 117.729",
		    "0 86.019 0"
		);
		roadblock1.campItems.Insert(rb1item10);
		
		SDRC_Structure rb1item11 = new SDRC_Structure;
		rb1item11.Set(
		    "{DDF59362051B28BC}Prefabs/Props/Military/Fortification/BarbedTape_KnifeRest.et",
		    "8.268 1 110.003",
		    "0 93.268 0"
		);
		roadblock1.campItems.Insert(rb1item11);
		
		//----------------------------------------------------
		SDRC_Occupation roadblock2 = new SDRC_Occupation();
		roadblock2.Set(
			"index 2: Roadblock",
			"0 0 0",
			"any",
			"Road is closed near ",
			"Are your ready to pay the toll?",
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
		conf.roadblocks.Insert(roadblock2);
		
		SDRC_Loot roadblock2loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_LAUNCHER", "WEAPON_RIFLE", "WEAPON_RIFLE",
				"UTIL_MAGAZINES", "UTIL_MAGAZINES", "UTIL_MAGAZINES", "UTIL_MAGAZINES", "UTIL_MAGAZINES",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO"
			};
		roadblock2loot.Set(0.6, lootItems);
		roadblock2.loot = roadblock2loot;
		
		SDRC_Structure rb2item0 = new SDRC_Structure;
		rb2item0.Set(
		    "{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
		    "10.866 1 140.452",
		    "0 2.385 0"
		);
		roadblock2.campItems.Insert(rb2item0);
		
		SDRC_Structure rb2item1 = new SDRC_Structure;
		rb2item1.Set(
		    "{365E4BF1FFFD8B30}PrefabsEditable/Auto/Structures/Walls/BarbedWire/E_BarbedTape_Long.et",
		    "6.01 0 148.186",
		    "0 -106.133 0"
		);
		roadblock2.campItems.Insert(rb2item1);
		
		SDRC_Structure rb2item2 = new SDRC_Structure;
		rb2item2.Set(
		    "{365E4BF1FFFD8B30}PrefabsEditable/Auto/Structures/Walls/BarbedWire/E_BarbedTape_Long.et",
		    "14.12 0 145.299",
		    "0 -45.299 0"
		);
		roadblock2.campItems.Insert(rb2item2);
		
		SDRC_Structure rb2item3 = new SDRC_Structure;
		rb2item3.Set(
		    "{365E4BF1FFFD8B30}PrefabsEditable/Auto/Structures/Walls/BarbedWire/E_BarbedTape_Long.et",
		    "13.278 0.732 134.665",
		    "0 -106.133 0"
		);
		roadblock2.campItems.Insert(rb2item3);
		
		SDRC_Structure rb2item4 = new SDRC_Structure;
		rb2item4.Set(
		    "{365E4BF1FFFD8B30}PrefabsEditable/Auto/Structures/Walls/BarbedWire/E_BarbedTape_Long.et",
		    "9.088 0 134.691",
		    "0 -76.67 0"
		);
		roadblock2.campItems.Insert(rb2item4);
		
		SDRC_Structure rb2item5 = new SDRC_Structure;
		rb2item5.Set(
		    "{365E4BF1FFFD8B30}PrefabsEditable/Auto/Structures/Walls/BarbedWire/E_BarbedTape_Long.et",
		    "5.972 0 137.246",
		    "0 -18.935 0"
		);
		roadblock2.campItems.Insert(rb2item5);
		
		SDRC_Structure rb2item6 = new SDRC_Structure;
		rb2item6.Set(
		    "{365E4BF1FFFD8B30}PrefabsEditable/Auto/Structures/Walls/BarbedWire/E_BarbedTape_Long.et",
		    "9.607 0 148.603",
		    "0 -90 0"
		);
		roadblock2.campItems.Insert(rb2item6);
		
		SDRC_Structure rb2item7 = new SDRC_Structure;
		rb2item7.Set(
		    "{6A735BB1318B2E76}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_bunker_plastic_camonet.et",
		    "10.187 1 140.88"
		);
		roadblock2.campItems.Insert(rb2item7);
		
		SDRC_Structure rb2item8 = new SDRC_Structure;
		rb2item8.Set(
		    "{A76FBE3B139F227A}PrefabsEditable/Auto/Structures/Signs/Military/E_SignCheckpoint_01_stop.et",
		    "6.063 1 143.004"
		);
		roadblock2.campItems.Insert(rb2item8);
		
		SDRC_Structure rb2item9 = new SDRC_Structure;
		rb2item9.Set(
		    "{A76FBE3B139F227A}PrefabsEditable/Auto/Structures/Signs/Military/E_SignCheckpoint_01_stop.et",
		    "12.532 1 148.443",
		    "0 -9.375 0"
		);
		roadblock2.campItems.Insert(rb2item9);		
	}	
}