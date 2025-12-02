//Mission SDRC_Mission_Chopper.c

class SDRC_Mission_Chopper : SDRC_Mission
{
	private const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Chopper.json";
	
	private ref SDRC_ChopperJsonApi m_ChopperJsonApi = new SDRC_ChopperJsonApi(DC_MISSIONCONFIG_FILE);	
	private ref SDRC_ChopperConfig m_Config;
	private ref SDRC_Chopper m_DC_Chopper = new SDRC_Chopper();
	
	private vector m_vPosOrigin = "0 0 0";
	private IEntity m_Vehicle = null;
	private SDRC_ChopperComp m_Vehicle_c;
	private VehicleHelicopterSimulation m_Vehicle_s;
	
	private SCR_AIGroup m_Crew = null;
	private SCR_AIGroup m_Crew2 = null;
	private int idx = 0;	

	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Chopper(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Load config
		m_ChopperJsonApi.CreateMissionFiles();
		m_ChopperJsonApi.Load();
		m_ChopperJsonApi.LoadMissionFiles();		
		m_Config = m_ChopperJsonApi.conf;
		
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
		vector pos = SDRC_MissionHelper.SelectMissionPos(m_DC_Chopper.general.pos);
		
		m_vPosOrigin[0] = SDRC_Misc.GetWorldSize()/2;
		m_vPosOrigin[2] = SDRC_Misc.GetWorldSize()/2;
	#ifdef SDRC_RELEASE
		m_vPosOrigin = SDRC_Misc.GetCoordinatesOnCircle(m_vPosOrigin, SDRC_Misc.GetWorldSize() * 0.7, Math.RandomInt(0, 360));
	#endif		
	#ifndef SDRC_RELEASE
		m_vPosOrigin = SDRC_Misc.GetCoordinatesOnCircle(m_vPosOrigin, SDRC_Misc.GetWorldSize() * 0.2, Math.RandomInt(0, 360));
	#endif
		
		//No suitable location found.
		if (pos == "0 0 0")
		{				
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.LOCATION_NOT_FOUND);
			return;
		}	
			
		//For requested missions we want have it as close as possible in the requested place.
		if (IsRequested())
		{
			pos = request.general.pos[0];
		}
		
