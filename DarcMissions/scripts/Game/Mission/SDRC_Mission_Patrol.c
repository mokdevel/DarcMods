//Mission SDRC_Mission_Patrol.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns groups to defend a location
*/

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Patrol : SDRC_Mission
{
	private ref SDRC_PatrolJsonApi m_PatrolJsonApi = new SDRC_PatrolJsonApi();	
	private ref SDRC_PatrolConfig m_Config;
	
	protected ref SDRC_Patrol m_DC_Patrol;		//Patrol configuration in use
	
	private vector m_vPosDestination = "0 0 0";

	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Patrol(vector pos = "0 0 0")
	{
		SDRC_Log.Add("[SDRC_Mission_Patrol] Constructor", LogLevel.SPAM);
				
		//Set some defaults
		SetType(DC_EMissionType.PATROL);

		//Load config
		m_PatrolJsonApi.Load();
		m_Config = m_PatrolJsonApi.conf;
		
		//Pick a configuration for mission
		int idx = SDRC_MissionHelper.SelectMissionIndex(m_Config.patrolList);
		if (idx == -1)
		{
			SDRC_Log.Add("[SDRC_Mission_Patrol] No patrols defined.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}
		m_DC_Patrol = m_Config.patrols[idx];

		//Check that ranges are not too big
		int worldSize = SDRC_Misc.GetWorldSize();
		SDRC_Log.Add("[SDRC_Mission_Patrol] Worldsize vs maxRange : " + worldSize + " vs " + m_DC_Patrol.waypointRange[1], LogLevel.DEBUG);
		
		//Set defaults
		m_vPosDestination = m_DC_Patrol.general.pos[1];
		
		if (!IsRequested())
		{
			pos = m_DC_Patrol.general.pos[0];
			
			//Find a location for the mission
			if (pos == "0 0 0")
			{
				pos = SDRC_MissionHelper.FindMissionPos(m_DC_Patrol.locationTypes);
			}
		}
	
		//If failed, stop
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SDRC_Log.Add("[SDRC_Mission_Patrol] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}	
		
		//Find a location for the destination. Only used for route
		if (m_vPosDestination == "0 0 0")
		{
			m_vPosDestination = SDRC_MissionHelper.FindMissionPos(m_DC_Patrol.locationTypes);
			if (m_vPosDestination != "0 0 0")
			{
				SDRC_Log.Add("[SDRC_Mission_Patrol] Patrol destination: " + m_vPosDestination, LogLevel.DEBUG);
			}
			else
			{
				SDRC_Log.Add("[SDRC_Mission_Patrol] Could not find destination location for ROUTE.", LogLevel.WARNING);
			}
		}

		if (pos == "0 0 0" || m_vPosDestination == "0 0 0")	//No suitable location found.
		{				
			SDRC_Log.Add("[SDRC_Mission_Patrol] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}	
		
		SetPos(pos, m_vPosDestination);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Patrol.general.posName));
		SetMarker(m_Config.showMarker, m_Config.markerIdx, m_Config.markerType);
		SetHint(m_Config.showHint, m_DC_Patrol.general.title, m_DC_Patrol.general.info);
		SetMessages(m_Config.showMessage, m_DC_Patrol.general.winMessage, m_DC_Patrol.general.loseMessage);		
		SetWinCondition(m_DC_Patrol.general.winCondition);		
		SetActiveDistance(m_Config.distanceToPlayer);				//Change the m_ActiveDistance to a mission specific one.

		SetState(DC_EMissionState.INIT);
	}

	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == DC_EMissionState.INIT)
		{
			MissionSpawn();
		}

		if (GetState() == DC_EMissionState.END)
		{
			MissionEnd();
			SetState(DC_EMissionState.EXIT);
		}	
				
		if (GetState() == DC_EMissionState.ACTIVE)
		{	
			//Move the position as the first patrol is moving. This way check for player distance works properly.
			if (m_Groups[0])
			{
				SetPos(m_Groups[0].GetOrigin());
				SDRC_DebugHelper.MoveDebugPos(GetId(), GetPos());
				MoveMarker();
/*				if (m_Config.showMarker)
				{
					SDRC_MapMarkerHelper.DeleteMarker(GetId());
					SetMarker(m_Config.showMarker, DC_EMissionIcon.GM_MISSION_PATROL_MAP, m_Config.markerType);
					ShowMarker();
				}*/				
			}
			
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
		IEntity entity;

		//Spawn mission AI 
		int groupCount = Math.RandomInt(m_DC_Patrol.groupCount[0], m_DC_Patrol.groupCount[1]);
		
		for (int i = 0; i < groupCount; i++)
		{
			SCR_AIGroup group = SDRC_MissionHelper.SpawnMissionAIGroup(m_DC_Patrol.groupTypes.GetRandomElement(), GetPos(), GetFaction());
			if (group)
			{
				SDRC_AIHelper.SetAIGroupSkill(group, m_DC_Patrol.aiSkill, m_DC_Patrol.aiPerception);					
				m_Groups.Insert(group);
				if (m_DC_Patrol.waypointGenType == DC_EWaypointGenerationType.ROUTE)
				{
					SDRC_WPHelper.CreateMissionAIWaypoints(group, m_DC_Patrol.waypointGenType, GetPos(), m_vPosDestination, m_DC_Patrol.waypointMoveType);
				}
				else
				{
					SDRC_WPHelper.CreateMissionAIWaypoints(group, m_DC_Patrol.waypointGenType, GetPos(), "0 0 0", m_DC_Patrol.waypointMoveType, m_DC_Patrol.waypointRange[0], m_DC_Patrol.waypointRange[1]);
				}
			}
			SDRC_Log.Add("[SDRC_Mission_Patrol:MissionSpawn] AI groups spawned: " + groupCount, LogLevel.DEBUG);								
		}
			
		SetState(DC_EMissionState.ACTIVE);
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_PatrolConfig : SDRC_MissionConfig
{
	//Mission specific	
	int patrolingTime;								//(seconds) Time to patrol. Once this time has passed and not players nearby, despawn mission.
	int distanceToPlayer;							//If no players this close to any players and patrolingTime has passed, despawn mission.
	ref array<ref int> patrolList = {};				//The indexes of patrols.
	ref array<ref SDRC_Patrol> patrols = {};		//List of patrols
}

//------------------------------------------------------------------------------------------------
class SDRC_Patrol : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref array<EMapDescriptorType> locationTypes = {};	
	ref array<int> groupCount = {};			//min, max	
	ref array<int> waypointRange = {};		//min, max
	DC_EWaypointGenerationType waypointGenType;
	DC_EWaypointMoveType waypointMoveType;
	ref array<string> groupTypes = {};	
	int aiSkill;
	float aiPerception	
	
	void Set(array<EMapDescriptorType> locationTypes_, array<int> groupCount_, array<int> waypointRange_, DC_EWaypointGenerationType waypointGenType_, DC_EWaypointMoveType waypointMoveType_, array<string> groupTypes_, int AISkill_, float aiPerception_)
	{
		locationTypes = locationTypes_;
		groupCount = groupCount_;
		waypointRange = waypointRange_;
		waypointGenType = waypointGenType_;
		waypointMoveType = waypointMoveType_;
		groupTypes = groupTypes_;
		aiSkill = AISkill_;
		aiPerception = aiPerception_;				
	}
}		

//------------------------------------------------------------------------------------------------
class SDRC_PatrolJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Patrol.json";
	ref SDRC_PatrolConfig conf = new SDRC_PatrolConfig();
	
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
		array<string> lootItems = {};
		
		//Default
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT * 3;
		conf.markerIdx = DC_EMissionIcon.GM_MISSION_PATROL_MAP;
		conf.showMarker = false;
		//Mission specific
		conf.patrolList = {0,0,0,1,2,3};
		conf.distanceToPlayer = 500;

		//----------------------------------------------------
		SDRC_Patrol patrol0 = new SDRC_Patrol();
		patrol0.general.Set(
			"index 0: Enemy patrols going between two points hopefully following roads",
			{"0 0 0", "0 0 0"},
			"any",
			"Patrol spotted near %l",
			"Intel tells them to travel to %d. Be careful while traveling on roads.",
			DC_EMissionWinCondition.AI_KILL_ALL,
			"Patrol near %l is no more.",
			"Patroling completed, the world is saved.", 
			0
		);		
		patrol0.Set
		(
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
			{1, 1},
			{0, 0},	//Not used with ROUTE
			DC_EWaypointGenerationType.ROUTE,
			DC_EWaypointMoveType.MOVE,
			{
				"G_SPECIAL"
			},
			50, 1.0
		);
		conf.patrols.Insert(patrol0);
		
		//----------------------------------------------------
		SDRC_Patrol patrol1 = new SDRC_Patrol();
		patrol1.general.Set(
			"index 1: Heavy patrol",
			{"0 0 0", "0 0 0"},
			"any",
			"Patrol in %l",
			"Beware!",
			DC_EMissionWinCondition.AI_KILL_75,
			"Well done!",
			"You're not a worthy enemy for this patrol.", 
			0
		);				
		patrol1.Set
		(
			{
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_FORTRESS
			},
			{1, 1},
			{200, 800},
			DC_EWaypointGenerationType.RADIUS,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_HEAVY", "G_LAUNCHER"
			},
			50, 1.0
		);
		conf.patrols.Insert(patrol1);

		//----------------------------------------------------
		SDRC_Patrol patrol2 = new SDRC_Patrol();
		patrol2.general.Set(
			"index 2: Enemy patrols between villages",
			{"0 0 0", "0 0 0"},
			"any",
			"Patrol seen in %l",
			"Be alert!",
			DC_EMissionWinCondition.AI_KILL_75,
			"The road from %l to %d is safe again.",
			"Patrol left the area.", 
			0
		);		
		patrol2.Set
		(
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL
			},
			{1, 1},
			{300, 700},
			DC_EWaypointGenerationType.SCATTERED,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_RECON", "G_LIGHT", "G_SMALL"
			},
			50, 1.0
		);
		conf.patrols.Insert(patrol2);
		
		//----------------------------------------------------
		SDRC_Patrol patrol3 = new SDRC_Patrol();
		patrol3.general.Set(
			"index 3: Small patrols with a few AIs",
			{"0 0 0", "0 0 0"},
			"any",
			"Enemy has been seen near %l",
			"Caution is advised.",
			DC_EMissionWinCondition.AI_KILL_50,
			"Patrol cleared!",
			"Such a small force was able to beat you.", 
			0
		);				
		patrol3.Set
		(
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL
			},
			{1, 1},
			{300, 700},
			DC_EWaypointGenerationType.RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_SMALL", "G_MEDICAL", "G_RECON"
			},
			50, 1.0
		);
		conf.patrols.Insert(patrol3);			
	}
}