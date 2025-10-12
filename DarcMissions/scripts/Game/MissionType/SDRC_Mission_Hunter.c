//Mission SDRC_Mission_Hunter.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns one (or more) hunter groups that search players. Original idea and parts of the 
code are from HunterKiller mod by Rabid Squirrel. This mod worked as a great example when taking the 
first steps in AR modding.

See: https://reforger.armaplatform.com/workshop/597324ECFC025225-HunterKiller 
Note: The original HunterKiller mod is discontinued.
*/

//TBD: It should be possible to define multiple different hunter types.

const string DC_MISSIONCONFIG_FILE_HUNTER = "dc_missionConfig_Hunter.json";

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Hunter : SDRC_Mission
{
	private ref SDRC_HunterJsonApi m_HunterJsonApi = new SDRC_HunterJsonApi(DC_MISSIONCONFIG_FILE_HUNTER);				
	private ref SDRC_HunterConfig m_Config = new SDRC_HunterConfig();	
	private ref SDRC_Hunter m_DC_Hunter = new SDRC_Hunter();
	
	private const int DC_LOCATION_SEACRH_ITERATIONS = 10;	//How many different spots to try for a mission before giving up	
	private const int DC_GROUP_SPAWN_DELAY = 2000;			//Delay between AI group spawns
	
	private ref array<IEntity> m_Locations = {};
	private int m_iGroupsToSpawn	= 0;	//Amount of groups to spawn
	private int m_iGroupsSpawned = 0;	//The amount of groups spawned. Between spawns, a group may be killed so the total of m_Groups is not reliable to know the count.
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Hunter(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Load config	
		m_HunterJsonApi.CreateMissionFiles();
		m_HunterJsonApi.Load();
		m_HunterJsonApi.LoadMissionFiles();			
		m_Config = m_HunterJsonApi.conf;
		
		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		int idx = m_Config.GetSubMissionIdx(GetSubIdx());
		if (idx == -1)
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.WRONG_SUBIDX);
			return;
		}
		m_DC_Hunter = m_Config.subMissions[idx];	
		HandleRequestGeneralVariables(m_DC_Hunter.general, request);
		
		//Set spawn count
		m_iGroupsToSpawn = m_DC_Hunter.ai.GetCount(m_DC_Hunter.general.difficulty);
		
		//Find position
		bool positionFound = false;
