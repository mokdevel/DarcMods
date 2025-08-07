//Mission SDRC_Mission_Occupation.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns groups to defend a location
*/

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Occupation : SDRC_Mission
{
	private ref SDRC_OccupationJsonApi m_OccupationJsonApi = new SDRC_OccupationJsonApi();	
	private ref SDRC_OccupationConfig m_Config;
	
	protected ref SDRC_Occupation m_DC_Occupation;		//Occupation configuration in use
	
	private int m_iSpawnIndex = 0;						//Counter for the item to spawn
	private float m_fSpawnRotation = 0;					//Rotation of the camp for random locations.

	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Occupation(vector pos = "0 0 0")
	{
		SDRC_Log.Add("[SDRC_Mission_Occupation] Constructor", LogLevel.SPAM);
				
		//Set some defaults
		SetType(DC_EMissionType.OCCUPATION);

		//Load config
		m_OccupationJsonApi.Load();
		m_Config = m_OccupationJsonApi.conf;
		
		//Pick a configuration for mission
		int idx = SDRC_MissionHelper.SelectMissionIndex(m_Config.occupationList);
		if (idx == -1)
		{
			SDRC_Log.Add("[SDRC_Mission_Occupation] No occupations defined.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}
		m_DC_Occupation = m_Config.occupations[idx];
				
		//Set defaults
		if (!IsRequested())
		{
			pos = m_DC_Occupation.pos;
			
			//Find a location for the mission
			if (pos == "0 0 0")
			{
				pos = SDRC_MissionHelper.FindMissionPos(m_DC_Occupation.locationTypes, m_DC_Occupation.emptySize);
				//Camps in random places are randomly rotated
				m_fSpawnRotation = Math.RandomFloat(0, 360);
			}
			else
			{
				pos = SDRC_MissionHelper.FindMissionPos(pos, m_DC_Occupation.emptySize);
			}
		}
		else
		{
			pos = SDRC_MissionHelper.FindMissionPos(pos, m_DC_Occupation.emptySize);
		}
		
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SDRC_Log.Add("[SDRC_Mission_Occupation] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}	
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Occupation.posName));
		SetTitle(m_DC_Occupation.title + "" + GetPosName());
		SetInfo(m_DC_Occupation.info);			
		SetMarker(m_Config.showMarker, DC_EMissionIcon.GM_MISSION_OCCUPATION_MAP, m_Config.markerType);
		SetShowHint(m_Config.showHint);

		SDRC_SpawnHelper.SetStructuresToOrigo(m_DC_Occupation.campItems);
			
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
		
		ready = SDRC_OccupationHelper.Spawn(this, m_iSpawnIndex, m_DC_Occupation, m_fSpawnRotation, m_Config.disableArsenal);
		m_iSpawnIndex++;			
		
		if (ready)
		{
			SetState(DC_EMissionState.ACTIVE);
		}
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_OccupationConfig : SDRC_MissionConfig
{
	//Mission specific	
	bool disableArsenal;									//Disable arsenal for vehicles so that no other items are found
	ref array<ref int> occupationList = {};					//The indexes of occupations.
	ref array<ref SDRC_Occupation> occupations = {};		//List of occupations
}

//------------------------------------------------------------------------------------------------
class SDRC_OccupationJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Occupation.json";
	ref SDRC_OccupationConfig conf = new SDRC_OccupationConfig();
	
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
		conf.occupationList = {0,0,0,1,1,1,1,2,2,2,2,2,3,3,3,4,5};		

		//----------------------------------------------------
		SDRC_Occupation occupation0 = new SDRC_Occupation();
		occupation0.Set
		(
			"index 0: Mission in villages and local areas.",
			"0 0 0",
			"any",
			"Guards patroling near ",
			"Avoid the location. Loot has already been lost.",
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL
			},
			{1, 2},
			{50, 300},
			DC_EWaypointGenerationType.RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_RECON", "G_LIGHT"
			},
			50, 1.0,
			3
		);
		conf.occupations.Insert(occupation0);
		
		//----------------------------------------------------
		SDRC_Occupation occupation1 = new SDRC_Occupation();
		occupation1.Set(
			"index 1: Bandit camp spawning to non city areas",
			"0 0 0",
			"any",
			"Bandit camp near ",
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
		conf.occupations.Insert(occupation1);
		
		SDRC_Loot occupation1loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			};
		occupation1loot.Set(0.7, lootItems);
		occupation1.loot = occupation1loot;
		
		SDRC_Structure ocu1item0 = new SDRC_Structure;
		ocu1item0.Set(
			"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
			"100.47 1 144.562"
		);
		occupation1.campItems.Insert(ocu1item0);

		SDRC_Structure ocu1item1 = new SDRC_Structure;
		ocu1item1.Set(
			"{39C308BBB5945B85}Prefabs/Props/Military/Furniture/ChairMilitary_US_02.et",
			"96.885 1 144.184",
			"0 119.334 0"
		);
		occupation1.campItems.Insert(ocu1item1);

		SDRC_Structure ocu1item2 = new SDRC_Structure;
		ocu1item2.Set(
			"{D9842C11742C00CF}Prefabs/Props/Civilian/Fireplace_01.et",
			"98.235 1 143.464"
		);
		occupation1.campItems.Insert(ocu1item2);

		SDRC_Structure ocu1item3 = new SDRC_Structure;
		ocu1item3.Set(
			"{EBC3D311A1B64FE6}PrefabsEditable/Auto/Structures/Military/Camps/TentSmallUS_01/E_TentSmallUS_01.et",
			"96.653 1 146.601",
			"0 -39.208 0"
		);
		occupation1.campItems.Insert(ocu1item3);	

		//----------------------------------------------------
		SDRC_Occupation occupation2 = new SDRC_Occupation();
		occupation2.Set(
			"index 2: Occupation that will spawn mainly to cities and towns.",
			"0 0 0",
			"any",
			"Occupation in ",
			"City is being occupied.",
			{
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_TOWN, 
				EMapDescriptorType.MDT_AIRPORT,
			},
			{2, 4},
			{30, 200},
			DC_EWaypointGenerationType.RADIUS,
			DC_EWaypointMoveType.RANDOM,		
			{
				"G_LIGHT", "G_LIGHT", "G_LIGHT", 
				"G_ADMIN", "G_HEAVY", "G_LAUNCHER", "G_MEDICAL"
			},
			50, 1.0,
			10		
		);
		conf.occupations.Insert(occupation2);

		SDRC_Loot occupation2loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"UTIL_OPTICS",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			};
		occupation2loot.Set(0.9, lootItems);
		occupation2.loot = occupation2loot;
		
		SDRC_Structure ocu2item0 = new SDRC_Structure;
		ocu2item0.Set(
			"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
			"82.81 1 134.682"
		);
		occupation2.campItems.Insert(ocu2item0);

		SDRC_Structure ocu2item1 = new SDRC_Structure;
		ocu2item1.Set(
			"{B6307C189CCCA0B9}Prefabs/Props/Military/Sandbags/Sandbag_01_round_high_plastic.et",
			"82.792 1 137.626"
		);
		occupation2.campItems.Insert(ocu2item1);

		SDRC_Structure ocu2item2 = new SDRC_Structure;
		ocu2item2.Set(
			"{B6307C189CCCA0B9}Prefabs/Props/Military/Sandbags/Sandbag_01_round_high_plastic.et",
			"85.102 1 133.641",
			"0 114.569 0"
		);
		occupation2.campItems.Insert(ocu2item2);

		SDRC_Structure ocu2item3 = new SDRC_Structure;
		ocu2item3.Set(
			"{E1343D495AB1956E}PrefabsEditable/Auto/Structures/Military/CamoNets/Soviet/E_CamoNet_Small_Top_Soviet.et",
			"82.303 1 134.704"
		);
		occupation2.campItems.Insert(ocu2item3);
		
		//----------------------------------------------------
		SDRC_Occupation occupation3 = new SDRC_Occupation();
		occupation3.Set(
			"index 3: Car crash in an unusual place",
			"0 0 0",
			"any",
			"Car crash near ",
			"Loot is up for grabs.",
			{
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_FUELSTATION,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_PARKING,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_RADIO,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
			},
			{1, 3},
			{10, 60},
			DC_EWaypointGenerationType.RANDOM,
			DC_EWaypointMoveType.RANDOM,		
			{
				"G_RECON"
			},
			50, 1.0,
			7
		);
		conf.occupations.Insert(occupation3);

		SDRC_Loot occupation3loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"UTIL_OPTICS",
				"WEAPON_LAUNCHER",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"
			};
		occupation3loot.Set(0.7, lootItems);
		occupation3.loot = occupation3loot;
		
		SDRC_Structure ocu3item0 = new SDRC_Structure;
		ocu3item0.Set(
			"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et",
			"78.569 1 110.113",
			"0 -34.136 0"
		);
		occupation3.campItems.Insert(ocu3item0);

		SDRC_Structure ocu3item1 = new SDRC_Structure;
		ocu3item1.Set(
			"{3A9EC9CE2B10F863}PrefabsEditable/Auto/Props/VehicleParts/Tires/E_Tire_M151A2.et",
			"77.671 1 117.212"
		);
		occupation3.campItems.Insert(ocu3item1);

		SDRC_Structure ocu3item2 = new SDRC_Structure;
		ocu3item2.Set(
			"{530705FBB61026D2}Prefabs/Props/Garbage/Cardboard/Cardboard_Pile_03.et",
			"76.743 1 111.704"
		);
		occupation3.campItems.Insert(ocu3item2);

		SDRC_Structure ocu3item3 = new SDRC_Structure;
		ocu3item3.Set(
			"{8BAF6C3ACF99388E}Prefabs/Props/Garbage/Cardboard/Cardboard_Pile_05.et",
			"79.362 1 108.878"
		);
		occupation3.campItems.Insert(ocu3item3);

		SDRC_Structure ocu3item4 = new SDRC_Structure;
		ocu3item4.Set(
			"{F8CFFBA89541B0E9}PrefabsEditable/Auto/Props/Crates/E_Crate_01_ivory.et",
			"80.819 1 113.166",
			"0 28.26 0"
		);
		occupation3.campItems.Insert(ocu3item4);

		SDRC_Structure ocu3item5 = new SDRC_Structure;
		ocu3item5.Set(
			"{FA34D99C60B233F0}PrefabsEditable/Auto/Props/Wrecks/E_S105_wreck.et",
			"79.036 1 113.879"
		);
		occupation3.campItems.Insert(ocu3item5);
		
		//----------------------------------------------------
		SDRC_Occupation occupation4 = new SDRC_Occupation();
		occupation4.Set(
			"index 4: Campers with a car and a tent",
			"0 0 0",
			"any",
			"Campers near ",
			"Rob them before they leave.",
			{
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_NAME_HILL,
				EMapDescriptorType.MDT_NAME_VALLEY			
			},
			{1, 3},
			{10, 90},
			DC_EWaypointGenerationType.RANDOM,
			DC_EWaypointMoveType.RANDOM,		
			{
				"G_SPECIAL", "G_HEAVY"
			},
			50, 1.0,
			12
		);
		conf.occupations.Insert(occupation4);

		SDRC_Loot occupation4loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT", "UTIL_ATTACHMENT",
				"UTIL_OPTICS",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"
			};
		occupation4loot.Set(0.4, lootItems);
		occupation4.loot = occupation4loot;		
		
		SDRC_Structure ocu4item0 = new SDRC_Structure;
		ocu4item0.Set(
		    "{E28501E93F8EFDC0}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_FIA_uncovered.et",
		    "84.933 1 97.416", 
		    "0 -43.327 0"
		);
		occupation4.campItems.Insert(ocu4item0);
		
		SDRC_Structure ocu4item1 = new SDRC_Structure;
		ocu4item1.Set(
		    "{0B24DD72DAF499F4}PrefabsEditable/Auto/Props/Military/AmmoBoxes/EquipmentBoxStack/E_EquipmentBoxStack_US_01_V6_covered.et",
		    "80.78 1 94.665",
		    "0 89.871 0"
		);
		occupation4.campItems.Insert(ocu4item1);
		
		SDRC_Structure ocu4item2 = new SDRC_Structure;
		ocu4item2.Set(
		    "{172DD50ACF177B9E}Prefabs/Props/Military/Furniture/ChairMilitary_USSR_01.et",
		    "79.397 1 92.375",
		    "0 -38.585 0"
		);
		occupation4.campItems.Insert(ocu4item2);
		
		SDRC_Structure ocu4item3 = new SDRC_Structure;
		ocu4item3.Set(
		    "{172DD50ACF177B9E}Prefabs/Props/Military/Furniture/ChairMilitary_USSR_01.et",
		    "79.581 1 90.916",
		    "0 -152.007 0"
		);
		occupation4.campItems.Insert(ocu4item3);
		
		SDRC_Structure ocu4item4 = new SDRC_Structure;
		ocu4item4.Set(
		    "{3EC72887AEC40F22}PrefabsEditable/Auto/Props/Furniture/TableOld_01/E_TableOld_01_white.et",
		    "79.55 1 91.494"
		);
		occupation4.campItems.Insert(ocu4item4);
		
		SDRC_Structure ocu4item5 = new SDRC_Structure;
		ocu4item5.Set(
		    "{BEC236C8D6F6E783}PrefabsEditable/Auto/Props/Furniture/ChairRecreation_01/E_ChairRecreation_01_red.et",
		    "76.986 1 91.978",
		    "0 60.6 0"
		);
		occupation4.campItems.Insert(ocu4item5);
		
		SDRC_Structure ocu4item6 = new SDRC_Structure;
		ocu4item6.Set(
		    "{C768E842A6F11CEE}Prefabs/Structures/Military/Camps/TentUSSR_01/TentUSSR_01_camonet.et",
		    "78.758 0 92.718"
		);
		occupation4.campItems.Insert(ocu4item6);
		
		SDRC_Structure ocu4item7 = new SDRC_Structure;
		ocu4item7.Set(
		    "{FF3ED8B26C05A940}PrefabsEditable/Auto/Props/Military/Camps/E_LanternMilitary_US_01.et",
		    "76.758 1 92.856"
		);
		occupation4.campItems.Insert(ocu4item7);
				
		//----------------------------------------------------
		SDRC_Occupation occupation5 = new SDRC_Occupation();
		occupation5.Set(
			"index 5: A small town",
			"0 0 0",
			"any",
			"Town near ",
			"The enemies are hiding in a ghost town.",
			{
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_NAME_VALLEY			
			},
			{1, 3},
			{10, 90},
			DC_EWaypointGenerationType.RANDOM,
			DC_EWaypointMoveType.RANDOM,		
			{
				"G_SPECIAL", "G_HEAVY"
			},
			50, 1.0,
			30
		);
		conf.occupations.Insert(occupation5);

		SDRC_Loot occupation5loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT", "UTIL_ATTACHMENT",
				"UTIL_OPTICS",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"
			};
		occupation5loot.Set(0.4, lootItems);
		occupation5.loot = occupation4loot;
		
		SDRC_Structure ocu5item0 = new SDRC_Structure;
		ocu5item0.Set(
			"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et",
			"130.023 1 81.107"
		);
		occupation5.campItems.Insert(ocu5item0);
		
		SDRC_Structure ocu5item1 = new SDRC_Structure;
		ocu5item1.Set(
			"{116C488674B5A4A6}Prefabs/Structures/Commercial/FuelStations/FuelStation_E_01/Dst/FuelStation_E_01_roof_Ruin_base.et",
			"143.453 1 79.224"
		);
		occupation5.campItems.Insert(ocu5item1);

		SDRC_Structure ocu5item2 = new SDRC_Structure;
		ocu5item2.Set(
			"{215C30FCB7EF7E90}PrefabsEditable/Auto/Props/Military/WaterTanks/E_MobileWaterTank_USSR_01.et",
			"143.21 1 73.047",
			"0 -104.533 0"
		);
		occupation5.campItems.Insert(ocu5item2);

		SDRC_Structure ocu5item3 = new SDRC_Structure;
		ocu5item3.Set(
			"{325991D9993FA95B}Prefabs/Structures/Houses/Village/HouseAddon_Shed_E_01/HouseAddon_Shed_E_01.et",
			"136.378 0 91.665",
			"0 -106.094 0"
		);
		occupation5.campItems.Insert(ocu5item3);

		SDRC_Structure ocu5item4 = new SDRC_Structure;
		ocu5item4.Set(
			"{39AF1EE567E58C60}Prefabs/Structures/Houses/Village/HouseAddon_Shed_E_01/Dst/HouseAddon_Shed_E_01_Ruin_base.et",
			"135.091 1 95.696",
			"0 75.947 0"
		);
		occupation5.campItems.Insert(ocu5item4);

		SDRC_Structure ocu5item5 = new SDRC_Structure;
		ocu5item5.Set(
			"{4472D9B48597C94D}Prefabs/Structures/Houses/Village/HouseAddon_Workshop_E_01/HouseAddon_Workshop_E_01_base.et",
			"124.517 0 82.525"
		);
		occupation5.campItems.Insert(ocu5item5);

		SDRC_Structure ocu5item6 = new SDRC_Structure;
		ocu5item6.Set(
			"{46F077BDA1AF95B2}PrefabsEditable/Auto/Props/Industrial/E_BarrelFuel_01_closed.et",
			"128.008 1 71.131"
		);
		occupation5.campItems.Insert(ocu5item6);

		SDRC_Structure ocu5item7 = new SDRC_Structure;
		ocu5item7.Set(
			"{71165C5EA36C797C}PrefabsEditable/Auto/Props/Infrastructure/SnowBreakers/E_SnowBreaker_E_01_C.et",
			"137.615 1 73.961",
			"0 -41.532 0"
		);
		occupation5.campItems.Insert(ocu5item7);

		SDRC_Structure ocu5item8 = new SDRC_Structure;
		ocu5item8.Set(
			"{74BA7E97319D69C3}PrefabsEditable/Auto/Props/VehicleParts/Tires/E_Tire_Ural4320_pile.et",
			"140.87 1 93.76"
		);
		occupation5.campItems.Insert(ocu5item8);

		SDRC_Structure ocu5item9 = new SDRC_Structure;
		ocu5item9.Set(
			"{82008EE10AB80D6E}PrefabsEditable/Auto/Props/Wrecks/E_UAZ469_wreck.et",
			"139.984 1 71.414",
			"0 -135.756 0"
		);
		occupation5.campItems.Insert(ocu5item9);

		SDRC_Structure ocu5item10 = new SDRC_Structure;
		ocu5item10.Set(
			"{A3EF3195AD211CDB}PrefabsEditable/Auto/Props/Industrial/Pallets/E_Pallet_01.et",
			"130.23 1 76.851",
			"0 31.799 0"
		);
		occupation5.campItems.Insert(ocu5item10);

		SDRC_Structure ocu5item11 = new SDRC_Structure;
		ocu5item11.Set(
			"{B724CDD1316A90B1}PrefabsEditable/Auto/Props/Industrial/Pallets/E_MarsBoxContainer_01_Wood.et",
			"143.397 1 90.518",
			"0 20.223 0"
		);
		occupation5.campItems.Insert(ocu5item11);

		SDRC_Structure ocu5item12 = new SDRC_Structure;
		ocu5item12.Set(
			"{BE34698CFA200F28}Prefabs/Structures/Houses/Shed/Shed_01/Dst/Shed_01_Ruin.et",
			"134.193 1 94.635"
		);
		occupation5.campItems.Insert(ocu5item12);

		SDRC_Structure ocu5item13 = new SDRC_Structure;
		ocu5item13.Set(
			"{ED100C4FA5097FC5}Prefabs/Structures/Houses/Village/HouseAddon_Garage_E_01/HouseAddon_Garage_E_01_base.et",
			"125.158 0 75.105",
			"0 90 0"
		);
		occupation5.campItems.Insert(ocu5item13);
		
		SDRC_Structure ocu5item14 = new SDRC_Structure;
		ocu5item14.Set(
			"{0542578CA422287A}PrefabsEditable/Auto/Props/Industrial/Repair/E_VehicleGarbage_01_pile_medium.et",
			"132.441 1 85.127"
		);
		occupation5.campItems.Insert(ocu5item14);			
	}	
}