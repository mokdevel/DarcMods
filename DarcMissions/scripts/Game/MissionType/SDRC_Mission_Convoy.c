//Mission SDRC_Mission_Convoy.c

//------------------------------------------------------------------------------------------------
/*!
A convoy traveling from A to B.

Note to self: RADIUS, SCATTERED : This could also work, but support was removed as unnecessary. The concoy would follow a path created with waypointRange starting from posStart. posDestination is ignored.
*/

//------------------------------------------------------------------------------------------------
enum SDRC_EMissionConvoyState
{
	SPAWN,
	INIT,
	MOVE_AI,
	GETIN,
	RUN
};

const string DC_MISSIONCONFIG_FILE_CONVOY = "dc_missionConfig_Convoy.json";
const int DC_MISSIONCONFIG_FILE_CONVOY_JSONVER = 2;
	
//------------------------------------------------------------------------------------------------
class SDRC_Mission_Convoy : SDRC_Mission
{
	private ref SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_CONVOY);	
	private ref SDRC_ConvoyConfig m_Config = new SDRC_ConvoyConfig();
	private ref SDRC_Convoy m_DC_Convoy = new SDRC_Convoy();
	
	private SDRC_EMissionConvoyState missionConvoyState = SDRC_EMissionConvoyState.SPAWN;
	
	private vector m_vPosDestination = "1 1 1";
	private IEntity m_Vehicle = null;
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Convoy(SDRC_EMissionType missionType, SDRC_MissionRequested request, bool staticMission = false)
	{
		//Load config
		if (!m_JsonApi.Load(m_Config, SDRC_MissionConfig.Cast(m_Config), DC_MISSIONCONFIG_FILE_CONVOY_JSONVER))
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.ERROR_LOADING_JSON);
			return;
		}
		m_Config.LoadMissionFiles(DC_MISSIONCONFIG_FILE_CONVOY_JSONVER);

		//Pick a configuration for mission
		SetSubIdx(SDRC_MissionHelper.SelectMissionIndex(m_Config.missionList, GetSubIdx()));
		int idx = m_Config.GetSubMissionIdx(GetSubIdx());
		if (idx == -1)
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.WRONG_SUBIDX);
			return;
		}
		m_DC_Convoy = m_Config.subMissions[idx];		
		HandleRequestGeneralVariables(m_DC_Convoy.general, request);
		
		//Find a location for the mission
		vector pos = "0 0 0";
		m_vPosDestination = m_DC_Convoy.general.pos[1];		//Destination from the defined SDRC_Convoy 
		
		//For requested missions we want have it as close as possible in the requested place.
		if (IsRequested())
		{
			pos = request.general.pos[0];
			m_vPosDestination = request.general.pos[1];
		}
		else
		{				
			pos = SDRC_MissionHelper.SelectMissionPos(m_DC_Convoy.general.pos, m_DC_Convoy.general.size, m_DC_Convoy.general.locationTypes);
		}

		//Find nearest road
		SDRC_RoadPos roadPosStart = new SDRC_RoadPos();				
		pos = SDRC_RoadHelper.FindClosestRoadposToPos(roadPosStart, pos, 1000);
		
		//If pos has been set, we blindly accept it. Do basic checking for pos.
		if (SDRC_MissionPosHelper.IsValidMissionPos(pos, onlyBasicChecks: (IsRequested() || IsStatic()), ignoreNonValidArea: IsRequested()) != SDRC_EMissionError.NONE)
		{
			pos = "0 0 0";
		}

