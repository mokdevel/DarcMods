//SDRC_ChopperComp_Drone.c

//This is the modified component for drone use

modded enum SDRC_EChopperSubType
{
	DRONE_DROPPER,
	DRONE_CRASHER,
}

//------------------------------------------------------------------------------------------------
class SDRC_ChopperParams_Drone : SDRC_ChopperParams
{	
	void SDRC_ChopperParams_Drone()
	{
		type = SDRC_EChopperType.DRONE;
		//NOTE: SubType has been set in the prefab
		
		//Turn
		turnSpeedDivider = 45;
		turnSpeedDegreeMin = 1;
		turnSpeedDegreeMax = 90;
		turnTimeIntervalBase = 40;
	
		//Roll 
		rollAngleMul = 2.8;
		
		//Pitch
		pitchAngleRad 	   =  10 * Math.DEG2RAD;
		pitchAngleRadFlat  = -85 * Math.DEG2RAD;	//0 points XX degress downwards
		pitchNoseAngleDown = pitchAngleRadFlat + (10 * Math.DEG2RAD);
		pitchNoseAngleUp   = pitchAngleRadFlat - (60 * Math.DEG2RAD);
		
		//Rotor force multipliers
		fRotorForceMulUp = 1.4 * 10;
		iRotorForceNormal = 30;
		iRotorForceRaise = 40;
		iRotorForceHover = 1;
		iRotorForceCrash = 10;
		
		//Obstacle awareness
		rayLenFront = 200;
		rayDown = 50;		
				
		//Flight parameters
		detourLerpPosition = 0.4;
		detourDivider = 5.5;
		
		//Attack and enemy related
		rayLenEnemy = 200;
		timeSearchAndDestroy = 120;
		attackHeightMul = -0.3;
		attackDefaultTime = 60;
		
		//Damage levels
		damageHeavy = 0.10;
		damageMedium = 0.50;
		damageLight = 0.80;
		
		//Waypoint values
		wpSteepAngle = 30;
				
		destinationForwardInitial = 100;
		destinationForward = 100;
		
		//Flight pattern related
		patrolRadius = 100;
	}
}

//------------------------------------------------------------------------------------------------
modded class SDRC_ChopperComp
{
	private bool m_bRegistered = false;
	private bool m_bAttackDone = false;
	
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

//		SetBehaviour(SDRC_EHeliBehaviour.SEARCH_AND_DESTROY_BEHAVIOUR, -1);
//		SetBehaviour(SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR, -1);
		
/*		array<ref SDRC_PlayerPos> playerPosArray = {};
		SDRC_PlayerHelper.GetPlayersClosestToPosition(playerPosArray, owner.GetOrigin(), 1000);
		if (!playerPosArray.IsEmpty())
		{
			SDRC_PlayerPos ppos = playerPosArray.GetRandomElement();
			AddDestination(SDRC_EFlyWayPointType.WP_SEARCH_DESTROY, ppos.pos, 600);
		} */
		
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
				SDRC_Log.Add("[SDRC_ChopperComp_Drone:TypeSetup] Faction: " + faction, LogLevel.DEBUG);
			}
