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
	private ref SDRC_CrashsiteConfig m_Config = new SDRC_CrashsiteConfig();
	private ref SDRC_Crashsite m_DC_Crashsite = new SDRC_Crashsite();
	
	private const int DC_LOCATION_SEACRH_ITERATIONS = 10;	//How many different spots to try for a mission before giving up	
			
	private DC_EMissionCrashSiteState missionCrashSiteState = DC_EMissionCrashSiteState.INIT;
	private vector m_vPosDestination = "0 0 0";				//The destination where the chopper is flying from mission position
	private float m_fAngle = 0;
	private IEntity m_Vehicle;
	private vector m_vVehiclePosOld;
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Crashsite(SDRC_MissionRequested request)
	{
		//Set some defaults
		SetType(DC_EMissionType.CRASHSITE);

		//Load config
		m_CrashsiteJsonApi.Load();
		m_Config = m_CrashsiteJsonApi.conf;
		
		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		if (GetSubIdx() == -1)
		{
			SetState(DC_EMissionState.FAILED);
			return;
		}
		m_DC_Crashsite = m_Config.crashsites[GetSubIdx()];
		HandleRequestGeneralVariables(m_DC_Crashsite.general, request);
		
		//Find position
		vector pos = m_DC_Crashsite.general.pos[0];
		bool positionFound = false;

		if (pos != "0 0 0")
		{
			//If pos has been set, we blindly accept it
			positionFound = true;
		}			
		else
		{
			for (int i = 0; i < DC_LOCATION_SEACRH_ITERATIONS; i++)
			{
				if (pos == "0 0 0")
				{
					pos = SDRC_MissionHelper.FindMissionPos(m_Config.distanceToMission, m_Config.distanceToPlayer);
				}
				
				if (pos != "0 0 0")
				{
					positionFound = true;
					break;
				}
				else
				{						
					SDRC_Log.Add("[SDRC_Mission_Crashsite] Invalid mission position. Try " + (i + 1) + "/" + DC_LOCATION_SEACRH_ITERATIONS, LogLevel.SPAM);
				}
			}
		}

		//No suitable location found.
		if (!positionFound)	
		{				
			SDRC_Log.Add("[SDRC_Mission_Crashsite] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}	
		
		//Set common parameters
		if (positionFound)	
		{		
			pos[1] = pos[1] + Math.RandomInt(m_Config.flyHeight[0], m_Config.flyHeight[0]);	//Adjust flight height
			int rnd = SDRC_Misc.GetWorldSize()/8;
			m_vPosDestination[0] = SDRC_Misc.GetWorldSize()/2 + Math.RandomFloat(-rnd, rnd);
			m_vPosDestination[2] = SDRC_Misc.GetWorldSize()/2 + Math.RandomFloat(-rnd, rnd);
			
			vector direction = vector.Direction(pos, m_vPosDestination);
			m_fAngle = SDRC_Misc.VectorToAngle(direction);
			
			SDRC_Log.Add("[SDRC_Mission_Crashsite] Helicopter flying from " + pos + " to " + m_vPosDestination + ". Angle: " + m_fAngle, LogLevel.DEBUG);
		}			
		
		SetPos(pos, m_vPosDestination);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Crashsite.general.posName));
		SetMarker(m_Config.showMarker, m_Config.markerIdx, m_Config.markerType);
		SetHint(m_Config.showHint, m_DC_Crashsite.general.title, m_DC_Crashsite.general.info);		
		SetMessages(m_Config.showMessage, m_DC_Crashsite.general.winMessage, m_DC_Crashsite.general.loseMessage);		
		SetWinCondition(m_DC_Crashsite.general.winCondition);

		//Set a marker for destination
		if (!SDRC_Conf.RELEASE)
		{			
			SDRC_MapMarkerHelper.CreateMapMarker(m_vPosDestination, DC_EMissionIcon.ICON_DEATHMARKER_SMALL_RED_MAP, GetId() + "_1", "Destination");
			SDRC_DebugHelper.AddDebugPos(m_vPosDestination, ARGB(50, 255, 0, 0), 10, GetId() + "_1");
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == DC_EMissionState.SPAWN)
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
							//SDRC_MapMarkerHelper.CreateMapMarker(GetPos(), DC_EMissionIcon.GM_MISSION_CRASHSITE_MAP, GetId(), "Crash site");
							SetMarker(m_Config.showMarker, DC_EMissionIcon.GM_MISSION_CRASHSITE_MAP, m_Config.markerType);
							ShowMarker();
						}
						missionCrashSiteState = DC_EMissionCrashSiteState.SPAWN_SITE;
					}
					else
					{
						MoveMarker();
/*						if (m_Config.showMarker)
						{						
							SDRC_MapMarkerHelper.DeleteMarker(GetId());
							//SDRC_MapMarkerHelper.CreateMapMarker(GetPos(), DC_EMissionIcon.GM_MISSION_HELICOPTER_MAP, GetId(), "");
							SetMarker(m_Config.showMarker, DC_EMissionIcon.GM_MISSION_HELICOPTER_MAP, m_Config.markerType);
							ShowMarker();
						}*/
					}
					break;
				case DC_EMissionCrashSiteState.SPAWN_SITE:
					IEntity entity;
				
					SDRC_SpawnHelper.SetStructuresToOrigo(m_DC_Crashsite.siteItems);
				
					float rotation = Math.RandomFloat(0, 360);			
					int i = 0;
				
					foreach (ref SDRC_Structure item : m_DC_Crashsite.siteItems)
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
					SCR_AIGroup group = SDRC_MissionHelper.SpawnMissionAIGroup(m_DC_Crashsite.groupTypes.GetRandomElement(), GetPos(), GetFaction());				
					if (group)
					{
						SDRC_AIHelper.SetAIGroupSkill(group, m_DC_Crashsite.aiSkill, m_DC_Crashsite.aiPerception);					
						m_Groups.Insert(group);
						SDRC_WPHelper.CreateMissionAIWaypoints(group, DC_EWaypointGenerationType.LOITER, GetPos(), "0 0 0", DC_EWaypointMoveType.LOITER);
					}
					SDRC_Log.Add("[SDRC_Mission_Crashsite:MissionSpawn] AI groups spawned ", LogLevel.DEBUG);								
					missionCrashSiteState = DC_EMissionCrashSiteState.RUN;
					SetState(DC_EMissionState.ACTIVE);		//Set the ACTIVE state again to properly get correct AI count.
					break;
				case DC_EMissionCrashSiteState.RUN:		
					if (!IsActive())
					{
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
	ref array<ref SDRC_Crashsite> crashsites = {};		//List of crashsites
}