/*		//If failed, stop
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.LOCATION_NOT_FOUND);
			return;
		}	*/
		
		//If failed, stop
		if (pos == "0 0 0")
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.ROAD_FOR_START_NOT_FOUND);
			return;
		}
	
		//Find a location for the destination
		if (m_vPosDestination == "0 0 0" && pos != "0 0 0")
		{
			m_vPosDestination = SDRC_MissionHelper.FindMissionDestination(m_DC_Convoy.general.locationTypes, pos, 500);
			if (m_vPosDestination != "0 0 0")
			{
				SDRC_RoadPos roadPos = new SDRC_RoadPos();
				m_vPosDestination = SDRC_RoadHelper.FindClosestRoadposToPos(roadPos, m_vPosDestination, 1000);
				if (m_vPosDestination == "0 0 0")
				{
					SDRC_Log.Add("[SDRC_Mission_Convoy] " +  GetId() + " : No destination road found.", LogLevel.ERROR);
				}
			}			
			else
			{
				SDRC_Log.Add("[SDRC_Mission_Convoy] " +  GetId() + " : Could not find destination location for ROUTE.", LogLevel.WARNING);
			}
		}		

		if (pos == "0 0 0" || m_vPosDestination == "0 0 0")	//No suitable location found.
		{				
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.LOCATION_NOT_FOUND);
			return;
		}	
		
		SetPos(pos, m_vPosDestination);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Convoy.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_DC_Convoy.general);		
		SetActiveDistance(m_Config.distanceToPlayer);				//Change the m_ActiveDistance to a mission specific one.
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == SDRC_EMissionState.SPAWN)
		{
			switch (missionConvoyState)
			{
				case SDRC_EMissionConvoyState.SPAWN:
					MissionSpawn();
					missionConvoyState = SDRC_EMissionConvoyState.INIT;
					break;
				case SDRC_EMissionConvoyState.INIT:
					//This state is mainly for delay to give time to vehicle and AI to finalize spawn. If removed, AI will not enter the vehicle.
					missionConvoyState = SDRC_EMissionConvoyState.MOVE_AI;
					break;
				case SDRC_EMissionConvoyState.MOVE_AI:
//					SDRC_VehicleHelper.MoveGroupsInVehicle(m_Groups, m_Vehicle);				
					foreach (SCR_AIGroup group : m_Groups)
					{	
						if (group)
						{
							SDRC_WPHelper.CreateMissionAIWaypoints(group, m_DC_Convoy.ai.waypointGenType, GetPos(), m_vPosDestination, m_DC_Convoy.ai.waypointMoveType);
						}
					}
					missionConvoyState = SDRC_EMissionConvoyState.GETIN;
					break;
				case SDRC_EMissionConvoyState.GETIN:
					//This state is mainly for delay to give AI time to climb to the vehicle.
					missionConvoyState = SDRC_EMissionConvoyState.RUN;
					break;			
				case SDRC_EMissionConvoyState.RUN:
					SetState(SDRC_EMissionState.ACTIVE);
					break;
			}
		}

		if (GetState() == SDRC_EMissionState.END)
		{
			MissionEnd();
			SetState(SDRC_EMissionState.EXIT);
		}	
				
		if (GetState() == SDRC_EMissionState.ACTIVE)
		{			
			//Move the position as the convoy is moving. This way check for player distance works properly.
			//If players have already stolen the vehicle, the map marker will stop moving.
			if ( (m_Vehicle) && (m_EntityList.Count() > 0) )
			{
				SetPos(m_Vehicle.GetOrigin());
				SDRC_DebugHelper.MoveDebugPos(GetId(), GetPos());
				MoveMarker();
			}
						
			if (!IsActive())
			{
				SetState(SDRC_EMissionState.END);
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
		//Spawn vehicle					
		string resourceName	= m_DC_Convoy.vehicleTypes.GetRandomElement();
		m_Vehicle = SDRC_SpawnHelper.SpawnItem(GetPos(), resourceName, m_DC_Convoy.general.size);
		
		if (!m_Vehicle)
		{
			//Could not spawn vehicle
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.COULD_NOT_SPAWN_VEHICLE, resourceName);
			return;			
		}
		
		SDRC_Log.Add("[SDRC_Mission_Convoy:MissionSpawn] " +  GetId() + " : Vehicle spawned: " + m_Vehicle, LogLevel.DEBUG);										
		
		m_EntityList.Insert(m_Vehicle);
		
		//Disable arsenal
		SDRC_VehicleHelper.EmptyStorage(m_Vehicle);
		SDRC_VehicleHelper.DisableVehicleArsenal(m_Vehicle, resourceName, m_Config.disableArsenal);
		
		AICarMovementComponent vehicle_c = AICarMovementComponent.Cast(m_Vehicle.FindComponent(AICarMovementComponent));
        vehicle_c.SetCruiseSpeed(m_DC_Convoy.cruiseSpeed);
		
		//----------------------------------------------------------------------
		//OLD WORKING CODE: Spawns AI first on ground and then move to vehicle.
		//NOTE: If you use this, uncomment the line: SDRC_VehicleHelper.MoveGroupsInVehicle(m_Groups, m_Vehicle);
		
/*		//Spawn mission AI
		int aiCount = m_DC_Convoy.ai.GetCount(m_DC_Convoy.general.difficulty);
		vector posg = GetPos();
		//Move the position slightly to avoid spawning under vehicle
		posg[0] = posg[0] + m_DC_Convoy.general.size;
		posg[2] = posg[2] + m_DC_Convoy.general.size;
		
		for (int i = 0; i < aiCount; i++)
		{		
			SCR_AIGroup group = SDRC_AIHelper.SpawnGroup(m_DC_Convoy.ai.types.GetRandomElement(), posg, GetFaction());
			if (group)
			{			
				SDRC_AIHelper.SetAIGroupSettings(group, m_DC_Convoy.ai.GetSkill(m_DC_Convoy.general.difficulty), m_DC_Convoy.ai.GetPerception(m_DC_Convoy.general.difficulty));
				m_Groups.Insert(group);					
			}
			
			posg[0] = posg[0] + 4;
		}*/
		
		//----------------------------------------------------------------------
		//TBD: NEW CODE: Spawns AI straight to the vehicle. Unfinished.
		
		//Spawn mission AI
		int aiCount = m_DC_Convoy.ai.GetCount(GetDifficulty());
		
		for (int i = 0; i < aiCount; i++)
		{		
			string groupToSpawn = m_DC_Convoy.ai.types.GetRandomElement();
			ResourceName aiType = SDRC_EnemyHelper.SelectEnemy(groupToSpawn, GetFaction());
			
//			SCR_AIGroup group = SDRC_AIHelper.GroupCreate(GetFaction(), GetPos(), aiType);
			SCR_AIGroup group = SDRC_AIHelper.GroupCreate(GetFaction(), GetPos());
			SDRC_VehicleHelper.SpawnGroupInVehicle(aiType, m_Vehicle, group, GetFaction());
			
			if (group)
			{			
				SDRC_AIHelper.SetAIGroupSettings(group, m_DC_Convoy.ai.GetSkill(GetDifficulty()), m_DC_Convoy.ai.GetPerception(GetDifficulty()));
				m_Groups.Insert(group);					
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	override void DoWin()
	{	
		//Put loot
		if (m_DC_Convoy.loot)			
		{
			m_DC_Convoy.loot.box = m_Vehicle;
			SDRC_MissionHelper.AddLoot(m_DC_Convoy.loot.box, m_DC_Convoy.loot.items, m_DC_Convoy.loot.itemChance, GetDifficulty());
			SDRC_Log.Add("[SDRC_Mission_Convoy:DoWin] " +  GetId() + " : Loot added.", LogLevel.DEBUG);								
		}		
		super.DoWin();
	}
}
	
//------------------------------------------------------------------------------------------------
class SDRC_ConvoyConfig : SDRC_MissionConfig
{
	//Mission specific
	int distanceToPlayer;							//If no players this close to any players and patrolingTime has passed, despawn mission.
	ref array<ref SDRC_Convoy> subMissions = {};	//List of convoys
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_ConvoyConfig data = SDRC_ConvoyConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------	
	override void LoadMissionFiles(int ver)
	{
		//Load mission files
		foreach (string missionFile : missionFiles)
		{
			SDRC_JsonApi2 jsonApi = new SDRC_JsonApi2(missionFile);
			SDRC_ConvoyConfig conf = new SDRC_ConvoyConfig();
			
			if (jsonApi.Load(conf, SDRC_MissionConfig.Cast(conf), ver, false))
			{
				foreach (SDRC_Convoy subMission : conf.subMissions)
				{
					subMissions.Insert(subMission);
				}
				foreach (int idx : conf.missionList)
				{
					missionList.Insert(idx);
				}
			}
		}
	}	
		
	//------------------------------------------------------------------------------------------------
	override void CreateMissionFiles()
	{
		super.CreateMissionFiles();
	}	
	
	//------------------------------------------------------------------------------------------------
	int GetSubMissionIdx(int subIdx)
	{
		int idx = -1;
		foreach (int i, SDRC_Convoy subMission : subMissions)
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
		missionType = SDRC_EMissionType.CONVOY;
		
		//Default
		disableArsenal = true;
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		missionList = {0,0,0,0,0,0,1,1,1,1,1,2,3,3,};
		//Mission specific
		distanceToPlayer = 500;
		//----------------------------------------------------
		subMissions.Insert(Convoy0());				
		subMissions.Insert(Convoy1());				
		subMissions.Insert(Convoy2());
		subMissions.Insert(Convoy3());
	}
		
	//----------------------------------------------------
	SDRC_Convoy Convoy0()
	{
		ref SDRC_Convoy convoy = new SDRC_Convoy();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Convoy is on the move.",
			"Leaked travel plans show a route from %l to %d",
			"The convoy was successfully intercepted.",
			"The convoy reached %d as planned.",);
		convoy.general.Set(
			0, "index 0: Convoy driving from .. to ..",
			{"0 0 0", "0 0 0"}, 5,
			{
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_VALLEY,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_HOSPITAL,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_AIRPORT
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_CONVOY_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		convoy.ai.Set(
			{1, 2},
			{"G_LIGHT", "G_MEDICAL"},
//			{3, 6},
//			{"C_OFFICER"},
			50, 1.0,
			{0, 0},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
		convoy.Set(
			{
				"{01F65EFB8D767A91}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_cargo.et",
				"{543799AC5C52989C}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_beige.et",
				"{259EE7B78C51B624}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469.et",
				"{D4855501D5B12AF2}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_uncovered_CIV_teal.et"
			},
			30
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", 
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", 
				"GEAR_BAG", 
				"CLOTHING_HEADGEAR", "CLOTHING_UNIFORM",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			};
		loot.Set(0.9, lootItems);
		convoy.loot = loot;			
	
		return convoy;
	}
	
	//----------------------------------------------------
	SDRC_Convoy Convoy1()
	{
		ref SDRC_Convoy convoy = new SDRC_Convoy();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Cargo truck is on the move.",
			"Follow the route from %l to %d",
			"Truck stopped, loot grabbed.",
			"All the goodies in the truck was never for you.",);
		convoy.general.Set(
			1, "index 1: Truck driving from .. to ..",
			{"0 0 0", "0 0 0"}, 7,
			{
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_VALLEY,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_FUELSTATION,
				EMapDescriptorType.MDT_PARKING,
				EMapDescriptorType.MDT_HOSPITAL,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_AIRPORT
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_CONVOY_MAP,		
			{SDRC_EDifficulty.RANDOM},
			0
		);
		convoy.ai.Set(
			{2, 3},
			{"G_RECON", "G_LIGHT"},
			40, 1.0,
			{0, 0},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
		convoy.Set(
			{
				"{F1FBD0972FA5FE09}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport.et",
				"{81FDAD5EB644CC3D}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport_covered.et",
				"{48A6D4372444B85A}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport_covered_closed.et", 
				"{16E32C3ABEAFC2C6}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_FIA_transport.et",
				"{1449105FD658EDFB}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_CIV_forest.et",
				"{FB219B49A448A8EA}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_covered_CIV_JZD.et",
				"{66241E0CEDFCEDFF}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_covered_CIV_orange.et",
				"{F66EAD0D2016B6BA}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_covered_CIV_blue.et",
			},
			20
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", "WEAPON_RIFLE", "WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", 
				"WEAPON_LAUNCHER",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", 
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_BAG", 
				"CLOTHING_HEADGEAR", "CLOTHING_UNIFORM",
			};
		loot.Set(0.9, lootItems);
		convoy.loot = loot;			
		
		return convoy;
	}
	
	//----------------------------------------------------
	SDRC_Convoy Convoy2()
	{
		ref SDRC_Convoy convoy = new SDRC_Convoy();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Armor spotted",
			"It's been seen in %l. It's to patrol to %d.",
			"Armor destroyed. Well done!",
			"Were you scared of a piece metal? Cowards!",); 
		convoy.general.Set(
			2, "index 2: Armor driving from .. to ..",
			{"0 0 0", "0 0 0"}, 7,
			{
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_VALLEY,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_FUELSTATION,
				EMapDescriptorType.MDT_PARKING,
				EMapDescriptorType.MDT_HOSPITAL,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_AIRPORT
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_CONVOY_MAP,		
			{SDRC_EDifficulty.RANDOM},
			0
		);
		convoy.ai.Set(
			{2, 3},
			{"G_HEAVY", "G_SPECIAL"},
			50, 1.0,
			{0, 0},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
		convoy.Set(
			{
				"{0FBF8F010F81A4E5}Prefabs/Vehicles/Wheeled/LAV25/LAV25.et",
				"{C012BB3488BEA0C2}Prefabs/Vehicles/Wheeled/BTR70/BTR70.et",
				"{254289B9C09904AB}Prefabs/Vehicles/Wheeled/BRDM2/BRDM2.et",
			},
			20
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", "WEAPON_RIFLE_BIG", 
				"WEAPON_HANDGUN", 
				"WEAPON_LAUNCHER", 
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", 
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_BAG", "GEAR_BAG", 
			};
		loot.Set(0.7, lootItems);
		convoy.loot = loot;

		return convoy;	
	}
			
	//----------------------------------------------------
	SDRC_Convoy Convoy3()
	{
		ref SDRC_Convoy convoy = new SDRC_Convoy();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Guns on the move",
			"Look out for a patrol between %l and %d.",
			"Nice shooting!",
			"Oh dear, your failure will be remembered.",); 
		convoy.general.Set(
			3, "index 3: Vehicle with a gun driving from .. to ..",
			{"0 0 0", "0 0 0"}, 4,
			{
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_VALLEY,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_FUELSTATION,
				EMapDescriptorType.MDT_PARKING,
				EMapDescriptorType.MDT_HOSPITAL,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_AIRPORT
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_CONVOY_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		convoy.ai.Set(
			{1, 2},
			{"G_RECON", "G_MEDICAL", "G_LIGHT"},
			50, 1.0,
			{0, 0},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
		convoy.Set(
			{
				"{F6B23D17D5067C11}Prefabs/Vehicles/Wheeled/M151A2/M151A2_M2HB.et",
				"{5168FEA3054D6D15}Prefabs/Vehicles/Wheeled/M151A2/M151A2_M2HB_MERDC.et",
				"{3EA6F47D95867114}Prefabs/Vehicles/Wheeled/M998/M1025_armed_M2HB.et",
				"{DD774A8FD0989A78}Prefabs/Vehicles/Wheeled/M998/M1025_armed_M2HB_MERDC.et",
			},
			20
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", 
				"WEAPON_HANDGUN", 
				"WEAPON_GRENADE", "WEAPON_GRENADE", 
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
			};
		loot.Set(0.9, lootItems);
		convoy.loot = loot;		
				
		return convoy;	
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_Convoy : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();	
	ref SDRC_MissionConfigQrf qrf = null;
	//Mission specific
	ref array<string> vehicleTypes = {};
	float cruiseSpeed;						//Speed to drive in km/h.
	//Optional settings
	ref SDRC_Loot loot = null;	
	
	void Set(array<string> vehicleTypes_, float cruiseSpeed_)
	{
		vehicleTypes = vehicleTypes_;
		cruiseSpeed = cruiseSpeed_;
	}
}