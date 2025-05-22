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
	void SDRC_Mission_Squatter()
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
		vector pos = m_DC_Squatter.pos;
		string posName = m_DC_Squatter.posName;
		m_iAiCount = Math.RandomInt(m_DC_Squatter.aiCount[0], m_DC_Squatter.aiCount[1]);
		
		//Find a location for the mission
		if (pos == "0 0 0")
		{
			float radius = m_Config.buildingRadius;
			
			//If no locationTypes defined, we search for any building matching on the map
			if (m_DC_Squatter.locationTypes.IsEmpty())
			{
				radius = -1;
			}
			else
			{
				pos = SDRC_MissionHelper.FindMissionPos(m_DC_Squatter.locationTypes, 2);
			}
			
			//Find the houses
			array<IEntity>buildings = {};
		
			SDRC_BuildingHelper.FindBuildings(buildings, m_DC_Squatter.buildingNames, pos, radius);
			if (!buildings.IsEmpty())
			{
				m_Building = buildings.GetRandomElement();
				pos = m_Building.GetOrigin();
				
				SDRC_Log.Add("[SDRC_Mission_Squatter] Building selected: " + m_Building.GetPrefabData().GetPrefabName() + " " + pos, LogLevel.DEBUG);
			}
			else
			{
				SDRC_Log.Add("[SDRC_Mission_Squatter] Could not find suitable building near " + SDRC_Locations.CreateName(pos, "any") + " " + pos, LogLevel.ERROR);
				pos = "0 0 0";				
			}
			
			if (!SDRC_MissionHelper.IsValidMissionPos(pos))
			{
				pos = "0 0 0";	//Mission position is not valid
			}
		}
		
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SDRC_Log.Add("[SDRC_Mission_Squatter] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}			
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, posName));
		SetTitle(m_DC_Squatter.title + "" + GetPosName());
		SetInfo(m_DC_Squatter.info);			
		SetMarker(m_Config.showMarker, DC_EMissionIcon.N_HOUSE);
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
				SDRC_Log.Add("[SDRC_Mission_Squatter:MissionRun] Mission over.", LogLevel.NORMAL);
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
		
		SDRC_Log.Add("[SDRC_Mission_Squatter:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);
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
	//Occupation specific
	string comment;							//Generic comment to describe the mission. Not used in game.
	vector pos;								//Position for mission. "0 0 0" used for random location chosen from locationTypes.
	string posName;							//Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes 
	string title;							//Title for the hint shown for players
	string info;							//Details for the hint shown for players
	ref array<EMapDescriptorType> locationTypes = {};
	ref array<int> aiCount = {};			//min, max
	ref array<string> aiTypes = {};
	int aiSkill;
	float aiPerception;
	ref array<string> buildingNames = {};
	//Optional settings
	string lootBox = "";					//The loot box
	ref SDRC_Loot loot = null;
	
	void Set(string comment_, vector pos_, string posName_, string title_, string info_, array<EMapDescriptorType> locationTypes_, array<int> aiCount_, array<string> aiTypes_, int aiSkill_, float aiPerception_, array<string> buildingNames_, string lootBox_)
	{
		comment = comment_;
		pos = pos_;
		posName = posName_;
		title = title_;
		info = info_;
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
		conf.showMarker = true;		
		//Mission specific
		conf.buildingRadius = 400;
		conf.squatterList = {0,1,2,2,3,3,3,4,5,5,5};
		
		//----------------------------------------------------
		SDRC_Squatter squatter0 = new SDRC_Squatter();
		squatter0.Set(
			"index 0: Squatters in cities",
			"0 0 0",
			"any",
			"Squatters near ",
			"Building has squatters with loot",		
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
				"WEAPON_ATTACHMENT",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter0loot.Set(0.7, lootItems);
		squatter0.loot = squatter0loot;
		
		//----------------------------------------------------
		SDRC_Squatter squatter1 = new SDRC_Squatter();
		squatter1.Set(
			"index 1: Squatters in control towers",
			"0 0 0",
			"any",
			"Enemy in ",
			"Control tower is being guarded.",		
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
				"WEAPON_ATTACHMENT",
				"WEAPON_OPTICS", "WEAPON_OPTICS",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter1loot.Set(0.8, lootItems);
		squatter1.loot = squatter1loot;		
	
		//----------------------------------------------------
		SDRC_Squatter squatter2 = new SDRC_Squatter();
		squatter2.Set(
			"index 2: Squatters in military locations",
			"0 0 0",
			"any",
			"Guards around ",
			"Military location has loot to steal.",		
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
				"WEAPON_ATTACHMENT",
				"WEAPON_OPTICS", "WEAPON_OPTICS",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter2loot.Set(0.6, lootItems);
		squatter2.loot = squatter2loot;
		
		//----------------------------------------------------
		SDRC_Squatter squatter3 = new SDRC_Squatter();
		squatter3.Set(
			"index 3: Military in industrial areas",
			"0 0 0",
			"any",
			"Industrial area near ",
			"Military has seized control of an industrial area. Don't shoot the civilians.",
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
				"WEAPON_ATTACHMENT",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter3loot.Set(0.6, lootItems);
		squatter3.loot = squatter3loot;
		
		//----------------------------------------------------
		SDRC_Squatter squatter4 = new SDRC_Squatter();
		squatter4.Set(
			"index 4: Enemy in churches and similar",
			"0 0 0",
			"any",
			"Sanctuary visitors near ",
			"Holy night, holy loot.",		
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
				"WEAPON_OPTICS",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter4loot.Set(0.6, lootItems);
		squatter4.loot = squatter4loot;
		
		//----------------------------------------------------
		SDRC_Squatter squatter5 = new SDRC_Squatter();
		squatter5.Set(
			"index 5: Shops and houses",
			"0 0 0",
			"any",
			"Burglars seen near ",
			"Go rob the robbers.",		
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
				"WEAPON_ATTACHMENT",
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter5loot.Set(0.6, lootItems);
		squatter5.loot = squatter5loot;		
	}
}