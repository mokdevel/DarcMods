//Mission SDRC_Mission_Chopper.c
//
//This is a concept of a mission. Some chopper to fly from A to B and do something. 
//This is completely unfinished. More like a PoC to show that a chopper can fly.

class SDRC_Mission_Chopper : SDRC_Mission
{
	private const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Chopper.json";
	
	private ref SDRC_ChopperJsonApi m_ChopperJsonApi = new SDRC_ChopperJsonApi(DC_MISSIONCONFIG_FILE);	
	private ref SDRC_ChopperConfig m_Config;
	private ref SDRC_Chopper m_DC_Chopper = new SDRC_Chopper();
	private VehicleHelicopterSimulation m_Vehicle_s;
	private IEntity m_Vehicle = null;;
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
		vector pos = "1300 10 2200";//pathPoints[0];//SDRC_MissionHelper.SelectMissionPos(m_DC_Chopper.general.pos);
		SetPos(pos /*, destination */);
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
		
//		string groupToSpawn = "{30ED11AA4F0D41E5}Prefabs/Groups/OPFOR/Group_USSR_FireGroup.et";
		string groupToSpawn = "{0D10CCEEC7B3EC34}Prefabs/Groups/OPFOR/Group_USSR_PlatoonHQ.et";
		m_Crew = SDRC_AIHelper.SpawnGroup(groupToSpawn, GetPos(), GetFaction());
		m_Crew2 = SDRC_AIHelper.SpawnGroup(groupToSpawn, GetPos(), GetFaction());
		//TBD: Set the skill and perception
		
//		vector aiPos = "0 1000 0";
//		m_Crew = SDRC_AIHelper.SpawnGroup(groupToSpawn, aiPos, GetFaction());
//		m_Crew2 = SDRC_AIHelper.SpawnGroup(groupToSpawn, aiPos, GetFaction());
		
		//Code for whatever you need for spawning things.
		EntitySpawnParams params = EntitySpawnParams();
//		string resourceName	= "{82704CE53C89C888}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying_Patrol.et";
//		string resourceName	= "{96D1D7E22C123DEE}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_Patrol.et";
		string resourceName	= "{3815F0A6CA3FF790}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HEDP_Flying_Patrol.et";
//		string resourceName	= "{5678893357C6FC10}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HE_Flying_Patrol.et";
//		string resourceName	= "{31203FC84104022C}Prefabs/Vehicles/Helicopters/UH1H/UH1H_armed_gunship_M261_Flying_Patrol.et";	//M261 MOD!
//		string resourceName	= "{7CB21626B43A2E48}Prefabs/Vehicles/Flying/Jetpack_Flying.et";
		vector pos = GetPos();
	
		//Spawn the resource exactly to pos
		Resource resource = Resource.Load(resourceName);
		vector transform[4];
				
		vector rotVector = "0 0 0";
		Math3D.MatrixIdentity3(transform);
		Math3D.AnglesToMatrix(rotVector, transform);
		transform[3] = pos;//m_vSplinePoints[0];
		
		//SDRC_SpawnHelper.GetTransformFromPosAndRot(transform, pos, 0, false);
        params.TransformMode = ETransformMode.WORLD;			
        params.Transform = transform;
		
		m_Vehicle = SDRC_SpawnHelper.SpawnEntityPrefabPersistence(resource, GetGame().GetWorld(), params);		
		
		m_Vehicle_s = VehicleHelicopterSimulation.Cast(m_Vehicle.FindComponent(VehicleHelicopterSimulation));
        m_Vehicle_s.EngineStart();
        m_Vehicle_s.SetThrottle(1.0);
        m_Vehicle_s.RotorSetForceScaleState(0, 1.3);	//Hovering 1.2
        m_Vehicle_s.RotorSetForceScaleState(1, 2);
		
		GetGame().GetCallqueue().CallLater(AddCrew, 6000, false, m_Crew);
		GetGame().GetCallqueue().CallLater(AddCrew, 7000, false, m_Crew2);
	}	
	
	void AddCrew(SCR_AIGroup crew)
	{
		if (crew)
		{
			SDRC_VehicleHelper.MoveGroupInVehicle(crew, m_Vehicle, true);
			SDRC_AIHelper.SetAIGroupSettings(crew, EAISkill.CYLON, 10);
		}
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_ChopperConfig : SDRC_MissionConfig
{
	int distanceToMission;								//Distance to mission when searching for a mission pos. Overrides missionFrame settings.
	int distanceToPlayer;								//Distance to player when searching for a mission pos. Overrides missionFrame settings.
	ref array<int> flyHeight = {};						//min, max - Spawn helicopter between these values.
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
	ref array<ref SDRC_HelicopterInfo> helicopterInfo = {};
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
		conf.flyHeight = {80, 120};
		//----------------------------------------------------
		conf.subMissions.Insert(Chopper0());
	};
			
	//----------------------------------------------------
	SDRC_Chopper Chopper0()
	{
		ref SDRC_Chopper chopper = new SDRC_Chopper();
		chopper.general.Set(
			0, "index 0: JUST FOR TESTING. MISSION DOES NOT WORK!",
			{"0 0 0"}, 0,
			{},
			"any",
			"Helicopter in distress",
			"A valuable cargo has crashed.",
			SDRC_EMissionWinCondition.AI_KILL_75,
			"The loot was salvaged. Crash, burn, loot.",
			"No loot for you today.", 
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HELICOPTER_MAP, 
			SDRC_EMissionDifficulty.NORMAL,
			0
		);
		chopper.ai.Set
		(
			{1, 2},
			{"G_LIGHT", "G_ADMIN"},
			20, 0.8,
			{0, 0},
			SDRC_EWaypointGenerationType.LOITER,
			SDRC_EWaypointMoveType.LOITER,
		);
		
		//----------------------------------------------------
		ref SDRC_HelicopterInfo heli00 = new SDRC_HelicopterInfo();
			heli00.Set("{40A3EEECFF765793}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_flying.et", 0.8, 0.8, 1.0);
			chopper.helicopterInfo.Insert(heli00);
		ref SDRC_HelicopterInfo heli01 = new SDRC_HelicopterInfo();
			heli01.Set("{6D71309125B8AEA2}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying.et",	0.7, 0.9, 1.0);
			chopper.helicopterInfo.Insert(heli01);
		ref SDRC_HelicopterInfo heli02 = new SDRC_HelicopterInfo();
			heli02.Set("{40A3EEECFF765793}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_flying.et",	0.8, 0.8, -1.0);
			chopper.helicopterInfo.Insert(heli02);		
		
		return chopper;
	}
}