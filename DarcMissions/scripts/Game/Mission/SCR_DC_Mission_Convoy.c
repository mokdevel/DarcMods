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
	private SCR_AIGroup m_Group = null;
	
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
		vector pos = m_DC_Convoy.posStart;
		m_PosDestination = m_DC_Convoy.posDestination;
		string posName = m_DC_Convoy.locationName;
		IEntity location = null;
		IEntity locationDestination = null;
//		bool allGood = true;
		
		//Find a location for the mission
		if (pos == "0 0 0")
		{
			location = SCR_DC_MissionHelper.FindMissionLocation(m_DC_Convoy.locationTypes);
			if(location)
			{
				SCR_DC_RoadPos roadPos = new SCR_DC_RoadPos;
				pos = SCR_DC_RoadHelper.FindClosestRoadposToPos(roadPos, location.GetOrigin());
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
			locationDestination = SCR_DC_MissionHelper.FindMissionDestination(m_DC_Convoy.locationTypes, pos, 500);
			if(locationDestination)
			{
				SCR_DC_RoadPos roadPos = new SCR_DC_RoadPos;
				m_PosDestination = SCR_DC_RoadHelper.FindClosestRoadposToPos(roadPos, locationDestination.GetOrigin());
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
			SetPosName(SCR_DC_Locations.CreateName(location, posName));
			SetTitle(m_DC_Convoy.title);
			SetInfo(m_DC_Convoy.info + "" + GetPosName() + " to " + SCR_DC_Locations.CreateName(locationDestination, posName));			
			SetMarker(m_Config.showMarker, DC_EMissionIcon.MISSION);
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
					MoveGroupInVehicle(m_Group, m_Vehicle);
					SCR_DC_WPHelper.CreateMissionAIWaypoints(m_Group, m_DC_Convoy.waypointGenType, GetPos(), m_PosDestination, DC_EWaypointMoveType.MOVE);
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
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionLifeCycleTime*1000);
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
		string resourceName	= "{543799AC5C52989C}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_beige.et";		
		m_Vehicle = SCR_DC_SpawnHelper.SpawnItem(GetPos(), resourceName);
		if(m_Vehicle)
		{
			m_EntityList.Insert(m_Vehicle);
		}
		
		AICarMovementComponent vehicle_c = AICarMovementComponent.Cast(m_Vehicle.FindComponent(AICarMovementComponent));
        vehicle_c.SetCruiseSpeed(30);

		//Spawn AI
		vector posg = GetPos() + "3 0 3";
		m_Group = SCR_DC_AIHelper.SpawnGroup("{84E5BBAB25EA23E5}Prefabs/Groups/BLUFOR/Group_US_FireTeam.et", posg);
		if (m_Group)
		{
			m_Groups.Insert(m_Group);					
		}
	}
		
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
				bool success = accessComponent.GetInVehicle(vehicle, slot, false, -1, ECloseDoorAfterActions.CLOSE_DOOR,false);
				return success;
			}			
//		}
		return false;
    }		
}

/*
//copy of the code from SCR_PlayerSpawnPoint
protected bool PrepareSpawnedEntityForVehicle_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnData data, IEntity entity, Vehicle vehicle)
    {
        #ifdef _ENABLE_RESPAWN_LOGS
        Print(string.Format("%1::PrepareSpawnedEntityForVehicle_S(playerId: %2, data: %3, entity: %4, vehicle: %5)", Type().ToString(),
                    requestComponent.GetPlayerId(),    data, entity, vehicle), LogLevel.NORMAL);
        #endif
        
        SCR_CompartmentAccessComponent accessComponent = SCR_CompartmentAccessComponent.Cast(entity.FindComponent(SCR_CompartmentAccessComponent));
        BaseCompartmentManagerComponent compartmentManager = BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(BaseCompartmentManagerComponent));
        
        array<BaseCompartmentSlot> compartments = {};
        int count = compartmentManager.GetCompartments(compartments);
        for (int i = 0; i < count; i++)
        {
            BaseCompartmentSlot slot = compartments[i];
            if (!slot.IsOccupied() && (!slot.IsReserved() || slot.IsReservedBy(entity)))
            {
                ChimeraWorld world = GetGame().GetWorld();
                return accessComponent.GetInVehicle(vehicle, slot, true, -1, ECloseDoorAfterActions.INVALID, world.IsGameTimePaused());
            }
        }
        
        return false;
    }
*/

