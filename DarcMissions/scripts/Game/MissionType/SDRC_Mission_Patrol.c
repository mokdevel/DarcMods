//Mission SDRC_Mission_Patrol.c

//------------------------------------------------------------------------------------------------
/*!

*/

const string DC_MISSIONCONFIG_FILE_PATROL = "dc_missionConfig_Patrol.json";
const int DC_MISSIONCONFIG_FILE_PATROL_JSONVER = 2;

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Patrol : SDRC_Mission
{
	private ref SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_PATROL);	
	private ref SDRC_PatrolConfig m_Config = new SDRC_PatrolConfig();	
	private ref SDRC_Patrol m_DC_Patrol = new SDRC_Patrol();
	
	private vector m_vPosDestination = "0 0 0";

	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Patrol(SDRC_EMissionType missionType, SDRC_MissionRequested request, bool staticMission = false)
	{
		//Load config
		if (!m_JsonApi.Load(m_Config, SDRC_MissionConfig.Cast(m_Config), DC_MISSIONCONFIG_FILE_PATROL_JSONVER))
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.ERROR_LOADING_JSON);
			return;
		}
		m_Config.LoadMissionFiles(DC_MISSIONCONFIG_FILE_PATROL_JSONVER);

		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		int idx = m_Config.GetSubMissionIdx(GetSubIdx());
		if (idx == -1)
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.WRONG_SUBIDX);
			return;
		}
		m_DC_Patrol = m_Config.subMissions[idx];		
		HandleRequestGeneralVariables(m_DC_Patrol.general, request);
		SetQrfConf(m_DC_Patrol.qrf);
		
		//Check that ranges are not too big
		int worldSize = SDRC_Misc.GetWorldSize();
		SDRC_Log.Add("[SDRC_Mission_Patrol] " +  GetId() + " : Worldsize vs maxRange : " + worldSize + " vs " + m_DC_Patrol.ai.waypointRange[1], LogLevel.SPAM);
		
		//Set defaults
		vector pos = "0 0 0";
		//If pos has been set, we blindly accept it. Do basic checking for pos.
		bool obc = (IsRequested() || IsStatic());
		m_vPosDestination = m_DC_Patrol.general.pos[1];
		
		//For requested missions we want have it as close as possible in the requested place.
		if (IsRequested())
		{
			pos = request.general.pos[0];
			m_vPosDestination = request.general.pos[1];
		}
		else
		{			
			pos = SDRC_MissionPosHelper.SelectMissionPos(m_DC_Patrol.general.pos, m_DC_Patrol.general.size, obc, m_DC_Patrol.general.locationTypes);
		}
		
		SDRC_EMissionError missionError = SDRC_MissionPosHelper.IsValidMissionPos(pos, obc, IsRequested());
		if (missionError != SDRC_EMissionError.NONE)
		{
			pos = "0 0 0";
			SetState(SDRC_EMissionState.FAILED, missionError);
			return;
		}			
		
		//Find a location for the destination. Only used for route
		if (m_vPosDestination == "0 0 0")
		{
			m_vPosDestination = SDRC_MissionPosHelper.FindMissionPosWithLocationTypes(m_DC_Patrol.general.locationTypes, m_DC_Patrol.general.size);
			SDRC_Log.Add("[SDRC_Mission_Patrol] " +  GetId() + " : Patrol destination: " + m_vPosDestination, LogLevel.SPAM);
		}

		if (pos == "0 0 0" || m_vPosDestination == "0 0 0")	//No suitable location found.
		{				
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.ROUTE_NOT_FOUND , "From " + pos + " to " + m_vPosDestination);
			return;
		}	
		
		SetPos(pos, m_vPosDestination);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Patrol.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_DC_Patrol.general);		
		SetActiveDistance(m_Config.distanceToPlayer);		//Change the m_iActiveDistance to a mission specific one.
		SetActiveTimeToEnd(20);								//Change the m_iActiveTimeToEnd to short one as there is no loot to gain.
	}

	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == SDRC_EMissionState.SPAWN)
		{
			MissionSpawn();
		}

		if (GetState() == SDRC_EMissionState.END)
		{
			MissionEnd();
			SetState(SDRC_EMissionState.EXIT);
		}	
				
		if (GetState() == SDRC_EMissionState.ACTIVE)
		{	
			//Move the position as the first patrol is moving. This way check for player distance works properly.
			if (!m_Groups.IsEmpty())
			{
				if (m_Groups[0])
				{
					SetPos(m_Groups[0].GetOrigin());
					SDRC_DebugHelper.MoveDebugPos(GetId(), GetPos());
					MoveMarker();
				}
			}
			
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
		IEntity entity;

		//Spawn mission AI 
		int aiCount = m_DC_Patrol.ai.GetCount(GetDifficulty());
		
		for (int i = 0; i < aiCount; i++)
		{
			//SCR_AIGroup group = SDRC_MissionHelper.SpawnMissionAIGroup(m_DC_Patrol.ai.types.GetRandomElement(), GetPos(), GetFaction());
			SCR_AIGroup group = SDRC_MissionHelper.SpawnMissionAIGroupRandom(m_DC_Patrol.ai.types, GetPos(), GetFaction());
			if (group)
			{
				SDRC_AIHelper.SetAIGroupSettings(group, m_DC_Patrol.ai.GetSkill(GetDifficulty()), m_DC_Patrol.ai.GetPerception(GetDifficulty()));					
				m_Groups.Insert(group);
				if (m_DC_Patrol.ai.waypointGenType == SDRC_EWaypointGenerationType.ROUTE)
				{
					SDRC_WPHelper.CreateMissionAIWaypoints(group, m_DC_Patrol.ai.waypointGenType, GetPos(), m_vPosDestination, m_DC_Patrol.ai.waypointMoveType);
				}
				else
				{
					SDRC_WPHelper.CreateMissionAIWaypoints(group, m_DC_Patrol.ai.waypointGenType, GetPos(), "0 0 0", m_DC_Patrol.ai.waypointMoveType, m_DC_Patrol.ai.waypointRange[0], m_DC_Patrol.ai.waypointRange[1]);
				}
			}			
			SDRC_Log.Add("[SDRC_Mission_Patrol:MissionSpawn] AI groups spawned: " + m_Groups.Count() + " (tried: " + aiCount + ")", LogLevel.DEBUG);
		}
		
		SetState(SDRC_EMissionState.ACTIVE);
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_Patrol : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();
	ref SDRC_MissionConfigQrf qrf = null;
}	

