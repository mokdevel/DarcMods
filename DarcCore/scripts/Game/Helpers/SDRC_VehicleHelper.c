//Helpers SDRC_VehicleHelper

//------------------------------------------------------------------------------------------------
/*!
Functions related to vehicles.
*/

class SDRC_VehicleHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Move multiple groups to a vehicle
	*/
    static void MoveGroupsInVehicle(array<SCR_AIGroup> groups, IEntity vehicle, bool forceTeleport = false)
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
					bool success = MoveEntityInVehicle(aiAgent, vehicle, i, forceTeleport);
					
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
	/*!
	Move single group to a vehicle
	*/
    static void MoveGroupInVehicle(AIGroup group, IEntity vehicle, bool forceTeleport = false)
    {
		array<AIAgent> groupMembers  = new array<AIAgent>;
		
		if (group)
		{
			group.GetAgents(groupMembers);
			
			int i = 0;
			foreach (AIAgent aiAgent: groupMembers)
			{
				bool success = MoveEntityInVehicle(aiAgent, vehicle, i, forceTeleport);
				
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
	/*!
	Move individual AI to a vehicle at a predefined or random slot
	*/	
    static bool MoveEntityInVehicle(AIAgent aiAgent, IEntity vehicle, int slotIdx, bool forceTeleport = false)
    {
		BaseCompartmentManagerComponent compartmentManager = BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(BaseCompartmentManagerComponent));
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		array<BaseCompartmentSlot> compartments = {};
		int slots = compartmentManager.GetCompartments(compartments);		
		
		if (slotIdx >= slots)
		{
			SDRC_Log.Add("[SDRC_VehicleHelper:MoveEntityInVehicle] slotIdx incorrect: " + slotIdx + "/" + slots, LogLevel.SPAM);
			return false;			
		}
				
		compartments.Clear();
		scr_compartmentManager.GetCompartmentsOfType(compartments, ECompartmentType.PILOT);
		scr_compartmentManager.GetCompartmentsOfType(compartments, ECompartmentType.TURRET);
		scr_compartmentManager.GetCompartmentsOfType(compartments, ECompartmentType.CARGO);
		SDRC_Log.Add("[SDRC_VehicleHelper:MoveEntityInVehicle] Compartments found: " + compartments.Count(), LogLevel.DEBUG);

		bool success = SetEntityInSlot(aiAgent, vehicle, compartments[slotIdx], forceTeleport);
				
		/* 
		// OLD SYSTEM
		array<int> slotPrio = {};
		
		slotPrio.Insert(-1);		//Reserve prio 0 slot for pilot
		slotPrio.Insert(-1);		//Reserve prio 0 slot for pilot

		int i = 0;
						
		foreach (BaseCompartmentSlot slot : compartments)
		{
			bool found = false;
			
			if (PilotCompartmentSlot.Cast(slot))
			{
				SDRC_Log.Add("[SDRC_VehicleHelper:MoveEntityInVehicle] Pilot slot: " + slot, LogLevel.SPAM);
				if (slotPrio[0] == -1)
				{
					slotPrio[0] = i;
					found = true;
				}
			}			
			else if (TurretCompartmentSlot.Cast(slot))
			{
				SDRC_Log.Add("[SDRC_VehicleHelper:MoveEntityInVehicle] Turret slot: " + slot, LogLevel.SPAM);
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
		bool success = SetEntityInSlot(aiAgent, vehicle, compartments[idx], forceTeleport);
		*/

		return success;
    }
	
	//------------------------------------------------------------------------------------------------
	/*!
	Set AI to a specific vehicle slot
	*/	
	static bool SetEntityInSlot(AIAgent aiAgent, IEntity vehicle, BaseCompartmentSlot slot, bool forceTeleport = false)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(aiAgent.GetControlledEntity());		
        CompartmentAccessComponent accessComponent = CompartmentAccessComponent.Cast(character.FindComponent(CompartmentAccessComponent));
		
		if (slot.IsOccupied() || !slot.IsCompartmentAccessible() || slot.IsReserved())
		{
			return false;
		}
		bool success = accessComponent.GetInVehicle(vehicle, slot, forceTeleport, -1, ECloseDoorAfterActions.CLOSE_DOOR, false);
		
		return success;
	}

	static bool IsPiloted(IEntity vehicle)
	{
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		array<BaseCompartmentSlot> compartments = {};
		scr_compartmentManager.GetFreeCompartmentsOfType(compartments, ECompartmentType.PILOT);

		if (compartments.IsEmpty())
		{
			return true;
		}
		
		return false;				
	}
	
	static int PilotCountAlive(IEntity vehicle)
	{
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		array<IEntity> occupants = {};
		scr_compartmentManager.GetOccupantsOfType(occupants, ECompartmentType.PILOT);
		
		int alive = 0;
		
		foreach(IEntity pilot : occupants)
		{
			if (SCR_AIDamageHandling.IsAlive(pilot))
			{
				alive ++;
			}
		}

		return alive;
	}	
	
	
	
	//------------------------------------------------------------------------------------------------
	static bool IsWorking(IEntity vehicle)
	{
		if(SCR_AIVehicleUsability.VehicleCanMove(vehicle) && !SCR_AIVehicleUsability.VehicleIsOnFire(vehicle))
			return true;
	
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	// Arsenal related
	//------------------------------------------------------------------------------------------------
		
	//------------------------------------------------------------------------------------------------
	/*!
	Disable arsenal on an entity
	*/
	static void DisableVehicleArsenal(IEntity entity, ResourceName resourceName, bool disable)
	{	
		//Disable arsenal
		string tempEntity = resourceName;
		tempEntity.ToLower();
		if (disable && (tempEntity.Contains("vehicle")) )
		{
			DisableArsenal(entity);
		}		
	}

	//------------------------------------------------------------------------------------------------
	/*! 
	Disable arsenal on an entity
	
	TBD: This works in mysterious ways. If you disable the Arsenal in SlotManagerComponent of the prefab, it is not visible in the game. 
	I have not found a way to do the same via script. So, we disable the arsenal. The content are cleared, but you can still see the empty arsenal.
	*/	
	static void DisableArsenal(IEntity entity)
	{	
		SCR_ArsenalComponent arsenalComponent;
			
		// Disable arsenal
		arsenalComponent = SCR_ArsenalComponent.FindArsenalComponent(entity);
		if (arsenalComponent)
		{				
			arsenalComponent.SetArsenalEnabled(false, false);
//			arsenalComponent.SetSupportedArsenalItemModes(0);
				
			SDRC_Log.Add("[SDRC_VehicleHelper:DisableArsenal] Disabling arsenal. Type: " + arsenalComponent.GetArsenalType(), LogLevel.SPAM);
		}			

		// Disable virtual arsenals					
		SlotManagerComponent slotManager = SlotManagerComponent.Cast(entity.FindComponent(SlotManagerComponent));
		if (slotManager)
		{
			array<EntitySlotInfo> slots = {};
			slotManager.GetSlotInfos(slots);			

			foreach (EntitySlotInfo slot : slots)
			{
				if (!slot)
					continue;

				//Print out the slot names
				//string str = slot.GetSourceName();
				//SDRC_Log.Add("[SDRC_VehicleHelper:DisableArsenal] Slot name: " + str, LogLevel.DEBUG);
				IEntity arsenal;
				arsenal = slot.GetAttachedEntity();

				if (!arsenal)
					continue;

//				arsenalComponent = SCR_ArsenalComponent.Cast(arsenal.FindComponent(SCR_ArsenalComponent));
				arsenalComponent = SCR_ArsenalComponent.FindArsenalComponent(arsenal);
				if (arsenalComponent)
				{
					arsenalComponent.SetArsenalEnabled(false, false);
//					arsenalComponent.SetSupportedArsenalItemModes(0);
					SDRC_Log.Add("[SDRC_VehicleHelper:DisableArsenal] Disabling virtual arsenal. Type: " + arsenalComponent.GetArsenalType(), LogLevel.SPAM);
				}
			}
		}		
/*		
		// TBD: Did not change the behaviour
		SCR_ResourceComponent resourceComponent;
		
		resourceComponent = SCR_ResourceComponent.FindResourceComponent(entity);
		if (resourceComponent)
		{
			resourceComponent.SetResourceTypeEnabled(false);
			resourceComponent.SetIsVisible(false);
			SCR_ResourceEncapsulator enc = resourceComponent.GetEncapsulator(EResourceType.SUPPLIES);
			if (enc)
			{
				enc.SetResourceRights(EResourceRights.NONE);
			}
		}
		
		SCR_ResourceContainer resourceContainer;			
		resourceContainer = SCR_ResourceContainer.Cast(entity.FindComponent(SCR_ResourceContainer));
		if (resourceContainer)
		{
			resourceContainer.SetResourceValue(0, true);
			resourceContainer.SetResourceValueUnsafe(0, true);
			resourceContainer.SetMaxResourceValue(0, true);
			resourceContainer.SetOnEmptyBehavior(EResourceContainerOnEmptyBehavior.DELETE);
		}*/
	}	
	
	//------------------------------------------------------------------------------------------------
	// Persistency related
	//------------------------------------------------------------------------------------------------
		
	//------------------------------------------------------------------------------------------------
	/*!
	Fix persistency for vehicle. For example for EPF.
	*/
	static void SetPersistency(IEntity entity)
	{	
		//Override in compat mod
	}	
}