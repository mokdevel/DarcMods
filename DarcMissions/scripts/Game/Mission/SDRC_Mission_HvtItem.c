//Mission SDRC_Mission_HvtItem.c

//------------------------------------------------------------------------------------------------
/*!
High Value Target (HVT) - Item target
*/

//------------------------------------------------------------------------------------------------
class SDRC_Mission_HvtItem : SDRC_Mission
{
	const int AI_TARGET_DESTROYED_CYCLE_TIME = 5000;
	
	private ref SDRC_HvtItemJsonApi m_HvtItemJsonApi = new SDRC_HvtItemJsonApi();	
	private ref SDRC_HvtItemConfig m_Config;
	private ref SDRC_HvtItem m_DC_HvtItem;			//HvtItem configuration in use
	
//	private int m_iGroupCount;
	private int m_iSpawnIndex = 0;					//Counter for the item to spawn
	private float m_fSpawnRotation = 0;				//Rotation of the camp for random locations.
	
	private IEntity m_Target = null;
		
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_HvtItem(vector pos = "0 0 0")
	{
		SDRC_Log.Add("[SDRC_Mission_HvtItem] Constructor", LogLevel.SPAM);
				
		//Set some defaults
		SetType(DC_EMissionType.HVTITEM);

		//Load config
		m_HvtItemJsonApi.Load();
		m_Config = m_HvtItemJsonApi.conf;
		
		//Pick a configuration for mission
		int idx = SDRC_MissionHelper.SelectMissionIndex(m_Config.hvtItemList);
		if (idx == -1)
		{
			SDRC_Log.Add("[SDRC_Mission_HvtItem] No HvtItems defined.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}
		m_DC_HvtItem = m_Config.hvtItems[idx];
		
		//Set defaults
		if (!IsRequested())
		{
			pos = m_DC_HvtItem.general.pos[0];
			
			//Find a location for the mission
			if (pos == "0 0 0")
			{
				pos = SDRC_MissionHelper.FindMissionPos(m_DC_HvtItem.locationTypes, m_DC_HvtItem.emptySize);
				//Camps in random places are randomly rotated
				m_fSpawnRotation = Math.RandomFloat(0, 360);
			}
			else
			{
				pos = SDRC_MissionHelper.FindMissionPos(pos, m_DC_HvtItem.emptySize);
			}
		}
		else
		{
			pos = SDRC_MissionHelper.FindMissionPos(pos, m_DC_HvtItem.emptySize);
		}
		
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SDRC_Log.Add("[SDRC_Mission_HvtItem] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}		
		
		SDRC_SpawnHelper.SetStructuresToOrigo(m_DC_HvtItem.campItems);
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_HvtItem.general.posName));
		SetMarker(m_Config.showMarker, m_Config.markerIdx, m_Config.markerType);
		SetHint(m_Config.showHint, m_DC_HvtItem.general.title, m_DC_HvtItem.general.info);
		SetMessages(m_Config.showMessage, m_DC_HvtItem.general.winMessage, m_DC_HvtItem.general.loseMessage);				
		SetWinCondition(m_DC_HvtItem.general.winCondition);
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == DC_EMissionState.SPAWN)
		{
			MissionSpawn();
			GetGame().GetCallqueue().CallLater(MissionRun, 2*1000);		//Spawn stuff every two seconds.
			//NOTE: ACTIVE set inside MissionSpawn()
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
		
		ready = SDRC_CampHelper.Spawn(this, m_iSpawnIndex, m_DC_HvtItem, m_fSpawnRotation, m_Config.disableArsenal);
		m_iSpawnIndex++;			
		
		if (ready)
		{
			m_Target = GetFromEntityList(m_DC_HvtItem.targetIdx);
			SDRC_Log.Add("[SDRC_Mission_HvtItem:MissionSpawn] Target: " + m_Target, LogLevel.DEBUG);
			
			GetGame().GetCallqueue().CallLater(IsTargetDestroyed, AI_TARGET_DESTROYED_CYCLE_TIME, false);
			
			SetState(DC_EMissionState.ACTIVE);
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	override void DoWin()
	{	
		SDRC_CampHelper.AddLoot(m_DC_HvtItem);
		super.DoWin();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	A loop that checks if the target has been eliminated.
	It also keeps the target disabled until a player is near by.
	*/
	void IsTargetDestroyed()
	{
		if (GetWinCondition() == DC_EMissionWinCondition.HVT_DESTROY_ITEM)
		{
			bool isDestroyed = false;
			if (m_Target)
			{				
				DamageManagerComponent damageManager = DamageManagerComponent.Cast(m_Target.FindComponent(DamageManagerComponent));
				if (damageManager)
				{
					float health = damageManager.GetHealthScaled();
					SDRC_Log.Add("[SDRC_Mission_HvtItem:IsTargetDestroyed] Target health: " + health, LogLevel.SPAM);
					if (health < 0.1)
					{
						isDestroyed = true;
					}
				}
			}	
			else
			{
				isDestroyed = true;
			}
			
			if (isDestroyed)
			{
				SDRC_Log.Add("[SDRC_Mission_HvtItem:IsTargetDestroyed] Target destroyed!", LogLevel.DEBUG);
				DoWin();
				return;
			}
		
			GetGame().GetCallqueue().CallLater(IsTargetDestroyed, AI_TARGET_DESTROYED_CYCLE_TIME, false);
		}
	}
}
	
//------------------------------------------------------------------------------------------------
class SDRC_HvtItemConfig : SDRC_MissionConfig
{
	//Mission specific
	
	//Variables here
	bool disableArsenal;							//Disable arsenal for vehicles so that no other items are found
	ref array<ref int> hvtItemList = {};			//The indexes of HvtItems.
	ref array<ref SDRC_HvtItem> hvtItems = {};			//List of HvtItems
}

//------------------------------------------------------------------------------------------------
class SDRC_HvtItem : SDRC_Camp
{
	SDRC_Camp camp = SDRC_Camp();
	int targetIdx; 
}

//------------------------------------------------------------------------------------------------
class SDRC_HvtItemJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_HvtItem.json";
	ref SDRC_HvtItemConfig conf = new SDRC_HvtItemConfig();
		
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
		conf.markerIdx = DC_EMissionIcon.GM_MISSION_HVTITEM_MAP;
		//Mission specific
		conf.hvtItemList = {0,1};
		//----------------------------------------------------
		conf.hvtItems.Insert(HvtItem0());				
		conf.hvtItems.Insert(HvtItem1());				
	};
	
	//----------------------------------------------------
	SDRC_HvtItem HvtItem0()
	{
		SDRC_HvtItem hvtItem = new SDRC_HvtItem();
		hvtItem.general.Set(
			"index 0: Destroy generator",
			{"0 0 0"},
			"any",
			"Destroy generator near %l",
			"The enemy is spreading propaganda.",
			DC_EMissionWinCondition.HVT_DESTROY_ITEM,
			"Target destroyed.",
			"You failed in your mission!",
			0
		);
		hvtItem.Set(
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
			DC_EWaypointGenerationType.LOITER,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_ADMIN", "G_LIGHT", "G_LIGHT"
			},
			50, 1.0,
			6
		);
		hvtItem.targetIdx = 1;
		
		SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
			};
		loot.Set(0.7, lootItems);
		hvtItem.loot = loot;
		
		SDRC_Structure item_0 = new SDRC_Structure;
		item_0.Set(
		    "{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et",
		    "243.294 1 174.755"
		);
		hvtItem.campItems.Insert(item_0);
		
		SDRC_Structure item_1 = new SDRC_Structure;
		item_1.Set(
		    "{4BEFC7FCCB2BA121}Prefabs/Props/Military/Generators/GeneratorMilitary_USSR_01/HvtGeneratorMilitary_USSR_01.et",
		    "249.628 1 176.903"
		);
		hvtItem.campItems.Insert(item_1);
		
		SDRC_Structure item_2 = new SDRC_Structure;
		item_2.Set(
		    "{B4F2701CBBE49C48}Prefabs/Props/Military/Antennas/Antenna_RC292_01.et",
		    "246.121 1 174.906"
		);
		hvtItem.campItems.Insert(item_2);
		
		SDRC_Structure item_3 = new SDRC_Structure;
		item_3.Set(
		    "{C108EB96CCC73E94}PrefabsEditable/Auto/Compositions/Misc/SubCompositions/E_Table_Radio_US_01.et",
		    "243.349 1 176.169"
		);
		hvtItem.campItems.Insert(item_3);
		
		SDRC_Structure item_4 = new SDRC_Structure;
		item_4.Set(
		    "{D0974856AF6DAE81}PrefabsEditable/Auto/Props/Military/Furniture/E_ChairMilitary_US_01.et",
		    "243.376 1 176.954",
		    "0 -40.034 0"
		);
		hvtItem.campItems.Insert(item_4);
		
		SDRC_Structure item_5 = new SDRC_Structure;
		item_5.Set(
		    "{DDF59362051B28BC}Prefabs/Props/Military/Fortification/BarbedTape_KnifeRest.et",
		    "252.327 1 177.128"
		);
		hvtItem.campItems.Insert(item_5);
		
		SDRC_Structure item_6 = new SDRC_Structure;
		item_6.Set(
		    "{DDF59362051B28BC}Prefabs/Props/Military/Fortification/BarbedTape_KnifeRest.et",
		    "251.787 1 181.118",
		    "0 -27.459 0"
		);
		hvtItem.campItems.Insert(item_6);
		
		SDRC_Structure item_7 = new SDRC_Structure;
		item_7.Set(
		    "{DDF59362051B28BC}Prefabs/Props/Military/Fortification/BarbedTape_KnifeRest.et",
		    "251.091 1 172.209",
		    "0 39.983 0"
		);
		hvtItem.campItems.Insert(item_7);
		
		return hvtItem;
	};		
	
