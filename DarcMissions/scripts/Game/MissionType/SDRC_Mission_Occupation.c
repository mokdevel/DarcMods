//Mission SDRC_Mission_Occupation.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns groups to defend a location
*/

const string DC_MISSIONCONFIG_FILE_OCCUPATION = "dc_missionConfig_Occupation.json";
const int DC_MISSIONCONFIG_FILE_OCCUPATION_JSONVER = 2;

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Occupation : SDRC_Mission
{
	private ref SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_OCCUPATION);	
	private ref SDRC_OccupationConfig m_Config = new SDRC_OccupationConfig();	
	private ref SDRC_Camp m_DC_Occupation = new SDRC_Camp();
	
	private int m_iSpawnIndex = 0;						//Counter for the item to spawn
	private float m_fSpawnRotation = 0;					//Rotation of the camp for random locations.

	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Occupation(SDRC_EMissionType missionType, SDRC_MissionRequested request, bool staticMission = false)
	{
		//Load config
		if (!m_JsonApi.Load(m_Config, SDRC_MissionConfig.Cast(m_Config), DC_MISSIONCONFIG_FILE_OCCUPATION_JSONVER))
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.ERROR_LOADING_JSON);
			return;
		}
		m_Config.LoadMissionFiles(DC_MISSIONCONFIG_FILE_OCCUPATION_JSONVER);
		
		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		int idx = m_Config.GetSubMissionIdx(GetSubIdx());
		if (idx == -1)
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.WRONG_SUBIDX);
			return;
		}
		m_DC_Occupation = m_Config.subMissions[idx];	
		HandleRequestGeneralVariables(m_DC_Occupation.general, request);
		SetQrfConf(m_DC_Occupation.qrf);
		
		//Find position
		vector pos = "0 0 0";
		//If pos has been set, we blindly accept it. Do basic checking for pos.
		bool obc = (IsRequested() || IsStatic());
		
		//For requested missions we want have it as close as possible in the requested place.
		if (IsRequested())
		{
			pos = request.general.pos[0];
		}
		else
		{				
			pos = SDRC_MissionPosHelper.SelectMissionPos(m_DC_Occupation.general.pos, m_DC_Occupation.general.size, obc, m_DC_Occupation.general.locationTypes, SDRC_Conf.POSITION_RANDOMIZATION);	//Randomization added to avoid the same position always
		}
		
		SDRC_EMissionError missionError = SDRC_MissionPosHelper.IsValidMissionPos(pos, obc, IsRequested());
		if (missionError != SDRC_EMissionError.NONE)
		{
			pos = "0 0 0";
			SetState(SDRC_EMissionState.FAILED, missionError);
			return;
		}			

		//Camps are randomly rotated
		m_fSpawnRotation = SDRC_Misc.RandomFloat(0, 360);
		SDRC_SpawnHelper.SetStructuresToOrigo(m_DC_Occupation.campItems);
				
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Occupation.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_DC_Occupation.general);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == SDRC_EMissionState.SPAWN)
		{
			MissionSpawn();
			GetGame().GetCallqueue().CallLater(MissionRun, SDRC_Conf.SPAWN_ITEM_DELAY);		//Spawn stuff slowly
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
		
		ready = SDRC_CampHelper.Spawn(this, m_iSpawnIndex, m_DC_Occupation, m_fSpawnRotation, m_Config.disableArsenal);
		m_iSpawnIndex++;			
		
		if ( (ready) && (GetState() != SDRC_EMissionState.FAILED) )
		{
			SetState(SDRC_EMissionState.ACTIVE);
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	override void DoWin()
	{	
		SDRC_CampHelper.AddCampLoot(m_DC_Occupation, GetDifficulty());
		super.DoWin();
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_OccupationConfig : SDRC_MissionConfig
{
	//Mission specific	
	ref array<ref SDRC_Camp> subMissions = {};		//List of occupations
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_OccupationConfig data = SDRC_OccupationConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------	
	override void LoadMissionFiles(int ver)
	{
		//Load mission files
		foreach (string missionFile : missionFiles)
		{
			SDRC_JsonApi2 jsonApi = new SDRC_JsonApi2(missionFile);
			SDRC_OccupationConfig conf = new SDRC_OccupationConfig();
			
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
		
		super.LoadMissionFiles(ver);
	}

	//------------------------------------------------------------------------------------------------
	override void CreateMissionFiles()
	{
		super.CreateMissionFiles();
		
		SDRC_JsonApi2 jsonApi = new SDRC_JsonApi2(SDRC_OccupationConfig_010.GetFileName());				
		SDRC_OccupationConfig_010 conf = new SDRC_OccupationConfig_010();
		jsonApi.Load(conf, SDRC_MissionConfig.Cast(conf), DC_MISSIONCONFIG_FILE_OCCUPATION_JSONVER, silent: true);
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
		missionType = SDRC_EMissionType.OCCUPATION;
		
		//Default		
		disableArsenal = true;
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		missionList = {0,0,0,1,1,1,1,2,2,2,2,2,3,3,3,4,5};		
//		missionFiles.Insert("dc_missionConfig_Occupation_010_horror.json");
		//Mission specific		
		//----------------------------------------------------
		subMissions.Insert(Occupation0());
		subMissions.Insert(Occupation1());
		subMissions.Insert(Occupation2());
		subMissions.Insert(Occupation3());
		subMissions.Insert(Occupation4());
		subMissions.Insert(Occupation5());
	};
	
	//----------------------------------------------------
	SDRC_Camp Occupation0()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Guards patroling near %l",
			"Avoid the location. Loot has already been lost.",
			"Guard patrol eliminated.",
			"The patrol kept %l safe from you. Pathetic.",);
		occupation.general.Set(
			0, "index 0: Mission in villages and local areas.",
			{"0 0 0"}, 3,
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_OCCUPATION_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{1, 2},
			{"G_RECON", "G_LIGHT"},
			50, 1.0,
			{50, 300},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		return occupation;
	};
		
	//----------------------------------------------------
	SDRC_Camp Occupation1()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Bandit camp near %l",
			"Bandits are protecting their valuable loot.",
			"Camp cleared. Take the loot and leave.",
			"The camp near %l was never destroyed. Bandits rule!",);
		occupation.general.Set(
			1, "index 1: Bandit camp spawning to non city areas",
			{"0 0 0"}, 6,
			{
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
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
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_OCCUPATION_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{1, 2},
			{"G_ADMIN", "G_LIGHT", "G_LIGHT"},
			50, 1.0,
			{25, 100},
			SDRC_EWaypointGenerationType.SCATTERED,//RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		ref SDRC_MissionConfigQrf qrf = new SDRC_MissionConfigQrf();		
		qrf.Set(
			{1, 4, 5, }, SDRC_EMissionSuccess.WIN,
			0.1, {30, 240}
		);
		occupation.qrf = qrf;		
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"UTIL_ATTACHMENT", "UTIL_ATTACHMENT",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_BAG", 
				"CLOTHING_HEADGEAR", "CLOTHING_UNIFORM",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			};
		loot.Set(0.7, lootItems);
		occupation.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
			"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
			"100.47 1 144.562"
		);
		occupation.campItems.Insert(item_0);

		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
			"{39C308BBB5945B85}Prefabs/Props/Military/Furniture/ChairMilitary_US_02.et",
			"96.885 1 144.184",
			"0 119.334 0"
		);
		occupation.campItems.Insert(item_1);

		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
			"{D9842C11742C00CF}Prefabs/Props/Civilian/Fireplace_01.et",
			"98.235 1 143.464"
		);
		occupation.campItems.Insert(item_2);

		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
			"{EBC3D311A1B64FE6}PrefabsEditable/Auto/Structures/Military/Camps/TentSmallUS_01/E_TentSmallUS_01.et",
			"96.653 1 146.601",
			"0 -39.208 0"
		);
		occupation.campItems.Insert(item_3);	
		
		return occupation;
	};

	//----------------------------------------------------
	SDRC_Camp Occupation2()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Occupation in %l",
			"City is being occupied.",
			"%l is free again!",
			"The enemy was stronger this time.",);
		occupation.general.Set(
			2, "index 2: Occupation that will spawn mainly to cities and towns.",
			{"0 0 0"}, 10,
			{
				EMapDescriptorType.MDT_NAME_CITY, EMapDescriptorType.MDT_NAME_CITY, EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY, EMapDescriptorType.MDT_NAME_CITY, EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_NAME_VILLAGE, EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_TOWN, 
				EMapDescriptorType.MDT_AIRPORT,
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_OCCUPATION_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{3, 5},
			{"G_LIGHT", "G_LIGHT", "G_LIGHT", 
			 "G_ADMIN", "G_HEAVY", "G_LAUNCHER", "G_MEDICAL"
			},
			30, 0.6,
			{30, 200},
			SDRC_EWaypointGenerationType.RADIUS,
			SDRC_EWaypointMoveType.RANDOM,		
		);

		ref SDRC_MissionConfigQrf qrf = new SDRC_MissionConfigQrf();		
		qrf.Set(
			{2, 4, 30, 31, 32, 33, 60}, SDRC_EMissionSuccess.WIN,
			0.7, {30, 240}
		);
		occupation.qrf = qrf;		
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE",
				"WEAPON_RIFLE_BIG", 
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"UTIL_OPTIC", "UTIL_OPTIC",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_BAG", 
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
			};
		loot.Set(0.9, lootItems);
		occupation.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
			"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
			"82.81 1 134.682"
		);
		occupation.campItems.Insert(item_0);

		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
			"{B6307C189CCCA0B9}Prefabs/Props/Military/Sandbags/Sandbag_01_round_high_plastic.et",
			"82.792 1 137.626"
		);
		occupation.campItems.Insert(item_1);

		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
			"{B6307C189CCCA0B9}Prefabs/Props/Military/Sandbags/Sandbag_01_round_high_plastic.et",
			"85.102 1 133.641",
			"0 114.569 0"
		);
		occupation.campItems.Insert(item_2);

		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
			"{E1343D495AB1956E}PrefabsEditable/Auto/Structures/Military/CamoNets/Soviet/E_CamoNet_Small_Top_Soviet.et",
			"82.303 1 134.704"
		);
		occupation.campItems.Insert(item_3);
		
		return occupation;
	};

	//----------------------------------------------------
	SDRC_Camp Occupation3()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Car crash near %l",
			"Loot is up for grabs.",
			"You stopped the get away. Enjoy the spoils.",
			"Car was fixed and the loot was lost.",);
		occupation.general.Set(
			3, "index 3: Car crash in an unusual place",
			{"0 0 0"}, 6,
			{
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_FUELSTATION,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_PARKING,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_RADIO,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_OCCUPATION_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{2, 3},
			{"G_RECON"},
			50, 1.0,
			{10, 60},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.RANDOM,		
		);

		ref SDRC_MissionConfigQrf qrf = new SDRC_MissionConfigQrf();		
		qrf.Set(
			{0, 2, 3, }, SDRC_EMissionSuccess.WIN,
			0.5, {30, 240}
		);
		occupation.qrf = qrf;		
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE", "WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_SHOTGUN", 
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"UTIL_OPTIC",
				"WEAPON_LAUNCHER", "WEAPON_LAUNCHER",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
			};
		loot.Set(0.7, lootItems);
		occupation.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
			"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et",
			"78.569 1 110.113",
			"0 -34.136 0"
		);
		occupation.campItems.Insert(item_0);

		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
			"{3A9EC9CE2B10F863}PrefabsEditable/Auto/Props/VehicleParts/Tires/E_Tire_M151A2.et",
			"77.671 1 117.212"
		);
		occupation.campItems.Insert(item_1);

		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
			"{530705FBB61026D2}Prefabs/Props/Garbage/Cardboard/Cardboard_Pile_03.et",
			"76.743 1 111.704"
		);
		occupation.campItems.Insert(item_2);

		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
			"{8BAF6C3ACF99388E}Prefabs/Props/Garbage/Cardboard/Cardboard_Pile_05.et",
			"79.362 1 108.878"
		);
		occupation.campItems.Insert(item_3);

		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
			"{F8CFFBA89541B0E9}PrefabsEditable/Auto/Props/Crates/E_Crate_01_ivory.et",
			"80.819 1 113.166",
			"0 28.26 0"
		);
		occupation.campItems.Insert(item_4);

		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
			"{FA34D99C60B233F0}PrefabsEditable/Auto/Props/Wrecks/E_S105_wreck.et",
			"79.036 1 113.879"
		);
		occupation.campItems.Insert(item_5);
		
		return occupation;
	};
	
	//----------------------------------------------------
	SDRC_Camp Occupation4()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Campers near %l",
			"Rob them before they leave.",
			"The camp is yours. Enjoy the loot and relax.",
			"The camp was packed and the campers left with their car.",);
		occupation.general.Set(
			4, "index 4: Campers with a car and a tent",
			{"0 0 0"}, 12,
			{
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_NAME_HILL,
				EMapDescriptorType.MDT_NAME_VALLEY,		
				EMapDescriptorType.MDT_CHURCH,
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_OCCUPATION_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{2, 2, 2, 3, 3, 30, 31},
			{"G_SPECIAL", "G_HEAVY"},
			50, 1.0,
			{10, 90},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.RANDOM,		
		);

		ref SDRC_MissionConfigQrf qrf = new SDRC_MissionConfigQrf();		
		qrf.Set(
			{0, 3, 6, }, SDRC_EMissionSuccess.WIN,
			0.1, {30, 240}
		);
		occupation.qrf = qrf;		
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"WEAPON_SHOTGUN", "WEAPON_SHOTGUN", 
				"UTIL_ATTACHMENT", "UTIL_ATTACHMENT",
				"UTIL_OPTIC",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_BAG", "GEAR_BAG", 
				"GEAR_HEADGEAR", "GEAR_HEADGEAR", 			
			};
		loot.Set(0.4, lootItems);
		occupation.loot = loot;		
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{E28501E93F8EFDC0}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_FIA_uncovered.et",
		    "84.933 1 97.416", 
		    "0 -43.327 0"
		);
		occupation.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{0B24DD72DAF499F4}PrefabsEditable/Auto/Props/Military/AmmoBoxes/EquipmentBoxStack/E_EquipmentBoxStack_US_01_V6_covered.et",
		    "80.78 1 94.665",
		    "0 89.871 0"
		);
		occupation.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{172DD50ACF177B9E}Prefabs/Props/Military/Furniture/ChairMilitary_USSR_01.et",
		    "79.397 1 92.375",
		    "0 -38.585 0"
		);
		occupation.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{172DD50ACF177B9E}Prefabs/Props/Military/Furniture/ChairMilitary_USSR_01.et",
		    "79.581 1 90.916",
		    "0 -152.007 0"
		);
		occupation.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{3EC72887AEC40F22}PrefabsEditable/Auto/Props/Furniture/TableOld_01/E_TableOld_01_white.et",
		    "79.55 1 91.494"
		);
		occupation.campItems.Insert(item_4);
		
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{BEC236C8D6F6E783}PrefabsEditable/Auto/Props/Furniture/ChairRecreation_01/E_ChairRecreation_01_red.et",
		    "76.986 1 91.978",
		    "0 60.6 0"
		);
		occupation.campItems.Insert(item_5);
		
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{C768E842A6F11CEE}Prefabs/Structures/Military/Camps/TentUSSR_01/TentUSSR_01_camonet.et",
		    "78.758 0 92.718"
		);
		occupation.campItems.Insert(item_6);
		
		ref SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
		    "{FF3ED8B26C05A940}PrefabsEditable/Auto/Props/Military/Camps/E_LanternMilitary_US_01.et",
		    "76.758 1 92.856"
		);
		occupation.campItems.Insert(item_7);

		return occupation;
	};
				
	//----------------------------------------------------
	SDRC_Camp Occupation5()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Settlement near %l",
			"The enemies are hiding in a ghost town.",
			"The ghost town neat %l has been cleared.",
			"Enemies have left with the loot. Shame on you.",);
		occupation.general.Set(
			5, "index 5: A small town",
			{"0 0 0"}, 30,
			{
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_NAME_VALLEY			
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_OCCUPATION_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{2, 3, 30, },
			{"G_SPECIAL", "G_HEAVY"},
			50, 1.0,
			{10, 90},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.RANDOM,		
		);

		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", "WEAPON_RIFLE",
				"WEAPON_MG",
				"WEAPON_SHOTGUN", 
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT", "UTIL_ATTACHMENT",
				"UTIL_OPTIC",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"CLOTHING_HEADGEAR", "CLOTHING_HEADGEAR", 
				"CLOTHING_UNIFORM", "CLOTHING_UNIFORM",
			};
		loot.Set(0.4, lootItems);
		occupation.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
			"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et",
			"130.023 1 81.107"
		);
		occupation.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
			"{116C488674B5A4A6}Prefabs/Structures/Commercial/FuelStations/FuelStation_E_01/Dst/FuelStation_E_01_roof_Ruin_base.et",
			"143.453 1 79.224"
		);
		occupation.campItems.Insert(item_1);

		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
			"{215C30FCB7EF7E90}PrefabsEditable/Auto/Props/Military/WaterTanks/E_MobileWaterTank_USSR_01.et",
			"143.21 1 73.047",
			"0 -104.533 0"
		);
		occupation.campItems.Insert(item_2);

		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
			"{325991D9993FA95B}Prefabs/Structures/Houses/Village/HouseAddon_Shed_E_01/HouseAddon_Shed_E_01.et",
			"136.378 0 91.665",
			"0 -106.094 0"
		);
		occupation.campItems.Insert(item_3);

		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
			"{39AF1EE567E58C60}Prefabs/Structures/Houses/Village/HouseAddon_Shed_E_01/Dst/HouseAddon_Shed_E_01_Ruin_base.et",
			"135.091 1 95.696",
			"0 75.947 0"
		);
		occupation.campItems.Insert(item_4);

		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
			"{4472D9B48597C94D}Prefabs/Structures/Houses/Village/HouseAddon_Workshop_E_01/HouseAddon_Workshop_E_01_base.et",
			"124.517 0 82.525"
		);
		occupation.campItems.Insert(item_5);

		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
			"{46F077BDA1AF95B2}PrefabsEditable/Auto/Props/Industrial/E_BarrelFuel_01_closed.et",
			"128.008 1 71.131"
		);
		occupation.campItems.Insert(item_6);

		ref SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
			"{71165C5EA36C797C}PrefabsEditable/Auto/Props/Infrastructure/SnowBreakers/E_SnowBreaker_E_01_C.et",
			"137.615 1 73.961",
			"0 -41.532 0"
		);
		occupation.campItems.Insert(item_7);

		ref SDRC_Structure item_8 = new SDRC_Structure();
		item_8.Set(
			"{74BA7E97319D69C3}PrefabsEditable/Auto/Props/VehicleParts/Tires/E_Tire_Ural4320_pile.et",
			"140.87 1 93.76"
		);
		occupation.campItems.Insert(item_8);

		ref SDRC_Structure item_9 = new SDRC_Structure();
		item_9.Set(
			"{82008EE10AB80D6E}PrefabsEditable/Auto/Props/Wrecks/E_UAZ469_wreck.et",
			"139.984 1 71.414",
			"0 -135.756 0"
		);
		occupation.campItems.Insert(item_9);

		ref SDRC_Structure item_10 = new SDRC_Structure();
		item_10.Set(
			"{A3EF3195AD211CDB}PrefabsEditable/Auto/Props/Industrial/Pallets/E_Pallet_01.et",
			"130.23 1 76.851",
			"0 31.799 0"
		);
		occupation.campItems.Insert(item_10);

		ref SDRC_Structure item_11 = new SDRC_Structure();
		item_11.Set(
			"{B724CDD1316A90B1}PrefabsEditable/Auto/Props/Industrial/Pallets/E_MarsBoxContainer_01_Wood.et",
			"143.397 1 90.518",
			"0 20.223 0"
		);
		occupation.campItems.Insert(item_11);

		ref SDRC_Structure item_12 = new SDRC_Structure();
		item_12.Set(
			"{BE34698CFA200F28}Prefabs/Structures/Houses/Shed/Shed_01/Dst/Shed_01_Ruin.et",
			"134.193 1 94.635"
		);
		occupation.campItems.Insert(item_12);

		ref SDRC_Structure item_13 = new SDRC_Structure();
		item_13.Set(
			"{ED100C4FA5097FC5}Prefabs/Structures/Houses/Village/HouseAddon_Garage_E_01/HouseAddon_Garage_E_01_base.et",
			"125.158 0 75.105",
			"0 90 0"
		);
		occupation.campItems.Insert(item_13);
		
		ref SDRC_Structure item_14 = new SDRC_Structure();
		item_14.Set(
			"{0542578CA422287A}PrefabsEditable/Auto/Props/Industrial/Repair/E_VehicleGarbage_01_pile_medium.et",
			"132.441 1 85.127"
		);
		occupation.campItems.Insert(item_14);

		return occupation;
	}			
}