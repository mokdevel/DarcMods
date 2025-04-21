//Mission SCR_DC_Mission_Squatter.c
//
//An example of a mission file with simple set. No real functionality.
//Do a "replace all" for the word "Squatter" with your mission name.

class SCR_DC_Mission_Squatter : SCR_DC_Mission
{
	private ref SCR_DC_SquatterJsonApi m_SquatterJsonApi = new SCR_DC_SquatterJsonApi();	
	private ref SCR_DC_SquatterConfig m_Config;

	private ref SCR_DC_Squatter m_DC_Squatter;	//Squatter configuration in use
	private IEntity m_Building;					//The building for the mission
		
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission_Squatter()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Squatter] Constructor", LogLevel.SPAM);
				
		//Set some defaults
		SCR_DC_Mission();
		SetType(DC_EMissionType.SQUATTER);

		//Load config
		m_SquatterJsonApi.Load();
		m_Config = m_SquatterJsonApi.conf;
		
		//Pick a configuration for mission
		int idx = SCR_DC_MissionHelper.SelectMissionIndex(m_Config.squatterList);
		if(idx == -1)
		{
			SCR_DC_Log.Add("[SCR_DC_Mission_Squatter] No squatters defined.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}
		m_DC_Squatter = m_Config.squatters[idx];
		
		//Set defaults
		vector pos = m_DC_Squatter.pos;
		string posName = m_DC_Squatter.posName;
		
		//Find a location for the mission
		if (pos == "0 0 0")
		{
			float radius = m_Config.buildingRadius;
			
			//If no locationTypes defined, we search for any building matching on the map
			if (m_DC_Squatter.locationTypes.IsEmpty())
			{
				radius = 9999;
			}
			else
			{
				pos = SCR_DC_MissionHelper.FindMissionPos(m_DC_Squatter.locationTypes, 2);
			}
			
			//Find the houses
			array<IEntity>buildings = {};
		
			SCR_DC_Misc.FindBuildings(buildings, m_DC_Squatter.buildingNames, pos, radius);
			if (!buildings.IsEmpty())
			{
				m_Building = buildings.GetRandomElement();
				pos = m_Building.GetOrigin();
				
				SCR_DC_Log.Add("[SCR_DC_Mission_Squatter] Building selectd: " + m_Building.GetPrefabData().GetPrefabName() + " " + pos, LogLevel.DEBUG);
			}
			else
			{
				SCR_DC_Log.Add("[SCR_DC_Mission_Squatter] Could not find suitable building near " + SCR_DC_Locations.CreateName(pos, "any") + " " + pos, LogLevel.ERROR);
				pos = "0 0 0";				
			}
			
			if (!SCR_DC_MissionHelper.IsValidMissionPos(pos))
			{
				pos = "0 0 0";	//Mission position is not valid
			}
		}
		
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SCR_DC_Log.Add("[SCR_DC_Mission_Squatter] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}			
		
		SetPos(pos);
		SetPosName(SCR_DC_Locations.CreateName(pos, posName));
		SetTitle(m_DC_Squatter.title + "" + GetPosName());
		SetInfo(m_DC_Squatter.info);			
		SetMarker(m_Config.showMarker, DC_EMissionIcon.MISSION);
		SetShowHint(m_Config.showHint);
			
		SetState(DC_EMissionState.INIT);			
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
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
			if (SCR_DC_AIHelper.AreAllGroupsDead(m_Groups))
			{
				if (!IsActive())
				{
					SCR_DC_Log.Add("[SCR_DC_Mission_Squatter:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
					SetState(DC_EMissionState.END);
				}
			}
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionCycleTime*1000);		
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		super.MissionEnd();	
		
		//The rest of your clean up code.
		
		SCR_DC_Log.Add("[SCR_DC_Mission_Squatter:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		//Code for whatever you need for spawning things.
		array<IEntity>buildings = {};
		
		SCR_DC_Misc.FindBuildings(buildings, m_DC_Squatter.buildingNames);
	
		IEntity building = buildings[0];
		array<string> aiCharacters = {};
		int aiCount = Math.RandomInt(m_DC_Squatter.aiCount[0], m_DC_Squatter.aiCount[1]);
		for (int i = 0; i < aiCount; i++)
		{
			aiCharacters.Insert(m_DC_Squatter.aiTypes.GetRandomElement());
		}		
	
		SCR_DC_AIHelper.SpawnAIInBuilding(building, m_DC_Squatter.aiTypes, m_DC_Squatter.aiSkill, m_DC_Squatter.aiPerception);
	
		float rotation = Math.RandomFloat(0, 360);
		IEntity entity = SCR_DC_SpawnHelper.SpawnItemInBuilding(building, m_DC_Squatter.lootBox, rotation, 1.5, false);
		if (entity)
		{
			m_EntityList.Insert(entity);
			
			//Put loot
			if (m_DC_Squatter.loot)			
			{
				m_DC_Squatter.loot.box = entity;
				SCR_DC_LootHelper.SpawnItemsToStorage(m_DC_Squatter.loot.box, m_DC_Squatter.loot.items, m_DC_Squatter.loot.itemChance);
				SCR_DC_Log.Add("[SCR_DC_Mission_Squatter:MissionSpawn] Loot added.", LogLevel.DEBUG);								
			}
		}
		else
		{
			SCR_DC_Log.Add("[SCR_DC_Mission_Squatter:MissionSpawn] Could not spawn loot box: " + m_DC_Squatter.lootBox, LogLevel.ERROR);								
		}
	}
}
	
//------------------------------------------------------------------------------------------------
class SCR_DC_SquatterConfig : SCR_DC_MissionConfig
{
	//Mission specific
	
	//Variables here
	int buildingRadius;									//The radius to search for suitable buildings.
	ref array<ref int> squatterList = {};				//The indexes of squatters.
	ref array<ref SCR_DC_Squatter> squatters = {};		//List of squatters
}

//------------------------------------------------------------------------------------------------
class SCR_DC_Squatter : Managed
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
	ref SCR_DC_Loot loot = null;
	
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
class SCR_DC_SquatterJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Squatter.json";
	ref SCR_DC_SquatterConfig conf = new SCR_DC_SquatterConfig;
		
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_MISSIONCONFIG_FILE);
		
		if(!loadContext)
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
		conf.missionCycleTime = DC_MISSION_CYCLE_TIME_DEFAULT;
		conf.showMarker = true;		
		//Mission specific
		conf.buildingRadius = 200;
		conf.squatterList = {2};
		
		//----------------------------------------------------
		SCR_DC_Squatter squatter0 = new SCR_DC_Squatter;
		squatter0.Set(
			"Squatters in cities",
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
				"{5117311FB822FD1F}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Officer.et",
				"{DCB41B3746FDD1BE}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Rifleman.et",
				"{DCB41B3746FDD1BE}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Rifleman.et",
				"{96C784C502AC37DA}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_MG.et",								
			},
			50, 0.6,
			{"ShopModern_", "Villa_", "MunicipalOffice_", "PubVillage_"},
			"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et"
		);
		conf.squatters.Insert(squatter0);	
		
		SCR_DC_Loot squatter0loot = new SCR_DC_Loot;
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
		SCR_DC_Squatter squatter1 = new SCR_DC_Squatter;
		squatter1.Set(
			"Squatters in control towers",
			"0 0 0",
			"any",
			"Enemy in ",
			"Control tower is being guarded.",		
			{
				//We pick any building that matches and ignore location
			},
			{6,10},
			{
				"{5117311FB822FD1F}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Officer.et",
				"{DCB41B3746FDD1BE}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Rifleman.et",
				"{DCB41B3746FDD1BE}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Rifleman.et",
				"{96C784C502AC37DA}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_MG.et",								
			},
			50, 0.8,
			{"ControlTowerMilitary_"},
			"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et"
		);
		conf.squatters.Insert(squatter1);	
		
		SCR_DC_Loot squatter1loot = new SCR_DC_Loot;
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
		SCR_DC_Squatter squatter2 = new SCR_DC_Squatter;
		squatter2.Set(
			"Squatters in militart locations towers",
			"0 0 0",
			"any",
			"Guards around ",
			"Military location has loot to steal.",		
			{
				//We pick any building that matches and ignore location
			},
			{4,10},
			{
				"{77D7BFD355620806}Prefabs/Characters/Factions/OPFOR/USSR_Army/KLMK/Character_USSR_Ammo_KLMK.et",
				"{9A12B3F6ABDF70BE}Prefabs/Characters/Factions/OPFOR/USSR_Army/KLMK/Character_USSR_AT_KLMK.et",
				"{8A60AEBD529FEB8B}Prefabs/Characters/Factions/OPFOR/USSR_Army/KLMK/Character_USSR_MG_KLMK.et",
				"{D66C215D6F03EFFD}Prefabs/Characters/Factions/OPFOR/USSR_Army/KLMK/Character_USSR_Medic_KLMK.et"
			},
			50, 0.8,
//			{"Barracks_01_military_camo_v1"},
			{"Office_E_", "Barracks_01_", "Barracks_E_02_"},
			"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et"
		);
		conf.squatters.Insert(squatter2);	
		
		SCR_DC_Loot squatter2loot = new SCR_DC_Loot;
		lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE", "WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"WEAPON_ATTACHMENT",
				"WEAPON_OPTICS", "WEAPON_OPTICS",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter2loot.Set(0.8, lootItems);
		squatter2.loot = squatter1loot;		
	}	
}