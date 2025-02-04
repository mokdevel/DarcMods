//Mission SCR_DC_Mission_Chopper.c
//
//An example of a mission file with simple set. No real functionality.

class SCR_DC_Mission_Chopper : SCR_DC_Mission
{
	private ref SCR_DC_ChopperJsonApi m_ChopperJsonApi = new SCR_DC_ChopperJsonApi();	
	private ref SCR_DC_ChopperConfig m_Config;
	
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission_Chopper()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Chopper] Constructor", LogLevel.DEBUG);
				
		//Set some defaults
		SCR_DC_Mission();

		//Load config
		m_ChopperJsonApi.Load();
		m_Config = m_ChopperJsonApi.conf;
		
		string posName = m_Config.posName;
		vector pos = m_Config.pos;
		
		SetTitle(m_Config.title + "" + posName);
		SetInfo(m_Config.info);
		SetPos(pos);
		SetPosName(posName);
		SetMarkerId(SCR_DC_MapMarkersUI.AddMarker(DC_ID_PREFIX, GetPos(), GetTitle()));

		SetState(DC_MissionState.INIT);			
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		if (GetState() == DC_MissionState.INIT)
		{
			MissionSpawn();
			SetState(DC_MissionState.ACTIVE);
		}

		if (GetState() == DC_MissionState.END)
		{
			MissionEnd();
			SetState(DC_MissionState.EXIT);
		}	
				
		if (GetState() == DC_MissionState.ACTIVE)
		{			
			//Add code for runtime
			
			//Eventually when mission is to ended do this:
			//SetState(DC_MissionState.END);
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionLifeCycleTime*1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		
		
	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		//Code for whatever you need for spawning things.
		IEntity entity = NULL;
		EntitySpawnParams params = EntitySpawnParams();
//		string resourceName	= "{70BAEEFC2D3FEE64}Prefabs/Vehicles/Helicopters/UH1H/UH1H.et";
		string resourceName	= "{6D71309125B8AEA2}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying.et";
		vector pos = "1053 69 2470";
//		vector pos = "1053 39 2470";
	
		//Spawn the resource exactly to pos
		Resource resource = Resource.Load(resourceName);
		vector transform[4];
		SCR_DC_SpawnHelper.GetTransformFromPosAndRot(transform, pos, 0);
        params.TransformMode = ETransformMode.WORLD;			
        params.Transform = transform;
		entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);		
	
		
		
		VehicleHelicopterSimulation m_Vehicle_s = VehicleHelicopterSimulation.Cast(entity.FindComponent(VehicleHelicopterSimulation));
        m_Vehicle_s.EngineStart();
        m_Vehicle_s.SetThrottle(1);
        m_Vehicle_s.RotorSetForceScaleState(0, 1);
        m_Vehicle_s.RotorSetForceScaleState(1, 1);
		
		vector velOrig = entity.GetPhysics().GetVelocity();
        vector rotVector = entity.GetAngles();
        vector vel = {velOrig[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * 50, velOrig[1], velOrig[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * 50 };
        entity.GetPhysics().SetVelocity(vel);
	}
}
	
//------------------------------------------------------------------------------------------------
class SCR_DC_ChopperConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	int missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_DEFAULT;	//How often the mission is run
	
	//Mission specific
	vector pos;
	string posName;
	string title;
	string info;
	
	//Variables here
}

//------------------------------------------------------------------------------------------------
class SCR_DC_ChopperJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Chopper.json";
	ref SCR_DC_ChopperConfig conf = new SCR_DC_ChopperConfig;
		
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_MISSIONCONFIG_FILE);
		
		if(!loadContext)
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
		//Mission specific
		conf.missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_DEFAULT;
		conf.pos = "0 0 0";
		conf.posName = "airport";
		conf.title = "Chopper mission at ";
		conf.info = "Some additional information for players";
	}	
}