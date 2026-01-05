//Mission SDRC_Mission_Stash.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns a stash for loot. This essentially the same as Occupation mission.if

The usage for Stash mission is mainly as a story ending when using DarcStories.
*/

const string DC_MISSIONCONFIG_FILE_STASH = "dc_missionConfig_Stash.json";

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Stash : SDRC_Mission
{
	private ref SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_STASH);	
	private ref SDRC_StashConfig m_Config = new SDRC_StashConfig();	
	private ref SDRC_Camp m_DC_Stash = new SDRC_Camp();
	
	private int m_iSpawnIndex = 0;						//Counter for the item to spawn
	private float m_fSpawnRotation = 0;					//Rotation of the camp for random locations.

	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Stash(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Load config
		m_JsonApi.Load(m_Config, SDRC_MissionConfig.Cast(m_Config));
		//m_Config.CreateMissionFiles();
		m_Config.LoadMissionFiles();
		
		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		int idx = m_Config.GetSubMissionIdx(GetSubIdx());
		if (idx == -1)
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.WRONG_SUBIDX);
			return;
		}
		m_DC_Stash = m_Config.subMissions[idx];
		HandleRequestGeneralVariables(m_DC_Stash.general, request);

		//Find a location for the mission
		vector pos = "0 0 0";
		
		//For requested missions we want have it as close as possible in the requested place.
		if (IsRequested())
		{
			pos = request.general.pos[0];
		}
		else
		{				
			pos = SDRC_MissionHelper.SelectMissionPos(m_DC_Stash.general.pos, m_DC_Stash.general.size, m_DC_Stash.general.locationTypes, SDRC_Conf.POSITION_RANDOMIZATION);
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
		SDRC_SpawnHelper.SetStructuresToOrigo(m_DC_Stash.campItems);
				
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Stash.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_DC_Stash.general);
		SetActiveDistance(m_Config.activeDistance);		//Change the m_iActiveDistance to a mission specific one.		
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == SDRC_EMissionState.SPAWN)
		{
			MissionSpawn();
			GetGame().GetCallqueue().CallLater(MissionRun, 2*1000);		//Spawn stuff every two seconds
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
		
		ready = SDRC_CampHelper.Spawn(this, m_iSpawnIndex, m_DC_Stash, m_fSpawnRotation, m_Config.disableArsenal);
		m_iSpawnIndex++;			
		
		if ( (ready) && (GetState() != SDRC_EMissionState.FAILED) )
		{
			SetState(SDRC_EMissionState.ACTIVE);
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	override void DoWin()
	{	
		SDRC_CampHelper.AddLoot(m_DC_Stash);
		super.DoWin();
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_StashConfig : SDRC_MissionConfig
{
	//Mission specific	
	int activeDistance;							//Distance of winning and keeping the mission active
	ref array<ref SDRC_Camp> subMissions = {};	//List of Stashs
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_StashConfig data = SDRC_StashConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------	
	override void LoadMissionFiles()
	{
		//Load mission files
		foreach (string missionFile : missionFiles)
		{
			SDRC_JsonApi2 jsonApi = new SDRC_JsonApi2(missionFile);
			SDRC_StashConfig conf = new SDRC_StashConfig();
			
			if (jsonApi.Load(conf, SDRC_MissionConfig.Cast(conf), false))
			{
				foreach (SDRC_Camp subMission : conf.subMissions)
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
		
		SDRC_JsonApi2 jsonApi = new SDRC_JsonApi2(SDRC_StashConfig_010.GetFileName());				
		SDRC_StashConfig_010 conf = new SDRC_StashConfig_010();
		jsonApi.Load(conf, SDRC_MissionConfig.Cast(conf));
	}
				
	//------------------------------------------------------------------------------------------------	
	int GetSubMissionIdx(int subIdx)
	{
		int idx = -1;
		foreach (int i, SDRC_Camp subMission : subMissions)
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
		
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		activeDistance = 50;
		missionList = {0,0,0};
		missionFiles.Insert("dc_missionConfig_Stash_010.json");
/*		#ifndef SDRC_RELEASE
			missionFiles.Insert("dc_missionConfig_Stash_01x.json");	//Just for testing that dummy files don't appear
		#endif*/
		//Mission specific		
		//----------------------------------------------------
		subMissions.Insert(Stash0());				
	};
	
	//----------------------------------------------------
	SDRC_Camp Stash0()
	{
		ref SDRC_Camp stash = new SDRC_Camp();
		stash.general.Set(
			0, "index 0: Random stash",
			{"0 0 0"}, 3,
			{},
			"any",
			"A stash near %l",
			"Hurry, loot is yours to take!",
			SDRC_EMissionWinCondition.FIND_IN_15,
			"Loot found.",
			"Loot lost.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_STASH_MAP,
			SDRC_EMissionDifficulty.NORMAL,
			0
		);		
		stash.ai.Set(
			{0, 1},
			{"G_RECON"},
			50, 1.0,
			{50, 300},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_HEADGEAR", "GEAR_HEADGEAR",
			};
		loot.Set(0.7, lootItems);
		stash.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
			"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
//			"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
//			"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et",
//			"{14B16D7580478D1A}Prefabs/Props/Civilian/LootSuitcase_01.et",
//			"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
//			"{DBC8E6A4DD948C96}Prefabs/Props/Military/SupplyBox/SupplyPortableContainers/SupplyPortableContainer_01/LootSupplyPortableContainers_01_large_item.et",
			"100.47 1 144.562"
		);
		stash.campItems.Insert(item_0);
		
		return stash;
	};	
}