//			factComp.SetAffiliatedFactionByKey(faction);
		}
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Type specific init finalization
	*/	
	override void TypeInitDone(IEntity owner)
	{
		super.TypeInitDone(owner);
		
		if (m_EntityType != SDRC_EChopperType.DRONE)
		{
			return;
		}

/*		//Set a search and destroy location
		int searchDistance = 300;
		int searchTime = 600;		
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (m_BaseGameMode)
		{
			searchDistance = m_BaseGameMode.chopperFrame.m_Config.drone.searchDistance;
			searchTime = m_BaseGameMode.chopperFrame.m_Config.drone.searchTime;
		}		
		vector destination = SDRC_ChopperHelper.GetDestinationForward(owner, searchDistance);
		AddDestination(SDRC_EFlyWayPointType.WP_SEARCH_DESTROY, destination, searchTime);				*/
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
				SDRC_Log.Add("[SDRC_ChopperComp_Drone:TypeGetHealthScaled] Drone destroyed: " + droneControllerComponent.m_DroneId, LogLevel.DEBUG);
				
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
	Setup attacks. Search for the enemy and then react on the finding.
	
	- Normal case: If enemy is seen, consider dropping the grenade.
	*/
	override void TypeAttackSetup(IEntity owner, vector hostilePos = vector.Zero)
	{
		super.TypeAttackSetup(owner, hostilePos);
		
		if (m_EntityType != SDRC_EChopperType.DRONE)
		{
			return;
		}

		//For dropper drone, the attack position needs to a bit further than the one defined. We want a fly by towards or over the player.
		if (m_EntitySubType == SDRC_EChopperSubType.DRONE_DROPPER)
		{				
			//Move it along the flight path.
			vector fromPos = m_vOrigin;
			if (!m_vSplinePoints.IsEmpty())
			{
				fromPos = m_vSplinePoints[m_vSplinePoints.Count() - 1];
			}
			vector direction = vector.Direction(fromPos, m_vAttackPosition);
			direction.Normalize();
			direction[1] = 0;		//Move only on XZ plane
			m_vAttackPosition = m_vAttackPosition + (direction * 100);
		}
		
		//For dropper drone, the attack position needs to a bit further than the one defined. We want a fly by towards or over the player.
		if (m_EntitySubType == SDRC_EChopperSubType.DRONE_CRASHER)
		{
			vector rndPos = vector.Zero; 
			rndPos[0] = SDRC_Misc.RandomInt(-params.patrolRadius, params.patrolRadius);
			rndPos[2] = SDRC_Misc.RandomInt(-params.patrolRadius, params.patrolRadius);
			
			AddDestination(SDRC_EFlyWayPointType.WP_FLY_IMMEDIATELY, owner.GetOrigin() + rndPos); 
			AddDestination(SDRC_EFlyWayPointType.WP_CRASH, m_vAttackPosition); 
			AddDestination(SDRC_EFlyWayPointType.WP_END); 					
			SetBehaviour(SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR, -1);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Handle attacks. Search for the enemy and then react on the finding.
	
	- Normal case: If enemy is seen, consider dropping the grenade.
	*/
	override void TypeHandleAttack(IEntity owner)
	{
		super.TypeHandleAttack(owner);
		
		if (m_EntityType != SDRC_EChopperType.DRONE)
		{
			return;
		}

		if (m_bAttackDone)
		{
			return;
		}
		
		SDRC_ChopperEnemyHelper.SearchForEnemy(owner);
		
		if (m_vEnemyPosition == vector.Zero)
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
		
		if (SDRC_Misc.RandomFloat(0, 1) > chance)
		{
			return;
		}
		
		switch (m_EntitySubType)
		{
			case SDRC_EChopperSubType.DRONE_DROPPER:
			{
				//Any target near position?
				if (SDRC_PlayerHelper.IsAnyPlayerCloseToPos(owner.GetOrigin(), distance, 0))
				{
					SAL_DroneControllerComponent droneControllerComponent = SAL_DroneControllerComponent.Cast(owner.FindComponent(SAL_DroneControllerComponent));		
					if (droneControllerComponent)
					{
						DroneGrenade(droneControllerComponent.m_DroneId);
						AddDestination(SDRC_EFlyWayPointType.WP_FLY_AWAY_IMMEDIATELY); 
						SetBehaviour(SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR, -1);
						m_bAttackDone = true;
					}
				}
				
				//If on a very low altitude, drop grenade and self destruct
				if (m_fAltitude < 2)
				{
					SAL_DroneControllerComponent droneControllerComponent = SAL_DroneControllerComponent.Cast(owner.FindComponent(SAL_DroneControllerComponent));		
					if (droneControllerComponent)
					{
						DroneGrenade(droneControllerComponent.m_DroneId);
						AddDestination(SDRC_EFlyWayPointType.WP_FLY_AWAY_IMMEDIATELY); 
						SetBehaviour(SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR, -1);
						m_bAttackDone = true;
					}
				}
				break;
			}
			case SDRC_EChopperSubType.DRONE_CRASHER:
			{
				//Any target near position?
				vector pos = SDRC_PlayerHelper.AnyPlayerPosCloseToPos(owner.GetOrigin(), distance * 10, 0);
				
				if (pos != vector.Zero)
				{
					AddDestination(SDRC_EFlyWayPointType.WP_ATTACK, pos); 
					m_bAttackDone = true;					
				}
				break;
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