//		vector pos = SDRC_MissionHelper.SelectMissionPos(m_DC_Hunter.general.pos);
		vector pos = m_DC_Hunter.general.pos.GetRandomElement();
		
		//For requested missions we want have it as close as possible in the requested place.
		int randomPos = -1;		
		if (IsRequested())
		{
			randomPos = 0;
		}

		if (pos != "0 0 0")
		{
			//If pos has been set, we blindly accept it
			positionFound = true;
		}			
		else
		{
			for (int i = 0; i < DC_LOCATION_SEACRH_ITERATIONS; i++)
			{
				if (m_DC_Hunter.general.locationTypes.IsEmpty())
				{
					pos = SDRC_Misc.GetRandomWorldPos();
				}
				else
				{
					pos = SDRC_MissionHelper.FindMissionPos(m_DC_Hunter.general.locationTypes, m_DC_Hunter.general.size, randomPos);
				}
							
				if (SDRC_MissionHelper.IsValidMissionPos(pos) == SDRC_EMissionError.NONE)
				{			
					//Find a position close to any player
					if (SDRC_PlayerHelper.IsAnyPlayerCloseToPos(pos, m_Config.maxDistanceToPlayer, m_Config.minDistanceToPlayer))
					{
						positionFound = true;
					
						SDRC_Log.Add("[SDRC_Mission_Hunter] Location for spawn " + pos, LogLevel.DEBUG);
						break;
					}
					else
					{						
						SDRC_Log.Add("[SDRC_Mission_Hunter] Invalid mission position. Try " + (i + 1) + "/" + DC_LOCATION_SEACRH_ITERATIONS, LogLevel.SPAM);
					}
				}
			}
		}
		
		if (!positionFound)	//No suitable location found.
		{				
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.LOCATION_NOT_FOUND);
			return;
		}	
				
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Hunter.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_DC_Hunter.general);		
/*		SetMarker(m_Config.showMarker, m_DC_Hunter.general.markerIcon, m_DC_Hunter.general.markerType);
		SetHint(m_Config.showHint, m_DC_Hunter.general.title, m_DC_Hunter.general.info);		
		SetMessages(m_Config.showMessage, m_DC_Hunter.general.winMessage, m_DC_Hunter.general.loseMessage);		
		SetWinCondition(m_DC_Hunter.general.winCondition);*/
		SetActiveDistance(m_Config.maxDistanceToPlayer);		//Change the m_iActiveDistance to a mission specific one.		
	}

	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == SDRC_EMissionState.SPAWN)
		{
			MissionSpawn();
			SetState(SDRC_EMissionState.ACTIVE);
		}
		
		if (GetState() == SDRC_EMissionState.END)
		{
			MissionEnd();
			SetState(SDRC_EMissionState.EXIT);
		}
		
		if (GetState() == SDRC_EMissionState.ACTIVE)
		{
			if (m_iGroupsSpawned < m_iGroupsToSpawn)
			{
				SDRC_Log.Add("[SDRC_Mission_Hunter:MissionRun] Waiting for all groups to spawn. " + m_iGroupsSpawned + "/" + m_iGroupsToSpawn + " ready.", LogLevel.DEBUG);
			}
			else
			{
				if (!IsActive())
				{
					SetState(SDRC_EMissionState.END);
				}
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
		SDRC_Log.Add(("[SDRC_Mission_Hunter:MissionSpawn] Spawning " + m_iGroupsToSpawn + " groups"), LogLevel.NORMAL);
		
		for (int i = 1; i <= m_iGroupsToSpawn; i++)
		{
			SDRC_Log.Add(("[SDRC_Mission_Hunter:MissionSpawn] Initiating spawn for group " + i + " of " + m_iGroupsToSpawn), LogLevel.DEBUG);
			
			GetGame().GetCallqueue().CallLater(SpawnGroup, (DC_GROUP_SPAWN_DELAY + i*1000), false);
		}
		
		SDRC_Log.Add("[SDRC_Mission_Hunter:MissionSpawn] INIT ready. Changing to ACTIVE state", LogLevel.DEBUG);		
	}

	//------------------------------------------------------------------------------------------------
	protected void GroupLifeCycle(SCR_AIGroup group)
	{
		if (group)
		{
			if (group.GetLeaderEntity())
			{			
				//Check if there are any nearby AI
				IEntity closestPlayer = SDRC_PlayerHelper.PlayerGetClosestToPos(group.GetLeaderEntity().GetOrigin(), 0, m_Config.maxDistanceToPlayer);
			
				if (closestPlayer != null)
				{
					if (group.GetAgentsCount() > 0)
					{
						SDRC_Log.Add("[SDRC_Mission_Hunter:GroupLifeCycle] Creating waypoint for group: " + group.GetID(), LogLevel.SPAM);
						
						SDRC_WPHelper.RemoveWaypoints(group);
						AIWaypoint wp = GetWaypoint(group);
						group.AddWaypoint(wp);
						GetGame().GetCallqueue().CallLater(GroupLifeCycle, m_Config.missionCycleTime*1000, false, group);
						return;
					}
				}
				else
				{
					// If there aren't any players close, delete the group
					SDRC_Log.Add("[SDRC_Mission_Hunter:GroupLifeCycle] No players nearby, deleting group: " + group.GetID(), LogLevel.NORMAL);
					SDRC_AIHelper.GroupDelete(group);
				}
			}
		}
	}	
		
	//------------------------------------------------------------------------------------------------	
	protected void SpawnGroup()
	{
		vector spawnLocation = GetSpawnPointForAI();
		
		if (spawnLocation)
		{
			string groupToSpawn = m_DC_Hunter.ai.types.GetRandomElement();
			SCR_AIGroup group = SDRC_AIHelper.SpawnGroup(groupToSpawn, spawnLocation, GetFaction());
			
			if (group)
			{
				SDRC_AIHelper.SetAIGroupSkill(group, m_DC_Hunter.ai.GetSkill(m_DC_Hunter.general.difficulty), m_DC_Hunter.ai.GetPerception(m_DC_Hunter.general.difficulty));					
				m_Groups.Insert(group);
				m_iGroupsSpawned++;
				SDRC_Log.Add("[SDRC_Mission_Hunter:SpawnHunterGroup] Group spawned to " + spawnLocation, LogLevel.NORMAL);				
			}
			else
			{
				SDRC_Log.Add("[SDRC_Mission_Hunter:SpawnHunterGroup] Unable to spawn group!", LogLevel.ERROR);
				return;
			}
			
			// Manage the life cycle for the spawned group
			GetGame().GetCallqueue().CallLater(GroupLifeCycle, m_Config.missionCycleTime*1000, false, group);
		}
		else
		{
			SDRC_Log.Add("[SDRC_Mission_Hunter:SpawnHunterGroup] Unable to find spawn point for group! Retrying...", LogLevel.WARNING);
			
			// Try again later
			GetGame().GetCallqueue().CallLater(SpawnGroup, DC_GROUP_SPAWN_DELAY, false);
		}
	}

	//------------------------------------------------------------------------------------------------	
	/*!
	Find a spawn position for hunters that is close to mission position.
	*/
	protected vector GetSpawnPointForAI()
	{
		vector pos = GetPos();
		
		if (SDRC_SpawnHelper.FindEmptyPos(pos, 100, 8))
		{						
			SDRC_Log.Add("[SDRC_Mission_Hunter:GetSpawnPointForAI] Pos: " + pos, LogLevel.SPAM);
		}
		else
			SDRC_Log.Add("[SDRC_Mission_Hunter:GetSpawnPointForAI] Could not find an empty pos.", LogLevel.ERROR);
		
		return pos;
	}
		
	//------------------------------------------------------------------------------------------------
	protected AIWaypoint GetWaypoint(SCR_AIGroup group)
	{
		IEntity closestPlayer = SDRC_PlayerHelper.PlayerGetClosestToPos(group.GetLeaderEntity().GetOrigin());
		
		if (closestPlayer != null)
		{
			AIWaypoint waypoint = SDRC_WPHelper.CreateWaypointEntity(SDRC_EWaypointMoveType.MOVE);
			waypoint.SetOrigin(SDRC_Misc.RandomizePos(closestPlayer.GetOrigin(), m_Config.rndDistanceToPlayer));
			return waypoint;
		}
		else
		{
			SDRC_Log.Add("[SDRC_Mission_Hunter:GetWaypoint] Unable to find player for waypoint creation!", LogLevel.ERROR);
			SDRC_AIHelper.GroupDelete(group);
		}
		
		return null;
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_HunterConfig : SDRC_MissionConfig
{
	//Mission specific
	int minDistanceToPlayer;						//Hunter group minimum distance to player for spawn
	int maxDistanceToPlayer;						//...max distance to despawn
	int rndDistanceToPlayer;						//The error on the location where AI thinks you are. (0..rndDistanceToPlayer)  
	ref array<ref SDRC_Hunter> subMissions = {};	//List of hunters
	
	//------------------------------------------------------------------------------------------------
	int GetSubMissionIdx(int subIdx)
	{
		int idx = -1;
		foreach (int i, SDRC_Hunter subMission : subMissions)
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
class SDRC_Hunter : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();
}

//------------------------------------------------------------------------------------------------
class SDRC_HunterJsonApi : SDRC_JsonApi
{
	ref SDRC_HunterConfig conf = new SDRC_HunterConfig();
	
	//------------------------------------------------------------------------------------------------
	void SDRC_HunterJsonApi(string fileName)
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
	}
	
	//------------------------------------------------------------------------------------------------
	void LoadMissionFiles()
	{
		//Load mission files
		foreach (string missionFile : conf.missionFiles)
		{
			SDRC_HunterJsonApi jsonApi = new SDRC_HunterJsonApi(missionFile);		
			if (jsonApi.Load(false))
			{
				foreach (SDRC_Hunter subMission : jsonApi.conf.subMissions)
				{
					conf.subMissions.Insert(subMission);
				}
				foreach (int idx : jsonApi.conf.missionList)
				{
					conf.missionList.Insert(idx);
				}
			}
		}
	}
				
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		//Default
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT * 3;		//The cycle with Hunter mission can be really slow
		conf.showMarker = false;
		conf.missionList = {0,0,0,1,1,1,2};
		//Mission specific
		conf.minDistanceToPlayer = 200;
		conf.maxDistanceToPlayer = 800;
		conf.rndDistanceToPlayer = 60;
		
		//----------------------------------------------------
		conf.subMissions.Insert(Hunter0());				
		conf.subMissions.Insert(Hunter1());				
		conf.subMissions.Insert(Hunter2());				
	}
	
	//----------------------------------------------------
	SDRC_Hunter Hunter0()
	{		
		ref SDRC_Hunter hunter = new SDRC_Hunter();
		hunter.general.Set(
			0, "index 0: general mission",
			{"0 0 0"}, 2,
			{},
			"any",
			"Hunters",
			"They are coming for you... Last time they were seen close to %l.",
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			"You outsmarted the hunters.",
			"Hunters lost track of you.", 
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HUNTER_MAP,
			SDRC_EMissionDifficulty.NORMAL,
			0
		);
		hunter.ai.Set(
			{1, 2},
			{"G_SMALL"},
			30, 0.4,
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE
		);
		return hunter;
	}	
	//----------------------------------------------------
	SDRC_Hunter Hunter1()
	{		
		ref SDRC_Hunter hunter = new SDRC_Hunter();
		hunter.general.Set(
			1, "index 1: general mission",
			{"0 0 0"}, 2,
			{},
			"any",
			"Hunters",
			"Sharpshooters are hunting you. Last known location was near %l.",
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			"Sharpshooters were not that sharp.",
			"Sharpshooters lost track of you.", 
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HUNTER_MAP,
			SDRC_EMissionDifficulty.NORMAL,
			0
		);
		hunter.ai.Set(
			{1, 1},
			{"G_SNIPER"},
			40, 0.8,
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE
		);
		return hunter;
	}		
	//----------------------------------------------------
	SDRC_Hunter Hunter2()
	{		
		ref SDRC_Hunter hunter = new SDRC_Hunter();
		hunter.general.Set(
			2, "index 2: Hunters from military",
			{"0 0 0"}, 2,
			{
				EMapDescriptorType.MDT_BASE,
/*				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_BUNKER,
				EMapDescriptorType.MDT_FORTRESS,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,*/
			},
			"any",
			"Recon",
			"Military base near %l has sent a recon team to hunt you.",
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			"Recon team cleared.",
			"Recon team lost track of you.", 
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HUNTER_MAP,
			SDRC_EMissionDifficulty.NORMAL,
			0
		);
		hunter.ai.Set(
			{1, 2},
			{"G_RECON"},
			40, 0.5,
			{0, 0},
			SDRC_EWaypointGenerationType.NONE, 
			SDRC_EWaypointMoveType.NONE
		);
		return hunter;
	}			
}