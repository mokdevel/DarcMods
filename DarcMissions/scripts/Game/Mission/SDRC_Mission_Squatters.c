//Mission SDRC_Mission_Squatter.c

//------------------------------------------------------------------------------------------------
/*!
A building is guarded by AIs with loot available.
*/

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Squatter : SDRC_Mission
{
	private ref SDRC_SquatterJsonApi m_SquatterJsonApi = new SDRC_SquatterJsonApi();	
	private ref SDRC_SquatterConfig m_Config;

	private ref SDRC_Squatter m_DC_Squatter;	//Squatter configuration in use
	private IEntity m_Building;					//The building for the mission
	private int m_iAiCount;
	private int m_iSpawnIndex = 0;				//Counter for the AI to spawn
		
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Squatter(vector pos = "0 0 0")
	{
		SDRC_Log.Add("[SDRC_Mission_Squatter] Constructor", LogLevel.SPAM);
				
		//Set some defaults
		SetType(DC_EMissionType.SQUATTER);

		//Load config
		m_SquatterJsonApi.Load();
		m_Config = m_SquatterJsonApi.conf;
		
		//Pick a configuration for mission
		int idx = SDRC_MissionHelper.SelectMissionIndex(m_Config.squatterList);
		if (idx == -1)
		{
			SDRC_Log.Add("[SDRC_Mission_Squatter] No squatters defined.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}
		m_DC_Squatter = m_Config.squatters[idx];
		
		//Set defaults
		m_iAiCount = Math.RandomInt(m_DC_Squatter.aiCount[0], m_DC_Squatter.aiCount[1]);
		float radius = 10;	//Default size for the radius
		array<IEntity>buildings = {};
		array<string>buildingFilter = {};
		
		//Find a location for the mission
		if (!IsRequested())
		{				
			pos = m_DC_Squatter.general.pos[0];
			radius = m_Config.buildingRadius;
			buildingFilter = m_DC_Squatter.buildingNames;
			
			if (pos == "0 0 0")
			{
				//If no locationTypes defined, we search for any building matching on the map
				if (m_DC_Squatter.locationTypes.IsEmpty())
				{
					radius = -1;
				}
				else
				{
					pos = SDRC_MissionHelper.FindMissionPos(m_DC_Squatter.locationTypes, 2);
//					buildingFilter = m_DC_Squatter.buildingNames;
				}
			}
		}
		else
		{
			//If the missions is requested, any building near the location will be accepted.
			buildingFilter.Insert("");
		}

		//Find the houses	
		SDRC_BuildingHelper.FindBuildings(buildings, buildingFilter, pos, radius);
		
		if (!buildings.IsEmpty())
		{
			m_Building = buildings.GetRandomElement();
			pos = m_Building.GetOrigin();
			
			if (!SDRC_MissionHelper.IsValidMissionPos(pos))
			{
				pos = "0 0 0";	//Mission position is not valid
			}
			else
			{
				SDRC_Log.Add("[SDRC_Mission_Squatter] Building selected: " + m_Building.GetPrefabData().GetPrefabName() + " " + pos, LogLevel.DEBUG);
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_Mission_Squatter] Could not find suitable building near " + SDRC_Locations.CreateName(pos, "any") + " " + pos, LogLevel.ERROR);
			pos = "0 0 0";				
		}
			
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SDRC_Log.Add("[SDRC_Mission_Squatter] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}			
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Squatter.general.posName));
		SetMarker(m_Config.showMarker, m_Config.markerIdx, m_Config.markerType);
		SetHint(m_Config.showHint, m_DC_Squatter.general.title, m_DC_Squatter.general.info);
		SetMessages(m_Config.showMessage, m_DC_Squatter.general.winMessage, m_DC_Squatter.general.loseMessage);				
			
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
//			SetState(DC_EMissionState.ACTIVE);
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
		//Spawn AI one by one. Sets missions active once ready.
		if (m_iSpawnIndex < m_iAiCount)
		{
			//Each AI is spawned in to its own group to be able to give individual waypoints to a character
			SCR_AIGroup group = SDRC_AIHelper.SpawnAIInBuilding(m_Building, m_DC_Squatter.aiTypes.GetRandomElement(), m_DC_Squatter.aiSkill, m_DC_Squatter.aiPerception, GetFaction());
			m_Groups.Insert(group);
			m_iSpawnIndex++;
		}
		else
		{
			float rotation = Math.RandomFloat(0, 360);
			IEntity entity = SDRC_SpawnHelper.SpawnItemInBuilding(m_Building, m_DC_Squatter.lootBox, rotation, 1.5, false);
			if (entity)
			{
				m_EntityList.Insert(entity);
				
				//Put loot
				if (m_DC_Squatter.loot)			
				{
					m_DC_Squatter.loot.box = entity;
					SDRC_LootHelper.SpawnItemsToStorage(m_DC_Squatter.loot.box, m_DC_Squatter.loot.items, m_DC_Squatter.loot.itemChance);
					SDRC_Log.Add("[SDRC_Mission_Squatter:MissionSpawn] Loot added.", LogLevel.DEBUG);								
				}
			}
			else
			{
				SDRC_Log.Add("[SDRC_Mission_Squatter:MissionSpawn] Could not spawn loot box: " + m_DC_Squatter.lootBox, LogLevel.ERROR);								
			}
			
			SetState(DC_EMissionState.ACTIVE);
		}
	}
}
	
//------------------------------------------------------------------------------------------------
class SDRC_SquatterConfig : SDRC_MissionConfig
{
	//Mission specific
	
	//Variables here
	int buildingRadius;									//The radius to search for suitable buildings.
	ref array<ref int> squatterList = {};				//The indexes of squatters.
	ref array<ref SDRC_Squatter> squatters = {};		//List of squatters
}

//------------------------------------------------------------------------------------------------
class SDRC_Squatter : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref array<EMapDescriptorType> locationTypes = {};
	ref array<int> aiCount = {};			//min, max
	ref array<string> aiTypes = {};
	int aiSkill;
	float aiPerception;
	ref array<string> buildingNames = {};
	//Optional settings
	string lootBox = "";					//The loot box
	ref SDRC_Loot loot = null;
	
	void Set(array<EMapDescriptorType> locationTypes_, array<int> aiCount_, array<string> aiTypes_, int aiSkill_, float aiPerception_, array<string> buildingNames_, string lootBox_)
	{
		locationTypes = locationTypes_;
		aiCount = aiCount_;
		aiTypes = aiTypes_;
		aiSkill = aiSkill_;
		aiPerception = aiPerception_;	
		buildingNames = buildingNames_;	
		lootBox = lootBox_;
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_SquatterJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Squatter.json";
	ref SDRC_SquatterConfig conf = new SDRC_SquatterConfig();
		
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
		conf.markerIdx = DC_EMissionIcon.GM_MISSION_SQUATTERS_MAP;
		//Mission specific
		conf.buildingRadius = 400;
		conf.squatterList = {0,1,2,2,3,3,3,4,5,5,5};
		
		//----------------------------------------------------
		SDRC_Squatter squatter0 = new SDRC_Squatter();
		squatter0.general.Set(
			"index 0: Squatters in cities",
			{"0 0 0"},
			"any",
			"Squatters near %l.",
			"Building has squatters with loot",		
			DC_EMissionWinCondition.KILL_AI_ALL,
			"Win message",
			"Lose message", 
			0		
		);
		squatter0.Set(
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
			{3,6},
			{
				"C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", 
				"C_OFFICER"
			},
			50, 0.6,
			{"ShopModern_", "Villa_", "MunicipalOffice_", "PubVillage_", "Office_E_", "MountainHotel_"},
			"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et"
		);
		conf.squatters.Insert(squatter0);	
		
		SDRC_Loot squatter0loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter0loot.Set(0.7, lootItems);
		squatter0.loot = squatter0loot;
		
		//----------------------------------------------------
		SDRC_Squatter squatter1 = new SDRC_Squatter();
		squatter1.general.Set(
			"index 1: Squatters in control towers",
			{"0 0 0"},
			"any",
			"Enemy in %l.",
			"Control tower is being guarded.",		
			DC_EMissionWinCondition.KILL_AI_ALL,
			"Win message",
			"Lose message", 
			0		
		);
		squatter1.Set(
			{
				//We pick any building that matches and ignore location
			},
			{6,10},
			{
				"C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", 
				"C_HEAVY", "C_HEAVY", 
				"C_LAUNCHER", "C_MEDIC", "C_OFFICER"
			},
			50, 0.8,
			{"ControlTowerMilitary_"},
			"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et"
		);
		conf.squatters.Insert(squatter1);	
		
		SDRC_Loot squatter1loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE", "WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"UTIL_OPTICS", "UTIL_OPTICS",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter1loot.Set(0.8, lootItems);
		squatter1.loot = squatter1loot;		
	
		//----------------------------------------------------
		SDRC_Squatter squatter2 = new SDRC_Squatter();
		squatter2.general.Set(
			"index 2: Squatters in military locations",
			{"0 0 0"},
			"any",
			"Guards around %l",
			"Military location has loot to steal.",		
			DC_EMissionWinCondition.KILL_AI_ALL,
			"Win message",
			"Lose message", 
			0		
		);
		squatter2.Set(
			{
				//We pick any building that matches and ignore location
			},
			{4,10},
			{
				"C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", 
				"C_HEAVY", "C_HEAVY", "C_HEAVY", 
				"C_LAUNCHER", "C_MEDIC", "C_OFFICER"
			},
			50, 0.8,
			{"Office_E_", "Barracks_01_", "Barracks_E_02_"},
			"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et"
		);
		conf.squatters.Insert(squatter2);	
		
		SDRC_Loot squatter2loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE", "WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"UTIL_OPTICS", "UTIL_OPTICS",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter2loot.Set(0.6, lootItems);
		squatter2.loot = squatter2loot;
		
		//----------------------------------------------------
		SDRC_Squatter squatter3 = new SDRC_Squatter();
		squatter3.general.Set(
			"index 3: Military in industrial areas",
			{"0 0 0"},
			"any",
			"Industrial area near %l",
			"Military has seized control of an industrial area. Don't shoot the civilians.",
			DC_EMissionWinCondition.KILL_AI_ALL,
			"Win message",
			"Lose message", 
			0		
		);
		squatter3.Set(
			{
				//We pick any building that matches and ignore location
			},
			{2,6},
			{
				"C_RIFLEMAN", "C_RIFLEMAN", "C_RIFLEMAN", 
				"C_HEAVY", "C_HEAVY", "C_HEAVY", 
				"C_RECON", "C_RECON", "C_RECON", 
				"C_OFFICER"
			},
			50, 0.8,
			{"DieselPowerPlant_", "CowShed_", "FireStation_", "Warehouse_", "TransformerStation_", "FactoryHall_"},
			"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et"
		);
		conf.squatters.Insert(squatter3);	
		
		SDRC_Loot squatter3loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter3loot.Set(0.6, lootItems);
		squatter3.loot = squatter3loot;
		
		//----------------------------------------------------
		SDRC_Squatter squatter4 = new SDRC_Squatter();
		squatter4.general.Set(
			"index 4: Enemy in churches and similar",
			{"0 0 0"},
			"any",
			"Sanctuary visitors near ",
			"Holy night, holy loot.",		
			DC_EMissionWinCondition.KILL_AI_ALL,
			"Win message",
			"Lose message", 
			0		
		);
		squatter4.Set(
			{
				//We pick any building that matches and ignore location
			},
			{3,7},
			{
				"C_RIFLEMAN"
			},
			50, 0.8,		
			{"Church_", "ChurchSmall_", "Mosque_", "Minaret"},
			"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et"
		);
		conf.squatters.Insert(squatter4);	
		
		SDRC_Loot squatter4loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_OPTICS",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter4loot.Set(0.6, lootItems);
		squatter4.loot = squatter4loot;
		
		//----------------------------------------------------
		SDRC_Squatter squatter5 = new SDRC_Squatter();
		squatter5.general.Set(
			"index 5: Shops and houses",
			{"0 0 0"},
			"any",
			"Burglars seen near %l",
			"Go rob the robbers.",		
			DC_EMissionWinCondition.KILL_AI_ALL,
			"Win message",
			"Lose message", 
			0		
		);
		squatter5.Set(
			{
				//We pick any building that matches and ignore location
			},
			{2,6},
			{
				"C_SPECIAL"
			},
			30, 0.3,
			{"ShopModern_", "House_Town_", "House_Village_", "FarmHouse_", "House_Wooden_"},
			"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et"
		);
		conf.squatters.Insert(squatter5);	
		
		SDRC_Loot squatter5loot = new SDRC_Loot();
		lootItems = {
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter5loot.Set(0.6, lootItems);
		squatter5.loot = squatter5loot;		
	}
}