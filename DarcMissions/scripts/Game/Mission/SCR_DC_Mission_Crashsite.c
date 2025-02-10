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
	SPAWN_SITE,
	SPAWN_AI,
	RUN
};

class SCR_DC_Mission_Crashsite : SCR_DC_Mission
{
	private ref SCR_DC_CrashsiteJsonApi m_CrashsiteJsonApi = new SCR_DC_CrashsiteJsonApi();	
	private ref SCR_DC_CrashsiteConfig m_Config;
	private const int DC_LOCATION_SEACRH_ITERATIONS = 30;	//How many different spots to try for a mission before giving up	
		
	private DC_EMissionCrashSiteState missionCrashSiteState = DC_EMissionCrashSiteState.INIT;
	private vector m_PosDestination = "0 0 0";	//The destination where the chopper is flying from mission position
	private float m_Angle = 0;
	private IEntity m_Vehicle;
	private vector m_VehiclePosOld;
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
		
		//Find position
		bool positionFound = false;
		vector pos;

		for (int i = 0; i < DC_LOCATION_SEACRH_ITERATIONS; i++)
		{
			pos = SCR_DC_MissionHelper.FindMissionPos(300, 500);
			if (pos != "0 0 0")
			{
				//Find flight positions from pos to m_PosDestination.
				positionFound = true;
				pos[1] = pos[1] + Math.RandomInt(80, 120);
				int rnd = SCR_DC_Misc.GetWorldSize()/8;
				m_PosDestination[0] = SCR_DC_Misc.GetWorldSize()/2 + Math.RandomFloat(-rnd, rnd);
				m_PosDestination[2] = SCR_DC_Misc.GetWorldSize()/2 + Math.RandomFloat(-rnd, rnd);
				
				vector direction = vector.Direction(pos, m_PosDestination);
				m_Angle = SCR_DC_Misc.VectorToAngle(direction);
				
				SCR_DC_Log.Add("[SCR_DC_Mission_Crashsite] Helicopter flying from " + pos + " to " + m_PosDestination + ". Angle: " + m_Angle, LogLevel.DEBUG);
				break;
			}
			else
			{						
				SCR_DC_Log.Add("[SCR_DC_Mission_Crashsite] Invalid mission position. Try " + (i + 1) + "/" + DC_LOCATION_SEACRH_ITERATIONS, LogLevel.SPAM);
			}
		}
		
		if (positionFound)
		{	
			SetTitle(m_Config.title);
			SetInfo(m_Config.info);
			SetPos(pos);
			SetPosName("");
			SetMarkerId(SCR_DC_MapMarkersUI.AddMarker(GetId(), GetPos(), GetTitle()));
	
			SetState(DC_MissionState.INIT);			

			//Set a marker for destination
			SetMarkerId(SCR_DC_MapMarkersUI.AddMarker(GetId() + "_1", m_PosDestination, "Destination"));
			SCR_DC_DebugHelper.AddDebugPos(m_PosDestination, Color.RED, 10, GetId() + "_1");
		}
		else
		{				
			//No suitable location found.
			SCR_DC_Log.Add("[SCR_DC_Mission_Crashsite] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;
		}	
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

						SetMarkerId(SCR_DC_MapMarkersUI.AddMarker(GetId(), GetPos(), "Crashsite"));
					
						//Make sure the chopper is destroyed
						DamageManagerComponent damageManager = DamageManagerComponent.Cast(m_Vehicle.FindComponent(DamageManagerComponent));
						if (damageManager)
							damageManager.SetHealthScaled(0);
					
						//TBD: Simulation should be set off to stop rolling etc.
						//VehicleHelicopterSimulation vehicle_s;
						//vehicle_s = VehicleHelicopterSimulation.Cast(m_Vehicle.FindComponent(VehicleHelicopterSimulation));
						SCR_DC_DebugHelper.MoveDebugPos(GetId(), GetPos());
						missionCrashSiteState = DC_EMissionCrashSiteState.SPAWN_SITE;
					}
					break;
				case DC_EMissionCrashSiteState.SPAWN_SITE:
					IEntity entity;
				
