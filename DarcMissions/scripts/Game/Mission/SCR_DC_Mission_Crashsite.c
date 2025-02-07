//Mission SCR_DC_Mission_Crashsite.c
//
//This is a concept of a mission. Some Crashsite to fly from A to B and do something. 
//This is completely unfinished. More like a PoC to show that a Crashsite can fly.

class SCR_DC_Mission_Crashsite : SCR_DC_Mission
{
	private ref SCR_DC_CrashsiteJsonApi m_CrashsiteJsonApi = new SCR_DC_CrashsiteJsonApi();	
	private ref SCR_DC_CrashsiteConfig m_Config;
	
	private IEntity m_Vehicle;
	private vector vehiclePosOld;
	private int idx = 0;	
	
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission_Crashsite()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Crashsite] Constructor", LogLevel.DEBUG);
				
		//Set some defaults
		SCR_DC_Mission();

		//Load config
		m_CrashsiteJsonApi.Load();
		m_Config = m_CrashsiteJsonApi.conf;
		
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
			IsStillFlying();
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
		EntitySpawnParams params = EntitySpawnParams();
//		string resourceName	= "{70BAEEFC2D3FEE64}Prefabs/Vehicles/Helicopters/UH1H/UH1H.et";
		string resourceName	= "{6D71309125B8AEA2}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying.et";
		vector pos = GetPos() + "0 40 0";
//		vector pos = "1053 39 2470";
	
		//Spawn the resource exactly to pos
		Resource resource = Resource.Load(resourceName);
		vector transform[4];
		SCR_DC_SpawnHelper.GetTransformFromPosAndRot(transform, pos, 0, false);
        params.TransformMode = ETransformMode.WORLD;			
        params.Transform = transform;
		m_Vehicle = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);		
		
		VehicleHelicopterSimulation m_Vehicle_s;
		m_Vehicle_s = VehicleHelicopterSimulation.Cast(m_Vehicle.FindComponent(VehicleHelicopterSimulation));
        m_Vehicle_s.EngineStart();
        m_Vehicle_s.SetThrottle(0.7);
        m_Vehicle_s.RotorSetForceScaleState(0, 0.8);	//Hovering 1.2
        m_Vehicle_s.RotorSetForceScaleState(1, 2);

		vector velOrig = m_Vehicle.GetPhysics().GetVelocity();
        vector rotVector = m_Vehicle.GetAngles();
		
        vector vel = {velOrig[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * 50, velOrig[1], velOrig[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * 50 };
		m_Vehicle.GetPhysics().SetVelocity(vel);
		
//		GetGame().GetCallqueue().CallLater(Path1, 5000);
	}
	
	private bool IsStillFlying()
	{
		vector pos = m_Vehicle.GetOrigin();

		PositionsCloseBy(pos, vehiclePosOld);

		vehiclePosOld = m_Vehicle.GetOrigin();
				
		return true;		
	}
	
	private bool PositionsCloseBy(vector pos0, vector pos1, float limit = 10)
	{
		float distance = vector.Distance(pos0, pos1);
		
		if (distance < limit)
		{
			SCR_DC_DebugHelper.MoveDebugPos(GetId(), pos0);
			return true;
		}
		return false;
	}
	
}
		
//------------------------------------------------------------------------------------------------
class SCR_DC_CrashsiteConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	int missionLifeCycleTime = 1000;//DC_MISSION_LIFECYCLE_TIME_DEFAULT;	//How often the mission is run
	
	//Mission specific
	vector pos;
	string posName;
	string title;
	string info;
	
	//Variables here
}

//------------------------------------------------------------------------------------------------
class SCR_DC_CrashsiteJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Crashsite.json";
	ref SCR_DC_CrashsiteConfig conf = new SCR_DC_CrashsiteConfig;
		
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
		conf.pos = "1053 39 2470";
		conf.posName = "airport";
		conf.title = "Crashsite mission at ";
		conf.info = "Some additional information for players";
	}	
}