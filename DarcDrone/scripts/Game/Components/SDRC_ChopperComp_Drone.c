//SDRC_ChopperComp_Drone.c

//This is the modified component for drone use

//------------------------------------------------------------------------------------------------
class SDRC_ChopperParams_Drone : SDRC_ChopperParams
{	
	void SDRC_ChopperParams_Drone()
	{
		type = SDRC_EChopperType.DRONE;
			
		//Turn
		turnSpeedDivider = 45;
		turnTimeIntervalBase = 40;
	
		//Roll 
		rollAngleMul = 2.8;
		
		//Pitch
		pitchAngleRad 	 =  18 * Math.DEG2RAD;
		pitchAngleRadFlat  = -45 * Math.DEG2RAD;
		pitchNoseAngleDown = -40 * Math.DEG2RAD;
		pitchNoseAngleUp   =  20 * Math.DEG2RAD;
		
		//Rotor force multipliers
		rotorForceMulUp = 1.4 * 10;
		
		//Waypoint values
		wpSteepAngle = 30;
														
		destinationForwardInitial = 200;
		destinationForward = 100;
	}
}

//------------------------------------------------------------------------------------------------
modded class SDRC_ChopperComp
{
	private bool m_bRegistered = false;
	
	//------------------------------------------------------------------------------------------------
	/*!
	This sets up the flight model params for a specific SDRC_EChopperType. Override this function for other types.
	This is called immediately when component is initialized.
	Executed only on server!
	*/	
	override void TypeSetupParams(IEntity owner)
	{
		super.TypeSetupParams(owner);
		
		if (m_EntityType != SDRC_EChopperType.DRONE)
		{
			return;
		}
		
		params = new SDRC_ChopperParams_Drone();		
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	This is the setup for a specific SDRC_EChopperType. Override this function in other types
	This is a delayed setup make sure the entity is properly initialized. 
	Executed only on server!
	Called from OnPostInit.
	*/
	override void TypeSetup(IEntity owner)
	{
		super.TypeSetup(owner);
		
		if (m_EntityType != SDRC_EChopperType.DRONE)
		{
			return;
		}

		SetBehaviour(SDRC_EHeliBehaviour.SEARCH_AND_DESTROY_BEHAVIOUR, -1);
		
		array<ref SDRC_PlayerPos> playerPosArray = {};
		SDRC_PlayerHelper.GetPlayersClosestToPosition(playerPosArray, owner.GetOrigin(), 1000);
		if (!playerPosArray.IsEmpty())
		{
			SDRC_PlayerPos ppos = playerPosArray.GetRandomElement();
			AddDestination(SDRC_EFlyWayPointType.WP_SEARCH_DESTROY, ppos.pos, 600);			
		}

		SAL_DroneControllerComponent droneControllerComponent = SAL_DroneControllerComponent.Cast(owner.FindComponent(SAL_DroneControllerComponent));		
		if (droneControllerComponent)
		{
			SDRC_Log.Add("[SDRC_ChopperComp_Drone:TypeSetup] Done - DroneId: " + droneControllerComponent.m_DroneId, LogLevel.DEBUG);
		}				
		else
		{
			SDRC_Log.Add("[SDRC_ChopperComp_Drone:TypeSetup] Failed. Could not find SAL_DroneControllerComponent.", LogLevel.ERROR);
		}
		
		FactionAffiliationComponent factComp = FactionAffiliationComponent.Cast(owner.FindComponent(FactionAffiliationComponent));
		if (factComp)
		{
			string faction = SDRC_ChopperCrewHelper.SelectFaction(owner, faction);

			SAL_DroneConnectionManager dcm = SAL_DroneConnectionManager.GetInstance();
			if (dcm)
			{
				int droneId = RplComponent.Cast(owner.FindComponent(RplComponent)).Id();
				dcm.UpdateDroneFactionServer(droneId, faction);
				dcm.RpcDo_UpdateDroneFactionServer(droneId, faction);
			}
//			factComp.SetAffiliatedFactionByKey(faction);
		}
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Type specific things within EOnFrame. Override this function in other types.
	*/	
	override void TypeEOnFrame(IEntity owner, float timeSlice)
	{
		super.TypeEOnFrame(owner, timeSlice);
		
		if (m_EntityType != SDRC_EChopperType.DRONE)
		{
			return;
		}
		
		//Handle registration to drone manager.
		if (!m_bRegistered)
		{
			SAL_DroneControllerComponent droneControllerComponent = SAL_DroneControllerComponent.Cast(owner.FindComponent(SAL_DroneControllerComponent));		
			if (droneControllerComponent)
			{
				if (droneControllerComponent.m_DroneId != -1)
				{
					SDRC_Log.Add("[SDRC_ChopperComp_Drone:TypeEOnFrame] Registering DroneId: " + droneControllerComponent.m_DroneId, LogLevel.DEBUG);
					droneControllerComponent.m_DroneManager.m_aActiveDrones.Insert(droneControllerComponent.m_DroneId);
					m_bRegistered = true;
				}
			}
		}
		
		//Control drone rotor speed
//		InputManager m_InputManager = GetGame().GetInputManager();
		//m_InputManager.SetActionValue("DroneUp", 1.0);
//		m_InputManager.SetActionValue("DroneUp", 0.0);
	}	
	
	//------------------------------------------------------------------------------------------------	
	// Misc
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get scaled health
	*/	
	override void TypeGetHealthScaled(IEntity owner, out float health)
	{
		super.TypeGetHealthScaled(owner, health);
		
		if (m_EntityType != SDRC_EChopperType.DRONE)
		{
			return;
		}

		SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.Cast(owner.FindComponent(SCR_DamageManagerComponent));
		SAL_DroneControllerComponent droneControllerComponent = SAL_DroneControllerComponent.Cast(owner.FindComponent(SAL_DroneControllerComponent));		
		
		health = 1;
		if ( (damageManager) && (droneControllerComponent) )
		{
			if (damageManager.IsDestroyed())
			{
				SDRC_Log.Add("[SDRC_ChopperComp_Drone:TypeGetHealthScaled] Drone destroyer - DroneId: " + droneControllerComponent.m_DroneId, LogLevel.DEBUG);
				
				droneControllerComponent.ArmDrone();						//The second call 'de-Arms the drone'.
				
				//InputManager m_InputManager = GetGame().GetInputManager();			
				//m_InputManager.SetActionValue("DroneUp", 0.0);	
				
				SAL_DroneSoundComponent soundComponent = SAL_DroneSoundComponent.Cast(owner.FindComponent(SAL_DroneSoundComponent));
				soundComponent.ShutOffEngine();
				
				damageManager.SetHealthScaled(0);
				health = 0;
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Handle the final parts after damage that breaks flying
	*/	
	override void TypeHandleDamageFinal(IEntity owner)
	{
		super.TypeHandleDamageFinal(owner);
		
		if (m_EntityType != SDRC_EChopperType.DRONE)
		{
			return;
		}
		
		//Drop the grenade
		SAL_DroneControllerComponent droneControllerComponent = SAL_DroneControllerComponent.Cast(owner.FindComponent(SAL_DroneControllerComponent));		
		if (droneControllerComponent)
		{
			DroneGrenade(droneControllerComponent.m_DroneId);
		}
		
		//Delete the drone
		delete owner;
	}
	
	//------------------------------------------------------------------------------------------------	
	// State handling
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Handle attacks
	- Normal case: If enemy is seen, consider shooting
	- Attack case: The location to bomb has been assigned. (m_vAttackPosition)
	*/
	override void HandleAttack(IEntity owner)
	{
		super.HandleAttack(owner);
		
		if (m_EntityType != SDRC_EChopperType.DRONE)
		{
			return;
		}

		float chance = 0.5;
		int distance = 10;

		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (m_BaseGameMode)
		{
			
			chance = m_BaseGameMode.chopperFrame.m_Config.drone.dropChance;
			distance = m_BaseGameMode.chopperFrame.m_Config.drone.dropDistanceToPlayer;
		}		
		
		if (SDRC_PlayerHelper.IsAnyPlayerCloseToPos(owner.GetOrigin(), distance, 0))
		{
			if (SDRC_Misc.RandomFloat(0, 1) < chance)
			{
				SAL_DroneControllerComponent droneControllerComponent = SAL_DroneControllerComponent.Cast(owner.FindComponent(SAL_DroneControllerComponent));		
				if (droneControllerComponent)
				{
					DroneGrenade(droneControllerComponent.m_DroneId);
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	// DRONE specific stuff
	//------------------------------------------------------------------------------------------------
		
	//------------------------------------------------------------------------------------------------
	/*!
	Handles grenadedrop
	*/	
	private void DroneGrenade(RplId droneId)
	{
		if (!Replication.FindItem(droneId))
			return;
		
		IEntity drone = RplComponent.Cast(Replication.FindItem(droneId)).GetEntity();
		if (!drone)
			return;
		
		SAL_DropperComponent dropperComp = SAL_DropperComponent.Cast(drone.FindComponent(SAL_DropperComponent));
		if (!dropperComp)
			return;
		
		SlotManagerComponent slotComp = SlotManagerComponent.Cast(drone.FindComponent(SlotManagerComponent));
		if (!slotComp)
			return;
		
		IEntity grenade = slotComp.GetSlotByName("GrenadeDropper").GetAttachedEntity();
		if (grenade == null)
			return;
		
		vector transform[4];
		grenade.GetTransform(transform);
		SCR_EntityHelper.DeleteEntityAndChildren(grenade);
		dropperComp.m_BGrenadeDropped = true;
		
		EntitySpawnParams spawnParams = EntitySpawnParams();
		spawnParams.Transform = transform;
		GetGame().SpawnEntityPrefab(Resource.Load(dropperComp.m_DropperGrenade), GetGame().GetWorld(), spawnParams);
		SAL_DroneConnectionManager.GetInstance().DropGrenadeBroadcast(droneId);		
	}
}