//Mission SDRC_Mission_HvtVip.c

//------------------------------------------------------------------------------------------------
/*!
High Value Target (HVT) - Very Important Person
*/

const string DC_MISSIONCONFIG_FILE_HVTVIP = "dc_missionConfig_HvtVip.json";
const int DC_MISSIONCONFIG_FILE_HVTVIP_VER = 1;

//------------------------------------------------------------------------------------------------
class SDRC_Mission_HvtVip : SDRC_Mission
{
	private ref SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_HVTVIP);	
	private ref SDRC_HvtVipConfig m_Config = new SDRC_HvtVipConfig();
	private ref SDRC_HvtVip m_DC_HvtVip = new SDRC_HvtVip();
	
	const int AI_ACTIVATE_DISTANCE = 8;
	const int AI_TARGET_DEAD_CYCLE_TIME = 5000;
	
	private IEntity m_Building;					//The building for the mission
	private int m_iGroupCount;
	private int m_iSpawnIndex = 0;				//Counter for the AI to spawn
	private SCR_AIGroup m_Target = null;
		
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_HvtVip(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Load config
		if (!m_JsonApi.Load(m_Config, SDRC_MissionConfig.Cast(m_Config), DC_MISSIONCONFIG_FILE_HVTVIP_VER))
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.ERROR_LOADING_JSON);
			return;
		}
		m_Config.LoadMissionFiles(DC_MISSIONCONFIG_FILE_HVTVIP_VER);
		
		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		int idx = m_Config.GetSubMissionIdx(GetSubIdx());
		if (idx == -1)
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.WRONG_SUBIDX);
			return;
		}
		m_DC_HvtVip = m_Config.subMissions[idx];			
		HandleRequestGeneralVariables(m_DC_HvtVip.general, request);
		
		//Set defaults
		m_iGroupCount = m_DC_HvtVip.ai.GetCount(GetDifficulty());
		float radius = 100;					//Default size for the radius. 
		array<string> buildingFilter = {};

		vector pos = SDRC_MissionHelper.SelectMissionPos(m_DC_HvtVip.general.pos, m_DC_HvtVip.general.size, m_DC_HvtVip.general.locationTypes);
						
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
			buildingFilter = m_DC_HvtVip.buildingNames;
			
			if (pos == "0 0 0")
			{
				//If no locationTypes defined, we search for any building matching on the map
				if (m_DC_HvtVip.general.locationTypes.IsEmpty())
				{
					radius = -1;
				}
				else
				{
					pos = SDRC_MissionHelper.FindMissionPos(m_DC_HvtVip.general.locationTypes, m_DC_HvtVip.general.size);
				}
			}
		}

		//If failed, stop
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.LOCATION_NOT_FOUND);
			return;
		}	
		
		//Find the mission house
		m_Building = SDRC_MissionHelper.FindMissionBuilding(pos, buildingFilter, radius);
		if (m_Building)
		{
			pos = m_Building.GetOrigin();
		}
		else //No suitable location found.
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.SUITABLE_BUILDING_NOT_FOUND);
			return;
		}			
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_HvtVip.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_DC_HvtVip.general);		
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
		if (m_iSpawnIndex < m_iGroupCount)
		{
			//SCR_AIGroup group = SDRC_MissionHelper.SpawnMissionAIGroup(m_DC_HvtVip.ai.types.GetRandomElement(), GetPos(), GetFaction());
			SCR_AIGroup group = SDRC_MissionHelper.SpawnMissionAIGroupRandom(m_DC_HvtVip.ai.types, GetPos(), GetFaction());
			if (group)
			{
				SDRC_AIHelper.SetAIGroupSettings(group, m_DC_HvtVip.ai.GetSkill(GetDifficulty()), m_DC_HvtVip.ai.GetPerception(GetDifficulty()));					
				SDRC_AIHelper.SetAIGroupMovementType(group, EMovementType.IDLE);
				m_Groups.Insert(group);				
				SDRC_WPHelper.CreateMissionAIWaypoints(group, SDRC_EWaypointGenerationType.LOITER, GetPos(), "0 0 0", SDRC_EWaypointMoveType.LOITER, 10, 50);				
			}
			m_iSpawnIndex++;
		}
		else
		{
			IEntity entity = SDRC_SpawnHelper.SpawnItemInBuildingWithLoot(m_Building, m_DC_HvtVip.lootBox);
			if (entity)
			{
				m_EntityList.Insert(entity);
				m_DC_HvtVip.loot.box = entity;
				//Handle loot difficulty
				m_DC_HvtVip.loot.itemChance = SDRC_MissionHelper.GetLootChance(m_DC_HvtVip.loot.itemChance, GetDifficulty());
			}
			else
			{
				SDRC_Log.Add("[SDRC_Mission_HvtVip:MissionSpawn] " +  GetId() + " : Could not spawn loot box: " + m_DC_HvtVip.lootBox, LogLevel.ERROR);								
			}

			//Spawn target enemy and add it to mission faction
			SCR_AIGroup group = SDRC_AIHelper.SpawnAIInBuilding(m_Building, m_DC_HvtVip.target, GetFaction(), m_DC_HvtVip.ai.GetSkill(), m_DC_HvtVip.ai.GetPerception(), );
			if (group)
			{			
				m_Groups.Insert(group);
				m_Target = group;
				SDRC_AIHelper.SetAIGroupMovementType(group, EMovementType.IDLE);
				Faction faction = SDRC_EnemyHelper.GetFactionWithName(GetFaction());
				if (!faction)
				{
					SDRC_Log.Add("[SDRC_Mission_HvtVip:MissionSpawn] " +  GetId() + " : Vip target faction not set.", LogLevel.ERROR);													
				}
				group.SetFaction(faction);
			}

			GetGame().GetCallqueue().CallLater(IsTargetDead, AI_TARGET_DEAD_CYCLE_TIME, false);
								
			SetState(SDRC_EMissionState.ACTIVE);			
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	override void DoWin()
	{	
		SDRC_Loot loot = m_DC_HvtVip.loot;
		SDRC_LootHelper.SpawnItemsToStorage(loot.box, loot.items, loot.itemChance, GetDifficulty());
		super.DoWin();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	A loop that checks if the target has been eliminated.
	It also keeps the target disabled until a player is near by.
	In order to win, the target has to be eliminated, state to ACTIVE and success to be UNKNOWN.
	*/
	void IsTargetDead()
	{
		if (GetWinCondition() == SDRC_EMissionWinCondition.HVT_KILL_VIP && GetState() == SDRC_EMissionState.ACTIVE && GetSuccess() == SDRC_EMissionSuccess.UNKNOWN)
		{
			if (SDRC_AIHelper.IsGroupDead(m_Target))
			{
				SDRC_Log.Add("[SDRC_Mission_HvtVip:IsTargetDead] " +  GetId() + " : Target dead!", LogLevel.DEBUG);
				DoWin();
				return;
			}
			
			//TBD: This could also use "CharacterControllerComponent > SetDisableMovementControls" to disable movement. Something to investigate.
			
			//Check if any player is near by and activate AI
			if (SDRC_PlayerHelper.IsAnyPlayerCloseToPos(m_Target.GetLeaderEntity().GetOrigin(), AI_ACTIVATE_DISTANCE, 0))
			{
				SDRC_AIHelper.SetAIGroupEnableDelayed(m_Target, true);
			}
			else
			{
				SDRC_AIHelper.SetAIGroupEnableDelayed(m_Target, false);
			}
			GetGame().GetCallqueue().CallLater(IsTargetDead, AI_TARGET_DEAD_CYCLE_TIME, false);
		}
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_HvtVip : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();
	#ifndef NEW_VERSION_WIP	
		ref SDRC_MissionConfigSecondWave secondWave = new SDRC_MissionConfigSecondWave();	
	#endif
	ref array<string> buildingNames = {};
	//Optional settings
	#ifdef NEW_VERSION_WIP		
		ref SDRC_MissionConfigSecondWave secondWave = null;
	#endif
	string lootBox = "";					//The loot box
	ref SDRC_Loot loot = null;
	string target;
	
	void Set(array<string> buildingNames_, string lootBox_, string target_)
	{
		buildingNames = buildingNames_;	
		lootBox = lootBox_;
		target = target_;
	}	
}
	
//------------------------------------------------------------------------------------------------
class SDRC_HvtVipConfig : SDRC_MissionConfig
{
	//Mission specific
	int buildingRadius;								//The radius to search for suitable buildings.
	ref array<ref SDRC_HvtVip> subMissions = {};	//List of HvtVips
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_HvtVipConfig data = SDRC_HvtVipConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------	
	override void LoadMissionFiles(int ver)
	{
		//Load mission files
		foreach (string missionFile : missionFiles)
		{
			SDRC_JsonApi2 jsonApi = new SDRC_JsonApi2(missionFile);
			SDRC_HvtVipConfig conf = new SDRC_HvtVipConfig();
			
			if (jsonApi.Load(conf, SDRC_MissionConfig.Cast(conf), ver, false))
			{
				foreach (SDRC_HvtVip subMission : conf.subMissions)
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
	override void CreateMissionFiles()
	{
		super.CreateMissionFiles();
	}
	
	//------------------------------------------------------------------------------------------------
	int GetSubMissionIdx(int subIdx)
	{
		int idx = -1;
		foreach (int i, SDRC_HvtVip subMission : subMissions)
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
		missionList = {0,1,2,3,3,4};
		//Mission specific
		buildingRadius = 400;
		//----------------------------------------------------
		subMissions.Insert(HvtVip0());				
		subMissions.Insert(HvtVip1());				
		subMissions.Insert(HvtVip2());				
		subMissions.Insert(HvtVip3());				
		subMissions.Insert(HvtVip4());				
	};
			
	//----------------------------------------------------
	SDRC_HvtVip HvtVip0()
	{
		ref SDRC_HvtVip HvtVip = new SDRC_HvtVip();
		HvtVip.general.Set(
			0, "index 0: HvtVips in cities",
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
			"Target near %l.",
			"Assassinate the target",
			SDRC_EMissionWinCondition.HVT_KILL_VIP,
			"The target has been neutralized.",
			"The target escaped.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HVTVIP_MAP,
			SDRC_EDifficulty.RANDOM,
			0		
		);
		HvtVip.ai.Set(
			{1, 2},
			{"G_LIGHT", "G_RECON",},
			50, 0.6,
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE		
		);
		HvtVip.Set(
			{"ShopModern_", "Villa_", "MunicipalOffice_", "PubVillage_", "Office_E_", "MountainHotel_"},
			"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
			"C_OFFICER"
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
		HvtVip.loot = loot;

		return HvtVip;	
	};
	
	//----------------------------------------------------
	SDRC_HvtVip HvtVip1()
	{
		ref SDRC_HvtVip HvtVip = new SDRC_HvtVip();
		HvtVip.general.Set(
			1, "index 1: HvtVips in control towers",
			{"0 0 0"}, 0,
			{
				//We pick any building that matches and ignore location
			},
			"any",
			"Flight controller near %l.",
			"Assassinate the target",
			SDRC_EMissionWinCondition.HVT_KILL_VIP,
			"The target has been neutralized.",
			"The target escaped.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HVTVIP_MAP,
			SDRC_EDifficulty.RANDOM,
			0		
		);
		HvtVip.ai.Set(
			{1, 3},
			{"G_LIGHT",},
			50, 0.6,
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE		
		);
		HvtVip.Set(
			{"ControlTowerMilitary_"},
			"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
			"C_OFFICER"
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT", "UTIL_OPTIC", 
				"UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", 
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL",
				"CLOTHING_HEADGEAR", "CLOTHING_UNIFORM",
			};
		loot.Set(0.7, lootItems);
		HvtVip.loot = loot;

		return HvtVip;	
	};	
	
	//----------------------------------------------------
	SDRC_HvtVip HvtVip2()
	{
		ref SDRC_HvtVip HvtVip = new SDRC_HvtVip();
		HvtVip.general.Set(
			2, "index 2: Businessman with bad business",
			{"0 0 0"}, 0,
			{
				//We pick any building that matches and ignore location
			},
			"any",
			"%l is bad for business",
			"Assassinate the business man conducting bad business.",
			SDRC_EMissionWinCondition.HVT_KILL_VIP,
			"The target has been neutralized.",
			"The target escaped.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HVTVIP_MAP,
			SDRC_EDifficulty.RANDOM,
			0		
		);
		HvtVip.ai.Set(
			{1, 2},
			{"G_HEAVY",},
			50, 0.6,
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE		
		);
		HvtVip.Set(
			{"Office_E_", "Barracks_01_", "Barracks_E_02_", "MountainHotel_"},
			"{14B16D7580478D1A}Prefabs/Props/Civilian/LootSuitcase_01.et",
			"{A517C72CEF150898}Prefabs/Characters/Factions/CIV/Businessman/Character_CIV_Businessman_2.et"
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT", "UTIL_OPTIC", 
				"UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", 
				"ITEM_MEDICAL", "ITEM_MEDICAL",	
				"ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_BAG", 
				"CLOTHING_HEADGEAR", "CLOTHING_HEADGEAR", 
				"CLOTHING_UNIFORM",	"CLOTHING_UNIFORM",
			};
		loot.Set(0.7, lootItems);
		HvtVip.loot = loot;

		return HvtVip;	
	};		
	
	//----------------------------------------------------
	SDRC_HvtVip HvtVip3()
	{
		ref SDRC_HvtVip HvtVip = new SDRC_HvtVip();
		HvtVip.general.Set(
			3, "index 3: Criminal in countryside",
			{"0 0 0"}, 0, 
			{
				//We pick any building that matches and ignore location
			},
			"any",
			"Criminal hiding in %l",
			"Assassinate the criminal boss. He tries to keep low profile.",
			SDRC_EMissionWinCondition.HVT_KILL_VIP,
			"The criminal got what he deserved.",
			"The judgement day is postponed.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HVTVIP_MAP,
			SDRC_EDifficulty.RANDOM,
			0		
		);
		HvtVip.ai.Set(
			{1,1},
			{"G_LIGHT",},
			50, 0.6,
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE		
		);
		HvtVip.Set(
			{"House_"},
			"{14B16D7580478D1A}Prefabs/Props/Civilian/LootSuitcase_01.et",
			"{E024A74F8A4BC644}Prefabs/Characters/Factions/CIV/Businessman/Character_CIV_Businessman_1.et"
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"WEAPON_GRENADE", 
				"UTIL_ATTACHMENT", 
				"UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", 
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_HANDWEAR", "GEAR_UNIFORM", 
			};
		loot.Set(0.7, lootItems);
		HvtVip.loot = loot;

		return HvtVip;	
	};		

	//----------------------------------------------------
	SDRC_HvtVip HvtVip4()
	{
		ref SDRC_HvtVip HvtVip = new SDRC_HvtVip();
		HvtVip.general.Set(
			4, "index 4: Drunks in pubs",
			{"0 0 0"}, 0, 
			{
				//We pick any building that matches and ignore location
			},
			"any",
			"Drunks and punks in %l",
			"Assassinate the drunk leader. Most likely hides in a pub.",
			SDRC_EMissionWinCondition.HVT_KILL_VIP,
			"Free drinks for everyone!",
			"No drinks for you this time.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HVTVIP_MAP,
			SDRC_EDifficulty.RANDOM,
			0		
		);
		HvtVip.ai.Set(
			{1,2},
			{"G_LIGHT", "G_ADMIN", "G_MEDICAL"},
			50, 0.6,
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE		
		);
		HvtVip.Set(
			{"PubVillage_", "ShopHouse_", },
			"{14B16D7580478D1A}Prefabs/Props/Civilian/LootSuitcase_01.et",
			"{A2B367FFF37E6416}Prefabs/Characters/Factions/CIV/Dockworker/Character_CIV_Dockworker_5.et"
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
				"CLOTHING_HEADGEAR", "CLOTHING_UNIFORM", "CLOTHING_UNIFORM", "CLOTHING_UNIFORM", 
				"WEAPON_GRENADE", 
				"UTIL_ATTACHMENT", 
				"UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", 
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", 
			};
		loot.Set(0.7, lootItems);
		HvtVip.loot = loot;

		return HvtVip;	
	};		
}