					SCR_DC_SpawnHelper.SetStructuresToOrigo(m_Config.siteItems);
				
					float rotation = Math.RandomFloat(0, 360);			
					int i = 0;
				
					foreach(SCR_DC_Structure item : m_Config.siteItems)
					{
						entity = SCR_DC_SpawnHelper.SpawnStructures(m_Config.siteItems, GetPos(), rotation, i);
						
						if (entity != NULL)
						{ 
							m_EntityList.Insert(entity);
						}
						else
						{
							SCR_DC_Log.Add("[SCR_DC_Mission_Crashsite:MissionSpawn] Could not load: " + item.m_Resource, LogLevel.ERROR);				
						}
						
						i++;			
					}
				
					//Put loot
					if (m_Config.loot)			
					{
						m_Config.loot.box = m_EntityList[1];	//Normally it's the first one, but we have added the chopper in the list as the first one.
						SCR_DC_LootHelper.SpawnItemsToStorage(m_Config.loot.box, m_Config.loot.items, m_Config.loot.itemChance);
						SCR_DC_Log.Add("[SCR_DC_Mission_Crashsite:MissionSpawn] Loot added.", LogLevel.DEBUG);								
					}
								
					missionCrashSiteState = DC_EMissionCrashSiteState.SPAWN_AI;
					break;								
				case DC_EMissionCrashSiteState.SPAWN_AI:

					SCR_AIGroup group = SCR_DC_MissionHelper.SpawnMissionAIGroup(m_Config.groupTypes.GetRandomElement(), GetPos());				
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
		SCR_DC_HelicopterInfo helicopterInfo = m_Config.helicopterInfo.GetRandomElement();
		vector pos = GetPos();

		//Spawn the resource exactly to pos		
		m_Vehicle = SCR_DC_SpawnHelper.SpawnItem(pos, helicopterInfo.resource, m_Angle, -1, false);
		m_EntityList.Insert(m_Vehicle);
		
		VehicleHelicopterSimulation m_Vehicle_s;
		m_Vehicle_s = VehicleHelicopterSimulation.Cast(m_Vehicle.FindComponent(VehicleHelicopterSimulation));
        m_Vehicle_s.EngineStart();
        m_Vehicle_s.SetThrottle(helicopterInfo.throttle);
        m_Vehicle_s.RotorSetForceScaleState(0, helicopterInfo.rotorForce);	//Hovering 1.2	.. was 0.8 for the other one
        m_Vehicle_s.RotorSetForceScaleState(1, 2);

		vector velOrig = m_Vehicle.GetPhysics().GetVelocity();
        vector rotVector = m_Vehicle.GetAngles();
		int speed = 40;
		