/*
BaseCompartmentManagerComponent slotCompMan = BaseCompartmentManagerComponent.Cast(spawnedVehicle.FindComponent(BaseCompartmentManagerComponent));
                        array<BaseCompartmentSlot> vehicleCompartments = new array<BaseCompartmentSlot>;
                        slotCompMan.GetCompartments(vehicleCompartments);
                        for(int j = 0; j < agents.Count(); j++)
                        {
                            AIAgent member = agents[j];
                            if(member)
                            {
                                SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(member.GetControlledEntity());
                                CompartmentAccessComponent CAComp = CompartmentAccessComponent.Cast(character.FindComponent(CompartmentAccessComponent));
                                if(CAComp && character && !character.IsInVehicle() && j < vehicleCompartments.Count())
                                {
                                    BaseCompartmentSlot slot = vehicleCompartments[j];
                                    if(slot)
                                        CAComp.GetInVehicle(spawnedVehicle, slot, true, -1, ECloseDoorAfterActions.INVALID, false);
                                }
                                else
                                {
                                    group.RemoveAgent(member);
                                    RplComponent.DeleteRplEntity(member.GetControlledEntity().GetRootParent(), false);
                                }
                            }
                        }
*/
	
//------------------------------------------------------------------------------------------------
class SCR_DC_ConvoyConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	int missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_DEFAULT;	//How often the mission is run
	bool showMarker;
	
	//Mission specific
	int convoyTime;									//Time to patrol, in seconds
	int distanceToPlayer;							//If no players this close to any players and patrolingTime has passed, despawn mission.
	ref array<ref int> convoyList = {};				//The indexes of convoys.
	ref array<ref SCR_DC_Convoy> convoys = {};		//List of convoys
}

//------------------------------------------------------------------------------------------------
class SCR_DC_Convoy : Managed
{
	//Patrol specific
	string comment;							//Generic comment to describe the mission. Not used in game.
	vector posStart;						//Start position for the patrol. "0 0 0" used for random location chosen from locationTypes.
	vector posDestination;					//Destination for the patrol to go to
	string locationName;					//Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes 
	string title;							//Title for the hint shown for players
	string info;							//Details for the hint shown for players
	ref array<EMapDescriptorType> locationTypes = {};
	DC_EWaypointGenerationType waypointGenType;
	string vehicleType;
	string groupType;
	
	void Set(string comment_, vector posStart_, vector posDestination_, string locationName_, string title_, string info_, array<EMapDescriptorType> locationTypes_, DC_EWaypointGenerationType waypointGenType_, string vehicleType_, string groupType_)
	{
		comment = comment_;
		posStart = posStart_;
		posDestination = posDestination_;
		locationName = locationName_;
		title = title_;
		info = info_;
		locationTypes = locationTypes_;
		waypointGenType = waypointGenType_;
		vehicleType = vehicleType_;
		groupType = groupType_;
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
		//Default
		conf.missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_DEFAULT;
		conf.showMarker = true;
		//Mission specific
		conf.convoyList = {0};
		conf.distanceToPlayer = 500;
		
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
				EMapDescriptorType.MDT_NAME_RIDGE
			},
			DC_EWaypointGenerationType.ROUTE,
			"{543799AC5C52989C}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_beige.et",
			"{84E5BBAB25EA23E5}Prefabs/Groups/BLUFOR/Group_US_FireTeam.et"
		);
		conf.convoys.Insert(convoy0);		
	}	
}