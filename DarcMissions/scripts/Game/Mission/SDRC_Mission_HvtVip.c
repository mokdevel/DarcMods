//Mission SDRC_Mission_HvtVip.c

//------------------------------------------------------------------------------------------------
/*!
High Value Target (HVT) - Very Important Person
*/

//------------------------------------------------------------------------------------------------
class SDRC_Mission_HvtVip : SDRC_Mission
{
	const int AI_ACTIVATE_DISTANCE = 8;
	const int AI_TARGET_DEAD_CYCLE_TIME = 5000;
	
	private ref SDRC_HvtVipJsonApi m_HvtVipJsonApi = new SDRC_HvtVipJsonApi();	
	private ref SDRC_HvtVipConfig m_Config = new SDRC_HvtVipConfig();
	private ref SDRC_HvtVip m_DC_HvtVip = new SDRC_HvtVip();
	
	private IEntity m_Building;					//The building for the mission
	private int m_iGroupCount;
	private int m_iSpawnIndex = 0;				//Counter for the AI to spawn
	private SCR_AIGroup m_Target = null;
		
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_HvtVip(SDRC_MissionRequested request)
	{
		//Set some defaults
		SetType(DC_EMissionType.HVTVIP);
		
		//Load config
		m_HvtVipJsonApi.Load();
		m_Config = m_HvtVipJsonApi.conf;
		
		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		if (GetSubIdx() == -1)
		{
			SetState(DC_EMissionState.FAILED);
			return;
		}	
		m_DC_HvtVip = m_Config.hvtVips[GetSubIdx()];
		HandleRequestGeneralVariables(m_DC_HvtVip.general, request);
		
		//Set defaults
		m_iGroupCount = Math.RandomInt(m_DC_HvtVip.groupCount[0], m_DC_HvtVip.groupCount[1]);
		float radius = 10;					//Default size for the radius. Mainly for requested missions to find the nearest building.
		array<string> buildingFilter = {};

		vector pos = m_DC_HvtVip.general.pos[0];
						
		//Find a location for the mission
		if (IsRequested() && pos != "0 0 0")
		{
			//If the missions is requested with a position, any building near the location will be accepted.
			buildingFilter.Insert("");
		}
		else
		{				
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

		//Find the mission house
		m_Building = SDRC_MissionHelper.FindMissionBuilding(pos, buildingFilter, radius);
		if (m_Building)
		{
			pos = m_Building.GetOrigin();
		}
		else //No suitable location found.
		{
			SDRC_Log.Add("[SDRC_Mission_HvtVip] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}			
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_HvtVip.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_DC_HvtVip.general);		
/*		SetMarker(m_Config.showMarker, m_DC_HvtVip.general.markerIcon, m_DC_HvtVip.general.markerType);
		SetHint(m_Config.showHint, m_DC_HvtVip.general.title, m_DC_HvtVip.general.info);
		SetMessages(m_Config.showMessage, m_DC_HvtVip.general.winMessage, m_DC_HvtVip.general.loseMessage);				
		SetWinCondition(m_DC_HvtVip.general.winCondition);*/
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
		//Spawn AI one by one. Sets missions active once ready.
		if (m_iSpawnIndex < m_iGroupCount)
		{
			SCR_AIGroup group = SDRC_MissionHelper.SpawnMissionAIGroup(m_DC_HvtVip.groupTypes.GetRandomElement(), GetPos(), GetFaction());
			if (group)
			{
				SDRC_AIHelper.SetAIGroupSkill(group, m_DC_HvtVip.aiSkill, m_DC_HvtVip.aiPerception);					
				SDRC_AIHelper.SetAIGroupMovementType(group, EMovementType.IDLE);
				m_Groups.Insert(group);				
				SDRC_WPHelper.CreateMissionAIWaypoints(group, DC_EWaypointGenerationType.LOITER, GetPos(), "0 0 0", DC_EWaypointMoveType.LOITER, 10, 50);				
			}
			m_iSpawnIndex++;
		}
		else
		{
			//IEntity entity = SDRC_SpawnHelper.SpawnItemInBuildingWithLoot(m_Building, m_DC_HvtVip.lootBox, true, m_DC_HvtVip.loot.items, m_DC_HvtVip.loot.itemChance);			
			IEntity entity = SDRC_SpawnHelper.SpawnItemInBuildingWithLoot(m_Building, m_DC_HvtVip.lootBox);
			if (entity)
			{
				m_EntityList.Insert(entity);
				m_DC_HvtVip.loot.box = entity;
			}
			else
			{
				SDRC_Log.Add("[SDRC_Mission_HvtVip:MissionSpawn] " +  GetId() + " : Could not spawn loot box: " + m_DC_HvtVip.lootBox, LogLevel.ERROR);								
			}

			//Spawn target enemy and add it to mission faction
			//TBD: The CIV target faction is not set properly
			SCR_AIGroup group = SDRC_AIHelper.SpawnAIInBuilding(m_Building, m_DC_HvtVip.target, m_DC_HvtVip.aiSkill, m_DC_HvtVip.aiPerception, GetFaction());
			if (group)
			{			
				m_Groups.Insert(group);
				m_Target = group;
				SDRC_AIHelper.SetAIGroupMovementType(group, EMovementType.IDLE);
				Faction faction = SDRC_AIHelper.GetFactionWithName(GetFaction());
				group.SetFaction(faction);
			}

			GetGame().GetCallqueue().CallLater(IsTargetDead, AI_TARGET_DEAD_CYCLE_TIME, false);
								
			SetState(DC_EMissionState.ACTIVE);			
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	override void DoWin()
	{	
		SDRC_Loot loot = m_DC_HvtVip.loot;
		SDRC_LootHelper.SpawnItemsToStorage(loot.box, loot.items, loot.itemChance);
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
		if (GetWinCondition() == DC_EMissionWinCondition.HVT_KILL_VIP && GetState() == DC_EMissionState.ACTIVE && GetSuccess() == DC_EMissionSuccess.UNKNOWN)
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
class SDRC_HvtVipConfig : SDRC_MissionConfig
{
	//Mission specific
	
	//Variables here
	int buildingRadius;								//The radius to search for suitable buildings.
	ref array<ref SDRC_HvtVip> hvtVips = {};		//List of HvtVips
}

//------------------------------------------------------------------------------------------------
class SDRC_HvtVip : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref array<EMapDescriptorType> locationTypes = {};
	ref array<int> groupCount = {};			//min, max
	ref array<string> groupTypes = {};
	int aiSkill;
	float aiPerception;
	ref array<string> buildingNames = {};
	//Optional settings
	string lootBox = "";					//The loot box
	ref SDRC_Loot loot = null;
	string target;
	
	void Set(array<EMapDescriptorType> locationTypes_, array<int> groupCount_, array<string> groupTypes_, int aiSkill_, float aiPerception_, array<string> buildingNames_, string lootBox_, string target_)
	{
		locationTypes = locationTypes_;
		groupCount = groupCount_;
		groupTypes = groupTypes_;
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
		conf.missionList = {0,0,1,2,3,3};
		//Mission specific
		conf.buildingRadius = 400;
		//----------------------------------------------------
		conf.hvtVips.Insert(HvtVip0());				
		conf.hvtVips.Insert(HvtVip1());				
		conf.hvtVips.Insert(HvtVip2());				
		conf.hvtVips.Insert(HvtVip3());				
	};
			
	//----------------------------------------------------
	SDRC_HvtVip HvtVip0()
	{
		ref SDRC_HvtVip HvtVip = new SDRC_HvtVip();
		HvtVip.general.Set(
			0, "index 0: HvtVips in cities",
			{"0 0 0"},
			"any",
			"Target near %l.",
			"Assassinate the target",
			DC_EMissionWinCondition.HVT_KILL_VIP,
			"The target has been neutralized.",
			"The target escaped.",
			"",
			"DARC_MISSION", DC_EMissionIcon.GM_MISSION_HVTVIP_MAP,
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
				"G_LIGHT", "G_RECON",
			},
			50, 0.6,
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
			{"0 0 0"},
			"any",
			"Flight controller near %l.",
			"Assassinate the target",
			DC_EMissionWinCondition.HVT_KILL_VIP,
			"The target has been neutralized.",
			"The target escaped.",
			"",
			"DARC_MISSION", DC_EMissionIcon.GM_MISSION_HVTVIP_MAP,
			0		
		);
		HvtVip.Set(
			{
				//We pick any building that matches and ignore location
			},
			{1,2},
			{
				"G_LIGHT",
			},
			50, 0.6,
			{"ControlTowerMilitary_"},
			"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
			"C_OFFICER"
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT", "UTIL_OPTIC", 
				"UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", 
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL",
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
			{"0 0 0"},
			"any",
			"%l is bad for business",
			"Assassinate the business man conducting bad business.",
			DC_EMissionWinCondition.HVT_KILL_VIP,
			"The target has been neutralized.",
			"The target escaped.",
			"",
			"DARC_MISSION", DC_EMissionIcon.GM_MISSION_HVTVIP_MAP,
			0		
		);
		HvtVip.Set(
			{
				//We pick any building that matches and ignore location
			},
			{1,1},
			{
				"G_HEAVY",
			},
			50, 0.6,
			{"Office_E_", "Barracks_01_", "Barracks_E_02_", "MountainHotel_"},
			"{14B16D7580478D1A}Prefabs/Props/Civilian/LootSuitcase_01.et",
			"{A517C72CEF150898}Prefabs/Characters/Factions/CIV/Businessman/Character_CIV_Businessman_2.et"
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"UTIL_ATTACHMENT", "UTIL_OPTIC", 
				"UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", 
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL",
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
			3, "index 3: Businessman in countryside",
			{"0 0 0"},
			"any",
			"Criminal hiding in %l",
			"Assassinate the criminal boss. He tries to keep low profile.",
			DC_EMissionWinCondition.HVT_KILL_VIP,
			"The criminal got what he deserved.",
			"The judgement day is posponed.",
			"",
			"DARC_MISSION", DC_EMissionIcon.GM_MISSION_HVTVIP_MAP,
			0		
		);
		HvtVip.Set(
			{
				//We pick any building that matches and ignore location
			},
			{1,1},
			{
				"G_LIGHT",
			},
			50, 0.6,
			{"House_"},
			"{14B16D7580478D1A}Prefabs/Props/Civilian/LootSuitcase_01.et",
			"{E024A74F8A4BC644}Prefabs/Characters/Factions/CIV/Businessman/Character_CIV_Businessman_1.et"
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"WEAPON_GRENADE", 
				"UTIL_ATTACHMENT", 
				"UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", 
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL",
			};
		loot.Set(0.7, lootItems);
		HvtVip.loot = loot;

		return HvtVip;	
	};		
	
}