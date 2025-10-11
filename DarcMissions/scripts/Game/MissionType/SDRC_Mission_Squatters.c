//Mission SDRC_Mission_Squatter.c

//------------------------------------------------------------------------------------------------
/*!
A building is guarded by AIs with loot available.
*/

const string DC_MISSIONCONFIG_FILE_SQUATTER = "dc_missionConfig_Squatter.json";

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Squatter : SDRC_Mission
{
	private ref SDRC_SquatterJsonApi m_SquatterJsonApi = new SDRC_SquatterJsonApi(DC_MISSIONCONFIG_FILE_SQUATTER);	
	private ref SDRC_SquatterConfig m_Config = new SDRC_SquatterConfig();
	private ref SDRC_Squatter m_DC_Squatter = new SDRC_Squatter();
	
	private IEntity m_Building;					//The building for the mission
	private int m_iAiCount;
	private int m_iSpawnIndex = 0;				//Counter for the AI to spawn
		
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Squatter(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Load config
		m_SquatterJsonApi.CreateMissionFiles();
		m_SquatterJsonApi.Load();
		m_SquatterJsonApi.LoadMissionFiles();	
		m_Config = m_SquatterJsonApi.conf;
		
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
		
		//Set defaults
		m_iAiCount = m_DC_Squatter.ai.GetCount(m_DC_Squatter.general.difficulty);
		float radius = 100;					//Default size for the radius. Mainly for requested missions to find the nearest building.
		array<string>buildingFilter = {};
		
		vector pos = SDRC_MissionHelper.SelectMissionPos(m_DC_Squatter.general.pos, m_DC_Squatter.general.size, m_DC_Squatter.general.locationTypes);
		
		//Find a location for the mission
		if (IsRequested())
		{
			//If the missions is requested with a position, any building near the location will be accepted.
			buildingFilter.Insert("");
			radius = 10;	//Try to find the nearest building.
		}
		else
		{				
			radius = m_Config.buildingRadius;
			buildingFilter = m_DC_Squatter.buildingNames;
			
			if (pos == "0 0 0")
			{
				//If no locationTypes defined, we search for any building matching on the map
				if (m_DC_Squatter.general.locationTypes.IsEmpty())
				{
					radius = -1;
				}
				else
				{
					pos = SDRC_MissionHelper.FindMissionPos(m_DC_Squatter.general.locationTypes, m_DC_Squatter.general.size, );
				}
			}
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
/*		SetMarker(m_Config.showMarker, m_DC_Squatter.general.markerIcon, m_DC_Squatter.general.markerType);
		SetHint(m_Config.showHint, m_DC_Squatter.general.title, m_DC_Squatter.general.info);
		SetMessages(m_Config.showMessage, m_DC_Squatter.general.winMessage, m_DC_Squatter.general.loseMessage);				
		SetWinCondition(m_DC_Squatter.general.winCondition);*/
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == SDRC_EMissionState.SPAWN)
		{
			MissionSpawn();
			GetGame().GetCallqueue().CallLater(MissionRun, 2*1000);		//Spawn stuff every two seconds. 
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
			SCR_AIGroup group = SDRC_AIHelper.SpawnAIInBuilding(m_Building, m_DC_Squatter.ai.types.GetRandomElement(), m_DC_Squatter.ai.GetSkill(m_DC_Squatter.general.difficulty), m_DC_Squatter.ai.GetPerception(m_DC_Squatter.general.difficulty), GetFaction());
			if (group)
			{
				m_Groups.Insert(group);
			}
			m_iSpawnIndex++;
		}
		else
		{
			IEntity entity = SDRC_SpawnHelper.SpawnItemInBuildingWithLoot(m_Building, m_DC_Squatter.lootBox);			
			if (entity)
			{
				m_EntityList.Insert(entity);
				m_DC_Squatter.loot.box = entity;
			}
			else
			{
				SDRC_Log.Add("[SDRC_Mission_Squatter:MissionSpawn] Could not spawn loot box: " + m_DC_Squatter.lootBox, LogLevel.ERROR);								
			}
		
			SetState(SDRC_EMissionState.ACTIVE);			
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	override void DoWin()
	{	
		ref SDRC_Loot loot = m_DC_Squatter.loot;
		SDRC_LootHelper.SpawnItemsToStorage(loot.box, loot.items, loot.itemChance);
		super.DoWin();
	}
}
	
//------------------------------------------------------------------------------------------------
class SDRC_SquatterConfig : SDRC_MissionConfig
{
	//Mission specific
	int buildingRadius;									//The radius to search for suitable buildings.
	ref array<ref SDRC_Squatter> subMissions = {};		//List of squatters

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
}

//------------------------------------------------------------------------------------------------
class SDRC_Squatter : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();		
	ref array<string> buildingNames = {};
	//Optional settings
	string lootBox = "";					//The loot box
	ref SDRC_Loot loot = null;
	
	void Set(array<string> buildingNames_, string lootBox_)
	{
		buildingNames = buildingNames_;	
		lootBox = lootBox_;
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_SquatterJsonApi : SDRC_JsonApi
{
	ref SDRC_SquatterConfig conf = new SDRC_SquatterConfig();
		
	//------------------------------------------------------------------------------------------------
	void SDRC_SquatterJsonApi(string fileName)
	{
		SetFileName(fileName);
	}
	
	//------------------------------------------------------------------------------------------------
	bool Load(bool createMissingFiles = true)
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(createMissingFiles);		
		if (!loadContext)
		{
			if (!createMissingFiles)
			{
				return false;
			}
			SetDefaults();
			Save();
			return true;
		}
		
		loadContext.ReadValue("", conf);
		return true;
	}	
	
	//------------------------------------------------------------------------------------------------
	void Save()
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen();
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}	

	//------------------------------------------------------------------------------------------------
	void CreateMissionFiles()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void LoadMissionFiles()
	{
		//Load mission files
		foreach (string missionFile : conf.missionFiles)
		{
			SDRC_SquatterJsonApi jsonApi = new SDRC_SquatterJsonApi(missionFile);		
			if (jsonApi.Load(false))
			{
				foreach (SDRC_Squatter subMission : jsonApi.conf.subMissions)
				{
					conf.subMissions.Insert(subMission);
				}
				foreach (int idx : jsonApi.conf.missionList)
				{
					conf.missionList.Insert(idx);
				}
			}
		}
	}		
			
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		//Default
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		conf.missionList = {0,1,2,2,3,3,3,4,5,5,5};
		//Mission specific
		conf.buildingRadius = 400;
		//----------------------------------------------------
		conf.subMissions.Insert(Squatter0());				
		conf.subMissions.Insert(Squatter1());				
		conf.subMissions.Insert(Squatter2());				
		conf.subMissions.Insert(Squatter3());				
		conf.subMissions.Insert(Squatter4());				
		conf.subMissions.Insert(Squatter5());				
	};
			
	//----------------------------------------------------
	SDRC_Squatter Squatter0()
	{
		ref SDRC_Squatter squatter = new SDRC_Squatter();
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
			"Squatters near %l.",
			"Building has squatters with loot",		
			SDRC_EMissionWinCondition.AI_KILL_75,
			"The loot has been secured.",
			"Squatters cleaned the house and left you nothing.", 
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP,
			SDRC_EMissionDifficulty.NORMAL,
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
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"
			};
		loot.Set(0.7, lootItems);
		squatter.loot = loot;

		return squatter;	
	};
					
	//----------------------------------------------------
	SDRC_Squatter Squatter1()
	{
		ref SDRC_Squatter squatter = new SDRC_Squatter();
		squatter.general.Set(
			1, "index 1: Squatters in control towers",
			{"0 0 0"}, 0,
			{
				//We pick any building that matches and ignore location
			},
			"any",
			"Enemy in %l",
			"Control tower is being guarded.",		
			SDRC_EMissionWinCondition.AI_KILL_75,
			"You did it! Control in %l has been restored.",
			"Control in %l has been lost.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP,
			SDRC_EMissionDifficulty.NORMAL,
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
			"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et"
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE", "WEAPON_RIFLE",
				"WEAPON_RIFLE_BIG", "WEAPON_RIFLE_BIG", 
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"UTIL_OPTIC", "UTIL_OPTIC",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL"
			};
		loot.Set(0.8, lootItems);
		squatter.loot = loot;		
	
		return squatter;	
	};
					
	//----------------------------------------------------
	SDRC_Squatter Squatter2()
	{
		ref SDRC_Squatter squatter = new SDRC_Squatter();
		squatter.general.Set(
			2, "index 2: Squatters in military locations",
			{"0 0 0"}, 0,
			{
				//We pick any building that matches and ignore location
			},
			"any",
			"Guards around %l",
			"Military location has loot to steal.",		
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			"Guards have been eliminated.",
			"Military has collected the loot and left.", 
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP,
			SDRC_EMissionDifficulty.NORMAL,
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
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE", "WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"UTIL_OPTIC", "UTIL_OPTIC",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL"
			};
		loot.Set(0.6, lootItems);
		squatter.loot = loot;
		
		return squatter;	
	};
					
	//----------------------------------------------------
	SDRC_Squatter Squatter3()
	{
		ref SDRC_Squatter squatter = new SDRC_Squatter();
		squatter.general.Set(
			3, "index 3: Military in industrial areas",
			{"0 0 0"}, 0,
			{
				//We pick any building that matches and ignore location
			},
			"any",
			"Industrial area near %l",
			"Military has seized control of an industrial area. Don't shoot the civilians.",
			SDRC_EMissionWinCondition.AI_KILL_50,
			"Main enemy forces have been destroyed. Continue to keep an eye for rogues.",
			"%l is in the hands of the enemy.", 
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP,
			SDRC_EMissionDifficulty.NORMAL,
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
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL"
			};
		loot.Set(0.6, lootItems);
		squatter.loot = loot;
		
		return squatter;	
	};
					
	//----------------------------------------------------
	SDRC_Squatter Squatter4()
	{
		ref SDRC_Squatter squatter = new SDRC_Squatter();
		squatter.general.Set(
			4, "index 4: Enemy in churches and similar",
			{"0 0 0"}, 0,
			{
				//We pick any building that matches and ignore location
			},
			"any",
			"Sanctuary visitors near %l",
			"Holy night, holy loot.",		
			SDRC_EMissionWinCondition.AI_KILL_RANDOM,
			"Your success will be remembered.",
			"Your effort has been struck down.", 
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP,
			SDRC_EMissionDifficulty.NORMAL,
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
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_OPTIC",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL"
			};
		loot.Set(0.6, lootItems);
		squatter.loot = loot;
		
		return squatter;	
	};
					
	//----------------------------------------------------
	SDRC_Squatter Squatter5()
	{
		ref SDRC_Squatter squatter = new SDRC_Squatter();
		squatter.general.Set(
			5, "index 5: Shops and houses",
			{"0 0 0"}, 0,
			{
				//We pick any building that matches and ignore location
			},
			"any",
			"Burglars seen near %l",
			"Go rob the robbers.",		
			SDRC_EMissionWinCondition.AI_KILL_50,
			"%l is open for business once again.",
			"Everything has been stolen.", 
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP,
			SDRC_EMissionDifficulty.NORMAL,
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
			"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et"
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"
			};
		loot.Set(0.6, lootItems);
		squatter.loot = loot;		

		return squatter;						
	}
}