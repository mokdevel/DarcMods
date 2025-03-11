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
		
		string posName = m_Config.posName;
		vector pos = m_Config.pos;
		pos = "1053 49 2470";					//!!!!!!!!!!!!!!!!!!!!
		
		SetTitle(m_Config.title + "" + posName);
		SetInfo(m_Config.info);
		SetPos(pos);
		SetPosName(posName);
		SetMarker(m_Config.showMarker, DC_EMissionIcon.MISSION);

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
			switch (missionConvoyState)
			{
				case DC_EMissionConvoyState.INIT:
					//This state is mainly for delay to give vehicle and AI to finalize spawn. If removed, AI will not enter the vehicle.
					missionConvoyState = DC_EMissionConvoyState.MOVE_AI;
					break;
				case DC_EMissionConvoyState.MOVE_AI:
					MoveGroupInVehicle(m_Group, m_Vehicle);
//					SCR_DC_WPHelper.CreateWaypoint(m_Group, "1674 0 3009");
					missionConvoyState = DC_EMissionConvoyState.RUN;
					break;
				case DC_EMissionConvoyState.RUN:
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
	vector pos;
	string posName;
	string title;
	string info;
	
	//Variables here
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
		conf.pos = "0 0 0";
		conf.posName = "A location name";
		conf.title = "Convoy mission";
		conf.info = "Some additional information for players";
	}	
}