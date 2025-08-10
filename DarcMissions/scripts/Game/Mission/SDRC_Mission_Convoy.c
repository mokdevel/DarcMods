//Mission SDRC_Mission_Convoy.c

//------------------------------------------------------------------------------------------------
/*!
A convoy traveling from A to B.

Note to self: RADIUS, SCATTERED : This could also work, but support was removed as unnecessary. The concoy would follow a path created with waypointRange starting from posStart. posDestination is ignored.
*/

//------------------------------------------------------------------------------------------------
enum DC_EMissionConvoyState
{
	INIT,
	MOVE_AI,
	RUN
};

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Convoy : SDRC_Mission
{
	private ref SDRC_ConvoyJsonApi m_ConvoyJsonApi = new SDRC_ConvoyJsonApi();	
	private ref SDRC_ConvoyConfig m_Config;
	private DC_EMissionConvoyState missionConvoyState = DC_EMissionConvoyState.INIT;	

	protected ref SDRC_Convoy m_DC_Convoy;		//Convoy configuration in use
	
	private vector m_vPosDestination = "1 1 1";
	private IEntity m_Vehicle = null;
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Convoy(vector pos = "0 0 0")
	{
		SDRC_Log.Add("[SDRC_Mission_Convoy] Constructor", LogLevel.SPAM);
				
		//Set some defaults
		SetType(DC_EMissionType.CONVOY);

		//Load config
		m_ConvoyJsonApi.Load();
		m_Config = m_ConvoyJsonApi.conf;

		//Pick a configuration for mission
		int idx = SDRC_MissionHelper.SelectMissionIndex(m_Config.convoyList);
		if (idx == -1)
		{
			SDRC_Log.Add("[SDRC_Mission_Convoy] No convoys defined.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}
		m_DC_Convoy = m_Config.convoys[idx];
		
		//Set defaults		
		m_vPosDestination = m_DC_Convoy.general.pos[1];		//Destination from the defined SDRC_Convoy 

		//If not a GM requested mission, use the default one.
		if (!IsRequested())
		{
			pos = m_DC_Convoy.general.pos[0];
		}

		//Find a location for the mission
		if (pos == "0 0 0")
		{
			pos = SDRC_MissionHelper.FindMissionPos(m_DC_Convoy.locationTypes);
		}

		//If failed, stop
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SDRC_Log.Add("[SDRC_Mission_Convoy] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}	
		
		//Find nearest road
		SDRC_RoadPos roadPosStart = new SDRC_RoadPos();				
		pos = SDRC_RoadHelper.FindClosestRoadposToPos(roadPosStart, pos, 1000);
		
		//If failed, stop
		if (pos == "0 0 0")
		{
			SDRC_Log.Add("[SDRC_Mission_Convoy] No start road found.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}
		
/*				
		if (!IsRequested())
		{
			pos = m_DC_Convoy.pos;
			
			//Find a location for the mission
			if (pos == "0 0 0")
			{
				pos = SDRC_MissionHelper.FindMissionPos(m_DC_Convoy.locationTypes);
				if (pos != "0 0 0")
				{
					SDRC_RoadPos roadPos = new SDRC_RoadPos();
					pos = SDRC_RoadHelper.FindClosestRoadposToPos(roadPos, pos);
					if (pos == "0 0 0")
					{
						SDRC_Log.Add("[SDRC_Mission_Convoy] No start road found.", LogLevel.ERROR);
					}
				}
				else
				{
					pos = "0 0 0";
				}
			}
		}
*/		
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
			SDRC_Log.Add("[SDRC_Mission_Convoy] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}	
		
		SetPos(pos, m_vPosDestination);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Convoy.general.posName));
		SetMarker(m_Config.showMarker, m_Config.markerIdx, m_Config.markerType);
		SetHint(m_Config.showHint, m_DC_Convoy.general.title, m_DC_Convoy.general.info);		
		SetMessages(m_Config.showMessage, m_DC_Convoy.general.winMessage, m_DC_Convoy.general.loseMessage);		
		SetWinCondition(m_DC_Convoy.general.winCondition);
		SetActiveDistance(m_Config.distanceToPlayer);				//Change the m_ActiveDistance to a mission specific one.
		
		SetState(DC_EMissionState.INIT);
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == DC_EMissionState.INIT)
		{
			MissionSpawn();
		}

		if (GetState() == DC_EMissionState.END)
		{
			MissionEnd();
			SetState(DC_EMissionState.EXIT);
		}	
				
		if (GetState() == DC_EMissionState.ACTIVE)
		{			
			switch (missionConvoyState)
			{
				case DC_EMissionConvoyState.INIT:
					//This state is mainly for delay to give vehicle and AI to finalize spawn. If removed, AI will not enter the vehicle.
					missionConvoyState = DC_EMissionConvoyState.MOVE_AI;
					break;
				case DC_EMissionConvoyState.MOVE_AI:
					MoveGroupInVehicle(m_Groups[0], m_Vehicle);
					SDRC_WPHelper.CreateMissionAIWaypoints(m_Groups[0], DC_EWaypointGenerationType.ROUTE, GetPos(), m_vPosDestination, DC_EWaypointMoveType.MOVE);
					missionConvoyState = DC_EMissionConvoyState.RUN;
					break;
				case DC_EMissionConvoyState.RUN:
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
						SetState(DC_EMissionState.END);
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
			SDRC_Log.Add("[SDRC_Mission_Convoy] Could not spawn vehicle: " + resourceName, LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;			
		}
		
		m_EntityList.Insert(m_Vehicle);
		
		//Disable arsenal
		SDRC_SpawnHelper.DisableVehicleArsenal(m_Vehicle, resourceName, m_Config.disableArsenal);
		
		AICarMovementComponent vehicle_c = AICarMovementComponent.Cast(m_Vehicle.FindComponent(AICarMovementComponent));
        vehicle_c.SetCruiseSpeed(m_DC_Convoy.cruiseSpeed);

		//Spawn AI
		vector posg = GetPos() + "3 0 3";
		SCR_AIGroup group = SDRC_AIHelper.SpawnGroup(m_DC_Convoy.groupTypes.GetRandomElement(), posg, GetFaction());
		if (group)
		{			
			SDRC_AIHelper.SetAIGroupSkill(group, m_DC_Convoy.aiSkill, m_DC_Convoy.aiPerception);
			m_Groups.Insert(group);					
		}
		
		//Put loot
		if (m_DC_Convoy.loot)			
		{
			m_DC_Convoy.loot.box = m_Vehicle;
			SDRC_LootHelper.SpawnItemsToStorage(m_DC_Convoy.loot.box, m_DC_Convoy.loot.items, m_DC_Convoy.loot.itemChance);
			SDRC_Log.Add("[SDRC_Mission_Convoy:MissionSpawn] Loot added.", LogLevel.DEBUG);								
		}		
		
		SetState(DC_EMissionState.ACTIVE);		
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
				MoveEntityInVehicle(aiAgent, vehicle, i);				
				i++;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
    bool MoveEntityInVehicle(AIAgent aiAgent, IEntity vehicle, int slotIdx = -1)
    {
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(aiAgent.GetControlledEntity());		
        CompartmentAccessComponent accessComponent = CompartmentAccessComponent.Cast(character.FindComponent(CompartmentAccessComponent));

		BaseCompartmentManagerComponent compartmentManager = BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(BaseCompartmentManagerComponent));
		array<BaseCompartmentSlot> compartments = {};
		int slots = compartmentManager.GetCompartments(compartments);		
		
		int i = slotIdx;
//        for (int i = 0; i < slots; i++)
//        {
		if (i < slots)
		{
			BaseCompartmentSlot slot = compartments[i];
            if (!slot.IsOccupied() && (!slot.IsReserved()))// || slot.IsReservedBy(aiAgent))
			{
				bool success = accessComponent.GetInVehicle(vehicle, slot, true, -1, ECloseDoorAfterActions.CLOSE_DOOR, false);
				return success;
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_Mission_Convoy:MoveEntityInVehicle] slotIdx incorrect: " + i + "/" + slots, LogLevel.DEBUG);											
		}
//		}
		return false;
    }		
}
	
//------------------------------------------------------------------------------------------------
class SDRC_ConvoyConfig : SDRC_MissionConfig
{
	//Mission specific
	int convoyTime;									//Time to patrol, in seconds
	int distanceToPlayer;							//If no players this close to any players and patrolingTime has passed, despawn mission.
	bool disableArsenal;							//Disable arsenal for vehicles so that no other items are found
	ref array<ref int> convoyList = {};				//The indexes of convoys.
	ref array<ref SDRC_Convoy> convoys = {};		//List of convoys
}

//------------------------------------------------------------------------------------------------
class SDRC_Convoy : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref array<EMapDescriptorType> locationTypes = {};	
	ref array<string> groupTypes = {};
	int aiSkill;
	float aiPerception	
	ref array<string> vehicleTypes = {};
	float cruiseSpeed;						//Speed to drive in km/h.
	//Optional settings
	ref SDRC_Loot loot = null;	
	
	void Set(array<EMapDescriptorType> locationTypes_, array<string> groupTypes_, int aiSkill_, float aiPerception_, array<string> vehicleTypes_, float cruiseSpeed_)
	{
		locationTypes = locationTypes_;
		groupTypes = groupTypes_;
		aiSkill = aiSkill_;
		aiPerception = aiPerception_;
		vehicleTypes = vehicleTypes_;
		cruiseSpeed = cruiseSpeed_;
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_ConvoyJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Convoy.json";
	ref SDRC_ConvoyConfig conf = new SDRC_ConvoyConfig();
		
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
		conf.markerIdx = DC_EMissionIcon.GM_MISSION_CONVOY_MAP;
		//Mission specific
		conf.convoyList = {0,0,0,1};
		conf.distanceToPlayer = 500;
		conf.disableArsenal = true;
		//----------------------------------------------------
		conf.convoys.Insert(Convoy0());				
		conf.convoys.Insert(Convoy1());				
		conf.convoys.Insert(Convoy2());
	}
		
	//----------------------------------------------------
	SDRC_Convoy Convoy0()
	{
		SDRC_Convoy convoy = new SDRC_Convoy();
		convoy.general.Set(
			"index 0: Convoy driving from .. to ..",
			{"0 0 0", "0 0 0"},
			"any",
			"Convoy is on the move.",
			"Leaked travel plans show a route from %l to %d",
			DC_EMissionWinCondition.AI_KILL_ALL,
			"The convoy was successfully intercepted.",
			"The convoy reached %d as planned.", 
			0
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
				"G_LIGHT", "G_MEDICAL"
			},
			50, 1.0,
			{
				"{543799AC5C52989C}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_beige.et",
				"{259EE7B78C51B624}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469.et",
				"{D4855501D5B12AF2}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_uncovered_CIV_teal.et"
			},
			30
		);
		
		SDRC_Loot loot = new SDRC_Loot();
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
		SDRC_Convoy convoy = new SDRC_Convoy();
		convoy.general.Set(
			"index 1: Truck driving from .. to ..",
			{"0 0 0", "0 0 0"},
			"any",
			"Cargo truck is on the move.",
			"Follow the route from %l to %d",
			DC_EMissionWinCondition.AI_KILL_ALL,
			"Truck stopped, loot grabbed.",
			"All the goodies in the truck was never for you.", 
			0
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
				"G_RECON"
			},
			40, 1.0,
			{
				"{92264FF932676C13}Prefabs/Vehicles/Wheeled/M923A1/M923A1_ammo.et",
				"{1449105FD658EDFB}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_CIV_forest.et",
				"{FB219B49A448A8EA}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_covered_CIV_JZD.et",
				"{F1FBD0972FA5FE09}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport.et"
			},
			20
		);
		
		SDRC_Loot loot = new SDRC_Loot();
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
		//TBD: Needs fixing. The AI will jump out immediately when seeing a player. The driver stays inside, but gunner not. Probably need to assign a gunner...
		SDRC_Convoy convoy = new SDRC_Convoy();
		convoy.general.Set(
			"index 2: Armor driving from .. to ..",
			{"0 0 0", "0 0 0"},
			"any",
			"Armor spotted",
			"It's been seen in %l. It's to patrol to %d.",
			DC_EMissionWinCondition.AI_KILL_ALL,
			"Armor destroyed. Well done!",
			"Were you scared of a piece metal? Cowards!", 
			0
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
				"G_HEAVY"
			},
			50, 1.0,
			{
				"{0FBF8F010F81A4E5}Prefabs/Vehicles/Wheeled/LAV25/LAV25.et",
				"{C012BB3488BEA0C2}Prefabs/Vehicles/Wheeled/BTR70/BTR70.et",
			},
			50
		);
		
		SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", 
				"WEAPON_HANDGUN", 
				"WEAPON_LAUNCHER", "WEAPON_LAUNCHER",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", 
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"			
			};
		loot.Set(0.9, lootItems);
		convoy.loot = loot;
		
		return convoy;	
	}	
}