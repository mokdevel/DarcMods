//Mission SDRC_Mission_Chopper.c
//
//This is a concept of a mission. Some chopper to fly from A to B and do something. 
//This is completely unfinished. More like a PoC to show that a chopper can fly.

class SDRC_Mission_Chopper : SDRC_Mission
{
	private ref SDRC_ChopperJsonApi m_ChopperJsonApi = new SDRC_ChopperJsonApi();	
	private ref SDRC_ChopperConfig m_Config;
	private VehicleHelicopterSimulation m_Vehicle_s;
	private IEntity m_Vehicle;
	private int idx = 0;	
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Chopper()
	{
		SDRC_Log.Add("[SDRC_Mission_Chopper] Constructor", LogLevel.DEBUG);
				
		//Set some defaults
		SetType(DC_EMissionType.CHOPPER);

		//Load config
		m_ChopperJsonApi.Load();
		m_Config = m_ChopperJsonApi.conf;
		
		string posName = m_Config.posName;
		vector pos = m_Config.pos;
		
		SetTitle(m_Config.title + "" + posName);
		SetInfo(m_Config.info);
		SetPos(pos);
		SetPosName(posName);
		SetMarker(m_Config.showMarker, DC_EMissionIcon.N_HELI);
		SetShowHint(m_Config.showHint);

		SetState(DC_EMissionState.INIT);			
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == DC_EMissionState.INIT)
		{
			MissionSpawn();
			SetState(DC_EMissionState.ACTIVE);
		}

		if (GetState() == DC_EMissionState.END)
		{
			MissionEnd();
			SetState(DC_EMissionState.EXIT);
		}	
				
		if (GetState() == DC_EMissionState.ACTIVE)
		{			
			//Add code for runtime
			
			//Eventually when mission is to ended do this:
			//SetState(DC_EMissionState.END);
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionCycleTime*1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		super.MissionEnd();
		
		SDRC_Log.Add("[SDRC_Mission_Chopper:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		//Code for whatever you need for spawning things.
		EntitySpawnParams params = EntitySpawnParams();
//		string resourceName	= "{70BAEEFC2D3FEE64}Prefabs/Vehicles/Helicopters/UH1H/UH1H.et";
		string resourceName	= "{6D71309125B8AEA2}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying.et";
		vector pos = "1053 49 2470";
//		vector pos = "1053 39 2470";
	
		//Spawn the resource exactly to pos
		Resource resource = Resource.Load(resourceName);
		vector transform[4];
		SDRC_SpawnHelper.GetTransformFromPosAndRot(transform, pos, 0);	//NOTE: This will snap to ground! 
        params.TransformMode = ETransformMode.WORLD;			
        params.Transform = transform;
		m_Vehicle = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);		
		
		m_Vehicle_s = VehicleHelicopterSimulation.Cast(m_Vehicle.FindComponent(VehicleHelicopterSimulation));
        m_Vehicle_s.EngineStart();
        m_Vehicle_s.SetThrottle(1);
        m_Vehicle_s.RotorSetForceScaleState(0, 1.1);	//Hovering 1.2
        m_Vehicle_s.RotorSetForceScaleState(1, 2);

//		SCR_HelicopterControllerComponent m_Vehicle_c = SCR_HelicopterControllerComponent.Cast(m_Vehicle.FindComponent(SCR_HelicopterControllerComponent));
//		m_Vehicle_c.<smth smth mas startup time>
				
		vector velOrig = m_Vehicle.GetPhysics().GetVelocity();
        vector rotVector = m_Vehicle.GetAngles();
		
        vector vel = {velOrig[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * 10, velOrig[1], velOrig[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * 10 };
        vector rot = {rotVector[0] + Math.Sin(rotVector[0] * Math.DEG2RAD) * 0, rotVector[1], rotVector[2] + Math.Cos(rotVector[2] * Math.DEG2RAD) * 0 };
		
        m_Vehicle.SetAngles(rot);				
		m_Vehicle.GetPhysics().SetVelocity(vel);
		
		GetGame().GetCallqueue().CallLater(Path1, 5000);		
	}
	
	private void Path1()
	{		
		array<float> rotor = {1.8, 2.5, 2.2, 1.3, 1.2, -1};
		array<float> vel0 =  {20,  30,  30,  25,  20};
		array<float> rot0 =  {10,  20,  20,  -20, -10};
		
        m_Vehicle_s.RotorSetForceScaleState(0, rotor[idx]);	//Hovering 1.2
        m_Vehicle_s.RotorSetForceScaleState(1, 2);
		
		vector velOrig = m_Vehicle.GetPhysics().GetVelocity();
//        vector rotVector = m_Vehicle.GetAngles();
        vector rotVector = m_Vehicle.GetYawPitchRoll();
		
        vector vel = {	velOrig[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * vel0[idx], 
						velOrig[1] + 0.5, 
						velOrig[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * vel0[idx] };		
        vector rot = {	rotVector[0] + Math.Sin(rotVector[0] * Math.DEG2RAD) * rot0[idx], 
						rotVector[1] + 0.5,
						rotVector[2] + Math.Cos(rotVector[2] * Math.DEG2RAD) * rot0[idx]};
		
//        m_Vehicle.SetAngles(rot);		
        m_Vehicle.SetYawPitchRoll(rot);		
        m_Vehicle.GetPhysics().SetVelocity(vel);		
		
		idx++;
		if (rotor[idx] != -1)
		{
			GetGame().GetCallqueue().CallLater(Path1, 8000, false);
		}
	}
}
		
//------------------------------------------------------------------------------------------------
class SDRC_ChopperConfig : SDRC_MissionConfig
{
	//Mission specific
	vector pos;
	string posName;
	string title;
	string info;
	
	//Variables here
}

//------------------------------------------------------------------------------------------------
class SDRC_ChopperJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Chopper.json";
	ref SDRC_ChopperConfig conf = new SDRC_ChopperConfig();
		
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
		conf.showMarker = true;		
		//Mission specific
		conf.pos = "0 0 0";
		conf.posName = "airport";
		conf.title = "Chopper mission at ";
		conf.info = "Some additional information for players";
	}	
}