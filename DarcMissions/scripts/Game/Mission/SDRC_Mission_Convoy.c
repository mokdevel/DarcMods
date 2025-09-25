//Mission SDRC_Mission_Convoy.c

//------------------------------------------------------------------------------------------------
/*!
A convoy traveling from A to B.

Note to self: RADIUS, SCATTERED : This could also work, but support was removed as unnecessary. The concoy would follow a path created with waypointRange starting from posStart. posDestination is ignored.
*/

const string DC_MISSIONCONFIG_FILE_CONVOY = "dc_missionConfig_Convoy.json";
	
//------------------------------------------------------------------------------------------------
enum SDRC_EMissionConvoyState
{
	INIT,
	MOVE_AI,
	RUN
};

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Convoy : SDRC_Mission
{
	private ref SDRC_ConvoyJsonApi m_ConvoyJsonApi = new SDRC_ConvoyJsonApi(DC_MISSIONCONFIG_FILE_CONVOY);	
	private ref SDRC_ConvoyConfig m_Config = new SDRC_ConvoyConfig();
	private ref SDRC_Convoy m_DC_Convoy = new SDRC_Convoy();
	
	private SDRC_EMissionConvoyState missionConvoyState = SDRC_EMissionConvoyState.INIT;	
	
	private vector m_vPosDestination = "1 1 1";
	private IEntity m_Vehicle = null;
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Convoy(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Load config
		m_ConvoyJsonApi.CreateMissionFiles();
		m_ConvoyJsonApi.Load();
		m_ConvoyJsonApi.LoadMissionFiles();
		m_Config = m_ConvoyJsonApi.conf;

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
		vector pos = m_DC_Convoy.general.pos[0];
		m_vPosDestination = m_DC_Convoy.general.pos[1];		//Destination from the defined SDRC_Convoy 
		
		//For requested missions we want have it as close as possible in the requested place.
		int randomPos = -1;		
		if (IsRequested())
		{
			randomPos = 0;
		}
		
		if (pos == "0 0 0")
		{
			pos = SDRC_MissionHelper.FindMissionPos(m_DC_Convoy.locationTypes, 5, randomPos);
		}

		//If failed, stop
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.LOCATION_NOT_FOUND);
			return;
		}	
		
		//Find nearest road
		SDRC_RoadPos roadPosStart = new SDRC_RoadPos();				
		pos = SDRC_RoadHelper.FindClosestRoadposToPos(roadPosStart, pos, 1000);
		
		//If failed, stop
		if (pos == "0 0 0")
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.ROAD_FOR_START_NOT_FOUND);
			return;
		}
	
		//Find a location for the destination
		if (m_vPosDestination == "0 0 0" && pos != "0 0 0")
		{
			m_vPosDestination = SDRC_MissionHelper.FindMissionDestination(m_DC_Convoy.locationTypes, pos, 500);
			if (m_vPosDestination != "0 0 0")
			{
				SDRC_RoadPos roadPos = new SDRC_RoadPos();
				m_vPosDestination = SDRC_RoadHelper.FindClosestRoadposToPos(roadPos, m_vPosDestination, 1000);
				if (m_vPosDestination == "0 0 0")
				{
					SDRC_Log.Add("[SDRC_Mission_Convoy] No destination road found.", LogLevel.ERROR);
				}
			}			
			else
			{
				SDRC_Log.Add("[SDRC_Mission_Convoy] Could not find destination location for ROUTE.", LogLevel.WARNING);
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
/*		SetMarker(m_Config.showMarker, m_DC_Convoy.general.markerIcon, m_DC_Convoy.general.markerType);
		SetHint(m_Config.showHint, m_DC_Convoy.general.title, m_DC_Convoy.general.info);		
		SetMessages(m_Config.showMessage, m_DC_Convoy.general.winMessage, m_DC_Convoy.general.loseMessage);		
		SetWinCondition(m_DC_Convoy.general.winCondition);*/
		SetActiveDistance(m_Config.distanceToPlayer);				//Change the m_ActiveDistance to a mission specific one.
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == SDRC_EMissionState.SPAWN)
		{
			MissionSpawn();
		}

		if (GetState() == SDRC_EMissionState.END)
		{
			MissionEnd();
			SetState(SDRC_EMissionState.EXIT);
		}	
				
		if (GetState() == SDRC_EMissionState.ACTIVE)
		{			
			switch (missionConvoyState)
			{
				case SDRC_EMissionConvoyState.INIT:
					//This state is mainly for delay to give vehicle and AI to finalize spawn. If removed, AI will not enter the vehicle.
					missionConvoyState = SDRC_EMissionConvoyState.MOVE_AI;
					break;
				case SDRC_EMissionConvoyState.MOVE_AI:				
					MoveGroupsInVehicle(m_Groups, m_Vehicle);				
					foreach (SCR_AIGroup group : m_Groups)
					{	
						if (group)
						{
							SDRC_WPHelper.CreateMissionAIWaypoints(group, m_DC_Convoy.ai.waypointGenType, GetPos(), m_vPosDestination, m_DC_Convoy.ai.waypointMoveType);
						}
					}
					missionConvoyState = SDRC_EMissionConvoyState.RUN;
					break;
				case SDRC_EMissionConvoyState.RUN:
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
					break;
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
		m_Vehicle = SDRC_SpawnHelper.SpawnItem(GetPos(), resourceName);
		
		if (!m_Vehicle)
		{
			//Could not spawn vehicle
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.COULD_NOT_SPAWN_VEHICLE, resourceName);
			return;			
		}
		
		m_EntityList.Insert(m_Vehicle);
		
		//Disable arsenal
		SDRC_SpawnHelper.DisableVehicleArsenal(m_Vehicle, resourceName, m_Config.disableArsenal);
		
		AICarMovementComponent vehicle_c = AICarMovementComponent.Cast(m_Vehicle.FindComponent(AICarMovementComponent));
        vehicle_c.SetCruiseSpeed(m_DC_Convoy.cruiseSpeed);

		//Spawn mission AI 
		int aiCount = m_DC_Convoy.ai.GetCount(m_DC_Convoy.general.difficulty);
		vector posg = GetPos() + "3 0 3";
		
		for (int i = 0; i < aiCount; i++)
		{		
			SCR_AIGroup group = SDRC_AIHelper.SpawnGroup(m_DC_Convoy.ai.types.GetRandomElement(), posg, GetFaction());
			if (group)
			{			
				SDRC_AIHelper.SetAIGroupSkill(group, m_DC_Convoy.ai.GetSkill(m_DC_Convoy.general.difficulty), m_DC_Convoy.ai.GetPerception(m_DC_Convoy.general.difficulty));
				m_Groups.Insert(group);					
			}
			
			posg[0] = posg[0] + 3;
		}
		
		//Put loot
		if (m_DC_Convoy.loot)			
		{
			m_DC_Convoy.loot.box = m_Vehicle;
			SDRC_LootHelper.SpawnItemsToStorage(m_DC_Convoy.loot.box, m_DC_Convoy.loot.items, m_DC_Convoy.loot.itemChance);
			SDRC_Log.Add("[SDRC_Mission_Convoy:MissionSpawn] Loot added.", LogLevel.DEBUG);								
		}		
		
		SetState(SDRC_EMissionState.ACTIVE);		
	}

	//------------------------------------------------------------------------------------------------
    private void MoveGroupsInVehicle(array<SCR_AIGroup> groups, IEntity vehicle)
    {
		array<AIAgent> groupMembers  = new array<AIAgent>;
		
		int i = 0;
		
		foreach (SCR_AIGroup group : groups)
		{				
			if (group)
			{
				group.GetAgents(groupMembers);
				
				foreach (AIAgent aiAgent: groupMembers)
				{
					bool success = MoveEntityInVehicle(aiAgent, vehicle, i);
					
					//Remove those AI that did not fit in the vehicle.
					if (!success)
					{
						SDRC_AIHelper.RemoveAIAgent(aiAgent);
					}
					
					i++;
				}
			}
		}
	}
			
	//------------------------------------------------------------------------------------------------
    private void MoveGroupInVehicle(AIGroup group, IEntity vehicle)
    {
		array<AIAgent> groupMembers  = new array<AIAgent>;
		
		if (group)
		{
			group.GetAgents(groupMembers);
			
			int i = 0;
			foreach (AIAgent aiAgent: groupMembers)
			{
				bool success = MoveEntityInVehicle(aiAgent, vehicle, i);
				
				//Remove those AI that did not fit in the vehicle.
				if (!success)
				{
					SDRC_AIHelper.RemoveAIAgent(aiAgent);
				}
				
				i++;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
    bool MoveEntityInVehicle(AIAgent aiAgent, IEntity vehicle, int slotIdx)
    {
		BaseCompartmentManagerComponent compartmentManager = BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(BaseCompartmentManagerComponent));
		array<BaseCompartmentSlot> compartments = {};
		int slots = compartmentManager.GetCompartments(compartments);		
		
		if (slotIdx >= slots)
		{
			SDRC_Log.Add("[SDRC_Mission_Convoy:MoveEntityInVehicle] slotIdx incorrect: " + slotIdx + "/" + slots, LogLevel.SPAM);
			return false;			
		}
		
		array<int> slotPrio = {};
		slotPrio.Insert(-1);		//Reserve prio 0 slot for pilot
		slotPrio.Insert(-1);		//Reserve prio 0 slot for pilot
		
		int i = 0;
						
		foreach (BaseCompartmentSlot slot : compartments)
		{
			bool found = false;
			
			if (PilotCompartmentSlot.Cast(slot))
			{
				SDRC_Log.Add("[SDRC_Mission_Convoy:MoveEntityInVehicle] Pilot slot: " + slot, LogLevel.SPAM);
				if (slotPrio[0] == -1)
				{
					slotPrio[0] = i;
					found = true;
				}
			}			
			else if (TurretCompartmentSlot.Cast(slot))
			{
				SDRC_Log.Add("[SDRC_Mission_Convoy:MoveEntityInVehicle] Turret slot: " + slot, LogLevel.SPAM);
				if (slotPrio[1] == -1)
				{
					slotPrio[1] = i;
					found = true;
				}
			}
			
			if (!found)
			{
				slotPrio.Insert(i);
			}
			
			i++;
		}
			
		int idx = slotPrio[slotIdx];
		if (idx == -1)
		{
			idx = slotIdx;
		}
		bool success = SetEntityInSlot(aiAgent, vehicle, compartments[idx]);

		return success;
    }
	
	//------------------------------------------------------------------------------------------------
	bool SetEntityInSlot(AIAgent aiAgent, IEntity vehicle, BaseCompartmentSlot slot)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(aiAgent.GetControlledEntity());		
        CompartmentAccessComponent accessComponent = CompartmentAccessComponent.Cast(character.FindComponent(CompartmentAccessComponent));
		
		if (slot.IsOccupied() || !slot.IsCompartmentAccessible() || slot.IsReserved())
		{
			return false;
		}
		bool success = accessComponent.GetInVehicle(vehicle, slot, true, -1, ECloseDoorAfterActions.CLOSE_DOOR, false);
		
		return success;
	}
}
	
//------------------------------------------------------------------------------------------------
class SDRC_ConvoyConfig : SDRC_MissionConfig
{
	//Mission specific
	int distanceToPlayer;							//If no players this close to any players and patrolingTime has passed, despawn mission.
	ref array<ref SDRC_Convoy> subMissions = {};	//List of convoys
	
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
}

//------------------------------------------------------------------------------------------------
class SDRC_Convoy : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();	
	ref array<EMapDescriptorType> locationTypes = {};	
	ref array<string> vehicleTypes = {};
	float cruiseSpeed;						//Speed to drive in km/h.
	//Optional settings
	ref SDRC_Loot loot = null;	
	
	void Set(array<EMapDescriptorType> locationTypes_, array<string> vehicleTypes_, float cruiseSpeed_)
	{
		locationTypes = locationTypes_;
		vehicleTypes = vehicleTypes_;
		cruiseSpeed = cruiseSpeed_;
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_ConvoyJsonApi : SDRC_JsonApi
{
	ref SDRC_ConvoyConfig conf = new SDRC_ConvoyConfig();

	//------------------------------------------------------------------------------------------------
	void SDRC_ConvoyJsonApi(string fileName)
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
			SDRC_ConvoyJsonApi jsonApi = new SDRC_ConvoyJsonApi(missionFile);		
			if (jsonApi.Load(false))
			{
				foreach (SDRC_Convoy subMission : jsonApi.conf.subMissions)
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
		//Default
		conf.disableArsenal = true;
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		conf.missionList = {0,0,0,0,0,0,0,1,1,1,1,1,2,3,3,};
		//Mission specific
		conf.distanceToPlayer = 500;
		conf.disableArsenal = true;
		//----------------------------------------------------
		conf.subMissions.Insert(Convoy0());				
		conf.subMissions.Insert(Convoy1());				
		conf.subMissions.Insert(Convoy2());
		conf.subMissions.Insert(Convoy3());
	}
		
	//----------------------------------------------------
	SDRC_Convoy Convoy0()
	{
		ref SDRC_Convoy convoy = new SDRC_Convoy();
		convoy.general.Set(
			0, "index 0: Convoy driving from .. to ..",
			{"0 0 0", "0 0 0"}, 5,
			"any",
			"Convoy is on the move.",
			"Leaked travel plans show a route from %l to %d",
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			"The convoy was successfully intercepted.",
			"The convoy reached %d as planned.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_CONVOY_MAP,
			SDRC_EMissionDifficulty.NORMAL,
			0
		);
		convoy.ai.Set(
			{0, 0},
			{"G_LIGHT", "G_MEDICAL"},
			50, 1.0,
			{0, 0},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
		convoy.Set(
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
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
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
		convoy.general.Set(
			1, "index 1: Truck driving from .. to ..",
			{"0 0 0", "0 0 0"}, 7,
			"any",
			"Cargo truck is on the move.",
			"Follow the route from %l to %d",
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			"Truck stopped, loot grabbed.",
			"All the goodies in the truck was never for you.", 
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_CONVOY_MAP,		
			SDRC_EMissionDifficulty.NORMAL,
			0
		);
		convoy.ai.Set(
			{1, 1},
			{"G_RECON", "G_LIGHT"},
			40, 1.0,
			{0, 0},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
		convoy.Set(
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
			{
				"{92264FF932676C13}Prefabs/Vehicles/Wheeled/M923A1/M923A1_ammo.et",
				"{F1FBD0972FA5FE09}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport.et",
				"{81FDAD5EB644CC3D}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport_covered.et",
				"{48A6D4372444B85A}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport_covered_closed.et", 
				"{16E32C3ABEAFC2C6}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_FIA_transport.et",
				"{1449105FD658EDFB}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_CIV_forest.et",
				"{FB219B49A448A8EA}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_covered_CIV_JZD.et",			
			},
			20
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", "WEAPON_RIFLE", "WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", 
				"WEAPON_LAUNCHER",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", 
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"			
			};
		loot.Set(0.9, lootItems);
		convoy.loot = loot;			
		
		return convoy;
	}
	
	//----------------------------------------------------
	SDRC_Convoy Convoy2()
	{
		ref SDRC_Convoy convoy = new SDRC_Convoy();
		convoy.general.Set(
			2, "index 2: Armor driving from .. to ..",
			{"0 0 0", "0 0 0"}, 7,
			"any",
			"Armor spotted",
			"It's been seen in %l. It's to patrol to %d.",
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			"Armor destroyed. Well done!",
			"Were you scared of a piece metal? Cowards!", 
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_CONVOY_MAP,		
			SDRC_EMissionDifficulty.NORMAL,
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
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"			
			};
		loot.Set(0.7, lootItems);
		convoy.loot = loot;

		return convoy;	
	}
			
	//----------------------------------------------------
	SDRC_Convoy Convoy3()
	{
		ref SDRC_Convoy convoy = new SDRC_Convoy();
		convoy.general.Set(
			3, "index 3: Vehicle with a gun driving from .. to ..",
			{"0 0 0", "0 0 0"}, 7,
			"any",
			"Guns on the move",
			"Look out for a patrol between %l and %d.",
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			"Nice shooting!",
			"Oh dear, your failure will be remembered.", 
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_CONVOY_MAP,
			SDRC_EMissionDifficulty.NORMAL,
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
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"			
			};
		loot.Set(0.9, lootItems);
		convoy.loot = loot;		
				
		return convoy;	
	}
}