//------------------------------------------------------------------------------------------------
class SDRC_Crashsite : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref array<string> groupTypes = {};
	int aiSkill;
	float aiPerception;
	ref array<ref SDRC_HelicopterInfo> helicopterInfo = {};
	//Optional settings
	ref SDRC_Loot loot = null;
	ref array<ref SDRC_Structure> siteItems = {};
	
	void Set(array<string> groupTypes_, int aiSkill_, float aiPerception_)
	{
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
		conf.markerIdx = DC_EMissionIcon.GM_MISSION_HELICOPTER_MAP;
		conf.missionList = {0,1};		
		//Mission specific
		conf.distanceToMission = 100;
		conf.distanceToPlayer = 500;
		conf.flyHeight = {80, 120};
		//----------------------------------------------------
		conf.crashsites.Insert(Crashsite0());
		conf.crashsites.Insert(Crashsite1());
	};
			
	//----------------------------------------------------
	SDRC_Crashsite Crashsite0()
	{
		ref SDRC_Crashsite crashsite = new SDRC_Crashsite();
		crashsite.general.Set(
			0, "index 0: general mission",
			{"0 0 0"},
			"any",
			"Helicopter in distress",
			"A valuable cargo has crashed.",
			DC_EMissionWinCondition.AI_KILL_75,
			"The loot was salvaged. Crash, burn, loot.",
			"No loot for you today.", 
			"",
			0
		);
		crashsite.Set
		(
			{
				"G_LIGHT", "G_ADMIN"
			},
			20, 0.8
		);
		
		//----------------------------------------------------
		ref SDRC_HelicopterInfo heli00 = new SDRC_HelicopterInfo();
			heli00.Set("{40A3EEECFF765793}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_flying.et", 0.8, 0.8, 1.0);
			crashsite.helicopterInfo.Insert(heli00);
		ref SDRC_HelicopterInfo heli01 = new SDRC_HelicopterInfo();
			heli01.Set("{6D71309125B8AEA2}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying.et",	0.7, 0.9, 1.0);
			crashsite.helicopterInfo.Insert(heli01);
		ref SDRC_HelicopterInfo heli02 = new SDRC_HelicopterInfo();
			heli02.Set("{40A3EEECFF765793}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_flying.et",	0.8, 0.8, -1.0);
			crashsite.helicopterInfo.Insert(heli02);		
		
		//----------------------------------------------------
		ref SDRC_Loot crashloot0 = new SDRC_Loot();
		array<string> lootItems0 = {
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			};
		crashloot0.Set(0.9, lootItems0);
		crashsite.loot = crashloot0;
				
		//----------------------------------------------------
		ref SDRC_Structure crashitem0 = new SDRC_Structure();
		crashitem0.Set(
			"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
			"97.911 1 121.527"
		);
		crashsite.siteItems.Insert(crashitem0);

		ref SDRC_Structure crashitem2 = new SDRC_Structure();
		crashitem2.Set(
			"{0542578CA422287A}PrefabsEditable/Auto/Props/Industrial/Repair/E_VehicleGarbage_01_pile_medium.et",
			"106.274 1 121.108"
		);
		crashsite.siteItems.Insert(crashitem2);

		ref SDRC_Structure crashitem3 = new SDRC_Structure();
		crashitem3.Set(
			"{310E849A808F9F5F}PrefabsEditable/Auto/Structures/Military/Camps/Canvas_Covers/US/E_CanvasCover_Folded_US.et",
			"104.552 1 126.903"
		);
		crashsite.siteItems.Insert(crashitem3);

		ref SDRC_Structure crashitem4 = new SDRC_Structure();
		crashitem4.Set(
			"{532795AD51CFBEDF}PrefabsEditable/Auto/Structures/Infrastructure/Piping/E_DieselPipe_01_hose_V2.et",
			"104.745 1 123.685"
		);
		crashsite.siteItems.Insert(crashitem4);

		ref SDRC_Structure crashitem5 = new SDRC_Structure();
		crashitem5.Set(
			"{7576CB87CAFAE6E8}PrefabsEditable/Auto/Structures/Military/CamoNets/US/E_CamoNet_AssemblyKit_US.et",
			"103.559 1 119.949",
			"0 -45.003 0"
		);
		crashsite.siteItems.Insert(crashitem5);
		
		return crashsite;
	}
			
	//----------------------------------------------------
	SDRC_Crashsite Crashsite1()
	{
		ref SDRC_Crashsite crashsite = new SDRC_Crashsite();
		crashsite.general.Set(
			1, "index 1: general mission",
			{"0 0 0"},
			"any",
			"Engine damage",
			"May day, may day! We're going down.",
			DC_EMissionWinCondition.AI_KILL_75,
			"The loot box was secured.",
			"The cargo was lost.", 
			"",
			0
		);
		crashsite.Set
		(
			{
				"G_HEAVY", "G_ADMIN"
			},
			20, 0.8
		);
		
		//----------------------------------------------------
		ref SDRC_HelicopterInfo heli10 = new SDRC_HelicopterInfo();
			heli10.Set("{40A3EEECFF765793}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_flying.et", 0.7, 0.8, 1.0);
			crashsite.helicopterInfo.Insert(heli10);
		ref SDRC_HelicopterInfo heli11 = new SDRC_HelicopterInfo();
			heli11.Set("{6D71309125B8AEA2}Prefabs/Vehicles/Helicopters/UH1H/UH1H_Flying.et",	0.6, 0.85, 1.0);
			crashsite.helicopterInfo.Insert(heli11);
		ref SDRC_HelicopterInfo heli12 = new SDRC_HelicopterInfo();
			heli12.Set("{40A3EEECFF765793}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_unarmed_transport_flying.et",	0.75, 0.75, -1.0);
			crashsite.helicopterInfo.Insert(heli12);		
		
		//----------------------------------------------------
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems1 = {
				"WEAPON_LAUNCHER", 
				"UTIL_OPTIC", 
				"WEAPON_RIFLE", "WEAPON_RIFLE", "WEAPON_RIFLE", 
				"UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", 
			};
		loot.Set(0.7, lootItems1);
		crashsite.loot = loot;
				
		//----------------------------------------------------
		ref SDRC_Structure crashitem_0 = new SDRC_Structure();
		crashitem_0.Set(
			"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
			"97.911 1 121.527"
		);
		crashsite.siteItems.Insert(crashitem_0);

		ref SDRC_Structure crashitem_1 = new SDRC_Structure();
		crashitem_1.Set(
			"{0542578CA422287A}PrefabsEditable/Auto/Props/Industrial/Repair/E_VehicleGarbage_01_pile_medium.et",
			"106.274 1 121.108"
		);
		crashsite.siteItems.Insert(crashitem_1);	
		return crashsite;
	}
}