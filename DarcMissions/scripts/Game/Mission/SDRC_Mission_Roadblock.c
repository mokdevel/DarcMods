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
		SetType(DC_EMissionType.ROADBLOCK);	//REMEMBER: Define your own ENUM in SDRC_Mission.c and change here. Don't use modded enum.

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
		
		//Set defaults
		if (!IsRequested())
		{
			pos = m_DC_Roadblock.pos;
			
			//Find a location for the mission
			if (pos == "0 0 0")
			{
				pos = SDRC_MissionHelper.FindMissionPos(m_DC_Roadblock.locationTypes, m_DC_Roadblock.emptySize);
				
				//Add randomization so that it's not always in the same place
				pos = SDRC_Misc.RandomizePos(pos, 150);
				
				//Find nearest road
				SDRC_RoadPos roadPos = new SDRC_RoadPos();				
				vector posOnRoad = SDRC_RoadHelper.FindClosestRoadposToPos(roadPos, pos);
				array<vector> roadPts;
				SDRC_RoadHelper.FindRoadPts(roadPts, roadPos.road);
				
				if (roadPts.IsEmpty())
				{
					pos = "0 0 0";
					SDRC_Log.Add("[SDRC_Mission_Roadblock] No roadpoints found.", LogLevel.ERROR);
				}
				else
				{
					SDRC_Log.Add("[SDRC_Mission_Roadblock] Roadpoints found: " + roadPts.Count(), LogLevel.DEBUG);
					
					int roadPointIndex = Math.RandomInt(0, roadPts.Count() - 1);
					pos = roadPts[roadPointIndex];
					posOnRoad = roadPts[roadPointIndex + 1];
	
					SDRC_DebugHelper.AddDebugPos(pos, ARGB(40, 192, 192, 192), 2, "NONE", 20);			//Gray
					SDRC_DebugHelper.AddDebugPos(posOnRoad, ARGB(40, 128, 128, 128), 2, "NONE", 20);	//Gray
									
					//Find the road direction. Roadblocks shall be aligned to road. 
					vector direction = vector.Direction(pos, posOnRoad);
					m_fSpawnRotation = SDRC_Misc.VectorToAngle(direction);
				}
			}
			else
			{
				pos = SDRC_MissionHelper.FindMissionPos(pos, m_DC_Roadblock.emptySize);
			}
		}
		else
		{
			pos = SDRC_MissionHelper.FindMissionPos(pos, m_DC_Roadblock.emptySize);
		}
		
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SDRC_Log.Add("[SDRC_Mission_Roadblock] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}	
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(GetPos(), m_Config.posName));
		SetTitle(m_Config.title + "" + GetPosName());
		SetInfo(m_Config.info);
		SetMarker(m_Config.showMarker, DC_EMissionIcon.N_MISSION);
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
				SDRC_Log.Add("[SDRC_Mission_Roadblock:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
				SetState(DC_EMissionState.END);
			}
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionCycleTime*1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		super.MissionEnd();	
		
		SDRC_Log.Add("[SDRC_Mission_Roadblock:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);
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
	bool disableArsenal;									//Disable arsenal for vehicles so that no other items are found	
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
		conf.roadblockList = {0};		
		
		//----------------------------------------------------
		SDRC_Occupation roadblock0 = new SDRC_Occupation();
		roadblock0.Set(
			"index 0: Roadblock",
			"0 0 0",
			"any",
			"Roadblock near ",
			"Bandits are protecting their valuable loot.",
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
			"Roadblock near ",
			"Bandits are protecting their valuable loot.",
			{
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_TOWN,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_FORTRESS
			},
			{1, 2},
			{0, 20},
			DC_EWaypointGenerationType.SCATTERED,//RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_LAUNCHER", "G_LIGHT", "G_LIGHT"
			},
			50, 1.0,
			6
		);
		conf.roadblocks.Insert(roadblock1);
		
		SDRC_Loot roadblock1loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
			};
		roadblock1loot.Set(0.7, lootItems);
		roadblock1.loot = roadblock1loot;
		
		SDRC_Structure rb1item0 = new SDRC_Structure;
		rb1item0.Set(
			"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
			"100.47 1 144.562"
		);
		roadblock1.campItems.Insert(rb1item0);

		SDRC_Structure rb1item1 = new SDRC_Structure;
		rb1item1.Set(
			"{39C308BBB5945B85}Prefabs/Props/Military/Furniture/ChairMilitary_US_02.et",
			"96.885 1 144.184",
			"0 119.334 0"
		);
		roadblock1.campItems.Insert(rb1item1);

		SDRC_Structure rb1item2 = new SDRC_Structure;
		rb1item2.Set(
			"{D9842C11742C00CF}Prefabs/Props/Civilian/Fireplace_01.et",
			"98.235 1 143.464"
		);
		roadblock1.campItems.Insert(rb1item2);

		SDRC_Structure rb1item3 = new SDRC_Structure;
		rb1item3.Set(
			"{EBC3D311A1B64FE6}PrefabsEditable/Auto/Structures/Military/Camps/TentSmallUS_01/E_TentSmallUS_01.et",
			"96.653 1 146.601",
			"0 -39.208 0"
		);
		roadblock1.campItems.Insert(rb1item3);	
	}	
}