		//Find position
		#ifdef DEBUG_CHOPPER
		pos = "1300 10 2200";
		#endif
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Chopper.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_DC_Chopper.general);		
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
			//Add code for runtime
			
			//Eventually when mission is to ended do this:
			//SetState(SDRC_EMissionState.END);
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
		//Spawn vehicle					
		SDRC_HelicopterInfo heliInfo = m_Config.helicopterInfo[m_DC_Chopper.heliList.GetRandomElement()];
		
		string resourceName	= heliInfo.resource;
		m_Vehicle = SDRC_SpawnHelper.SpawnItem(GetPos(), resourceName, m_DC_Chopper.general.size, -1);
		m_Vehicle_s = VehicleHelicopterSimulation.Cast(m_Vehicle.FindComponent(VehicleHelicopterSimulation));
		m_Vehicle_c = SDRC_ChopperComp.Cast(m_Vehicle.FindComponent(SDRC_ChopperComp));
		
		if ( (!m_Vehicle) || (!m_Vehicle_s) || (!m_Vehicle_c) )
		{
			//Could not spawn vehicle
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.COULD_NOT_SPAWN_VEHICLE, resourceName);
			return;			
		}
		
		SDRC_Log.Add("[SDRC_Mission_Chopper:MissionSpawn] Vehicle spawned: " + m_Vehicle, LogLevel.DEBUG);										
		
		m_EntityList.Insert(m_Vehicle);
        m_Vehicle_s.EngineStart();
        m_Vehicle_s.SetThrottle(heliInfo.throttle);
        m_Vehicle_s.RotorSetForceScaleState(0, heliInfo.rotorForce);
        m_Vehicle_s.RotorSetForceScaleState(1, heliInfo.rotor2Force);
		m_Vehicle_c.SetHeli(m_DC_Chopper.rotorForceUp, m_DC_Chopper.speed[0], m_DC_Chopper.speed[1], m_DC_Chopper.throttle, m_DC_Chopper.flyHeight[0], m_DC_Chopper.flyHeight[1], m_DC_Chopper.wpType, m_DC_Chopper.flyDistance[0], m_DC_Chopper.flyDistance[1]);
		m_Vehicle_c.InitFlightPath(m_Vehicle, m_vPosOrigin, GetPos());
				
		//Spawn mission AI
		int aiCount = m_DC_Chopper.ai.GetCount(m_DC_Chopper.general.difficulty);
		for (int i = 0; i < aiCount; i++)
		{		
			SCR_AIGroup group = SDRC_AIHelper.SpawnGroup(m_DC_Chopper.ai.types.GetRandomElement(), GetPos(), GetFaction());
			if (group)
			{			
				SDRC_AIHelper.SetAIGroupSettings(group, m_DC_Chopper.ai.GetSkill(m_DC_Chopper.general.difficulty), m_DC_Chopper.ai.GetPerception(m_DC_Chopper.general.difficulty));
				GetGame().GetCallqueue().CallLater(AddCrewDelayed, 6000, false, group);
				m_Groups.Insert(group);					
			}
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	void AddCrewDelayed(SCR_AIGroup group)
	{
		if (group)
		{
			SDRC_VehicleHelper.MoveGroupInVehicle(group, m_Vehicle, true);
			SDRC_AIHelper.SetAIGroupSettings(group, m_DC_Chopper.ai.GetSkill(m_DC_Chopper.general.difficulty), m_DC_Chopper.ai.GetPerception(m_DC_Chopper.general.difficulty));
		}
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_ChopperConfig : SDRC_MissionConfig
{
	int distanceToMission;								//Distance to mission when searching for a mission pos. Overrides missionFrame settings.
	int distanceToPlayer;								//Distance to player when searching for a mission pos. Overrides missionFrame settings.
	ref array<ref SDRC_HelicopterInfo> helicopterInfo = {};
	ref array<ref SDRC_Chopper> subMissions = {};		//List of crashsites
	
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
}

//------------------------------------------------------------------------------------------------
class SDRC_Chopper : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();
	//Optional settings
	#ifndef NEW_VERSION_WIP	
		ref SDRC_MissionConfigSecondWave secondWave = new SDRC_MissionConfigSecondWave();	
	#endif
	#ifdef NEW_VERSION_WIP		
		ref SDRC_MissionConfigSecondWave secondWave = null;
	#endif	
	
	//Mission specific
	ref array<int> heliList = {};
	ref array<int> flyHeight = {};						//min, max - Spawn helicopter between these values.
	ref array<int> speed = {};							//min, max - 
	float throttle;
	float rotorForceUp;
	ref array<int> flyDistance = {};					//min, max - Distance for finding new positions
	SDRC_EHeliWaypointGenerationType wpType; 
		
	void Set(array<int> heliList_, array<int> flyHeight_, array<int> speed_, float throttle_, float rotorForceUp_, array<int> flyDistance_, SDRC_EHeliWaypointGenerationType wpType_)
	{
		heliList = heliList_;
		flyHeight = flyHeight_;
		speed = speed_;
		throttle = throttle_;
		rotorForceUp = rotorForceUp_;
		flyDistance = flyDistance_;
		wpType = wpType_;
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_ChopperJsonApi : SDRC_JsonApi
{
	ref SDRC_ChopperConfig conf = new SDRC_ChopperConfig();
		
	//------------------------------------------------------------------------------------------------
	void SDRC_ChopperJsonApi(string fileName)
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
			SDRC_ChopperJsonApi jsonApi = new SDRC_ChopperJsonApi(missionFile);		
			if (jsonApi.Load(false))
			{
				foreach (SDRC_Chopper subMission : jsonApi.conf.subMissions)
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
		conf.disableArsenal = true;
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		conf.missionList = {0};		
		//Mission specific
		conf.distanceToMission = 100;
		conf.distanceToPlayer = 500;
		
		//----------------------------------------------------
		conf.helicopterInfo.Insert(Heli00());
		conf.helicopterInfo.Insert(Heli01());
		conf.helicopterInfo.Insert(Heli02());
		conf.helicopterInfo.Insert(Heli03());
		conf.helicopterInfo.Insert(Heli04());
		
		conf.subMissions.Insert(Chopper0());
	};
	//----------------------------------------------------
	SDRC_HelicopterInfo Heli00()
	{
		ref SDRC_HelicopterInfo heli = new SDRC_HelicopterInfo();
		heli.Set("{3815F0A6CA3FF790}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HEDP_Flying_Patrol.et", 1.01, 2.20, 1.01);
		return heli;		
	}
	
	SDRC_HelicopterInfo Heli01()
	{
		ref SDRC_HelicopterInfo heli = new SDRC_HelicopterInfo();
		heli.Set("{5678893357C6FC10}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HE_Flying_Patrol.et", 1.01, 1.40, 1.01);
		return heli;		
	}
	
	SDRC_HelicopterInfo Heli02()
	{	
		ref SDRC_HelicopterInfo heli = new SDRC_HelicopterInfo();
		heli.Set("{82704CE53C89C888}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying_Patrol.et",	1.01, 1.40, 1.01);
		return heli;		
	}
	
	SDRC_HelicopterInfo Heli03()
	{	
		ref SDRC_HelicopterInfo heli = new SDRC_HelicopterInfo();
		heli.Set("{96D1D7E22C123DEE}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_Patrol.et",	1.01, 1.40, 1.01);
		return heli;		
	}
	
	SDRC_HelicopterInfo Heli04()
	{	
		ref SDRC_HelicopterInfo heli = new SDRC_HelicopterInfo();
		heli.Set("{31203FC84104022C}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_gunship_M261_Flying_Patrol.et",	1.01, 1.40, 1.01);	//M261 MOD!
		return heli;		
	}
	
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
			"Helicopter destroyed.", 
			"Helicopter lost track of you.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HELICOPTER_MAP, 
			SDRC_EMissionDifficulty.NORMAL,
			0
		);
		chopper.ai.Set
		(
			{1, 2},
			{"G_LIGHT", "G_ADMIN"},
			10, 0.1,
			{0, 0},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.LOITER,
		);
		chopper.Set
		(
			{0},
			{45, 90},
			{10, 30},
			1.2, 100,
			{300, 600},
			SDRC_EHeliWaypointGenerationType.RANDOM,			
		);
		
		return chopper;
	}
}