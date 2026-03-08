//Helpers SDRC_VehicleHelper

//------------------------------------------------------------------------------------------------
/*!
Functions related to vehicles.
*/

sealed class SDRC_VehicleHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn single group to a vehicle
	*/
	static void SpawnGroupInVehicle(ResourceName aiResourceName, IEntity vehicle, AIGroup group)
    {
		if (!vehicle)
		{
			SDRC_Log.Add("[SDRC_VehicleHelper:SpawnGroupInVehicle] Vehicle not available (null).", LogLevel.ERROR);
			return;
		}
		
		if (!group)
		{
			SDRC_Log.Add("[SDRC_VehicleHelper:SpawnGroupInVehicle] Group not available (null).", LogLevel.ERROR);
			return;
		}
		
		//Find the compartments and prioritize them in order: PILOT, TURRET, CARGO
		BaseCompartmentManagerComponent compartmentManager = BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(BaseCompartmentManagerComponent));
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		array<BaseCompartmentSlot> compartments = {};
		int slots = compartmentManager.GetCompartments(compartments);		
		
		compartments.Clear();
		scr_compartmentManager.GetCompartmentsOfType(compartments, ECompartmentType.PILOT);
		scr_compartmentManager.GetCompartmentsOfType(compartments, ECompartmentType.TURRET);
		scr_compartmentManager.GetCompartmentsOfType(compartments, ECompartmentType.CARGO);
		SDRC_Log.Add("[SDRC_VehicleHelper:SpawnGroupInVehicle] Compartments found: " + compartments.Count(), LogLevel.SPAM);

		//Fill aiPrefabs with the names of AI to spawn
		array<ResourceName> aiPrefabs = {};
		
		//If character, handle separately
		if (aiResourceName.Contains("Prefabs/Characters/"))
		{
			aiPrefabs.Insert(aiResourceName);
		}
		else //If group, we need to find the right prefabs to spawn
		{
			//Find the list of AIs in the group prefab		
			int count = SDRC_AIHelper.GroupGetEntitySourceMembers(aiResourceName, aiPrefabs);
			SDRC_Log.Add("[SDRC_VehicleHelper:SpawnGroupInVehicle] Prefabs found: " + count, LogLevel.DEBUG);
		}
			
		//Do the spawning	
		foreach (ResourceName aiPrefab : aiPrefabs)
		{
			if (aiPrefab == "")
			{
				continue;
			}
			
			SDRC_Log.Add("[SDRC_VehicleHelper:SpawnGroupInVehicle] Prefab to spawn: " + aiPrefab, LogLevel.DEBUG);
			foreach (BaseCompartmentSlot compartment : compartments)
			{
				if (compartment.IsOccupied() || !compartment.IsCompartmentAccessible() || compartment.IsReserved())
				{
					continue;
				}

				IEntity character = compartment.SpawnCharacterInCompartment(aiPrefab, group);
				if (character)
				{
					compartment.SetReserved(character);
					SDRC_SpawnHelper.SetPersistence(character, false);
					SDRC_Log.Add("[SDRC_VehicleHelper:SpawnGroupInVehicle] Spawned.", LogLevel.SPAM);
					break;
				}
			}
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Move multiple groups to a vehicle
	*/
    static void MoveGroupsInVehicle(array<SCR_AIGroup> groups, IEntity vehicle, bool forceTeleport = false)
    {
		if (!vehicle)
		{
			SDRC_Log.Add("[SDRC_VehicleHelper:MoveEntityInVehicle] Vehicle not available (null).", LogLevel.ERROR);
			return;
		}
		
		array<AIAgent> groupMembers  = new array<AIAgent>;
		
		int i = 0;
		
		foreach (SCR_AIGroup group : groups)
		{				
			if (group)
			{
				group.GetAgents(groupMembers);
				
				foreach (AIAgent aiAgent : groupMembers)
				{
					bool success = MoveEntityInVehicle(aiAgent, vehicle, i, forceTeleport);
					
					//Remove those AI that did not fit in the vehicle.
					if (!success)
					{
						SDRC_AIHelper.RemoveAIAgent(aiAgent);
						SDRC_Log.Add("[SDRC_VehicleHelper:MoveGroupsInVehicle] Failed to move AI to vehicle. Deleting.", LogLevel.DEBUG);
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
		if (!vehicle)
		{
			SDRC_Log.Add("[SDRC_VehicleHelper:MoveEntityInVehicle] Vehicle not available (null).", LogLevel.ERROR);
			return;
		}
		
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
					SDRC_Log.Add("[SDRC_VehicleHelper:MoveGroupsInVehicle] Failed to move AI to vehicle. Deleting.", LogLevel.DEBUG);
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
		if (!vehicle)
		{
			SDRC_Log.Add("[SDRC_VehicleHelper:MoveEntityInVehicle] Vehicle not available (null).", LogLevel.ERROR);
			return false;			
		}
		
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
		SDRC_Log.Add("[SDRC_VehicleHelper:MoveEntityInVehicle] Compartments found: " + compartments.Count(), LogLevel.SPAM);

		bool success = SetEntityInSlot(aiAgent, vehicle, compartments[slotIdx], forceTeleport);

		return success;
    }
	
	//------------------------------------------------------------------------------------------------
	/*!
	Ask AI to get out from vehicle. By default only cargo team is jumping out.
	\param groups Groups that were ordered to leave the chopper
	*/	
    static void GetOutVehicle(IEntity vehicle, out array<SCR_AIGroup> groups, ECompartmentType compartmentType = ECompartmentType.CARGO)
    {
		if (!vehicle)
		{
			SDRC_Log.Add("[SDRC_VehicleHelper:GetOutVehicle] Vehicle not available (null).", LogLevel.ERROR);
		}
		
		BaseCompartmentManagerComponent compartmentManager = BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(BaseCompartmentManagerComponent));
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		array<BaseCompartmentSlot> compartments = {};
		compartments.Clear();
		scr_compartmentManager.GetCompartmentsOfType(compartments, compartmentType);
		SDRC_Log.Add("[SDRC_VehicleHelper:GetOutVehicle] Compartments found: " + compartments.Count(), LogLevel.SPAM);

		array<EntityID> groupIds = {};
		
		
		foreach (int i, BaseCompartmentSlot compartment : compartments)
		{
			ChimeraCharacter character = ChimeraCharacter.Cast(compartment.GetOccupant());						
			if (!character)
			{
				continue;
			}
			
			AIControlComponent ctrl = AIControlComponent.Cast(character.FindComponent(AIControlComponent));			
			SCR_ChimeraAIAgent aiAgent = SCR_ChimeraAIAgent.Cast(ctrl.GetAIAgent());
			
			GetGame().GetCallqueue().CallLater(GetOutDelayed, i*1000, false, character, vehicle);
			
/*			
			CompartmentAccessComponent compAccess = SCR_CompartmentAccessComponent.Cast(character.GetCompartmentAccessComponent());
			if (compAccess)
			{
//				compAccess.GetOutVehicle(EGetOutType.ANIMATED, 0, false, false);
				compAccess.GetOutVehicle(EGetOutType.TELEPORT, 0, false, false);
			}
*/			
			//Collect the groups that were ordered to climb out. 
			SCR_AIGroup AIgroup = SCR_AIGroup.Cast(aiAgent.GetParentGroup());			
			
			EntityID groupId = AIgroup.GetID();
			
			if (!groupIds.Contains(groupId))
			{
				groupIds.Insert(groupId);
				groups.Insert(AIgroup);
				
//				AIGroup group = aiAgent.GetParentGroup();
//				AIGroup group = AIGroup.Cast(AIgroup);
				AIgroup.SetNewLeader(aiAgent);
			}
		}
    }	
	
	static void GetOutDelayed(ChimeraCharacter character, IEntity vehicle)
	{
		if ( (!character) || (!vehicle) )
		{
			return;
		}
		
		CompartmentAccessComponent compAccess = SCR_CompartmentAccessComponent.Cast(character.GetCompartmentAccessComponent());
		if (compAccess)
		{
			compAccess.GetOutVehicle(EGetOutType.ANIMATED, 0, false, false);
//			compAccess.GetOutVehicle(EGetOutType.TELEPORT, 0, false, false);
			
			// ///////////////////////////////////////////////////////////////////////////////////////////////
			//! New Edited ATiM- START
			//! Added "IEntity vehicle" to this ( GetOutDelayed ) method, used by SCR_AIMoveFromDangerBehavior.
			AIControlComponent controlComponent = character.GetAIControlComponent();
			
			if (controlComponent)
			{
				AIAgent agent = controlComponent.GetAIAgent();
				
				if (agent)
				{
					SCR_ChimeraAIAgent chimeraAIAgent = SCR_ChimeraAIAgent.Cast(agent);
					
					if (chimeraAIAgent)
					{					
						// compAccess.GetVehicleIn(vehicleEntity);
						
						SCR_AIUtilityComponent utilityComponent = chimeraAIAgent.m_UtilityComponent;
						
						if (utilityComponent)
						{
//							vector dangerPos = SDRC_Misc.RandomizePos(vehicle.GetOrigin(), 50);
							vector dangerPos = vehicle.GetOrigin();
							
//							SCR_AIMoveFromVehicleHornBehavior moveFromDangerBehavior = new SCR_AIMoveFromVehicleHornBehavior(utilityComponent, null, dangerPos, dangerEntity: vehicle);
							SCR_AIMoveFromDangerBehavior moveFromDangerBehavior = new SCR_AIMoveFromDangerBehavior(utilityComponent, null, dangerPos, dangerEntity: vehicle);
//							SCR_AIMoveFromUnsafeAreaBehavior moveFromDangerBehavior = new SCR_AIMoveFromUnsafeAreaBehavior(utilityComponent, null, vehicle.GetOrigin(), vehicle, 100);
							
							utilityComponent.AddAction(moveFromDangerBehavior);
							
							#ifdef WORKBENCH
								SCR_AIDebugVisualization.VisualizeMessage(utilityComponent.m_OwnerEntity, "SDRC_VehicleHelper > GetOutDelayed > SCR_AIMoveFromDangerBehavior", EAIDebugCategory.NONE, 1.0, Color.White, 11, true);
							#endif
						}
					}
				}
			}
			//! New Edited ATiM- END
			// ///////////////////////////////////////////////////////////////////////////////////////////////
		}
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
		
		bool success = accessComponent.GetInVehicle(vehicle, slot, forceTeleport, -1, ECloseDoorAfterActions.CLOSE_DOOR, true);
		if (success)
		{
			slot.SetReserved(character);			
		}
		
		return success;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Check if vehicle has atleast one pilot - dead or alive
	*/	
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
	
	//------------------------------------------------------------------------------------------------
	/*!
	Return the amount of pilots alive in the vehicle
	*/	
	static int PilotCountAlive(IEntity vehicle)
	{
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		array<IEntity> occupants = {};
		scr_compartmentManager.GetOccupantsOfType(occupants, ECompartmentType.PILOT);
		
		int alive = 0;
		
		foreach (IEntity pilot : occupants)
		{
			if (SCR_AIDamageHandling.IsAlive(pilot))
			{
				alive ++;
			}
		}

		return alive;
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Find all groups in vehicle
	*/
	static void GroupFindAll(IEntity vehicle, out array<AIGroup> groups)
	{
		if (vehicle)
		{			
			SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
			
			array<IEntity> agents = {};
			scr_compartmentManager.GetOccupants(agents);
			
			groups.Clear();
			
			foreach (IEntity agent : agents)
			{
				AIGroup group = SDRC_AIHelper.GetAIGroup(agent);
				
/*				AIGroup group;
				SCR_ChimeraCharacter chimeraChar = SCR_ChimeraCharacter.Cast(agent);
				if (!chimeraChar)
				{
					continue;
				}
				AIControlComponent aicc = chimeraChar.GetAIControlComponent();
				if (!aicc)
				{
					continue;
				}
				AIAgent aiAgent = aicc.GetControlAIAgent();
				if (!aiAgent)
				{
					continue;
				}
				
				group = aiAgent.GetParentGroup();*/
				
				if (group != null)
				{
					if (!groups.Contains(group))
					{
						groups.Insert(group);
					}
				}
			}
			
			SDRC_Log.Add("[SDRC_VehicleHelper:GroupFindAll] Groups in vehicle: " + groups.Count(), LogLevel.DEBUG);
		}
	}	
		
	//------------------------------------------------------------------------------------------------
	static bool IsVehicle(IEntity entity)
	{
		Vehicle vehicle = Vehicle.Cast(entity);

		if (!vehicle)
		{
			return false;
		}
				
		return true;
	}	
	
	//------------------------------------------------------------------------------------------------
	static bool IsWorking(IEntity vehicle)
	{
		if (SCR_AIVehicleUsability.VehicleCanMove(vehicle) && !SCR_AIVehicleUsability.VehicleIsOnFire(vehicle))
			return true;
	
		return false;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Empty vehicle storage
	*/
	static void EmptyStorage(IEntity entity)
	{
		//Vehicle vehicle = Vehicle.Cast(entity);
		if (!IsVehicle(entity))
		{
			return;
		}
		
		ScriptedInventoryStorageManagerComponent storageManager = ScriptedInventoryStorageManagerComponent.Cast(entity.FindComponent(ScriptedInventoryStorageManagerComponent));			
		if (storageManager)
		{			
			array<IEntity> items = {};
			storageManager.GetItems(items);
			
			foreach (IEntity item : items)
			{
				storageManager.TryDeleteItem(item);
			}
		}
		else
		{
			ResourceName res = entity.GetPrefabData().GetPrefabName();
			SDRC_Log.Add("[SDRC_LootHelper:EmptyStorage] storageManager not found on: " + SDRC_Misc.GetSimpleEntityName(res), LogLevel.ERROR);
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Get count of compartments of given type (PILOT, GUNNER, CARGO)
	*/
	static int GetCompartmentCountOfType(IEntity vehicle, ECompartmentType type)
	{
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(vehicle.FindComponent(SCR_BaseCompartmentManagerComponent));
		if (!scr_compartmentManager)
		{
			SDRC_Log.Add("[SDRC_VehicleHelper:GetCompartmentCountOfType] SCR_BaseCompartmentManagerComponent not found.", LogLevel.ERROR);
			return 0;
		}
		
		array<BaseCompartmentSlot> compartments = {};
		compartments.Clear();
		scr_compartmentManager.GetCompartmentsOfType(compartments, type);
		SDRC_Log.Add("[SDRC_VehicleHelper:GetCompartmentCountOfType] Found: " + compartments.Count() + " (" + type + ")", LogLevel.DEBUG);
		
		return compartments.Count();
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
	// Persistence related
	//------------------------------------------------------------------------------------------------
		
	//------------------------------------------------------------------------------------------------
	/*!
	Fix persistence for vehicle. For example for EPF.
	
	NOTE: You shall/should override this in your compat mod
	*/
	
	static void SetPersistence(IEntity entity, bool persistence = true)
	{	
		//Override in compat mod
		SDRC_SpawnHelper.SetPersistence(entity, persistence);
	}	
}