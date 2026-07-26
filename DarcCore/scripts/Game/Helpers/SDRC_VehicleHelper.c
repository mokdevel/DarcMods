//Helpers SDRC_VehicleHelper

//------------------------------------------------------------------------------------------------
/*!
Functions related to vehicles.
*/
class SDRC_VehicleHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn single group to a vehicle
	\param aiResourceName
	\param vehicle
	\param group
	\param compartmentType The compartment to spawn to. -1 = any free compartment.
	*/
	static void SpawnGroupInVehicle(ResourceName aiResourceName, IEntity vehicle, AIGroup group, string faction, ECompartmentType compartmentType = -1)
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
		
		if (compartmentType == -1)
		{
			scr_compartmentManager.GetCompartmentsOfType(compartments, ECompartmentType.PILOT);
			scr_compartmentManager.GetCompartmentsOfType(compartments, ECompartmentType.TURRET);
			scr_compartmentManager.GetCompartmentsOfType(compartments, ECompartmentType.CARGO);
		}
		else 
		{
			scr_compartmentManager.GetCompartmentsOfType(compartments, compartmentType);
		}
		
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

		SDRC_AIHelper.GroupHandleRandomized(aiPrefabs, faction);
					
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
					// This fixes the issue where AI is spawned invisible and not entering the vehicle before GM is moving. 
					// Thanks to Gramps for finding the fix.
					// ---
					SCR_ChimeraCharacter chimeraCharacter = SCR_ChimeraCharacter.Cast(character);
					CharacterControllerComponent characterController = chimeraCharacter.GetCharacterController();			
					AIControlComponent aiControlComponent = characterController.GetAIControlComponent();
					AIAgent aiAgent = aiControlComponent.GetControlAIAgent();			
					aiAgent.SetLOD(1);					
					// ---
					
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
		
		if (!aiAgent)
		{
			SDRC_Log.Add("[SDRC_VehicleHelper:MoveEntityInVehicle] aiAgent not available (null).", LogLevel.ERROR);
			return false;			
		}
		// This fixes the issue where AI is spawned invisible and not entering the vehicle before GM is moving. 
		// Thanks to Gramps for finding the fix.
		aiAgent.SetLOD(1);
		
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
			
			if (!aiAgent)
			{
				continue;
			}
			
			GetGame().GetCallqueue().CallLater(GetOutDelayed, (i * 500), false, character, vehicle);
			
			//Collect the groups that were ordered to climb out. 
			SCR_AIGroup AIgroup = SCR_AIGroup.Cast(aiAgent.GetParentGroup());					
			EntityID groupId = AIgroup.GetID();
			
			if (!groupIds.Contains(groupId))
			{
				groupIds.Insert(groupId);
				groups.Insert(AIgroup);
			}
		}
    }	
	
	//------------------------------------------------------------------------------------------------
	static void GetOutDelayed(ChimeraCharacter character, IEntity vehicle)
	{
		if ( (!character) || (!vehicle) )
		{
			return;
		}
		
		CompartmentAccessComponent compAccess = SCR_CompartmentAccessComponent.Cast(character.GetCompartmentAccessComponent());
		if (compAccess)
		{
//			compAccess.GetOutVehicle(EGetOutType.ANIMATED, 0, false, false);
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
						chimeraAIAgent.m_bIsGetOutVehicle = true;
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
		
		// This fixes the issue where AI is spawned invisible and not entering the vehicle before GM is moving. 
		// Thanks to Gramps for finding the fix.
		aiAgent.SetLOD(1);
		
		bool success = accessComponent.GetInVehicle(vehicle, slot, forceTeleport, -1, ECloseDoorAfterActions.CLOSE_DOOR, true);
		//bool success = accessComponent.GetInVehicle(vehicle, slot, forceTeleport, -1, ECloseDoorAfterActions.INVALID, false);
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
		
		if (!scr_compartmentManager)
		{
			return 0;
		}		
		
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
			
			if (!scr_compartmentManager)
			{
				return;
			}
			
			array<IEntity> agents = {};
			scr_compartmentManager.GetOccupants(agents);
			
			groups.Clear();
			
			foreach (IEntity agent : agents)
			{
				AIGroup group = SDRC_AIHelper.GetAIGroup(agent);
				
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
	// Damage settings
	//------------------------------------------------------------------------------------------------	
			
	//------------------------------------------------------------------------------------------------
	static float GetHealth(IEntity owner)
	{
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
		if (!damageManager)
		{
			return 10000;
		}

		float health = 0;			//Total health for each hitzone
		
       	array<HitZone> hitzones = {};
       	int count = damageManager.GetAllHitZones(hitzones);		
		
		foreach (HitZone hitZone : hitzones)
		{
			health += hitZone.GetHealth();
		}
		
/*		//This is the simple solution. Not really usable
		
		float health = -1;
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
		if (damageManager)
		{
			health = damageManager.GetHealth();
			damageManager.GetState()			
		}*/
		
		return health;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get vehicle scaled damage. Value is between 0..1 and is an average of all hitzones.
	*/
	static float GetHealthScaled(IEntity owner, bool simpleCounting = true)
	{
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
		if (!damageManager)
		{
			return 1;
		}

		float healthScaled = 1.0;	//Scaled damage - set to 100% as default
		float healthTotal = 0;		//Total damage used for calculations
		
       	array<HitZone> hitzones = {};
       	int count = damageManager.GetAllHitZones(hitzones);		
		
		if (simpleCounting)
		{
			//Counts everything including rotors etc
			foreach (HitZone hitZone : hitzones)
			{
				healthTotal += hitZone.GetHealthScaled();
			}			
		}
		else
		{
			//Only care about engine and geabox
			count = 0;
			foreach (HitZone hitZone : hitzones)
			{
				SCR_EngineHitZone hz_e = SCR_EngineHitZone.Cast(hitZone);
				if (hz_e)
				{
					healthTotal += hitZone.GetHealthScaled();
					count++;
					continue;
				}
					
				SCR_GearboxHitZone hz_g = SCR_GearboxHitZone.Cast(hitZone);
				if (hz_g)
				{
					healthTotal += hitZone.GetHealthScaled();
					count++;
					continue;
				}

				/*SCR_RotorHitZone hz_r = SCR_RotorHitZone.Cast(hitZone);
				if (hz_r)
				{
					healthTotal += hitZone.GetHealthScaled();	//Rotor damage affects only 25%
					count++;
					continue;
				}*/
			}
		}

		if (count > 0)
		{
			healthScaled = healthTotal/count;
		}
		
		return healthScaled;
	}
	
	//------------------------------------------------------------------------------------------------
	static EDamageState GetDamageState(IEntity owner)
	{
		EDamageState state = EDamageState.UNDAMAGED;
		
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
		if (damageManager)
		{
			state = damageManager.GetState();			
		}
		
		return state;
	}
	
	//------------------------------------------------------------------------------------------------
	static void SetHealth(IEntity owner, float health)
	{
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(owner.FindComponent(DamageManagerComponent));
		if (damageManager)
		{
			damageManager.SetHealthScaled(health);
		}
		
		SDRC_Log.Add("[SDRC_VehicleHelper:SetHealth] Setting health: " + health, LogLevel.DEBUG);
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

//------------------------------------------------------------------------------------------------
/*!
New modded "ChimeraAIAgent" class ATiM-
*/
//------------------------------------------------------------------------------------------------
modded class SCR_ChimeraAIAgent : ChimeraAIAgent
{
	IEntity m_VehicleEntity = null;
	bool m_bIsGetOutVehicle = false;
}

//------------------------------------------------------------------------------------------------
/*!
New modded "BehaviorBase" class ATiM-
*/
//------------------------------------------------------------------------------------------------
modded class SCR_AIBehaviorBase : SCR_AIActionBase
{
	//---------------------------------------------------------------------------------------------------------------------------------
	void SCR_AIBehaviorBase(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity)
	{
		m_bUseCombatMove = true;
	}
}

//------------------------------------------------------------------------------------------------
/*!
New modded "GetOutVehicle" class ATiM-
*/
//------------------------------------------------------------------------------------------------
modded class SCR_AIGetOutVehicle : SCR_AIVehicleBehavior
{
	protected IEntity m_VehicleEntity;
	
	protected bool m_bIsGetOutVehicle;
	
	protected SCR_ChimeraAIAgent m_SDRC_ChimeraAIAgent;
	
	//------------------------------------------------------------------------------------------------
	void SCR_AIGetOutVehicle(SCR_AIUtilityComponent utility, SCR_AIActivityBase groupActivity, IEntity vehicleEntity, float delay_s = 0, float priority = PRIORITY_BEHAVIOR_GET_OUT_VEHICLE, float priorityLevel = PRIORITY_LEVEL_NORMAL)
	{
		// m_VehicleEntity = vehicleEntity;
		
		if (utility)
		{
			AIAgent agent = utility.GetAIAgent();
			
			if (agent)
			{
				m_SDRC_ChimeraAIAgent = SCR_ChimeraAIAgent.Cast(agent);
				
				if (m_SDRC_ChimeraAIAgent)
				{
					m_VehicleEntity = vehicleEntity;
					
					// m_SDRC_ChimeraAIAgent.m_VehicleEntity = vehicleEntity;
					
					// m_bIsGetOutVehicle = m_SDRC_ChimeraAIAgent.m_bIsGetOutVehicle;
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActionRemoved()
	{
		super.OnActionRemoved();
		
		if (m_SDRC_ChimeraAIAgent)
		{
			m_bIsGetOutVehicle = m_SDRC_ChimeraAIAgent.m_bIsGetOutVehicle;
			
			if (m_bIsGetOutVehicle)
			{
				MoveFromVehicle();
				
				m_SDRC_ChimeraAIAgent.m_bIsGetOutVehicle = false;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void MoveFromVehicle()
	{
//		IEntity vehicleEntity;
//		
//		if (m_SDRC_ChimeraAIAgent)
//			vehicleEntity = chimeraAIAgent.m_VehicleEntity;
//		
//		vehicleEntity = m_VehicleEntity;
		
		if (m_VehicleEntity)
		{
			m_bUseCombatMove = true;
			
			vector origin = m_Utility.GetOrigin();
			
			SCR_AICombatMoveRequest_Move rq = new SCR_AICombatMoveRequest_Move();
			
			rq.m_eReason = SCR_EAICombatMoveReason.MOVE_FROM_DANGER;
			
			rq.m_vTargetPos = m_VehicleEntity.GetOrigin();
			
			vector vectorAwayFromVehicle = vector.Direction(rq.m_vTargetPos, origin);
			
			vectorAwayFromVehicle.Normalize();
			
			float asideMoveDistance = 5.0;
			
			vector movePos = origin + asideMoveDistance * vectorAwayFromVehicle;
			
			rq.m_vMovePos = movePos;
			
			//------------------------------------------------------------------------------------------------
			rq.m_bTryFindCover = true;
			rq.m_bFailIfNoCover = false;
			
			rq.m_fCoverSearchDistMin = 15.0;
			rq.m_fCoverSearchDistMax = 30.0;
			
			rq.m_bCheckCoverVisibility = false;
			rq.m_bUseCoverSearchDirectivity = false;
			
			//------------------------------------------------------------------------------------------------
			rq.m_bAimAtTarget = false;
			
			rq.m_eMovementType = EMovementType.SPRINT;
			
			rq.m_fMoveDuration_s = Math.RandomFloat(1.5, 3.0);
			
			if (Math.RandomFloat01() < 0.5)
				rq.m_fMoveDuration_s *= 1.5;
			
			if (Math.RandomFloat01() < 0.5)
				rq.m_eStanceMoving = ECharacterStance.STAND;
			else
				rq.m_eStanceMoving = ECharacterStance.CROUCH;
			
			if (Math.RandomFloat01() < 0.5)
				rq.m_eStanceEnd = ECharacterStance.STAND;
			else
				rq.m_eStanceEnd = ECharacterStance.CROUCH;
			
			rq.m_fCoverSearchSectorHalfAngleRad = 0.5 * Math.PI;
			
			rq.m_eDirection = SCR_EAICombatMoveDirection.FORWARD;
			
			// rq.m_eDirection = SCR_EAICombatMoveDirection.ANYWHERE;
			
			if (Math.RandomFloat01() < 0.5)
			{
				if (Math.RandomFloat01() < 0.5)
					rq.m_eDirection = SCR_EAICombatMoveDirection.LEFT;
				else
					rq.m_eDirection = SCR_EAICombatMoveDirection.RIGHT;
				
				if (rq.m_eDirection == SCR_EAICombatMoveDirection.LEFT || rq.m_eDirection == SCR_EAICombatMoveDirection.LEFT)
					rq.m_fCoverSearchSectorHalfAngleRad = 0.3 * Math.PI;
			}
			
			//------------------------------------------------------------------------------------------------
			if (Math.RandomIntInclusive(0, 1) == 1)
				rq.m_bAimAtTargetEnd = true;
			else
				rq.m_bAimAtTargetEnd = false;
			
			m_Utility.m_CombatMoveState.ApplyNewRequest(rq);
			
			#ifdef WORKBENCH
//				SCR_AIDebugVisualization.VisualizeMessage(m_Utility.m_OwnerEntity, "SCR_AIIdleBehavior > OnActionSelected > SCR_AIMoveFromDangerBehavior", EAIDebugCategory.NONE, 1.0, Color.White, 13, true);
			#endif
		}
	}
}