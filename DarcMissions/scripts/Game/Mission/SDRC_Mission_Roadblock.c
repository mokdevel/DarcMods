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
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Roadblock(vector pos = "0 0 0")
	{
		SDRC_Log.Add("[SDRC_Mission_Roadblock] Constructor", LogLevel.SPAM);
				
		//Set some defaults
		SetType(DC_EMissionType.ROADBLOCK);	//REMEMBER: Define your own ENUM in SDRC_Mission.c and change here. Don't use modded enum.

		//Load config
		m_RoadblockJsonApi.Load();
		m_Config = m_RoadblockJsonApi.conf;
		pos = m_Config.pos;
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(GetPos(), m_Config.posName));
		SetTitle(m_Config.title + "" + GetPosName());
		SetInfo(m_Config.info);
		SetMarker(m_Config.showMarker, DC_EMissionIcon.N_MISSION);
		SetShowHint(m_Config.showHint);

		SetState(DC_EMissionState.INIT);			
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == DC_EMissionState.INIT)
		{
			MissionSpawn();
			SetState(DC_EMissionState.ACTIVE);
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
		
		//The rest of your clean up code.
		
		SDRC_Log.Add("[SDRC_Mission_Roadblock:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		//Code for whatever you need for spawning things.
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
		SDRC_Occupation roadblock1 = new SDRC_Occupation();
		roadblock1.Set(
			"index 0: Roadblock",
			"0 0 0",
			"any",
			"Roadblock near ",
			"Bandits are protecting their valuable loot.",
			{
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_FORTRESS
			},
			{1, 2},
			{25, 100},
			DC_EWaypointGenerationType.SCATTERED,//RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_ADMIN", "G_LIGHT", "G_LIGHT"
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
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
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