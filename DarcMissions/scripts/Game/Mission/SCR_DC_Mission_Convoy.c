//Mission SCR_DC_Mission_Convoy.c
//
//A convoy traveling from A to B

enum DC_EMissionConvoyState
{
	INIT,
	MOVE_AI,
	RUN
};

class SCR_DC_Mission_Convoy : SCR_DC_Mission
{
	private ref SCR_DC_ConvoyJsonApi m_ConvoyJsonApi = new SCR_DC_ConvoyJsonApi();	
	private ref SCR_DC_ConvoyConfig m_Config;
	private DC_EMissionConvoyState missionConvoyState = DC_EMissionConvoyState.INIT;	

	protected ref SCR_DC_Convoy m_DC_Convoy;		//Convoy configuration in use
	
	private vector m_PosDestination = "1 1 1";
	private IEntity m_Vehicle = null;
	
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission_Convoy()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Convoy] Constructor", LogLevel.DEBUG);
				
		//Set some defaults
		SCR_DC_Mission();
		SetType(DC_EMissionType.CONVOY);

		//Load config
		m_ConvoyJsonApi.Load();
		m_Config = m_ConvoyJsonApi.conf;

		//Pick a configuration for mission
		int idx = SCR_DC_MissionHelper.SelectMissionIndex(m_Config.convoyList);
		if(idx == -1)
		{
			SCR_DC_Log.Add("[SCR_DC_Mission_Convoy] No occupations defined.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;
		}
		m_DC_Convoy = m_Config.convoys[idx];

		//Set defaults
		vector pos = m_DC_Convoy.pos;
		m_PosDestination = m_DC_Convoy.posDestination;
		string posName = m_DC_Convoy.posName;
		
		//Find a location for the mission
		if (pos == "0 0 0")
		{
			pos = SCR_DC_MissionHelper.FindMissionPos(m_DC_Convoy.locationTypes);
			if(pos != "0 0 0")
			{
				SCR_DC_RoadPos roadPos = new SCR_DC_RoadPos;
				pos = SCR_DC_RoadHelper.FindClosestRoadposToPos(roadPos, pos);
				if(pos == "0 0 0")
				{
					SCR_DC_Log.Add("[SCR_DC_Mission_Convoy] No start road found.", LogLevel.ERROR);
				}
			}
			else
			{
				pos = "0 0 0";
			}
		}

		//Find a location for the destination
		if (m_PosDestination == "0 0 0" && pos != "0 0 0")
		{
			m_PosDestination = SCR_DC_MissionHelper.FindMissionDestination(m_DC_Convoy.locationTypes, pos, 500);
			if(m_PosDestination != "0 0 0")
			{
				SCR_DC_RoadPos roadPos = new SCR_DC_RoadPos;
				m_PosDestination = SCR_DC_RoadHelper.FindClosestRoadposToPos(roadPos, m_PosDestination);
				if(m_PosDestination == "0 0 0")
				{
					SCR_DC_Log.Add("[SCR_DC_Mission_Convoy] No destination road found.", LogLevel.ERROR);
				}
			}			
			else
			{
				SCR_DC_Log.Add("[SCR_DC_Mission_Convoy] Could not find destination location for ROUTE.", LogLevel.WARNING);
			}
		}		

		//If all is ok, let's finalize the mission creation				
		if (pos != "0 0 0" && m_PosDestination != "0 0 0")
		{	
			SetPos(pos);
			SetPosName(SCR_DC_Locations.CreateName(pos, posName));
			SetTitle(m_DC_Convoy.title);
			SetInfo(m_DC_Convoy.info + "" + GetPosName() + " to " + SCR_DC_Locations.CreateName(m_PosDestination, "any"));			
			SetMarker(m_Config.showMarker, DC_EMissionIcon.MISSION);
			SetShowHint(m_Config.showHint);			
			SetActiveDistance(m_Config.distanceToPlayer);				//Change the m_ActiveDistance to a mission specific one.
			
			SetState(DC_MissionState.INIT);
		}
		else
		{				
			//No suitable location found.
			SCR_DC_Log.Add("[SCR_DC_Mission_Convoy] Could not find suitable location.", LogLevel.ERROR);
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
			switch (missionConvoyState)
			{
				case DC_EMissionConvoyState.INIT:
					//This state is mainly for delay to give vehicle and AI to finalize spawn. If removed, AI will not enter the vehicle.
					missionConvoyState = DC_EMissionConvoyState.MOVE_AI;
					break;
				case DC_EMissionConvoyState.MOVE_AI:
					MoveGroupInVehicle(m_Groups[0], m_Vehicle);
					SCR_DC_WPHelper.CreateMissionAIWaypoints(m_Groups[0], m_DC_Convoy.waypointGenType, GetPos(), m_PosDestination, DC_EWaypointMoveType.MOVE);
					missionConvoyState = DC_EMissionConvoyState.RUN;
					break;
				case DC_EMissionConvoyState.RUN:
					//Move the position as the convoy is moving. This way check for player distance works properly.
					if(m_Vehicle)
					{
						SetPos(m_Vehicle.GetOrigin());
						SCR_DC_DebugHelper.MoveDebugPos(GetId(), GetPos());
					}
								
					if (SCR_DC_AIHelper.AreAllGroupsDead(m_Groups))
					{
						if (!IsActive())
						{
							SCR_DC_Log.Add("[SCR_DC_Mission_Convoy:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
							SetState(DC_MissionState.END);
						}
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
		
		SCR_DC_Log.Add("[SCR_DC_Mission_Convoy:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);				
	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		//Spawn vehicle					
		string resourceName	= m_DC_Convoy.vehicleTypes.GetRandomElement();
		m_Vehicle = SCR_DC_SpawnHelper.SpawnItem(GetPos(), resourceName);
		if(m_Vehicle)
		{
			m_EntityList.Insert(m_Vehicle);
		}
		
		//Disable arsenal
		SCR_DC_SpawnHelper.DisableVehicleArsenal(m_Vehicle, resourceName, m_Config.disableArsenal);
		
		AICarMovementComponent vehicle_c = AICarMovementComponent.Cast(m_Vehicle.FindComponent(AICarMovementComponent));
        vehicle_c.SetCruiseSpeed(m_DC_Convoy.cruiseSpeed);

		//Spawn AI
		vector posg = GetPos() + "3 0 3";
		SCR_AIGroup group = SCR_DC_AIHelper.SpawnGroup(m_DC_Convoy.groupTypes.GetRandomElement(), posg);
		if (group)
		{			
			SCR_DC_AIHelper.SetAIGroupSkill(group, m_DC_Convoy.aiSkill, m_DC_Convoy.aiPerception);
			m_Groups.Insert(group);					
		}
		
		//Put loot
		if (m_DC_Convoy.loot)			
		{
			m_DC_Convoy.loot.box = m_Vehicle;
			SCR_DC_LootHelper.SpawnItemsToStorage(m_DC_Convoy.loot.box, m_DC_Convoy.loot.items, m_DC_Convoy.loot.itemChance);
			SCR_DC_Log.Add("[SCR_DC_Mission_Convoy:MissionSpawn] Loot added.", LogLevel.DEBUG);								
		}		
		
	}
		
	//------------------------------------------------------------------------------------------------
    private void MoveGroupInVehicle(AIGroup group, IEntity vehicle)
    {
		array<AIAgent> groupMembers  = new array<AIAgent>;
		
		if(group)
		{
			group.GetAgents(groupMembers);
			
			int i = 0;
			foreach(AIAgent aiAgent: groupMembers)
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
			BaseCompartmentSlot slot = compartments[i];
            if (!slot.IsOccupied() && (!slot.IsReserved()))// || slot.IsReservedBy(aiAgent))
			{
				bool success = accessComponent.GetInVehicle(vehicle, slot, true, -1, ECloseDoorAfterActions.CLOSE_DOOR, false);
				return success;
			}
//		}
		return false;
    }		
}
	
//------------------------------------------------------------------------------------------------
class SCR_DC_ConvoyConfig : SCR_DC_MissionConfig
{
	//Mission specific
	int convoyTime;									//Time to patrol, in seconds
	int distanceToPlayer;							//If no players this close to any players and patrolingTime has passed, despawn mission.
	bool disableArsenal;							//Disable arsenal for vehicles so that no other items are found
	ref array<ref int> convoyList = {};				//The indexes of convoys.
	ref array<ref SCR_DC_Convoy> convoys = {};		//List of convoys
}

//------------------------------------------------------------------------------------------------
class SCR_DC_Convoy : Managed
{
	//Patrol specific
	string comment;							//Generic comment to describe the mission. Not used in game.
	vector pos;								//Start position for the patrol. "0 0 0" used for random location chosen from locationTypes.
	vector posDestination;					//Destination for the patrol to go to
	string posName;							//Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes 
	string title;							//Title for the hint shown for players
	string info;							//Details for the hint shown for players
	ref array<EMapDescriptorType> locationTypes = {};	
	DC_EWaypointGenerationType waypointGenType;
	ref array<string> groupTypes = {};
	int aiSkill;
	float aiPerception	
	ref array<string> vehicleTypes = {};
	float cruiseSpeed;						//Speed to drive in km/h.
	//Optional settings
	ref SCR_DC_Loot loot = null;	
	
	void Set(string comment_, vector pos_, vector posDestination_, string posName_, string title_, string info_, array<EMapDescriptorType> locationTypes_, DC_EWaypointGenerationType waypointGenType_, array<string> groupTypes_, int aiSkill_, float aiPerception_, array<string> vehicleTypes_, )
	{
		comment = comment_;
		pos = pos_;
		posDestination = posDestination_;
		posName = posName_;
		title = title_;
		info = info_;
		locationTypes = locationTypes_;
		waypointGenType = waypointGenType_;
		groupTypes = groupTypes_;
		aiSkill = aiSkill_;
		aiPerception = aiPerception_;
		vehicleTypes = vehicleTypes_;
	}
}

//------------------------------------------------------------------------------------------------
class SCR_DC_ConvoyJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Convoy.json";
	ref SCR_DC_ConvoyConfig conf = new SCR_DC_ConvoyConfig;
		
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
		array<string> lootItems = {};
		
		//Default
		conf.missionCycleTime = DC_MISSION_CYCLE_TIME_DEFAULT;
		conf.showMarker = true;
		//Mission specific
		conf.convoyList = {0};
		conf.distanceToPlayer = 500;
		conf.disableArsenal = true;
		
		//----------------------------------------------------
		SCR_DC_Convoy convoy0 = new SCR_DC_Convoy;
		convoy0.Set
		(
			"Convoy driving from .. to ..",
			"0 0 0",
			"0 0 0",
			"any",
			"Convoy is on the move.",
			"Leaked travel plans show a route from ",
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
			DC_EWaypointGenerationType.ROUTE,
			{
				"{5B08C42EA0661A20}Prefabs/Groups/OPFOR/KLMK/Group_USSR_LightFireTeam_KLMK.et",
				"{8EDE6E160E71ABB4}Prefabs/Groups/OPFOR/KLMK/Group_USSR_SapperTeam_KLMK.et",
				"{8E29E7581DE832CC}Prefabs/Groups/OPFOR/KLMK/Group_USSR_MedicalSection_KLMK.et"
			},
			50, 1.0,
			{
				"{543799AC5C52989C}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_beige.et",
				"{259EE7B78C51B624}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469.et",
				"{D4855501D5B12AF2}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_uncovered_CIV_teal.et"
			}		
		);
		conf.convoys.Insert(convoy0);	
		
		SCR_DC_Loot convoy0loot = new SCR_DC_Loot;
		lootItems = {
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			};
		convoy0loot.Set(0.9, lootItems);
		convoy0.loot = convoy0loot;			
	}	
}