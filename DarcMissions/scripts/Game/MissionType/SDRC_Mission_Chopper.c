//Mission SDRC_Mission_Chopper.c

const string DC_MISSIONCONFIG_FILE_CHOPPER = "dc_missionConfig_Chopper.json";
const int DC_MISSIONCONFIG_FILE_CHOPPER_VER = 1;

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Chopper : SDRC_Mission
{
	private ref SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_CHOPPER);	
	private ref SDRC_ChopperConfig m_Config = new SDRC_ChopperConfig();
	private ref SDRC_Chopper m_DC_Chopper = new SDRC_Chopper();
	
	private vector m_vPosOrigin = "0 0 0";
	private IEntity m_Vehicle = null;
	private SDRC_ChopperComp m_Vehicle_c;
	private VehicleHelicopterSimulation m_Vehicle_s;
	
	private int m_iFlyEndTime;					//How long to fly away
	private const int MIN_FLY_END_TIME = 120;
	private const int MAX_FLY_END_TIME = 300;
	private bool m_bKeepOnFlying = true;

	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Chopper(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Load config
		if (!m_JsonApi.Load(m_Config, SDRC_MissionConfig.Cast(m_Config), DC_MISSIONCONFIG_FILE_CHOPPER_VER))
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.ERROR_LOADING_JSON);
			return;
		}
		m_Config.LoadMissionFiles(DC_MISSIONCONFIG_FILE_CHOPPER_VER);
		
		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		int idx = m_Config.GetSubMissionIdx(GetSubIdx());
		if (idx == -1)
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.WRONG_SUBIDX);
			return;
		}
		m_DC_Chopper = m_Config.subMissions[idx];	
		HandleRequestGeneralVariables(m_DC_Chopper.general, request);

		//Find position
		vector pos = "0 0 0";

		//For requested missions we want have it as close as possible in the requested place.
		if (IsRequested())
		{
			pos = request.general.pos[0];
		}
		else
		{				
			pos = SDRC_MissionHelper.SelectMissionPos(m_DC_Chopper.general.pos, m_DC_Chopper.general.size, m_DC_Chopper.general.locationTypes, SDRC_Conf.POSITION_RANDOMIZATION);	//Randomization added to avoid the same position always
		}
		
		//No suitable location found.
		if (pos == "0 0 0")
		{				
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.LOCATION_NOT_FOUND);
			return;
		}	

		//Where to start the flight		
		m_vPosOrigin = SDRC_Misc.GetRandomWorldPosPercentage(m_Config.distanceToStart);
	#ifndef SDRC_RELEASE
		m_vPosOrigin = SDRC_Misc.GetRandomWorldPosPercentage(0.00001);
	#endif
		
		//Set end time for mission.
		m_iFlyEndTime = m_Config.activeTime * 0.2;
		m_iFlyEndTime = Math.ClampInt(m_iFlyEndTime, MIN_FLY_END_TIME, MAX_FLY_END_TIME);
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Chopper.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_DC_Chopper.general);		
		if (!IsStatic())
		{
			InitActiveTime(m_Config.activeTime);
		}
		SetActiveDistance(-1);					//Disable active distance checking. 
		SetActiveTimeToEnd(m_iFlyEndTime);		//Change the m_iActiveTimeToEnd to short one as there is no loot to gain.		
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == SDRC_EMissionState.SPAWN)
		{
			MissionSpawn();
			if (GetState() == SDRC_EMissionState.FAILED)
			{
				SetState(SDRC_EMissionState.END);
			}
			else
			{
				SetState(SDRC_EMissionState.ACTIVE);				
			}
		}

		if (GetState() == SDRC_EMissionState.END)
		{
			MissionEnd();
			SetState(SDRC_EMissionState.EXIT);
		}	
				
		if (GetState() == SDRC_EMissionState.ACTIVE)
		{
			bool flyAway = false;
			
			//Check if we should leave
			if (m_bKeepOnFlying)
			{
				//Have we flown long enough
				if (GetActiveTime() < m_iFlyEndTime)
				{
					flyAway = true;
				}
				
				//Search waypoint: Check if enemy players has been found
				if (m_DC_Chopper.wpType == SDRC_EHeliWaypointGenerationType.SEARCH)
				{
					if (m_Vehicle_c)
					{
						vector enemyPos = m_Vehicle_c.GetEnemyPosition();
						if (enemyPos != "0 0 0")
						{
							m_Vehicle_c.EnemyHandled();
							flyAway = true;
							SpawnHunterMission(enemyPos);
							SDRC_Log.Add("[SDRC_Mission_Chopper:MissionRun] Enemy found at " + enemyPos + ". Sending Hunters.", LogLevel.NORMAL);
						}
					}
				}				
			}

			//Is there a request to fly away and end the mission
			if ( (m_bKeepOnFlying) && (flyAway) )
			{
				if (m_Vehicle_c)
				{
					float size = SDRC_Misc.GetWorldSize() * 0.7;
					vector pos = m_Vehicle.GetTransformAxis(2);
					pos.Normalize();
					pos = pos * size;
					m_Vehicle_c.AddDestination(pos, true);
					m_Vehicle_c.CreateFlyPath(m_Vehicle.GetOrigin(), true);
					m_Vehicle_c.SetSpeed(max : m_DC_Chopper.speed[1] * 1.5);
				}
				SetActiveTime(m_iFlyEndTime);
				ResetActiveTime();
				m_bKeepOnFlying = false;
				DoLose();
				SDRC_Log.Add("[SDRC_Mission_Chopper:MissionRun] " +  GetId() + " : Chopper to fly away. Mission ending.", LogLevel.DEBUG);
			}
			
			//Check for other reasons to stop flying
			if (!IsActive())
			{				
				SetState(SDRC_EMissionState.END);
				m_bKeepOnFlying = false;
			}
			
			//Move the marker and mission position to where the helicopter is			
			if (m_DC_Chopper.wpType != SDRC_EHeliWaypointGenerationType.PATROL)
			{
				if (m_Vehicle)
				{
					SetPos(m_Vehicle.GetOrigin());
					SDRC_DebugHelper.MoveDebugPos(GetId(), GetPos());
					MoveMarker();
				}
			}
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionCycleTime*1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		if (m_Vehicle_c)
		{
			m_Vehicle_c.Clear();
		}
				
		super.MissionEnd();
	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		//Spawn vehicle
		string resourceName	= SDRC_SpawnHelper.SelectResourceName(m_DC_Chopper.heliList);		
		m_Vehicle = SDRC_SpawnHelper.SpawnItem(GetPos(), resourceName, m_DC_Chopper.general.size, -1);
		
		if (m_Vehicle)
		{
			m_Vehicle_s = VehicleHelicopterSimulation.Cast(m_Vehicle.FindComponent(VehicleHelicopterSimulation));
			m_Vehicle_c = SDRC_ChopperComp.Cast(m_Vehicle.FindComponent(SDRC_ChopperComp));
		}
		
		if ( (!m_Vehicle) || (!m_Vehicle_s) || (!m_Vehicle_c) )
		{
			//Could not spawn vehicle
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.COULD_NOT_SPAWN_VEHICLE, resourceName);
			return;			
		}
		
		SDRC_Log.Add("[SDRC_Mission_Chopper:MissionSpawn] " +  GetId() + " : Vehicle spawned: " + m_Vehicle, LogLevel.DEBUG);
		
		m_EntityList.Insert(m_Vehicle);
		m_Vehicle_c.SetHeli(m_DC_Chopper.speed[0], m_DC_Chopper.speed[1], m_DC_Chopper.flyHeight[0], m_DC_Chopper.flyHeight[1], m_DC_Chopper.wpType, m_DC_Chopper.flyDistance[0], m_DC_Chopper.flyDistance[1]);
		m_Vehicle_c.SetSearchForEnemy(true);
		m_Vehicle_c.InitFlyPath(m_Vehicle, m_vPosOrigin, GetPos());

		//Spawn pilots if such is available 
		ResourceName pilot = SDRC_EnemyHelper.SelectEnemy("C_CREW", GetFaction());
				
		if (pilot != "")
		{
			SCR_AIGroup group = SDRC_AIHelper.GroupCreate(GetFaction(), GetPos());
			
			for (int i = 0; i < 2; i++)
			{		
				SDRC_VehicleHelper.SpawnGroupInVehicle(pilot, m_Vehicle, group);
				
				if (group)
				{			
					SDRC_AIHelper.SetAIGroupSettings(group, m_DC_Chopper.ai.GetSkill(GetDifficulty()), m_DC_Chopper.ai.GetPerception(GetDifficulty()));
					m_Groups.Insert(group);					
				}
			}
		}
		
		//Spawn mission AI
		int aiCount = m_DC_Chopper.ai.GetCount(GetDifficulty());
		
		for (int i = 0; i < aiCount; i++)
		{		
			string groupToSpawn = m_DC_Chopper.ai.types.GetRandomElement();
			ResourceName aiType = SDRC_EnemyHelper.SelectEnemy(groupToSpawn, GetFaction());
			
			SCR_AIGroup group = SDRC_AIHelper.GroupCreate(GetFaction(), GetPos());
			SDRC_VehicleHelper.SpawnGroupInVehicle(aiType, m_Vehicle, group);
			
			if (group)
			{			
				SDRC_AIHelper.SetAIGroupSettings(group, m_DC_Chopper.ai.GetSkill(GetDifficulty()), m_DC_Chopper.ai.GetPerception(GetDifficulty()));
				m_Groups.Insert(group);					
			}
		}		
		
		m_Vehicle_c.Ready(m_Vehicle);
	}
	
	//------------------------------------------------------------------------------------------------
	static void SpawnHunterMission(vector pos)
	{	
		//Find a location that does not have players really close by.
		for (int i = 0; i < 10; i++)
		{
			pos = SDRC_Misc.GetCoordinatesOnCircle(pos, SDRC_Misc.RandomInt(150, 200), SDRC_Misc.RandomInt(0, 360));
			if (SDRC_MissionPosHelper.IsValidMissionPos(pos, 50, 100))
			{
				break;
			}
			SDRC_Log.Add("[SDRC_Mission_Chopper:SpawnHunterMission] Position not optimal.", LogLevel.WARNING);
		}
		
		string resourceName = SDRC_MissionEnumHelper.GetMissionPrefab(SDRC_EMissionType.HUNTER);
		if (resourceName == "")
		{
			SDRC_Log.Add("[SDRC_Mission_Chopper:SpawnHunterMission] Could not spawn mission.", LogLevel.ERROR);
			return;
		}
		
		IEntity missionEntity = SDRC_SpawnHelper.SpawnItem(pos, resourceName, 0, -1);
		if (!missionEntity)
		{		
			SDRC_Log.Add("[SDRC_Mission_Chopper:SpawnHunterMission] Could not spawn mission.", LogLevel.ERROR);
			return;
		}
		GetGame().GetCallqueue().CallLater(SetMissionParameters_Delayed, 2000, false, missionEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	static void SetMissionParameters_Delayed(IEntity missionEntity)
	{
		SDRC_DarcMissionGM ent = SDRC_DarcMissionGM.Cast(missionEntity);
		if (ent)
		{
			SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
			
			SDRC_DarcMissionEditableRequestComp requestComp = SDRC_DarcMissionEditableRequestComp.Cast(ent.FindComponent(SDRC_DarcMissionEditableRequestComp));
			requestComp.general = general;
			requestComp.SetRequestId(1234);
			requestComp.SetMissionType(SDRC_EMissionType.HUNTER);
			requestComp.SetSubIdx(0);
		}	
		else
		{
			SDRC_Log.Add("[SDRC_Mission_Chopper:SetMissionParameters_Delayed] Enable to set. Defaults will be used. ", LogLevel.WARNING);
		}	
	}		
}

//------------------------------------------------------------------------------------------------
class SDRC_Chopper
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();
	//Optional settings
	#ifndef NEW_VERSION_WIP	
		ref SDRC_MissionConfigSecondWave secondWave = new SDRC_MissionConfigSecondWave();	
	#endif
	#ifdef NEW_VERSION_WIP		
		ref SDRC_MissionConfigQrf qrf = null;
	#endif
	
	//Mission specific
	ref array<string> heliList = {};
	ref array<int> flyHeight = {};						//min, max - Spawn helicopter between these values.
	ref array<int> speed = {};							//min, max - 
	ref array<float> flyDistance = {};					//min, max - Distance for finding new positions
	SDRC_EHeliWaypointGenerationType wpType; 
		
	void Set(array<string> heliList_, array<int> flyHeight_, array<int> speed_, array<float> flyDistance_, SDRC_EHeliWaypointGenerationType wpType_)
	{
		heliList = heliList_;
		flyHeight = flyHeight_;
		speed = speed_;
		flyDistance = flyDistance_;
		wpType = wpType_;
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_ChopperConfig : SDRC_MissionConfig
{
	int distanceToMission;									//Distance to mission when searching for a mission pos. Overrides missionFrame settings.
	int distanceToPlayer;									//Distance to player when searching for a mission pos. Overrides missionFrame settings.
	float distanceToStart;									//(percentage) Distance from the center of world for the helicopter to spawn.
	int activeTime;											//The time the mission should be running until the chopper flies away.
	ref array<ref SDRC_Chopper> subMissions = {};			//List of sub missions
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_ChopperConfig data = SDRC_ChopperConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		
	
	//------------------------------------------------------------------------------------------------
	int GetSubMissionIdx(int subIdx)
	{
		int idx = -1;
		foreach (int i, SDRC_Chopper subMission : subMissions)
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
		
		showMarker = false;
		disableArsenal = true;
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		missionList = {0,1,1,2,2,3,4,4};
		//Mission specific
		distanceToMission = 100;
		distanceToPlayer = 500;
		distanceToStart = 0.49;
	#ifdef SDRC_RELEASE
		activeTime = 30*60;
	#endif
	#ifndef SDRC_RELEASE
		activeTime = 25*60;
	#endif
		
		//----------------------------------------------------
		subMissions.Insert(Chopper0());
		subMissions.Insert(Chopper1());
		subMissions.Insert(Chopper2());
		subMissions.Insert(Chopper3());
		subMissions.Insert(Chopper4());
	};
	
	//----------------------------------------------------
	SDRC_Chopper Chopper0()
	{
		ref SDRC_Chopper chopper = new SDRC_Chopper();
		chopper.general.Set(
			0, "index 0: Randomly flying chopper",
			{"0 0 0", "0 0 0"}, 0,
			{},
			"any",
			"Helicopter patroling",
			"Avoid being seen.",
			SDRC_EMissionWinCondition.AI_KILL_75,
			"Helicopter is not your problem anymore.", 
			"Helicopter lost track of you.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_CHOPPER_MAP, 
			SDRC_EDifficulty.RANDOM,
			0
		);
		chopper.ai.Set
		(
			{1, 2},
			{"G_SMALL", "G_ADMIN", "G_RECON"},
			30, 0.6,
			{0, 0},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.LOITER,
		);
		chopper.Set
		(
			{
			 "{5BBDA2DACF9CDCA4}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_Patrol.et",
			},
			{35, 70},
			{7, 25},
			{0.2, 0.5},
			SDRC_EHeliWaypointGenerationType.RANDOM,	
		);
		
		return chopper;
	}
	
	//----------------------------------------------------
	SDRC_Chopper Chopper1()
	{
		ref SDRC_Chopper chopper = new SDRC_Chopper();
		chopper.general.Set(
			1, "index 1: Gunships",
			{"0 0 0", "0 0 0"}, 0,
			{},
			"any",
			"Gunship hunter",
			"You will be shot if you're seen.",
			SDRC_EMissionWinCondition.AI_KILL_75,
			"Gunship funship .. it's gone.", 
			"Gunship lost track of you.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_CHOPPER_MAP, 
			SDRC_EDifficulty.RANDOM,
			0
		);
		chopper.ai.Set
		(
			{1, 2},
			{"G_SMALL", "G_LIGHT"},
			60, 1.0,
			{0, 0},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.LOITER,
		);
		chopper.Set
		(
			{
/*			 "{446634BB04ED3705}Prefabs/Vehicles/Helicopters/UH1H/SP02_GUNSHIP_Patrol.et",
			 "{96D1D7E22C123DEE}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_Patrol.et",
			 "{4CFDE3580182C452}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_gunship_HEDP_sharkNose_Patrol.et",
			 "{5678893357C6FC10}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HE_Patrol.et",
			 "{3815F0A6CA3FF790}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HEDP_Patrol.et",
			 //From: https://reforger.armaplatform.com/workshop/6720D3B2BEBC691E-Mi24and28forDarcMissions
			 "{EDF7AA9A54BFD6F8}Prefabs/Vehicles/Helicopters/Mi24/Mi24V_armed_UPK23_Patrol.et",
			 "{842F4FB8DACE28D4}Prefabs/Vehicles/Helicopters/MI28/MI28N_Grey_Patrol.et",*/
			 //From: https://reforger.armaplatform.com/workshop/684F3C94BD457F85-KA-52forDarcMissions
			 "{490B9AAF7C1DDF1B}Prefabs/Vehicles/Helicopters/KA52/KA52_UPK_X2_Patrol.et",			
			},
			{40, 80},
			{10, 30},
			{0.2, 0.4},
			SDRC_EHeliWaypointGenerationType.RANDOM,	
		);
		
		return chopper;
	}
	
	//----------------------------------------------------
	SDRC_Chopper Chopper2()
	{
		ref SDRC_Chopper chopper = new SDRC_Chopper();
		chopper.general.Set(
			2, "index 2: Patrol around cities and towns",
			{"0 0 0", "0 0 0"}, 0,
			{	
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_TOWN, 
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
			},
			"any",
			"Guardian from the heaven",
			"Area near %l is being guarded from the air.",
			SDRC_EMissionWinCondition.AI_KILL_75,
			"Guardians are now angels.", 
			"Guards have left.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_CHOPPER_MAP, 
			SDRC_EDifficulty.RANDOM,
			0
		);
		chopper.ai.Set
		(
			{1, 2},
			{"G_HEAVY", "G_SMALL"},
			60, 1.0,
			{0, 0},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.LOITER,
		);
		chopper.Set
		(
			{
			 "{446634BB04ED3705}Prefabs/Vehicles/Helicopters/UH1H/SP02_GUNSHIP_Patrol.et",
			 "{96D1D7E22C123DEE}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_Patrol.et",
			 "{4CFDE3580182C452}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_gunship_HEDP_sharkNose_Patrol.et",
			 "{5678893357C6FC10}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HE_Patrol.et",
			 "{3815F0A6CA3FF790}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HEDP_Patrol.et",
			 //From: https://reforger.armaplatform.com/workshop/6720D3B2BEBC691E-Mi24and28forDarcMissions
			 "{EDF7AA9A54BFD6F8}Prefabs/Vehicles/Helicopters/Mi24/Mi24V_armed_UPK23_Patrol.et",
			
			},
			{30, 60},
			{7, 25},
			{50, 200},
			SDRC_EHeliWaypointGenerationType.PATROL,	
		);
		
		return chopper;
	}

	//----------------------------------------------------
	SDRC_Chopper Chopper3()
	{
		ref SDRC_Chopper chopper = new SDRC_Chopper();
		chopper.general.Set(
			3, "index 3: Third party choppers",
			{"0 0 0", "0 0 0"}, 0,
			{	
				EMapDescriptorType.MDT_NAME_LOCAL,
			},
			"any",
			"Heli patrol sighted",
			"Initial destination is near %l.",
			SDRC_EMissionWinCondition.AI_KILL_75,
			"Patrol does not bother you anymore.",
			"Patrol has left.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_CHOPPER_MAP, 
			SDRC_EDifficulty.RANDOM,
			0
		);
		chopper.ai.Set
		(
			{1, 2},
			{"G_HEAVY", "G_SMALL"},
			60, 1.0,
			{0, 0},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.LOITER,
		);
		chopper.Set
		(
			{
			 //From: https://reforger.armaplatform.com/workshop/672F2BB6523FBA29-WZHelisforDarcMissions
			 "{FED75A40AD78101F}Prefabs/Vehicles/Helicopters/Mi-1/Mi1PK_scout_Patrol.et",
             "{9515A6A2F7309954}Prefabs/Vehicles/Helicopters/Mi-1/Mi1PKRocketpod_Patrol.et",
			 "{26436A51FE36D07C}Prefabs/Vehicles/Helicopters/Ka-137/Ka137_Patrol.et",
			 //From: https://reforger.armaplatform.com/workshop/6720D3B2BEBC691E-Mi24and28forDarcMissions
			 "{EDF7AA9A54BFD6F8}Prefabs/Vehicles/Helicopters/Mi24/Mi24V_armed_UPK23_Patrol.et",
			 "{842F4FB8DACE28D4}Prefabs/Vehicles/Helicopters/MI28/MI28N_Grey_Patrol.et",
			 //From: https://reforger.armaplatform.com/workshop/684F3C94BD457F85-KA-52forDarcMissions
			 "{490B9AAF7C1DDF1B}Prefabs/Vehicles/Helicopters/KA52/KA52_UPK_X2_Patrol.et",
			 //A few default ones to spawn
			 "{96D1D7E22C123DEE}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_Patrol.et",
			 "{5678893357C6FC10}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HE_Patrol.et",
			},
			{40, 80},
			{10, 30},
			{0.2, 0.4},
			SDRC_EHeliWaypointGenerationType.RANDOM,	
		);
		
		return chopper;
	}

	//----------------------------------------------------
	SDRC_Chopper Chopper4()
	{
		ref SDRC_Chopper chopper = new SDRC_Chopper();
		chopper.general.Set(
			4, "index 4: Search patrol sending hunters",
			{"0 0 0", "0 0 0"}, 0,
			{},
			"any",
			"Helicopter recon",
			"Hide!",
			SDRC_EMissionWinCondition.AI_KILL_75,
			"Search patrol avoided.", 
			"Hunters are sent to your location.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_CHOPPER_MAP, 
			SDRC_EDifficulty.RANDOM,
			0
		);
		chopper.ai.Set
		(
			{1, 2},
			{"G_LIGHT"},
			30, 0.6,
			{0, 0},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.LOITER,
		);
		chopper.Set
		(
			{
			 "{435F663B9456C29E}Prefabs/Vehicles/Helicopters/UH1H/UH1H_civ_livery_v1_Patrol.et",
			//From: https://reforger.armaplatform.com/workshop/6850D5F667CEFF94-AH-6MforDarcMissions
			 "{2B0F7648840C4F6E}Prefabs/Vehicles/Helicopters/AH6M/OPFOR/MH6M_OPFOR_Patrol.et",
			 "{19022AB51719F2AD}Prefabs/Vehicles/Helicopters/AH6M/OPFOR/AH6M_OPFOR_M134_Patrol.et",
			 "{C4590C7F97F99DB2}Prefabs/Vehicles/Helicopters/AH6M/MH6M_Patrol.et",
			 "{87314096BD3C9D1A}Prefabs/Vehicles/Helicopters/AH6M/AH6M_M134_Patrol.et"
			},
			{35, 70},
			{7, 25},
			{0.2, 0.5},
			SDRC_EHeliWaypointGenerationType.SEARCH,	
		);
		
		return chopper;
	}	
			
}