	//----------------------------------------------------
	SDRC_HvtItem HvtItem1()
	{
		SDRC_HvtItem hvtItem = new SDRC_HvtItem();
		hvtItem.general.Set(
			"index 1: Destroy supplies",
			{"0 0 0"},
			"any",
			"A supply truck near %l",
			"A truck has crashed and the supplies needs to be destroyed.",
			DC_EMissionWinCondition.HVT_DESTROY_ITEM,
			"Supplies never reached the enemy. Good work!",
			"The enemy will fight with their bellies full.",
			0
		);
		hvtItem.Set(
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
			{1, 2},
			{25, 100},
			DC_EWaypointGenerationType.LOITER,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_ADMIN", "G_LIGHT", "G_LIGHT", "G_HEAVY"
			},
			50, 1.0,
			10
		);
		hvtItem.targetIdx = 1;
		
		SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"UTIL_ATTACHMENT", "UTIL_OPTICAL",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", 
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
			};
		loot.Set(0.7, lootItems);
		hvtItem.loot = loot;
		
		SDRC_Structure item_0 = new SDRC_Structure;
		item_0.Set(
		    "{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
		    "250.977 1 204.513",
		    "0 -92.923 0"
		);
		hvtItem.campItems.Insert(item_0);
		
		SDRC_Structure item_1 = new SDRC_Structure;
		item_1.Set(
		    "{47E0234DF4169EE5}Prefabs/Props/Military/CargoContainers/HvtCargoContainer_01_10ft_US_supplies.et",
		    "249.9 1 196.961",
		    "0 -18.644 0"
		);
		hvtItem.campItems.Insert(item_1);
		
		SDRC_Structure item_2 = new SDRC_Structure;
		item_2.Set(
		    "{0542578CA422287A}PrefabsEditable/Auto/Props/Industrial/Repair/E_VehicleGarbage_01_pile_medium.et",
		    "247.843 1 200.018"
		);
		hvtItem.campItems.Insert(item_2);
		
		SDRC_Structure item_3 = new SDRC_Structure;
		item_3.Set(
		    "{0542578CA422287A}PrefabsEditable/Auto/Props/Industrial/Repair/E_VehicleGarbage_01_pile_medium.et",
		    "249.795 1 196.908",
		    "0 -24.662 0"
		);
		hvtItem.campItems.Insert(item_3);
		
		SDRC_Structure item_4 = new SDRC_Structure;
		item_4.Set(
		    "{563AD1F822D50049}Prefabs/EffectsModuleEntities/SDRC_BigSmoke.et",
		    "249.128 1.952 202.453"
		);
		hvtItem.campItems.Insert(item_4);
		
		SDRC_Structure item_5 = new SDRC_Structure;
		item_5.Set(
		    "{9E5EDE35EFBF70B7}PrefabsEditable/Auto/Props/Wrecks/E_M923A1_wreck.et",
		    "250.028 1 200.953",
		    "0 -50.699 0"
		);
		hvtItem.campItems.Insert(item_5);
		
		return hvtItem;
	};	
}