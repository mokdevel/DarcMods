//Mission SDRC_Mission_Stash.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns a stash for loot. This essentially the same as Occupation mission.if

The usage for Stash mission is mainly as a story ending when using DarcStories.
*/

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Stash : SDRC_Mission
{
	private ref SDRC_StashJsonApi m_StashJsonApi = new SDRC_StashJsonApi();	
	private ref SDRC_StashConfig m_Config = new SDRC_StashConfig();	
	private ref SDRC_Camp m_DC_Stash = new SDRC_Camp();
	
	private int m_iSpawnIndex = 0;						//Counter for the item to spawn
	private float m_fSpawnRotation = 0;					//Rotation of the camp for random locations.

	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Stash(DC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Load config
		m_StashJsonApi.Load();
		m_Config = m_StashJsonApi.conf;
		
		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		if (GetSubIdx() == -1)
		{
			SetState(DC_EMissionState.FAILED, DC_EMissionError.WRONG_SUBIDX);
			return;
		}
		m_DC_Stash = m_Config.Stashs[GetSubIdx()];
		HandleRequestGeneralVariables(m_DC_Stash.general, request);
				
		//Camps are randomly rotated
		m_fSpawnRotation = Math.RandomFloat(0, 360);
		
		//Find the position
		vector pos = m_DC_Stash.general.pos[0];
		
		//Find a location for the mission
		if (pos == "0 0 0")
		{
			pos = SDRC_MissionHelper.FindMissionPos(m_DC_Stash.locationTypes, m_DC_Stash.emptySize);
		}
		else
		{
			pos = SDRC_MissionHelper.FindMissionPos(pos, m_DC_Stash.emptySize);
		}
		
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SetState(DC_EMissionState.FAILED, DC_EMissionError.LOCATION_NOT_FOUND);
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
		
		if (GetState() == DC_EMissionState.SPAWN)
		{
			MissionSpawn();
			GetGame().GetCallqueue().CallLater(MissionRun, 2*1000);		//Spawn stuff every two seconds
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
		
		ready = SDRC_CampHelper.Spawn(this, m_iSpawnIndex, m_DC_Stash, m_fSpawnRotation, m_Config.disableArsenal);
		m_iSpawnIndex++;			
		
		if (ready)
		{
			SetState(DC_EMissionState.ACTIVE);
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
	bool disableArsenal;						//Disable arsenal for vehicles so that no other items are found
	int activeDistance;							//Distance of winning and keeping the mission active
	ref array<ref SDRC_Camp> Stashs = {};		//List of Stashs
}

//------------------------------------------------------------------------------------------------
class SDRC_StashJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Stash.json";
	ref SDRC_StashConfig conf = new SDRC_StashConfig();
	
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
		conf.activeDistance = 50;
		conf.missionList = {0};
		//Mission specific		
		//----------------------------------------------------
		conf.Stashs.Insert(Stash0());				
	};
	
	//----------------------------------------------------
	SDRC_Camp Stash0()
	{
		ref SDRC_Camp stash = new SDRC_Camp();
		stash.general.Set(
			0, "index 0: Random stash",
			{"0 0 0"},
			"any",
			"A stash near %l",
			"Loot is yours to take",
			DC_EMissionWinCondition.FIND_IN_15,
			"Loot found.",
			"Loot lost.",
			"",
			"DARC_MISSION", DC_EMissionIcon.GM_MISSION_STASH_MAP,
			0
		);
		
		stash.Set(
			{
			},
			{0, 0},
			{50, 300},
			DC_EWaypointGenerationType.RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
			},
			50, 1.0,
			3
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