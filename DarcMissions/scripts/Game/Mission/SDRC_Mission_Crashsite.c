//Mission SDRC_Mission_Crashsite.c

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

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Crashsite : SDRC_Mission
{
	private ref SDRC_CrashsiteJsonApi m_CrashsiteJsonApi = new SDRC_CrashsiteJsonApi();	
	private ref SDRC_CrashsiteConfig m_Config;
	private const int DC_LOCATION_SEACRH_ITERATIONS = 30;	//How many different spots to try for a mission before giving up	

	protected ref SDRC_Crashsite m_DC_Crashsite;	//Occupation configuration in use
			
	private DC_EMissionCrashSiteState missionCrashSiteState = DC_EMissionCrashSiteState.INIT;
	private vector m_vPosDestination = "0 0 0";	//The destination where the chopper is flying from mission position
	private float m_fAngle = 0;
	private IEntity m_Vehicle;
	private vector m_vVehiclePosOld;
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Crashsite()
	{
		SDRC_Log.Add("[SDRC_Mission_Crashsite] Constructor", LogLevel.SPAM);
		
		//Set some defaults
		SDRC_Mission();
		SetType(DC_EMissionType.CRASHSITE);

		//Load config
		m_CrashsiteJsonApi.Load();
		m_Config = m_CrashsiteJsonApi.conf;
		
		//Pick a configuration for mission
		int idx = SDRC_MissionHelper.SelectMissionIndex(m_Config.crashsiteList);
		if (idx == -1)
		{
			SDRC_Log.Add("[SDRC_Mission_Crashsite] No crashsites defined.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}
		m_DC_Crashsite = m_Config.crashsites[idx];
		
		//Find position
		bool positionFound = false;
		vector pos;

		for (int i = 0; i < DC_LOCATION_SEACRH_ITERATIONS; i++)
		{
			pos = SDRC_MissionHelper.FindMissionPos(m_Config.distanceToMission, m_Config.distanceToPlayer);
			if (pos != "0 0 0")
			{
				//Find flight positions from pos to m_PosDestination.
				positionFound = true;
				pos[1] = pos[1] + Math.RandomInt(m_Config.flyHeight[0], m_Config.flyHeight[0]);	//Adjust flight height
				int rnd = SDRC_Misc.GetWorldSize()/8;
				m_vPosDestination[0] = SDRC_Misc.GetWorldSize()/2 + Math.RandomFloat(-rnd, rnd);
				m_vPosDestination[2] = SDRC_Misc.GetWorldSize()/2 + Math.RandomFloat(-rnd, rnd);
				
				vector direction = vector.Direction(pos, m_vPosDestination);
				m_fAngle = SDRC_Misc.VectorToAngle(direction);
				
				SDRC_Log.Add("[SDRC_Mission_Crashsite] Helicopter flying from " + pos + " to " + m_vPosDestination + ". Angle: " + m_fAngle, LogLevel.DEBUG);
				break;
			}
			else
			{						
				SDRC_Log.Add("[SDRC_Mission_Crashsite] Invalid mission position. Try " + (i + 1) + "/" + DC_LOCATION_SEACRH_ITERATIONS, LogLevel.SPAM);
			}
		}
		
		if (!positionFound)	//No suitable location found.
		{				
			SDRC_Log.Add("[SDRC_Mission_Crashsite] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}	
		
		SetPos(pos);
		SetPosName("");
		SetTitle(m_DC_Crashsite.title);
		SetInfo(m_DC_Crashsite.info);
		SetMarker(m_Config.showMarker, DC_EMissionIcon.N_HELI);
		SetShowHint(m_Config.showHint);

		SetState(DC_EMissionState.INIT);			

		//Set a marker for destination
		if (!SDRC_Conf.RELEASE)
		{			
			SDRC_MapMarkerHelper.CreateMapMarker(m_vPosDestination, DC_EMissionIcon.TARGET_O, GetId() + "_1", "Destination");
			SDRC_DebugHelper.AddDebugPos(m_vPosDestination, ARGB(50, 255, 0, 0), 10, GetId() + "_1");
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
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
			switch (missionCrashSiteState)
			{
				case DC_EMissionCrashSiteState.INIT:
					//This state is here only to give the chopper some time to fly.
					missionCrashSiteState = DC_EMissionCrashSiteState.FLYING;
					break;
				case DC_EMissionCrashSiteState.FLYING:
					SetPos(m_Vehicle.GetOrigin());
				
					if (!IsStillFlying(m_Vehicle))
					{
						//Make sure the chopper is destroyed
						DamageManagerComponent damageManager = DamageManagerComponent.Cast(m_Vehicle.FindComponent(DamageManagerComponent));
						if (damageManager)
							damageManager.SetHealthScaled(0);
					
						//TBD: Simulation should be set off to stop rolling etc.
						//VehicleHelicopterSimulation vehicle_s;
						//vehicle_s = VehicleHelicopterSimulation.Cast(m_Vehicle.FindComponent(VehicleHelicopterSimulation));
						SDRC_DebugHelper.MoveDebugPos(GetId(), GetPos());
						if (m_Config.showMarker)
						{
							SDRC_MapMarkerHelper.DeleteMarker(GetId());
							SDRC_MapMarkerHelper.CreateMapMarker(GetPos(), DC_EMissionIcon.N_FIRE, GetId(), "Crash site");
						}
						missionCrashSiteState = DC_EMissionCrashSiteState.SPAWN_SITE;
					}
					else
					{
						if (m_Config.showMarker)
						{
							SDRC_MapMarkerHelper.DeleteMarker(GetId());
							SDRC_MapMarkerHelper.CreateMapMarker(GetPos(), DC_EMissionIcon.N_HELI, GetId(), "");
						}
					}
					break;
				case DC_EMissionCrashSiteState.SPAWN_SITE:
					IEntity entity;
				
					SDRC_SpawnHelper.SetStructuresToOrigo(m_DC_Crashsite.siteItems);
				
					float rotation = Math.RandomFloat(0, 360);			
					int i = 0;
				
					foreach (SDRC_Structure item : m_DC_Crashsite.siteItems)
					{
						entity = SDRC_SpawnHelper.SpawnStructures(m_DC_Crashsite.siteItems, GetPos(), rotation, i);
						
						if (entity != NULL)
						{ 
							m_EntityList.Insert(entity);
						}
						else
						{
							SDRC_Log.Add("[SDRC_Mission_Crashsite:MissionSpawn] Could not load: " + item.GetResource(), LogLevel.ERROR);				
						}
						
						i++;			
					}
				
					//Put loot
					if (m_DC_Crashsite.loot)			
					{
						m_DC_Crashsite.loot.box = m_EntityList[1];	//Normally it's the first one, but we have added the chopper in the list as the first one.
						SDRC_LootHelper.SpawnItemsToStorage(m_DC_Crashsite.loot.box, m_DC_Crashsite.loot.items, m_DC_Crashsite.loot.itemChance);
						SDRC_Log.Add("[SDRC_Mission_Crashsite:MissionSpawn] Loot added.", LogLevel.DEBUG);								
					}
								
					missionCrashSiteState = DC_EMissionCrashSiteState.SPAWN_AI;
					break;								
				case DC_EMissionCrashSiteState.SPAWN_AI:
					SCR_AIGroup group = SDRC_MissionHelper.SpawnMissionAIGroup(m_DC_Crashsite.groupTypes.GetRandomElement(), GetPos());				
					if (group)
					{
						SDRC_AIHelper.SetAIGroupSkill(group, m_DC_Crashsite.aiSkill, m_DC_Crashsite.aiPerception);					
						m_Groups.Insert(group);
						SDRC_WPHelper.CreateMissionAIWaypoints(group, DC_EWaypointGenerationType.LOITER, GetPos(), "0 0 0", DC_EWaypointMoveType.LOITER);
					}
					SDRC_Log.Add("[SDRC_Mission_Crashsite:MissionSpawn] AI groups spawned ", LogLevel.DEBUG);								
					missionCrashSiteState = DC_EMissionCrashSiteState.RUN;
					break;
				case DC_EMissionCrashSiteState.RUN:		
					if (!IsActive())
					{
						SDRC_Log.Add("[SDRC_Mission_Crashsite:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
						SetState(DC_EMissionState.END);
					}
					break;
				default:
					//Nothing
			}
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionCycleTime*1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		super.MissionEnd();	
			
		SDRC_Log.Add("[SDRC_Mission_Crashsite:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		EntitySpawnParams params = EntitySpawnParams();
		SDRC_HelicopterInfo helicopterInfo = m_DC_Crashsite.helicopterInfo.GetRandomElement();
		vector pos = GetPos();

		//Spawn the resource exactly to pos		
		m_Vehicle = SDRC_SpawnHelper.SpawnItem(pos, helicopterInfo.resource, m_fAngle, -1, false);
		if (m_Vehicle)
		{
			m_EntityList.Insert(m_Vehicle);
		}
		
		VehicleHelicopterSimulation m_Vehicle_s;
		m_Vehicle_s = VehicleHelicopterSimulation.Cast(m_Vehicle.FindComponent(VehicleHelicopterSimulation));
        m_Vehicle_s.EngineStart();
        m_Vehicle_s.SetThrottle(helicopterInfo.throttle);
        m_Vehicle_s.RotorSetForceScaleState(0, helicopterInfo.rotorForce);
        m_Vehicle_s.RotorSetForceScaleState(1, helicopterInfo.rotor2Force);

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
		if (!SDRC_Misc.IsPosNearPos(pos, m_vVehiclePosOld, 10))
		{
			m_vVehiclePosOld = pos;
			return true;
		}
		
		return false;
	}	
}
		
//------------------------------------------------------------------------------------------------
class SDRC_HelicopterInfo : Managed
{
	string resource;
	float throttle;
	float rotorForce;
	float rotor2Force;

	void Set(string resource_, float throttle_, float rotorForce_, float rotor2Force_)
	{
		resource = resource_;
		throttle = throttle_;
		rotorForce = rotorForce_;
		rotor2Force = rotor2Force_;
	};
}

//------------------------------------------------------------------------------------------------

class SDRC_CrashsiteConfig : SDRC_MissionConfig
{
	//Mission specific	
	int distanceToMission;								//Distance to mission when searching for a mission pos. Overrides missionFrame settings.
	int distanceToPlayer;								//Distance to player when searching for a mission pos. Overrides missionFrame settings.
	ref array<int> flyHeight = {};						//min, max - Spawn helicopter between these values.
	ref array<ref int> crashsiteList = {};				//The indexes of crashsites.
	ref array<ref SDRC_Crashsite> crashsites = {};	//List of crashsites
}

//------------------------------------------------------------------------------------------------
class SDRC_Crashsite : Managed
{
	//Mission specific
	string comment;							//Generic comment to describe the mission. Not used in game.
	string title;							//Title for the hint shown for players
	string info;							//Details for the hint shown for players
	ref array<string> groupTypes = {};
	int aiSkill;
	float aiPerception;
	ref array<ref SDRC_HelicopterInfo> helicopterInfo = {};
	//Optional settings
	ref SDRC_Loot loot = null;
	ref array<ref SDRC_Structure> siteItems = {};
	
	void Set(string comment_, string title_, string info_, array<string> groupTypes_, int aiSkill_, float aiPerception_)
	{
		comment = comment_;
		title = title_;
		info = info_;
		groupTypes = groupTypes_;
		aiSkill = aiSkill_;
		aiPerception = aiPerception_;
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_CrashsiteJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Crashsite.json";
	ref SDRC_CrashsiteConfig conf = new SDRC_CrashsiteConfig();
		
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
		conf.version = 1;
		conf.author = "darc";
		//Default
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		conf.showMarker = true;
		//Mission specific
		conf.distanceToMission = 100;
		conf.distanceToPlayer = 500;
		conf.flyHeight = {80, 120};
		conf.crashsiteList = {0};
		
		//----------------------------------------------------
		SDRC_Crashsite crashsite0 = new SDRC_Crashsite();
		crashsite0.Set
		(
			"Gogland: Mission to be used with Escapists.",
			"Helicopter in distress ",
			"A valuable cargo has crashed",	
			{
				"G_LIGHT", "G_ADMIN"
			},
			20, 0.8
		);
		conf.crashsites.Insert(crashsite0);
		
		//----------------------------------------------------
		SDRC_HelicopterInfo heli0 = new SDRC_HelicopterInfo();
			heli0.Set("{40A3EEECFF765793}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_flying.et", 0.8, 0.8, 1.0);
			crashsite0.helicopterInfo.Insert(heli0);
		SDRC_HelicopterInfo heli1 = new SDRC_HelicopterInfo();
			heli1.Set("{6D71309125B8AEA2}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying.et",	0.7, 0.9, 1.0);
			crashsite0.helicopterInfo.Insert(heli1);
		SDRC_HelicopterInfo heli2 = new SDRC_HelicopterInfo();
			heli2.Set("{40A3EEECFF765793}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_flying.et",	0.8, 0.8, -1.0);
			crashsite0.helicopterInfo.Insert(heli2);		
		
		//----------------------------------------------------
		SDRC_Loot crashloot = new SDRC_Loot;
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
		crashsite0.loot = crashloot;
				
		//----------------------------------------------------
		SDRC_Structure crashitem0 = new SDRC_Structure();
		crashitem0.Set(
			//"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
	        "{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
	        "921.983 39 2629.78"
	    );
		crashsite0.siteItems.Insert(crashitem0);
		SDRC_Structure crashitem1 = new SDRC_Structure();
		crashitem1.Set(
	        "{33E84AF90E5FE1E5}Prefabs/Vehicles/Helicopters/UH1H/Dst/Dbr/Dbr_UH1H_Blade_03.et",
	        "924.145 39 2635.076",
	        "0 29.077 0"
	    );
		crashsite0.siteItems.Insert(crashitem1);
		SDRC_Structure crashitem2 = new SDRC_Structure();
		crashitem2.Set(
	        "{342E852E9A1847EA}Prefabs/Props/Industrial/Repair/VehicleGarbage_01_pile_medium.et",
	        "928.642 39 2628.902",
	        "0 37.793 0"
	    );
		crashsite0.siteItems.Insert(crashitem2);
		SDRC_Structure crashitem3 = new SDRC_Structure();
		crashitem3.Set(
	        "{D674060002BA768E}Prefabs/Vehicles/Helicopters/UH1H/Dst/Dbr/Dbr_UH1H_Blade_02.et",
	        "928.925 39 2633.846"
	    );
		crashsite0.siteItems.Insert(crashitem3);		
		SDRC_Structure crashitem4 = new SDRC_Structure();
		crashitem4.Set(
			"{F4561FBC26102515}Prefabs/Particles/Metal/Vehicle/Dbr_Helicopter_Rotor.et",
			"925.622 39.009 2628.648"
		);
		crashsite0.siteItems.Insert(crashitem4);		
		SDRC_Structure crashitem5 = new SDRC_Structure();
		crashitem5.Set(
			"{F4561FBC26102515}Prefabs/Particles/Metal/Vehicle/Dbr_Helicopter_Rotor.et",
			"931.951 39 2631.805",
			"0 68.972 0"
		);
		crashsite0.siteItems.Insert(crashitem5);		
		SDRC_Structure crashitem6 = new SDRC_Structure();
		crashitem6.Set(
			"{F4561FBC26102515}Prefabs/Particles/Metal/Vehicle/Dbr_Helicopter_Rotor.et",
			"922.173 39 2632.577",
			"0 68.972 0"
		);
		crashsite0.siteItems.Insert(crashitem6);	
	}	
}