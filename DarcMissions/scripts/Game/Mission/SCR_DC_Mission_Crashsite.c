//Mission SCR_DC_Mission_Crashsite.c

//------------------------------------------------------------------------------------------------
/*!
A chopper flys and crashes. Loot and defending AI is spawned.
*/

//------------------------------------------------------------------------------------------------

enum DC_EMissionCrashSiteState
{
	INIT,
	FLYING,
	SPAWN,
	RUN
};

class SCR_DC_Mission_Crashsite : SCR_DC_Mission
{
	private ref SCR_DC_CrashsiteJsonApi m_CrashsiteJsonApi = new SCR_DC_CrashsiteJsonApi();	
	private ref SCR_DC_CrashsiteConfig m_Config;
		
	private DC_EMissionCrashSiteState missionCrashSiteState = DC_EMissionCrashSiteState.INIT;
	private IEntity m_Vehicle;
	private vector vehiclePosOld;
	private int idx = 0;	
	
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission_Crashsite()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Crashsite] Constructor", LogLevel.DEBUG);

		
//		SCR_DC_Log.Add("[SCR_DC_Mission_Crashsite] Class test: " + SCR_DC_Misc.IsClassAvailable("SCR_DC_Mission_Crashsite"), LogLevel.DEBUG);
						
		//Set some defaults
		SCR_DC_Mission();

		//Load config
		m_CrashsiteJsonApi.Load();
		m_Config = m_CrashsiteJsonApi.conf;
		
		string posName = m_Config.posName;
		vector pos = m_Config.pos;		
//		vector pos = FindMissionPos(
		
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
			
			switch (missionCrashSiteState)
			{
				case DC_EMissionCrashSiteState.INIT:
					//This state is here only to give the chopper some time to fly.
					missionCrashSiteState = DC_EMissionCrashSiteState.FLYING;
					break;
				case DC_EMissionCrashSiteState.FLYING:
					if (!IsStillFlying(m_Vehicle))
					{
						SetPos(m_Vehicle.GetOrigin());
						//TBD: Simulation should be set off to stop rolling etc.
						//VehicleHelicopterSimulation vehicle_s;
						//vehicle_s = VehicleHelicopterSimulation.Cast(m_Vehicle.FindComponent(VehicleHelicopterSimulation));
						SCR_DC_DebugHelper.MoveDebugPos(GetId(), GetPos());
						missionCrashSiteState = DC_EMissionCrashSiteState.SPAWN;
					}
					break;
				case DC_EMissionCrashSiteState.SPAWN:
				
					SCR_AIGroup group = SCR_DC_MissionHelper.SpawnMissionAIGroup(m_Config.groupTypes.GetRandomElement(), GetPos());				
//					SCR_AIGroup group = SCR_DC_MissionHelper.SpawnMissionAIGroup("{58251EDC277CE499}622120A5448725E3/Prefabs/Groups/Group_Zombies_USSR.et", GetPos());
//					SCR_AIGroup group = SCR_DC_MissionHelper.SpawnMissionAIGroup("{657590C1EC9E27D3}Prefabs/Groups/OPFOR/Group_USSR_LightFireTeam.et", GetPos());
					if (group)
					{
						m_Groups.Insert(group);
						SCR_DC_MissionHelper.CreateMissionAIWaypoints(group, m_Config.waypointRange[0], m_Config.waypointRange[1], DC_EWaypointMoveType.PATROLCYCLE , DC_EWaypointRndType.SCATTERED);
					}
					SCR_DC_Log.Add("[SCR_DC_Mission_Crashsite:MissionSpawn] AI groups spawned ", LogLevel.DEBUG);								
					missionCrashSiteState = DC_EMissionCrashSiteState.RUN;
					break;
				case DC_EMissionCrashSiteState.RUN:		
					if (SCR_DC_AIHelper.AreAllGroupsDead(m_Groups))
					{
						if (!IsActive())
						{
							SCR_DC_Log.Add("[SCR_DC_Mission_Crashsite:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
							SetState(DC_MissionState.END);
						}
					}
					break;
				default:
					//Nothing
			}
			
			//Add code for runtime
			//Eventually when mission is to ended do this:
			//SetState(DC_MissionState.END);
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionLifeCycleTime*1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		super.MissionEnd();	
			
		SCR_DC_Log.Add("[SCR_DC_Mission_Crashsite:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		//Code for whatever you need for spawning things.
		EntitySpawnParams params = EntitySpawnParams();
//		string resourceName	= "{70BAEEFC2D3FEE64}Prefabs/Vehicles/Helicopters/UH1H/UH1H.et";
		string resourceName	= "{6D71309125B8AEA2}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying.et";
		vector pos = GetPos() + "0 40 0";
	
		//Spawn the resource exactly to pos		
		m_Vehicle = SCR_DC_SpawnHelper.SpawnItem(pos, resourceName, 40, -1, false);
		m_EntityList.Insert(m_Vehicle);
		
/*		Resource resource = Resource.Load(resourceName);
		vector transform[4];
		SCR_DC_SpawnHelper.GetTransformFromPosAndRot(transform, pos, 0, false);
        params.TransformMode = ETransformMode.WORLD;			
        params.Transform = transform;
		m_Vehicle = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);		
		m_EntityList.Insert(m_Vehicle);*/
		
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
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if the entity is still moving. In case the distance has not changed that much, we define the chopper not to be moving anymore.
	*/
	private bool IsStillFlying(IEntity vehicle)
	{
		vector pos = vehicle.GetOrigin();
		if(SCR_DC_Misc.IsPosNearPos(pos, vehiclePosOld, 3))
		{
			return true;
		}
		
		vehiclePosOld = pos;
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
	ref array<int> waypointRange = {};		//min, max
	ref array<string> groupTypes = {};
	
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
		conf.waypointRange = {10,50};
		conf.groupTypes = 
		{
			"{657590C1EC9E27D3}Prefabs/Groups/OPFOR/Group_USSR_LightFireTeam.et"
		}
		
	}	
}