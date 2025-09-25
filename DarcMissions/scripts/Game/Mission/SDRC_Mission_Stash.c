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
	private ref SDRC_StashJsonApi m_StashJsonApi = new SDRC_StashJsonApi(DC_MISSIONCONFIG_FILE_STASH);	
	private ref SDRC_StashConfig m_Config = new SDRC_StashConfig();	
	private ref SDRC_Camp m_DC_Stash = new SDRC_Camp();
	
	private int m_iSpawnIndex = 0;						//Counter for the item to spawn
	private float m_fSpawnRotation = 0;					//Rotation of the camp for random locations.

	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Stash(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Load config
		m_StashJsonApi.CreateMissionFiles();
		m_StashJsonApi.Load();
		m_StashJsonApi.LoadMissionFiles();
		m_Config = m_StashJsonApi.conf;
		
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
				
		//Camps are randomly rotated
		m_fSpawnRotation = Math.RandomFloat(0, 360);
		
		//Find the position
		vector pos = SDRC_MissionHelper.SelectMissionPos(m_DC_Stash.general.pos, m_DC_Stash.general.emptySize, m_DC_Stash.locationTypes);
		
/*		//Find a location for the mission
		if (pos == "0 0 0")
		{
			pos = SDRC_MissionHelper.FindMissionPos(m_DC_Stash.locationTypes, m_DC_Stash.general.emptySize);
		}
		else
		{
			pos = SDRC_MissionHelper.FindMissionPos(pos, m_DC_Stash.general.emptySize);
		}*/
		
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.LOCATION_NOT_FOUND);
			return;
		}	

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
		
		if (ready)
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
}

//------------------------------------------------------------------------------------------------
class SDRC_StashJsonApi : SDRC_JsonApi
{
	ref SDRC_StashConfig conf = new SDRC_StashConfig();
	
	//------------------------------------------------------------------------------------------------
	void SDRC_StashJsonApi(string fileName)
	{		
		SetFileName(fileName);
	}
			
	//------------------------------------------------------------------------------------------------
	bool Load(bool createMissingFiles = true)
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(createMissingFiles);		
		if (!loadContext)
		{
			if (!createMissingFiles)
			{
				return false;
			}
			SetDefaults();
			Save();
			return true;
		}
		
		loadContext.ReadValue("", conf);
		return true;
	}	
	
	//------------------------------------------------------------------------------------------------
	void Save()
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen();
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}
	
	//------------------------------------------------------------------------------------------------
	void CreateMissionFiles()
	{
		SDRC_Stash_010_JsonApi stash010_JsonApi = new SDRC_Stash_010_JsonApi();		
		stash010_JsonApi.Load();
	}
	
	//------------------------------------------------------------------------------------------------
	void LoadMissionFiles()
	{
		//Load mission files
		foreach (string missionFile : conf.missionFiles)
		{
			SDRC_StashJsonApi jsonApi = new SDRC_StashJsonApi(missionFile);		
			if (jsonApi.Load(false))
			{
				foreach (SDRC_Camp subMission : jsonApi.conf.subMissions)
				{
					conf.subMissions.Insert(subMission);
				}
				foreach (int idx : jsonApi.conf.missionList)
				{
					conf.missionList.Insert(idx);
				}
				//conf.missionList.InsertAll(jsonApi.conf.missionList);	//TBD: Not sure why this does not work
			}
		}
	}
			
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		//Default		
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		conf.activeDistance = 50;
		conf.missionList = {0};
		conf.missionFiles.Insert("dc_missionConfig_Stash_010.json");
		conf.missionFiles.Insert("dc_missionConfig_Stash_01x.json");
		//Mission specific		
		//----------------------------------------------------
		conf.subMissions.Insert(Stash0());				
	};
	
	//----------------------------------------------------
	SDRC_Camp Stash0()
	{
		ref SDRC_Camp stash = new SDRC_Camp();
		stash.general.Set(
			0, "index 0: Random stash",
			{"0 0 0"}, 3,
			"any",
			"A stash near %l",
			"Loot is yours to take",
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
		stash.Set(
			{
			},
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
			};
		loot.Set(0.7, lootItems);
		stash.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
			"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
			"100.47 1 144.562"
		);
		stash.campItems.Insert(item_0);
		
		return stash;
	};
}