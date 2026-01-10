//Mission SDRC_Mission_HvtItem.c

//------------------------------------------------------------------------------------------------
/*!
High Value Target (HVT) - Item target
*/

const string DC_MISSIONCONFIG_FILE_HVTITEM = "dc_missionConfig_HvtItem.json";
const int DC_MISSIONCONFIG_FILE_HVTITEM_VER = 1;

//------------------------------------------------------------------------------------------------
class SDRC_Mission_HvtItem : SDRC_Mission
{
	const int AI_TARGET_DESTROYED_CYCLE_TIME = 5000;
	
	private ref SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_HVTITEM);	
	private ref SDRC_HvtItemConfig m_Config = new SDRC_HvtItemConfig();
	private ref SDRC_HvtItem m_DC_HvtItem = new SDRC_HvtItem();
	
//	private int m_iGroupCount;
	private int m_iSpawnIndex = 0;					//Counter for the item to spawn
	private float m_fSpawnRotation = 0;				//Rotation of the camp for random locations.
	
	private IEntity m_Target = null;
		
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_HvtItem(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Load config
		if (!m_JsonApi.Load(m_Config, SDRC_MissionConfig.Cast(m_Config), DC_MISSIONCONFIG_FILE_HVTITEM_VER))
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.ERROR_LOADING_JSON);
			return;
		}
		m_Config.LoadMissionFiles(DC_MISSIONCONFIG_FILE_HVTITEM_VER);
		
		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		int idx = m_Config.GetSubMissionIdx(GetSubIdx());
		if (idx == -1)
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.WRONG_SUBIDX);
			return;
		}
		m_DC_HvtItem = m_Config.subMissions[idx];			
		HandleRequestGeneralVariables(m_DC_HvtItem.general, request);
		
		//Find position
		vector pos = "0 0 0";
		
		//For requested missions we want have it as close as possible in the requested place.
		if (IsRequested())
		{
			pos = request.general.pos[0];
		}
		else
		{		
			pos = SDRC_MissionHelper.SelectMissionPos(m_DC_HvtItem.general.pos, m_DC_HvtItem.general.size, m_DC_HvtItem.general.locationTypes);
		}

		//If pos has been set, we blindly accept it. Do basic checking for pos.
		if (SDRC_MissionPosHelper.IsValidMissionPos(pos, onlyBasicChecks: IsRequested()) != SDRC_EMissionError.NONE)
		{
			pos = "0 0 0";
		}
				
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.LOCATION_NOT_FOUND);
			return;
		}		
		
		//Camps are randomly rotated
		m_fSpawnRotation = SDRC_Misc.RandomFloat(0, 360);
		SDRC_SpawnHelper.SetStructuresToOrigo(m_DC_HvtItem.campItems);
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_HvtItem.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_DC_HvtItem.general);		
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
		bool ready = false;
		
		ready = SDRC_CampHelper.Spawn(this, m_iSpawnIndex, m_DC_HvtItem, m_fSpawnRotation, m_Config.disableArsenal);
		m_iSpawnIndex++;			
		
		if ( (ready) && (GetState() != SDRC_EMissionState.FAILED) )
		{
			m_Target = GetFromEntityList(m_DC_HvtItem.targetIdx);
			SDRC_Log.Add("[SDRC_Mission_HvtItem:MissionSpawn] " +  GetId() + " : Target: " + m_Target, LogLevel.DEBUG);
			
			GetGame().GetCallqueue().CallLater(IsTargetDestroyed, AI_TARGET_DESTROYED_CYCLE_TIME, false);
			
			SetState(SDRC_EMissionState.ACTIVE);
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	override void DoWin()
	{	
		SDRC_CampHelper.AddLoot(m_DC_HvtItem, GetDifficulty());
		super.DoWin();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	A loop that checks if the target has been eliminated.
	In order to win, the target has to be eliminated, state to ACTIVE and success to be UNKNOWN.
	*/
	void IsTargetDestroyed()
	{
		if (GetWinCondition() == SDRC_EMissionWinCondition.HVT_DESTROY_ITEM && GetState() == SDRC_EMissionState.ACTIVE && GetSuccess() == SDRC_EMissionSuccess.UNKNOWN)
		{
			bool isDestroyed = false;
			if (m_Target)
			{				
				DamageManagerComponent damageManager = DamageManagerComponent.Cast(m_Target.FindComponent(DamageManagerComponent));
				if (damageManager)
				{
					float health = damageManager.GetHealthScaled();
					SDRC_Log.Add("[SDRC_Mission_HvtItem:IsTargetDestroyed] " +  GetId() + " : Target health: " + health, LogLevel.SPAM);
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
				SDRC_Log.Add("[SDRC_Mission_HvtItem:IsTargetDestroyed] " +  GetId() + " : Target destroyed!", LogLevel.DEBUG);
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
	ref array<ref SDRC_HvtItem> subMissions = {};	//List of HvtItems
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_HvtItemConfig data = SDRC_HvtItemConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		
	
	//------------------------------------------------------------------------------------------------
	override void CreateMissionFiles()
	{
		super.CreateMissionFiles();
		
		SDRC_JsonApi2 jsonApi = new SDRC_JsonApi2(SDRC_HvtItemConfig_010.GetFileName());				
		SDRC_HvtItemConfig_010 conf = new SDRC_HvtItemConfig_010();
		jsonApi.Load(conf, SDRC_MissionConfig.Cast(conf), DC_MISSIONCONFIG_FILE_HVTITEM_VER);		
	}
	
	//------------------------------------------------------------------------------------------------
	override void LoadMissionFiles(int ver)
	{
		//Load mission files
		foreach (string missionFile : missionFiles)
		{
			SDRC_JsonApi2 jsonApi = new SDRC_JsonApi2(missionFile);
			SDRC_HvtItemConfig conf = new SDRC_HvtItemConfig();
			
			if (jsonApi.Load(conf, SDRC_MissionConfig.Cast(conf), ver, false))
			{
				foreach (SDRC_HvtItem subMission : conf.subMissions)
				{
					subMissions.Insert(subMission);
				}
				foreach (int idx : conf.missionList)
				{
					missionList.Insert(idx);
				}
			}
		}
	}	

	//------------------------------------------------------------------------------------------------
	int GetSubMissionIdx(int subIdx)
	{
		int idx = -1;
		foreach (int i, SDRC_HvtItem subMission : subMissions)
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
		
		//Default
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		missionList = {10, 11, 12};//{0,1,2};
		missionFiles.Insert("dc_missionConfig_HvtItem_010.json");		
		//Mission specific
		//----------------------------------------------------
		subMissions.Insert(HvtItem0());				
		subMissions.Insert(HvtItem1());				
		subMissions.Insert(HvtItem2());				
	};
	
	//----------------------------------------------------
	SDRC_HvtItem HvtItem0()
	{
		ref SDRC_HvtItem hvtItem = new SDRC_HvtItem();
		hvtItem.general.Set(
			0, "index 0: Destroy generator",
			{"0 0 0"}, 10,
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
			"Destroy generator near %l",
			"The enemy is spreading propaganda.",
			SDRC_EMissionWinCondition.HVT_DESTROY_ITEM,
			"Target destroyed.",
			"You failed in your mission!",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HVTITEM_MAP,
			SDRC_EDifficulty.RANDOM,
			0
		);
		hvtItem.ai.Set(
			{1, 2},
			{"G_ADMIN", "G_LIGHT", "G_LIGHT"},
			50, 1.0,
			{25, 100},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);	
		hvtItem.targetIdx = 1;
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_BAG", "GEAR_BAG", 
			};
		loot.Set(0.7, lootItems);
		hvtItem.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et",
		    "243.294 1 174.755"
		);
		hvtItem.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{4BEFC7FCCB2BA121}Prefabs/Props/Military/Generators/GeneratorMilitary_USSR_01/HvtGeneratorMilitary_USSR_01.et",
		    "249.628 1 176.903"
		);
		hvtItem.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{B4F2701CBBE49C48}Prefabs/Props/Military/Antennas/Antenna_RC292_01.et",
		    "246.121 1 174.906"
		);
		hvtItem.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{C108EB96CCC73E94}PrefabsEditable/Auto/Compositions/Misc/SubCompositions/E_Table_Radio_US_01.et",
		    "243.349 1 176.169"
		);
		hvtItem.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{D0974856AF6DAE81}PrefabsEditable/Auto/Props/Military/Furniture/E_ChairMilitary_US_01.et",
		    "243.376 1 176.954",
		    "0 -40.034 0"
		);
		hvtItem.campItems.Insert(item_4);
		
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{DDF59362051B28BC}Prefabs/Props/Military/Fortification/BarbedTape_KnifeRest.et",
		    "252.327 1 177.128"
		);
		hvtItem.campItems.Insert(item_5);
		
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{DDF59362051B28BC}Prefabs/Props/Military/Fortification/BarbedTape_KnifeRest.et",
		    "251.787 1 181.118",
		    "0 -27.459 0"
		);
		hvtItem.campItems.Insert(item_6);
		
		ref SDRC_Structure item_7 = new SDRC_Structure();
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
		ref SDRC_HvtItem hvtItem = new SDRC_HvtItem();
		hvtItem.general.Set(
			1, "index 1: Destroy supplies",
			{"0 0 0"}, 20,
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
			"any",
			"A supply truck near %l",
			"A truck has crashed and the supplies needs to be destroyed.",
			SDRC_EMissionWinCondition.HVT_DESTROY_ITEM,
			"Supplies never reached the enemy. Good work!",
			"The enemy will fight with their bellies full.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HVTITEM_MAP,
			SDRC_EDifficulty.RANDOM,
			0
		);
		hvtItem.ai.Set(
			{1, 2},
			{"G_ADMIN", "G_LIGHT", "G_LIGHT", "G_HEAVY"},
			50, 1.0,
			{25, 100},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);	
		hvtItem.targetIdx = 1;
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"UTIL_ATTACHMENT", "UTIL_OPTIC",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", 
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"CLOTHING_HEADGEAR", 
				"CLOTHING_UNIFORM",	"CLOTHING_UNIFORM",			
			};
		loot.Set(0.7, lootItems);
		hvtItem.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
		    "250.977 1 204.513",
		    "0 -92.923 0"
		);
		hvtItem.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{47E0234DF4169EE5}Prefabs/Props/Military/CargoContainers/HvtCargoContainer_01_10ft_US_supplies.et",
		    "249.9 1 196.961",
		    "0 -18.644 0"
		);
		hvtItem.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{0542578CA422287A}PrefabsEditable/Auto/Props/Industrial/Repair/E_VehicleGarbage_01_pile_medium.et",
		    "247.843 1 200.018"
		);
		hvtItem.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{0542578CA422287A}PrefabsEditable/Auto/Props/Industrial/Repair/E_VehicleGarbage_01_pile_medium.et",
		    "249.795 1 196.908",
		    "0 -24.662 0"
		);
		hvtItem.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{563AD1F822D50049}Prefabs/EffectsModuleEntities/SDRC_BigSmoke.et",
		    "249.128 1.952 202.453"
		);
		hvtItem.campItems.Insert(item_4);
		
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{9E5EDE35EFBF70B7}PrefabsEditable/Auto/Props/Wrecks/E_M923A1_wreck.et",
		    "250.028 1 200.953",
		    "0 -50.699 0"
		);
		hvtItem.campItems.Insert(item_5);
		
		return hvtItem;
	};	
	
	//----------------------------------------------------
	SDRC_HvtItem HvtItem2()
	{
		ref SDRC_HvtItem hvtItem = new SDRC_HvtItem();
		hvtItem.general.Set(
			2, "index 2: Destroy antenna",
			{"0 0 0"}, 20,
			{},
			"any",
			"Radio antenna close to %l",
			"Disrupt the enemy communications. Destroy the antenna.",
			SDRC_EMissionWinCondition.HVT_DESTROY_ITEM,
			"The comms are dead.",
			"Enemy communication is working loud and clear.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HVTITEM_MAP,		
			SDRC_EDifficulty.RANDOM,
			0
		);
		hvtItem.ai.Set(
			{2, 4},
			{"G_ADMIN", "G_RECON", "G_SPECIAL", "G_LIGHT"},
			50, 1.0,
			{25, 150},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);		
		hvtItem.targetIdx = 9;
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", "WEAPON_RIFLE", "WEAPON_RIFLE", "WEAPON_RIFLE_BIG",
				"UTIL_ATTACHMENT", "UTIL_OPTIC",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", 
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_BAG", "GEAR_BAG", 
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
			};
		loot.Set(0.7, lootItems);
		hvtItem.loot = loot;
		
		// Loot Crate (first position)
		SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
		    "239.161 1 129.774"
		);
		hvtItem.campItems.Insert(item_0);
		
		// Generator Floodlight
		SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{0E94F28FD722B1D8}Prefabs/Props/Military/Generators/GeneratorFloodlight_US_01.et",
		    "241.481 1 131.634",
		    "0 -23.351 0"
		);
		hvtItem.campItems.Insert(item_1);
		
		// Czech Hedgehog group
		SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{7F8D6A0D827EA34B}Prefabs/Props/Military/Fortification/CzechHedgehog_01_base.et",
		    "253.171 0 136.978"
		);
		hvtItem.campItems.Insert(item_2);
		
		SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{7F8D6A0D827EA34B}Prefabs/Props/Military/Fortification/CzechHedgehog_01_base.et",
		    "251.835 0 139.877"
		);
		hvtItem.campItems.Insert(item_3);
		
		SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{7F8D6A0D827EA34B}Prefabs/Props/Military/Fortification/CzechHedgehog_01_base.et",
		    "241.803 0 126.524"
		);
		hvtItem.campItems.Insert(item_4);
		
		// Sandbag Wall group
		SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{9C9C4BED9E19C374}Prefabs/Props/Military/Sandbags/Sandbag_01_wall_solid_burlap.et",
		    "246.482 1 142.942"
		);
		hvtItem.campItems.Insert(item_5);
		
		SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{9C9C4BED9E19C374}Prefabs/Props/Military/Sandbags/Sandbag_01_wall_solid_burlap.et",
		    "239.978 1 141.694",
		    "0 -20.77 0"
		);
		hvtItem.campItems.Insert(item_6);
		
		SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
		    "{9C9C4BED9E19C374}Prefabs/Props/Military/Sandbags/Sandbag_01_wall_solid_burlap.et",
		    "245.447 1 128.339",
		    "0 154.533 0"
		);
		hvtItem.campItems.Insert(item_7);
		
		SDRC_Structure item_8 = new SDRC_Structure();
		item_8.Set(
		    "{9C9C4BED9E19C374}Prefabs/Props/Military/Sandbags/Sandbag_01_wall_solid_burlap.et",
		    "250.782 1 132.262",
		    "0 133.763 0"
		);
		hvtItem.campItems.Insert(item_8);
		
		// Radar
		SDRC_Structure item_9 = new SDRC_Structure();
		item_9.Set(
		    "{A528F04F41F2A2D0}Prefabs/Structures/Military/Radar/ApproachRadar_RPL5_01/ApproachRadar_RPL5_01_on.et",
		    "244.809 1 137.097"
		);
		hvtItem.campItems.Insert(item_9);
		
		// Round Sandbag group
		SDRC_Structure item_10 = new SDRC_Structure();
		item_10.Set(
		    "{B31A14BC1A0E7776}Prefabs/Props/Military/Sandbags/Sandbag_01_round_burlap.et",
		    "243.004 1 143.131",
		    "0 -6.802 0"
		);
		hvtItem.campItems.Insert(item_10);
		
		SDRC_Structure item_11 = new SDRC_Structure();
		item_11.Set(
		    "{B31A14BC1A0E7776}Prefabs/Props/Military/Sandbags/Sandbag_01_round_burlap.et",
		    "248.668 1 129.663",
		    "0 147.731 0"
		);
		hvtItem.campItems.Insert(item_11);
		
		// End High Sandbag group
		SDRC_Structure item_12 = new SDRC_Structure();
		item_12.Set(
		    "{B547CF929FCC6BB7}Prefabs/Props/Military/Sandbags/Sandbag_01_end_high_burlap.et",
		    "252.245 1 133.815",
		    "0 -45.633 0"
		);
		hvtItem.campItems.Insert(item_12);
		
		SDRC_Structure item_13 = new SDRC_Structure();
		item_13.Set(
		    "{B547CF929FCC6BB7}Prefabs/Props/Military/Sandbags/Sandbag_01_end_high_burlap.et",
		    "248.517 1 142.565",
		    "0 28.291 0"
		);
		hvtItem.campItems.Insert(item_13);
		
		// Latrine
		SDRC_Structure item_14 = new SDRC_Structure();
		item_14.Set(
		    "{C32544547260D3F1}PrefabsEditable/Auto/Structures/Civilian/E_Latrine_01.et",
		    "235.318 1 130.793",
		    "0 149.011 0"
		);
		hvtItem.campItems.Insert(item_14);
		
		// Dirt Pile
		SDRC_Structure item_15 = new SDRC_Structure();
		item_15.Set(
		    "{C86856FFC84B573A}PrefabsEditable/Auto/Structures/Military/Fortifications/E_DirtPile_01_large.et",
		    "241.817 0.827 136.378"
		);
		hvtItem.campItems.Insert(item_15);
		
		// Fuel Pallet
		SDRC_Structure item_16 = new SDRC_Structure();
		item_16.Set(
		    "{DB4F61D5F37432F7}Prefabs/Props/Military/Camps/PalletFuel_01.et",
		    "238.013 1 136.755",
		    "0 -16.443 0"
		);
		hvtItem.campItems.Insert(item_16);
		
		// Portable Cabin
		SDRC_Structure item_17 = new SDRC_Structure();
		item_17.Set(
		    "{E30E1250FD927736}Prefabs/Structures/Industrial/Houses/PortableCabin_E_01/PortableCabin_E_01_beige.et",
		    "236.477 0 133.305",
		    "0 -36.874 0"
		);
		hvtItem.campItems.Insert(item_17);
		
		// Camo Net Folded
		SDRC_Structure item_18 = new SDRC_Structure();
		item_18.Set(
		    "{F922F6F1895F41E4}PrefabsEditable/Auto/Structures/Military/CamoNets/Soviet/E_CamoNet_Folded_Soviet.et",
		    "237.727 1 129.295",
		    "0 31.985 0"
		);
		hvtItem.campItems.Insert(item_18);
		
		return hvtItem;
	};				
}

//------------------------------------------------------------------------------------------------
class SDRC_HvtItem : SDRC_Camp
{
	SDRC_Camp camp = SDRC_Camp();
	int targetIdx; 
}