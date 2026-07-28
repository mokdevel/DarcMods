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
	SAL_DroneControllerComponent m_DroneControllerComponent;

	//------------------------------------------------------------------------------------------------
	/*!
	This sets up the flight model params for a specific SDRC_EChopperType. Override this function for other types.
	This is called immediately when component is initialized.
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
	*/
	override void TypeSetup(IEntity owner)
	{
		super.TypeSetup(owner);
		
		if (m_EntityType != SDRC_EChopperType.DRONE)
		{
			return;
		}

		//DRONE specific
		m_DroneControllerComponent = SAL_DroneControllerComponent.Cast(owner.FindComponent(SAL_DroneControllerComponent));		
		if (m_DroneControllerComponent)
		{
			m_DroneControllerComponent.ArmDrone();
			m_DroneControllerComponent.m_bIsActive = true;
//			m_DroneControllerComponent.m_bIsArmed = true;			
			m_DroneControllerComponent.m_iOwner = -2;			//Set it as random ID. Shall not match a real player.
			m_DroneControllerComponent.m_bIsConnected = true;
/*			m_DroneControllerComponent.m_iOwner = GetGame().GetPlayerController();
			if (m_DroneControllerComponent.m_iOwner == -1)
			{
				m_DroneControllerComponent.m_iOwner = 0;
			}*/
			//m_DroneControllerComponent.m_InputManager.SetActionValue("DroneUp", 3.0);
		}
		else
		{
			SDRC_Log.Add("[SDRC_ChopperComp_Drone:TypeSetup] SAL_DroneControllerComponent not found! Aborting. ", LogLevel.ERROR);
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
		
		SDRC_Log.Add("[SDRC_ChopperComp_Drone:TypeSetup] Done - DroneId: " + m_DroneControllerComponent.m_DroneId, LogLevel.DEBUG);
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
			if (m_DroneControllerComponent)
			{
				if (m_DroneControllerComponent.m_DroneId != -1)
				{
					SDRC_Log.Add("[SDRC_ChopperComp_Drone:TypeEOnFrame] Registering DroneId: " + m_DroneControllerComponent.m_DroneId, LogLevel.DEBUG);
					m_DroneControllerComponent.m_DroneManager.m_aActiveDrones.Insert(m_DroneControllerComponent.m_DroneId);
					m_bRegistered = true;
				}
			}
		}
		
		//Control drone rotor speed
		InputManager m_InputManager = GetGame().GetInputManager();
		m_InputManager.SetActionValue("DroneUp", 1.0);
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

		SCR_DamageManagerComponent m_DamageManager = SCR_DamageManagerComponent.Cast(owner.FindComponent(SCR_DamageManagerComponent));

		health = 1;
		if (m_DamageManager.IsDestroyed())
		{
			SDRC_Log.Add("[SDRC_ChopperComp_Drone:TypeGetHealthScaled] Drone destroyer - DroneId: " + m_DroneControllerComponent.m_DroneId, LogLevel.DEBUG);
			
			m_DroneControllerComponent.ArmDrone();						//The second call 'de-Arms the drone'.
			
			InputManager m_InputManager = GetGame().GetInputManager();			
			m_InputManager.SetActionValue("DroneUp", 0.0);	
			
			SAL_DroneSoundComponent soundComponent = SAL_DroneSoundComponent.Cast(owner.FindComponent(SAL_DroneSoundComponent));
			soundComponent.ShutOffEngine();
			
			m_DamageManager.SetHealthScaled(0);
			health = 0;
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
		DroneGrenade(m_DroneControllerComponent.m_DroneId);
		
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
		
		if (SDRC_PlayerHelper.IsAnyPlayerCloseToPos(owner.GetOrigin(), 10, 0))
		{
			if (SDRC_Misc.RandomFloat(0, 1) < 0.50)
			{
				DroneGrenade(m_DroneControllerComponent.m_DroneId);
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