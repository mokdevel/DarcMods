//Mission SCR_DC_Mission_Convoy.c
//
//A convoy traveling from A to B

class SCR_DC_Mission_Convoy : SCR_DC_Mission
{
	private ref SCR_DC_ConvoyJsonApi m_ConvoyJsonApi = new SCR_DC_ConvoyJsonApi();	
	private ref SCR_DC_ConvoyConfig m_Config;
	
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission_Convoy()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Convoy] Constructor", LogLevel.DEBUG);
				
		//Set some defaults
		SCR_DC_Mission();

		//Load config
		m_ConvoyJsonApi.Load();
		m_Config = m_ConvoyJsonApi.conf;
		
		string posName = m_Config.posName;
		vector pos = m_Config.pos;
		
		SetTitle(m_Config.title + "" + posName);
		SetInfo(m_Config.info);
		SetPos(pos);
		SetPosName(posName);
		SCR_DC_MapMarkerHelper.CreateMapMarker(GetPos(), DC_EMissionIcon.MISSION, GetId(), GetTitle());

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
			//Add code for runtime
			
			//Eventually when mission is to ended do this:
			//SetState(DC_MissionState.END);
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionLifeCycleTime*1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			

	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
//		string resourceName	= "{F649585ABB3706C4}Prefabs/Vehicles/Wheeled/M151A2/M151A2.et";
		string resourceName	= "{543799AC5C52989C}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_beige.et";		
		vector pos = "1053 49 2470";		
		IEntity vehicle = SCR_DC_SpawnHelper.SpawnItem(pos, resourceName);
		
		AICarMovementComponent vehicle_c = AICarMovementComponent.Cast(vehicle.FindComponent(AICarMovementComponent));
        vehicle_c.SetCruiseSpeed(30);
		
//		AIAgent aiAgent;
//		aiAgent = SCR_DC_AIHelper.SpawnAIAgent("{6058AB54781A0C52}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_AMG.et", pos);
//		SCR_DC_AIHelper.GroupAddAI(aiAgent);
//		AIGroup group = aiAgent.GetParentGroup();
		
		
		vector posg = pos + "3 0 3";
//		AIGroup group = SCR_DC_AIHelper.SpawnGroup("{6F5A71376479B353}Prefabs/Characters/Factions/CIV/ConstructionWorker/Character_CIV_ConstructionWorker_1.et", posg);
//		AIGroup group = SCR_DC_AIHelper.SpawnGroup("{3BF36BDEEB33AEC9}Prefabs/Groups/BLUFOR/Group_US_SentryTeam.et", posg);
		AIGroup group = SCR_DC_AIHelper.SpawnGroup("{84E5BBAB25EA23E5}Prefabs/Groups/BLUFOR/Group_US_FireTeam.et", posg);		
//		AIGroup group = SCR_DC_AIHelper.SpawnGroup("{6058AB54781A0C52}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_AMG.et", posg);
		
		GetGame().GetCallqueue().CallLater(MoveAI, 5000, false, group, vehicle);
	}
		
	private void MoveAI(AIGroup group, IEntity vehicle)
	{
//		MoveInPlayerVehicle(aiAgent, entity);
		
/*		array<AIAgent> groupMembers  = new array<AIAgent>;		
		if(group)
		{
			group.GetAgents(groupMembers);
		}		
		AIAgent aiAgent = groupMembers[0];
		MoveEntityInVehicle(aiAgent, vehicle); */

//		AIGroup group = aiAgent.GetParentGroup();
		MoveGroupInVehicle(group, vehicle);
		SCR_DC_WPHelper.CreateWaypoint(group, "1674 0 3009");
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
		//Mission specific
		conf.missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_DEFAULT;
		conf.pos = "0 0 0";
		conf.posName = "A location name";
		conf.title = "Convoy mission";
		conf.info = "Some additional information for players";
	}	
}