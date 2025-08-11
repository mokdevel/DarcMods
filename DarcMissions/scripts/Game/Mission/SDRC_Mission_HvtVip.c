//Mission SDRC_Mission_HvtVip.c

//------------------------------------------------------------------------------------------------
/*!
High Value Target (HVT) - Very Important Person
*/

//------------------------------------------------------------------------------------------------
class SDRC_Mission_HvtVip : SDRC_Mission
{
	private ref SDRC_HvtVipJsonApi m_HvtVipJsonApi = new SDRC_HvtVipJsonApi();	
	private ref SDRC_HvtVipConfig m_Config;

	private ref SDRC_HvtVip m_DC_HvtVip;	//HvtVip configuration in use
	private IEntity m_Building;					//The building for the mission
	private int m_iAiCount;
	private int m_iSpawnIndex = 0;				//Counter for the AI to spawn
		
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_HvtVip(vector pos = "0 0 0")
	{
		SDRC_Log.Add("[SDRC_Mission_HvtVip] Constructor", LogLevel.SPAM);
				
		//Set some defaults
		SetType(DC_EMissionType.HVTVIP);

		//Load config
		m_HvtVipJsonApi.Load();
		m_Config = m_HvtVipJsonApi.conf;
		
		//Pick a configuration for mission
		int idx = SDRC_MissionHelper.SelectMissionIndex(m_Config.HvtVipList);
		if (idx == -1)
		{
			SDRC_Log.Add("[SDRC_Mission_HvtVip] No HvtVips defined.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}
		m_DC_HvtVip = m_Config.HvtVips[idx];
		
		//Set defaults
		m_iAiCount = Math.RandomInt(m_DC_HvtVip.aiCount[0], m_DC_HvtVip.aiCount[1]);
		float radius = 10;	//Default size for the radius. Mainly for requested missions to find the nearest building.
		array<string>buildingFilter = {};
		
		//Find a location for the mission
		if (!IsRequested())
		{				
			pos = m_DC_HvtVip.general.pos[0];
			radius = m_Config.buildingRadius;
			buildingFilter = m_DC_HvtVip.buildingNames;
			
			if (pos == "0 0 0")
			{
				//If no locationTypes defined, we search for any building matching on the map
				if (m_DC_HvtVip.locationTypes.IsEmpty())
				{
					radius = -1;
				}
				else
				{
					pos = SDRC_MissionHelper.FindMissionPos(m_DC_HvtVip.locationTypes, 2);
				}
			}
		}
		else
		{
			//If the missions is requested, any building near the location will be accepted.
			buildingFilter.Insert("");
		}

		//Find the mission house
		m_Building = SDRC_MissionHelper.FindMissionBuilding(pos, buildingFilter, radius);
		if (m_Building)
		{
			pos = m_Building.GetOrigin();
		}
		else
		{
			pos = "0 0 0";
		}
			
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SDRC_Log.Add("[SDRC_Mission_HvtVip] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}			
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_HvtVip.general.posName));
		SetMarker(m_Config.showMarker, m_Config.markerIdx, m_Config.markerType);
		SetHint(m_Config.showHint, m_DC_HvtVip.general.title, m_DC_HvtVip.general.info);
		SetMessages(m_Config.showMessage, m_DC_HvtVip.general.winMessage, m_DC_HvtVip.general.loseMessage);				
			
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
			SCR_AIGroup group = SDRC_AIHelper.SpawnAIInBuilding(m_Building, m_DC_HvtVip.aiTypes.GetRandomElement(), m_DC_HvtVip.aiSkill, m_DC_HvtVip.aiPerception, GetFaction());
			m_Groups.Insert(group);
			m_iSpawnIndex++;
		}
		else
		{
			float rotation = Math.RandomFloat(0, 360);
			IEntity entity = SDRC_SpawnHelper.SpawnItemInBuilding(m_Building, m_DC_HvtVip.lootBox, rotation, 1.5, false);
			if (entity)
			{
				m_EntityList.Insert(entity);
				
				//Put loot
				if (m_DC_HvtVip.loot)			
				{
					m_DC_HvtVip.loot.box = entity;
					SDRC_LootHelper.SpawnItemsToStorage(m_DC_HvtVip.loot.box, m_DC_HvtVip.loot.items, m_DC_HvtVip.loot.itemChance);
					SDRC_Log.Add("[SDRC_Mission_HvtVip:MissionSpawn] Loot added.", LogLevel.DEBUG);								
				}
			}
			else
			{
				SDRC_Log.Add("[SDRC_Mission_HvtVip:MissionSpawn] Could not spawn loot box: " + m_DC_HvtVip.lootBox, LogLevel.ERROR);								
			}
			
			SetState(DC_EMissionState.ACTIVE);
		}
	}
}
	
//------------------------------------------------------------------------------------------------
class SDRC_HvtVipConfig : SDRC_MissionConfig
{
	//Mission specific
	
	//Variables here
	int buildingRadius;									//The radius to search for suitable buildings.
	ref array<ref int> HvtVipList = {};				//The indexes of HvtVips.
	ref array<ref SDRC_HvtVip> HvtVips = {};		//List of HvtVips
}

//------------------------------------------------------------------------------------------------
class SDRC_HvtVip : Managed
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
	string target;
	
	void Set(array<EMapDescriptorType> locationTypes_, array<int> aiCount_, array<string> aiTypes_, int aiSkill_, float aiPerception_, array<string> buildingNames_, string lootBox_, string target_)
	{
		locationTypes = locationTypes_;
		aiCount = aiCount_;
		aiTypes = aiTypes_;
		aiSkill = aiSkill_;
		aiPerception = aiPerception_;	
		buildingNames = buildingNames_;	
		lootBox = lootBox_;
		target = target_;
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_HvtVipJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_HvtVip.json";
	ref SDRC_HvtVipConfig conf = new SDRC_HvtVipConfig();
		
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
		//Default
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		conf.markerIdx = DC_EMissionIcon.GM_MISSION_HVTVIP_MAP;
		//Mission specific
		conf.buildingRadius = 400;
		conf.HvtVipList = {0};
		//----------------------------------------------------
		conf.HvtVips.Insert(HvtVip0());				
	};
			
	//----------------------------------------------------
	SDRC_HvtVip HvtVip0()
	{
		SDRC_HvtVip HvtVip = new SDRC_HvtVip();
		HvtVip.general.Set(
			"index 0: HvtVips in cities",
			{"0 0 0"},
			"any",
			"Target near %l.",
			"Assassinate the target",
			DC_EMissionWinCondition.HVT_VIP,
			"The target has been neutralized.",
			"The target escaped.",
			0		
		);
		HvtVip.Set(
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
			{1,2},
			{
				"G_RIFLE", 
			},
			50, 0.6,
			{"ShopModern_", "Villa_", "MunicipalOffice_", "PubVillage_", "Office_E_", "MountainHotel_"},
			"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
			"C_OFFICER"
		);
		
		SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"
			};
		loot.Set(0.7, lootItems);
		HvtVip.loot = loot;

		return HvtVip;	
	};
}