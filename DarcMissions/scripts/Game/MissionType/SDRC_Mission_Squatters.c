//Mission SDRC_Mission_Squatter.c

//------------------------------------------------------------------------------------------------
/*!
A building is guarded by AIs with loot available.
*/

const string DC_MISSIONCONFIG_FILE_SQUATTER = "dc_missionConfig_Squatter.json";
const int DC_MISSIONCONFIG_FILE_SQUATTER_JSONVER = 2;

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Squatter : SDRC_Mission
{
	private ref SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_SQUATTER);	
	private ref SDRC_SquatterConfig m_Config = new SDRC_SquatterConfig();
	private ref SDRC_Squatter m_DC_Squatter = new SDRC_Squatter();
	
	private IEntity m_Building;					//The building for the mission
	private int m_iAiCount;
	private int m_iSpawnIndex = 0;				//Counter for the AI to spawn
		
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Squatter(SDRC_EMissionType missionType, SDRC_MissionRequested request, bool staticMission = false)
	{
		//Load config
		if (!m_JsonApi.Load(m_Config, SDRC_MissionConfig.Cast(m_Config), DC_MISSIONCONFIG_FILE_SQUATTER_JSONVER))
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.ERROR_LOADING_JSON);
			return;
		}
		m_Config.LoadMissionFiles(DC_MISSIONCONFIG_FILE_SQUATTER_JSONVER);
		
		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		int idx = m_Config.GetSubMissionIdx(GetSubIdx());
		if (idx == -1)
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.WRONG_SUBIDX);
			return;
		}
		m_DC_Squatter = m_Config.subMissions[idx];
		HandleRequestGeneralVariables(m_DC_Squatter.general, request);
		SetQrfConf(m_DC_Squatter.qrf);
		
		//Set defaults
		m_iAiCount = m_DC_Squatter.ai.GetCount(GetDifficulty());
		float radius = m_Config.buildingRadius;
		array<string>buildingFilter = {};
		
		//Find a location for the mission
		vector pos = "0 0 0";
		//If pos has been set, we blindly accept it. Do basic checking for pos.
		bool obc = (IsRequested() || IsStatic());
		
		//Find a location for the mission
		if (IsRequested())
		{
			pos = request.general.pos[0];
			//If the missions is requested with a position, any building near the location will be accepted.
			buildingFilter.Insert("");
			radius = 10;	//Try to find the nearest building.
		}
		else
		{
			pos = SDRC_MissionPosHelper.SelectMissionPos(m_DC_Squatter.general.pos, m_DC_Squatter.general.size, obc, m_DC_Squatter.general.locationTypes);
			buildingFilter = m_DC_Squatter.buildingNames;
			if (m_DC_Squatter.general.locationTypes.IsEmpty())
			{
				//If no locationTypes defined, we search for any building matching on the map
				radius = -1;
			}
		}

		SDRC_EMissionError missionError = SDRC_MissionPosHelper.IsValidMissionPos(pos, obc, IsRequested());
		if (missionError != SDRC_EMissionError.NONE)
		{
			pos = "0 0 0";
			SetState(SDRC_EMissionState.FAILED, missionError);
			return;
		}			
		
		//Find the mission house
		m_Building = SDRC_MissionHelper.FindMissionBuilding(pos, buildingFilter, radius);
		if (m_Building)
		{
			//SDRC_Misc.GetSimpleEntityName(m_Building.GetPrefabData().GetPrefabName())
			pos = m_Building.GetOrigin();
		}
		else //No suitable location found.
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.SUITABLE_BUILDING_NOT_FOUND);
			return;
		}			
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Squatter.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_DC_Squatter.general);		
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == SDRC_EMissionState.SPAWN)
		{
			MissionSpawn();
			GetGame().GetCallqueue().CallLater(MissionRun, SDRC_Conf.SPAWN_ITEM_DELAY);		//Spawn stuff slowly
			//NOTE: ACTIVE set inside MissionSpawn()
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
		//Spawn AI one by one. Sets missions active once ready.
		if (m_iSpawnIndex < m_iAiCount)
		{
			//Each AI is spawned in to its own group to be able to give individual waypoints to a character
			SCR_AIGroup group = SDRC_AIHelper.SpawnAIInBuilding(m_Building, m_DC_Squatter.ai.types.GetRandomElement(), GetFaction(), m_DC_Squatter.ai.GetSkill(GetDifficulty()), m_DC_Squatter.ai.GetPerception(GetDifficulty()), );
			if (group)
			{
				m_Groups.Insert(group);
			}
			m_iSpawnIndex++;
		}
		else
		{
			IEntity entity = SDRC_SpawnHelper.SpawnItemInBuilding(m_Building, m_DC_Squatter.lootBox);
			if (entity)
			{
				m_EntityList.Insert(entity);
				m_DC_Squatter.loot.box = entity;
			}
			else
			{
				SDRC_Log.Add("[SDRC_Mission_Squatter:MissionSpawn] " +  GetId() + " : Could not spawn loot box: " + m_DC_Squatter.lootBox, LogLevel.ERROR);								
			}
		
			SetState(SDRC_EMissionState.ACTIVE);			
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	override void DoWin()
	{	
		ref SDRC_Loot loot = m_DC_Squatter.loot;
		SDRC_MissionHelper.AddLoot(loot.box, loot.items, loot.itemChance, GetDifficulty());
		super.DoWin();
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_Squatter : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();
	ref SDRC_MissionConfigQrf qrf = null;
	//Mission specific
	ref array<string> buildingNames = {};
	string lootBox = "";					//The loot box
	ref SDRC_Loot loot = null;
	
	void Set(array<string> buildingNames_, string lootBox_)
	{
		buildingNames = buildingNames_;	
		lootBox = lootBox_;
	}	
}
	
//------------------------------------------------------------------------------------------------
class SDRC_SquatterConfig : SDRC_MissionConfig
{
	//Mission specific
	int buildingRadius;									//The radius to search for suitable buildings.
	ref array<ref SDRC_Squatter> subMissions = {};		//List of squatters

	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_SquatterConfig data = SDRC_SquatterConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------	
	override void LoadMissionFiles(int ver)
	{
		//Load mission files
		foreach (string missionFile : missionFiles)
		{
			SDRC_JsonApi2 jsonApi = new SDRC_JsonApi2(missionFile);
			SDRC_SquatterConfig conf = new SDRC_SquatterConfig();
			
			if (jsonApi.Load(conf, SDRC_MissionConfig.Cast(conf), ver, false))
			{
				foreach (SDRC_Squatter subMission : conf.subMissions)
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
	}	
	
	//------------------------------------------------------------------------------------------------	
	int GetSubMissionIdx(int subIdx)
	{
		int idx = -1;
		foreach (int i, SDRC_Squatter subMission : subMissions)
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
		missionType = SDRC_EMissionType.SQUATTERS;
		
		//Default		
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		missionList = {0,1,2,2,3,3,3,4,5,5,5};
		//Mission specific
		buildingRadius = 800;
		//----------------------------------------------------
		subMissions.Insert(Squatter0());				
		subMissions.Insert(Squatter1());				
		subMissions.Insert(Squatter2());				
		subMissions.Insert(Squatter3());				
		subMissions.Insert(Squatter4());				
		subMissions.Insert(Squatter5());				
	};
			
	//----------------------------------------------------
	SDRC_Squatter Squatter0()
	{
		ref SDRC_Squatter squatter = new SDRC_Squatter();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Squatters near %l.",
			"Building has squatters with loot",		
			"The loot has been secured.",
			"Squatters cleaned the house and left you nothing.",);
		squatter.general.Set(
			0, "index 0: Squatters in cities",
			{"0 0 0"}, 0, 
			{
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_TOWN, 
				EMapDescriptorType.MDT_AIRPORT,
			},
			"any",
			{message},		
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP,
			{SDRC_EDifficulty.RANDOM},
			0		
		);
		squatter.ai.Set(
			{3, 6},
			{"C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", 
			 "C_OFFICER"
			},
			50, 0.6,
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE
		);
		squatter.Set(
			{"ShopModern_", "Villa_", "MunicipalOffice_", "PubVillage_", "Office_E_", "MountainHotel_"},
			"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et"
		);
		
		ref SDRC_MissionConfigQrf qrf = new SDRC_MissionConfigQrf();		
		qrf.Set(
			{0, 3, }, SDRC_EMissionSuccess.WIN,
			0.2, {30, 240}
		);
		squatter.qrf = qrf;		
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT", "UTIL_ATTACHMENT",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_BAG", 
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
				"CLOTHING_HEADGEAR", "CLOTHING_UNIFORM",
			};
		loot.Set(0.5, lootItems);
		squatter.loot = loot;

		return squatter;	
	};
					
	//----------------------------------------------------
	SDRC_Squatter Squatter1()
	{
		ref SDRC_Squatter squatter = new SDRC_Squatter();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Enemy in %l",
			"Control tower is being guarded.",		
			"You did it! Control in %l has been restored.",
			"Control in %l has been lost.",);
		squatter.general.Set(
			1, "index 1: Squatters in control towers",
			{"0 0 0"}, 0,
			{
				//We pick any building that matches and ignore location
			},
			"any",
			{message},		
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP,
			{SDRC_EDifficulty.RANDOM},
			0		
		);
		squatter.ai.Set(
			{6, 10},
			{"C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", 
			 "C_HEAVY", "C_HEAVY", 
			 "C_LAUNCHER", "C_MEDIC", "C_OFFICER"
			},
			50, 0.8,
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE
		);
		squatter.Set(
			{"ControlTowerMilitary_"},
			"{D15A294D5138ADFF}Prefabs/Props/Military/AmmoBoxes/US/LootEquipmentBoxWooden_Equipment_01_US.et"
		);
		
		ref SDRC_MissionConfigQrf qrf = new SDRC_MissionConfigQrf();		
		qrf.Set(
			{30, 31, 32, 33}, SDRC_EMissionSuccess.WIN,
			0.7, {30, 240}
		);
		squatter.qrf = qrf;		
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE", "WEAPON_RIFLE",
				"WEAPON_RIFLE_BIG", "WEAPON_RIFLE_BIG", 
				"WEAPON_LAUNCHER", "WEAPON_LAUNCHER", 
				"WEAPON_SHOTGUN", "WEAPON_SHOTGUN", 				
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"UTIL_OPTIC", "UTIL_OPTIC",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL",
				"CLOTHING_HEADGEAR", "CLOTHING_HEADGEAR", 
				"CLOTHING_UNIFORM", "CLOTHING_UNIFORM",
			};
		loot.Set(0.8, lootItems);
		squatter.loot = loot;		
	
		return squatter;	
	};
					
	//----------------------------------------------------
	SDRC_Squatter Squatter2()
	{
		ref SDRC_Squatter squatter = new SDRC_Squatter();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Guards around %l",
			"Military location has loot to steal.",		
			"Guards have been eliminated.",
			"Military has collected the loot and left.",);
		squatter.general.Set(
			2, "index 2: Squatters in military locations",
			{"0 0 0"}, 0,
			{
				//We pick any building that matches and ignore location
			},
			"any",
			{message},		
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP,
			{SDRC_EDifficulty.RANDOM},
			0		
		);
		squatter.ai.Set(
			{4, 10},
			{"C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", 
			 "C_HEAVY", "C_HEAVY", "C_HEAVY", 
			 "C_LAUNCHER", "C_MEDIC", "C_OFFICER"
			},
			50, 0.8,
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE
		);
		squatter.Set(
			{"Office_E_", "Barracks_01_", "Barracks_E_02_"},
			"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et"
		);
		
		ref SDRC_MissionConfigQrf qrf = new SDRC_MissionConfigQrf();		
		qrf.Set(
			{30, 31, 32}, SDRC_EMissionSuccess.WIN,
			0.6, {30, 240}
		);
		squatter.qrf = qrf;		
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE", "WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"WEAPON_SHOTGUN",
				"UTIL_ATTACHMENT",
				"UTIL_OPTIC", "UTIL_OPTIC",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
			};
		loot.Set(0.5, lootItems);
		squatter.loot = loot;
		
		return squatter;	
	};
					
	//----------------------------------------------------
	SDRC_Squatter Squatter3()
	{
		ref SDRC_Squatter squatter = new SDRC_Squatter();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Industrial area near %l",
			"Military has seized control of an industrial area. Don't shoot the civilians.",
			"Main enemy forces have been destroyed. Continue to keep an eye for rogues.",
			"%l is in the hands of the enemy.",);
		squatter.general.Set(
			3, "index 3: Military in industrial areas",
			{"0 0 0"}, 0,
			{
				//We pick any building that matches and ignore location
			},
			"any",
			{message},		
			SDRC_EMissionWinCondition.AI_KILL_50,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP,
			{SDRC_EDifficulty.RANDOM},
			0		
		);
		squatter.ai.Set(
			{2, 6},
			{"C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", 
			 "C_HEAVY", "C_HEAVY", "C_HEAVY", 
			 "C_RECON", "C_RECON", "C_RECON", 
			 "C_OFFICER"
			},
			50, 0.8,
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE
		);
		squatter.Set(
			{"DieselPowerPlant_", "CowShed_", "FireStation_", "Warehouse_", "TransformerStation_", "FactoryHall_"},
			"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et"
		);
		
		ref SDRC_MissionConfigQrf qrf = new SDRC_MissionConfigQrf();		
		qrf.Set(
			{0, 1, 2, 3, }, SDRC_EMissionSuccess.WIN,
			0.5, {30, 240}
		);
		squatter.qrf = qrf;		
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"WEAPON_SHOTGUN", "WEAPON_SHOTGUN", 
				"UTIL_ATTACHMENT", "UTIL_ATTACHMENT", "UTIL_ATTACHMENT",			
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL",
				"UTIL_ATTACHMENT", "UTIL_ATTACHMENT", "UTIL_ATTACHMENT", 
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
			};
		loot.Set(0.6, lootItems);
		squatter.loot = loot;
		
		return squatter;	
	};
					
	//----------------------------------------------------
	SDRC_Squatter Squatter4()
	{
		ref SDRC_Squatter squatter = new SDRC_Squatter();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Sanctuary visitors near %l",
			"Holy night, holy loot.",		
			"Your success will be remembered.",
			"Your effort has been struck down.",);
		squatter.general.Set(
			4, "index 4: Enemy in churches and similar",
			{"0 0 0"}, 0,
			{
				//We pick any building that matches and ignore location
			},
			"any",
			{message},		
			SDRC_EMissionWinCondition.AI_KILL_RANDOM,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP,
			{SDRC_EDifficulty.RANDOM},
			0		
		);
		squatter.ai.Set(
			{3, 7},
			{"C_RIFLEMAN"},
			50, 0.8,		
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE
		);
		squatter.Set(
			{"Church_", "ChurchSmall_", "Mosque_", "Minaret"},
			"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et"
		);
		
		ref SDRC_MissionConfigQrf qrf = new SDRC_MissionConfigQrf();		
		qrf.Set(
			{0, 3, 4, }, SDRC_EMissionSuccess.WIN,
			0.3, {30, 240}
		);
		squatter.qrf = qrf;		
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_OPTIC", "UTIL_OPTIC",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_BAG", "GEAR_BAG", 
				"CLOTHING_HEADGEAR", "CLOTHING_HEADGEAR", 
				"CLOTHING_UNIFORM",	"CLOTHING_UNIFORM",	"CLOTHING_UNIFORM",				
			};
		loot.Set(0.4, lootItems);
		squatter.loot = loot;
		
		return squatter;	
	};
					
	//----------------------------------------------------
	SDRC_Squatter Squatter5()
	{
		ref SDRC_Squatter squatter = new SDRC_Squatter();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Burglars seen near %l",
			"Go rob the robbers.",		
			"%l is open for business once again.",
			"Everything has been stolen.",);
		squatter.general.Set(
			5, "index 5: Shops and houses",
			{"0 0 0"}, 0,
			{
				//We pick any building that matches and ignore location
			},
			"any",
			{message},		
			SDRC_EMissionWinCondition.AI_KILL_50,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP,
			{SDRC_EDifficulty.RANDOM},
			0		
		);
		squatter.ai.Set(
			{2, 6},
			{"C_SPECIAL"},
			30, 0.3,
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE
		);
		squatter.Set(
			{"ShopModern_", "House_Town_", "House_Village_", "FarmHouse_", "House_Wooden_"},
//			{"{6E38A03731914D8F}Prefabs/Structures/Houses/Village/House_Village_E_1I01/House_Village_E_1I01_beige_V2.et"},
//			{"ControlTower"},
//			{"House_Village_E_1I01"},
//			{"House_Village_E_"},
//			{"Villa_E_"},
//			{"Villa_E_", "House_Village_E_", "ControlTower"},
//			{"ShopModern_", "House_Town_", "House_Village_", "FarmHouse_", "House_Wooden_", "Church_"},
			"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et"
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"UTIL_ATTACHMENT", "UTIL_ATTACHMENT", "UTIL_ATTACHMENT", "UTIL_ATTACHMENT",
				"WEAPON_SHOTGUN", "WEAPON_SHOTGUN", 
				"UTIL_OPTIC",
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_BAG", "GEAR_BAG", 
				"CLOTHING_HEADGEAR", "CLOTHING_HEADGEAR", "CLOTHING_HEADGEAR", 
				"CLOTHING_UNIFORM",
			};
		loot.Set(0.4, lootItems);
		squatter.loot = loot;		

		return squatter;						
	}
}