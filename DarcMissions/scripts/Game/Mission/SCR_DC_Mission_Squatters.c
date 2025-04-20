//Mission SCR_DC_Mission_Squatter.c
//
//An example of a mission file with simple set. No real functionality.
//Do a "replace all" for the word "Squatter" with your mission name.

class SCR_DC_Mission_Squatter : SCR_DC_Mission
{
	private ref SCR_DC_SquatterJsonApi m_SquatterJsonApi = new SCR_DC_SquatterJsonApi();	
	private ref SCR_DC_SquatterConfig m_Config;

	protected ref SCR_DC_Squatter m_DC_Squatter;	//Squatter configuration in use
		
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission_Squatter()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Squatter] Constructor", LogLevel.DEBUG);
				
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
			SetState(DC_MissionState.EXIT);
			return;
		}
		m_DC_Squatter = m_Config.squatters[idx];
		
		//Set defaults
		vector pos = m_DC_Squatter.pos;
		string posName = m_DC_Squatter.posName;
		
		//Find a location for the mission
		if (pos == "0 0 0")
		{
			//TBD: Find location and building.
			//pos = SCR_DC_MissionHelper.FindMissionPos(m_DC_Squatter.locationTypes, m_Config.emptySize);
		}
		
		if (pos != "0 0 0")
		{					
			SetPos(pos);
			SetPosName(SCR_DC_Locations.CreateName(pos, posName));
			SetTitle(m_DC_Squatter.title + "" + GetPosName());
			SetInfo(m_DC_Squatter.info);			
			SetMarker(m_Config.showMarker, DC_EMissionIcon.MISSION);
			SetShowHint(m_Config.showHint);
				
			SetState(DC_MissionState.INIT);			
		}
		else
		{				
			//No suitable location found.
			SCR_DC_Log.Add("[SCR_DC_Mission_Squatter] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;
		}	
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		if (GetState() == DC_MissionState.INIT)
		{
			MissionSpawn();
			SetState(DC_MissionState.ACTIVE);
		}

		if (GetState() == DC_MissionState.END)
		{
			MissionEnd();
			SetState(DC_MissionState.EXIT);
		}	
				
		if (GetState() == DC_MissionState.ACTIVE)
		{			
			//Add code for runtime
			
/*			//Eventually when mission is to be ended do this:
			//SetState(DC_MissionState.END);

			//For example:			
			if (SCR_DC_aiHelper.AreAllGroupsDead(m_Groups))
			{
				SCR_DC_Log.Add("[SCR_DC_Mission_Squatter:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
				SetState(DC_MissionState.END);
			}*/			
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
		IEntity entity = SCR_DC_SpawnHelper.SpawnItemInBuilding(building, m_DC_Squatter.lootBox, rotation, 1.2, false);
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
		conf.squatterList = {0};
		
		//----------------------------------------------------
		SCR_DC_Squatter squatter0 = new SCR_DC_Squatter;
		squatter0.Set(
			"Bandit camp spawning to non city areas",
			"0 0 0",
			"any",
			"Bandit camp near ",
			"Building has squatters with loot",		
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
			{4,8},
			{
				"{5117311FB822FD1F}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Officer.et",
				"{DCB41B3746FDD1BE}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Rifleman.et",
				"{DCB41B3746FDD1BE}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Rifleman.et",
				"{96C784C502AC37DA}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_MG.et",
				"{7DE1CBA32A0225EB}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Randomized.et"						
			},
			50, 1.0,
			{"ShopModern_", "Villa_", "MunicipalOffice_", "PubVillage_"},
			"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et"
		);
		conf.squatters.Insert(squatter0);	
		
		SCR_DC_Loot squatter0loot = new SCR_DC_Loot;
		lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"WEAPON_ATTACHMENT",
				"WEAPON_OPTICS",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"
			};
		squatter0loot.Set(0.9, lootItems);
		squatter0.loot = squatter0loot;
	}	
}