//------------------------------------------------------------------------------------------------
class SDRC_PatrolConfig : SDRC_MissionConfig
{
	//Mission specific	
	int patrolingTime;								//(seconds) Time to patrol. Once this time has passed and no players nearby, despawn mission.
	int distanceToPlayer;							//If no players this close to any players and patrolingTime has passed, despawn mission.
	ref array<ref SDRC_Patrol> subMissions = {};	//List of patrols
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_PatrolConfig data = SDRC_PatrolConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------	
	override void LoadMissionFiles(int ver, bool silent = false)
	{
		//Load mission files
		foreach (string missionFile : missionFiles)
		{
			SDRC_JsonApi2 jsonApi = new SDRC_JsonApi2(missionFile);
			SDRC_PatrolConfig conf = new SDRC_PatrolConfig();
			
			if (jsonApi.Load(conf, SDRC_MissionConfig.Cast(conf), ver, false))
			{
				foreach (SDRC_Patrol subMission : conf.subMissions)
				{
					//We need to fix the subIdx so that there are no duplicates				
					//Find the usable subIdx by searching the current biggest one + 1.
					int freeIndex = subMissions.Count();
					
					foreach (string mod : subMission.general.modList)
					{
						if (!SDRC_Misc.IsAddonLoaded(mod))
						{
							if (!silent)
							{
								SDRC_Log.Add("[SDRC_MissionConfig:LoadMissionFiles] For " + subMission.general.comment + " (" + missionFile + ") to work, a mod is needed: " + mod, LogLevel.WARNING);
							}
						}
						else
						{						
							//Fix indexes
							int subIdx = subMission.general.subIdx;
							subMission.general.subIdx = freeIndex;
							
							//Add the subMissions to the main list. 
							subMissions.Insert(subMission);
							
							foreach (int idx : conf.missionList)
							{
								if (idx == subIdx)
								{
									missionList.Insert(freeIndex);
								}
							}
						}
					}
				}
			}
		}		
		super.LoadMissionFiles(ver);			
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
		foreach (int i, SDRC_Patrol subMission : subMissions)
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
		missionType = SDRC_EMissionType.PATROL;
		
		//Default		
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT * 3;
		showMarker = false;
		missionList = {0,0,0,1,2,3};
		//Mission specific
		distanceToPlayer = 500;
		//----------------------------------------------------
		subMissions.Insert(Patrol0());
		subMissions.Insert(Patrol1());
		subMissions.Insert(Patrol2());
		subMissions.Insert(Patrol3());
	};
	