        vector vel = {velOrig[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * speed, velOrig[1], velOrig[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * speed };
		m_Vehicle.GetPhysics().SetVelocity(vel);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if the entity is still moving. In case the distance has not changed that much, we define the chopper not to be moving anymore.
	*/
	private bool IsStillFlying(IEntity vehicle)
	{
		vector pos = vehicle.GetOrigin();
		if(!SCR_DC_Misc.IsPosNearPos(pos, m_VehiclePosOld, 10))
		{
			m_VehiclePosOld = pos;
			return true;
		}
		
		return false;
	}	
}
		
//------------------------------------------------------------------------------------------------
class SCR_DC_HelicopterInfo : Managed
{
	string resource;
	float throttle;
	float rotorForce;

	void Set(string resource_, float throttle_, float rotorForce_)
	{
		resource = resource_;
		throttle = throttle_;
		rotorForce = rotorForce_;
	};
}

//------------------------------------------------------------------------------------------------

class SCR_DC_CrashsiteConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	int missionLifeCycleTime = 1000;//DC_MISSION_LIFECYCLE_TIME_DEFAULT;	//How often the mission is run
	
	//Mission specific
	string title;
	string info;
	ref array<ref SCR_DC_HelicopterInfo> helicopterInfo = {};
	ref array<ref SCR_DC_Structure> siteItems = {};
	ref SCR_DC_Loot loot = null;
	ref array<string> groupTypes = {};
	ref array<int> waypointRange = {};		//min, max
	
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
		conf.title = "Helicopter in distress ";
		conf.info = "A valuable cargo has crashed";
		conf.waypointRange = {10,50};
		conf.groupTypes = 
		{
			"{657590C1EC9E27D3}Prefabs/Groups/OPFOR/Group_USSR_LightFireTeam.et",
			//"{58251EDC277CE499}622120A5448725E3/Prefabs/Groups/Group_Zombies_USSR.et"
		};
		
		SCR_DC_HelicopterInfo heli0 = new SCR_DC_HelicopterInfo;
		heli0.Set(
			"{40A3EEECFF765793}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_flying.et",
			0.8,
			1.3
		);
		conf.helicopterInfo.Insert(heli0);
		SCR_DC_HelicopterInfo heli1 = new SCR_DC_HelicopterInfo;
		heli1.Set(
			"{6D71309125B8AEA2}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying.et",
			0.7,
			0.9
		);
		conf.helicopterInfo.Insert(heli1);
		
		SCR_DC_Structure crashitem0 = new SCR_DC_Structure;
		crashitem0.Set(
			//"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
	        "{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
	        "921.983 39 2629.78"
	    );
		conf.siteItems.Insert(crashitem0);
		SCR_DC_Structure crashitem1 = new SCR_DC_Structure;
		crashitem1.Set(
	        "{33E84AF90E5FE1E5}Prefabs/Vehicles/Helicopters/UH1H/Dst/Dbr/Dbr_UH1H_Blade_03.et",
	        "924.145 39 2635.076",
	        "0 29.077 0"
	    );
		conf.siteItems.Insert(crashitem1);
		SCR_DC_Structure crashitem2 = new SCR_DC_Structure;
		crashitem2.Set(
	        "{342E852E9A1847EA}Prefabs/Props/Industrial/Repair/VehicleGarbage_01_pile_medium.et",
	        "928.642 39 2628.902",
	        "0 37.793 0"
	    );
		conf.siteItems.Insert(crashitem2);
		SCR_DC_Structure crashitem3 = new SCR_DC_Structure;
		crashitem3.Set(
	        "{D674060002BA768E}Prefabs/Vehicles/Helicopters/UH1H/Dst/Dbr/Dbr_UH1H_Blade_02.et",
	        "928.925 39 2633.846"
	    );
		conf.siteItems.Insert(crashitem3);		
		SCR_DC_Structure crashitem4 = new SCR_DC_Structure;
		crashitem4.Set(
			"{F4561FBC26102515}Prefabs/Particles/Metal/Vehicle/Dbr_Helicopter_Rotor.et",
			"925.622 39.009 2628.648"
		);
		conf.siteItems.Insert(crashitem4);		
		SCR_DC_Structure crashitem5 = new SCR_DC_Structure;
		crashitem5.Set(
			"{F4561FBC26102515}Prefabs/Particles/Metal/Vehicle/Dbr_Helicopter_Rotor.et",
			"931.951 39 2631.805",
			"0 68.972 0"
		);
		conf.siteItems.Insert(crashitem5);		
		SCR_DC_Structure crashitem6 = new SCR_DC_Structure;
		crashitem6.Set(
			"{F4561FBC26102515}Prefabs/Particles/Metal/Vehicle/Dbr_Helicopter_Rotor.et",
			"922.173 39 2632.577",
			"0 68.972 0"
		);
		conf.siteItems.Insert(crashitem6);
		
		SCR_DC_Loot crashloot = new SCR_DC_Loot;
		array<string> lootItems = {
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			};
		crashloot.Set(0.9, lootItems);
		conf.loot = crashloot;		
	}	
}