	//----------------------------------------------------
	SDRC_Patrol Patrol0()
	{
		ref SDRC_Patrol patrol = new SDRC_Patrol();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Patrol spotted near %l",
			"Intel tells them to travel to %d. Be careful while traveling on roads.",
			"Patrol near %l is no more.",
			"Patroling completed, the world is saved.",);
		patrol.general.Set(
			0, "index 0: Enemy patrols going between two points hopefully following roads",
			{"0 0 0", "0 0 0"}, 2,
			{
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_VALLEY,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_RIDGE
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			{},
//			{"", "FIA"},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_PATROL_MAP,
			{SDRC_EDifficulty.RANDOM},
//			{SDRC_EDifficulty.HARD, SDRC_EDifficulty.TOUGH},
			0
		);		
		patrol.ai.Set(
			{1, 1},
			{"G_SPECIAL"},
			50, 1.0,
			{0, 0},	//Not used with ROUTE
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
			
		return patrol;
	};
		
	//----------------------------------------------------
	SDRC_Patrol Patrol1()
	{
		ref SDRC_Patrol patrol = new SDRC_Patrol();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Patrol in %l",
			"Beware!",
			"Well done!",
			"You're not a worthy enemy for this patrol.",);
		patrol.general.Set(
			1, "index 1: Heavy patrol",
			{"0 0 0", "0 0 0"}, 2,
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
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_PATROL_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);			
		patrol.ai.Set(
			{1, 1},
			{"G_HEAVY", "G_LAUNCHER"},
			50, 1.0,
			{200, 800},
			SDRC_EWaypointGenerationType.RADIUS,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
			
		return patrol;
	};

	//----------------------------------------------------
	SDRC_Patrol Patrol2()
	{
		ref SDRC_Patrol patrol = new SDRC_Patrol();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Patrol seen in %l",
			"Be alert!",
			"The road from %l to %d is safe again.",
			"Patrol left the area.",);
		patrol.general.Set(
			2, "index 2: Enemy patrols between villages",
			{"0 0 0", "0 0 0"}, 2,
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_PATROL_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);		
		patrol.ai.Set(
			{1, 1},
			{
				"G_RECON", "G_LIGHT", "G_SMALL"
			},
			50, 1.0,
			{300, 700},
			SDRC_EWaypointGenerationType.SCATTERED,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		return patrol;
	};
		
	//----------------------------------------------------
	SDRC_Patrol Patrol3()
	{
		ref SDRC_Patrol patrol = new SDRC_Patrol();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Enemy has been seen near %l",
			"Caution is advised.",
			"Patrol cleared!",
			"Such a small force was able to beat you.",);
		patrol.general.Set(			
			3, "index 3: Small patrols with a few AIs",
			{"0 0 0", "0 0 0"}, 2,
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_50,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_PATROL_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);				
		patrol.ai.Set(
			{1, 1},
			{
				"G_SMALL", "G_MEDICAL", "G_RECON"
			},
			50, 1.0,
			{300, 700},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		return patrol;
	}
}