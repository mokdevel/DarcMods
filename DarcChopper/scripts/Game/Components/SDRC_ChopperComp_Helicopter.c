//SDRC_ChopperComp_Helicopter.c

//This is the modified component for helicopter use

//------------------------------------------------------------------------------------------------
class SDRC_ChopperParams_Helicopter : SDRC_ChopperParams
{
	void SDRC_ChopperParams_Helicopter()
	{
		type = SDRC_EChopperType.HELICOPTER;
				
		//Turn
		turnSpeedDivider = 92;
		turnSpeedDegreeMin = 1;
		turnSpeedDegreeMax = 90;
		turnTimeIntervalBase = 60;
	
		//Roll 
		rollAngleMul = 2.4;
		
		//Pitch
		pitchAngleRad 	   =  5 * Math.DEG2RAD;
		pitchAngleRadFlat  = -85 * Math.DEG2RAD;	//0 points XX degress downwards
		pitchNoseAngleDown = pitchAngleRadFlat + (10 * Math.DEG2RAD);
		pitchNoseAngleUp   = pitchAngleRadFlat - (80 * Math.DEG2RAD);
		
		//Rotor force multipliers
		rotorForceMulUp = 1.3 * 10;
		rotorForceNormal = 30;
		rotorForceRaise = 40;
		rotorForceHover = 0;

		//Obstacle awareness
		rayLenFront = 400;
		rayDown = 50;
				
		//Flight parameters
		detourLerpPosition = 0.4;
		detourDivider = 5.5;
		
		//Attack and enemy related
		rayLenEnemy = 1000;
		timeSearchAndDestroy = 600;
		attackHeightMul = 0.5;
		
		//Damage levels
		damageHeavy = 0.90;
		damageMedium = 0.96;
		damageLight = 0.98;
		
		//Waypoint values
		wpSteepAngle = 60;
														
		destinationForwardInitial = 400;
		destinationForward = 200;
		
		//Flight pattern related
		patrolRadius = 300;	
	}
}

//------------------------------------------------------------------------------------------------
modded class SDRC_ChopperComp
{
	//------------------------------------------------------------------------------------------------
	/*!
	This sets up the flight model params for a specific SDRC_EChopperType. Override this function for other types.
	This is called immediately when component is initialized.
	Executed only on server!
	Called from OnPostInit.
	*/	
	override void TypeSetupParams(IEntity owner)
	{
		super.TypeSetupParams(owner);
		
		if (m_EntityType != SDRC_EChopperType.HELICOPTER)
		{
			return;
		}
		
		params = new SDRC_ChopperParams_Helicopter();
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	This is the setup for a specific SDRC_EChopperType. Override this function in other types.
	This is a delayed setup make sure the entity is properly initialized. 
	Executed only on server!
	*/
	override void TypeSetup(IEntity owner)
	{
		super.TypeSetup(owner);
		
		if (m_EntityType != SDRC_EChopperType.HELICOPTER)
		{
			return;
		}

		//HELICOPTER specific
						
		//Set engine on		
		SetEngine(true, m_fThrottle, m_fRotorForce0, m_fRotorForce1);
		
		if (m_RocketPrefabs.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_ChopperComp] No rockets available.", LogLevel.NORMAL);
		}
		else
		{
			m_RocketPrefab = m_RocketPrefabs.GetRandomElement();
			SDRC_Log.Add("[SDRC_ChopperComp] Using rockets: " + SDRC_Misc.GetSimpleEntityName(m_RocketPrefab), LogLevel.NORMAL);
		}
	
		//SDRC_ChopperEnemyHelper.GetWeapons(owner);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Type specific init finalization
	*/	
	override void TypeInitDone(IEntity owner)
	{
		super.TypeInitDone(owner);
		
		if (m_EntityType != SDRC_EChopperType.HELICOPTER)
		{
			return;
		}
		
		/* NOTHING NEEDED FOR HELICOPTER */
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Type specific things within EOnFrame. Override this function in other types.
	*/	
	override void TypeEOnFrame(IEntity owner, float timeSlice)
	{
		super.TypeEOnFrame(owner, timeSlice);
		
		if (m_EntityType != SDRC_EChopperType.HELICOPTER)
		{
			return;
		}
		
		/* NOTHING NEEDED FOR HELICOPTER */
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
		
		if (m_EntityType != SDRC_EChopperType.HELICOPTER)
		{
			return;
		}

		health = SDRC_VehicleHelper.GetHealthScaled(owner, false);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Handle the final parts after damage that breaks flying
	*/	
	override void TypeHandleDamageFinal(IEntity owner)
	{
		super.TypeHandleDamageFinal(owner);
		
		if (m_EntityType != SDRC_EChopperType.HELICOPTER)
		{
			return;
		}
		
		//Make the chopper fly unsteadily
		VehicleHelicopterSimulation helicopter_s = VehicleHelicopterSimulation.Cast(owner.GetRootParent().FindComponent(VehicleHelicopterSimulation));
		if (helicopter_s)
		{
			float force = SDRC_Misc.RandomFloat(0, 0.1);
	        helicopter_s.RotorSetForceScaleState(0, force);
			force = SDRC_Misc.RandomFloat(0.1, 2.5);
	        helicopter_s.RotorSetForceScaleState(1, force);
			force = SDRC_Misc.RandomFloat(0.0, 0.1);
			helicopter_s.SetThrottle(force);
		}				
	}	
	
	//------------------------------------------------------------------------------------------------	
	// State handling
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Handle attacks. Search for the enemy and then react on the finding.
	
	- Normal case: If enemy is seen, consider shooting
	- Attack case: The location to bomb has been assigned. (m_vAttackPosition)
	*/
	override void TypeHandleAttack(IEntity owner)
	{
		super.TypeHandleAttack(owner);
		
		if (m_EntityType != SDRC_EChopperType.HELICOPTER)
		{
			return;
		}
		
		//Handle attacks:
		if (m_fTimerAttack < 0)	
		{
			//Normal case:
			SDRC_ChopperEnemyHelper.SearchForEnemy(owner);
			
			if (m_fTimeRocketDelay > m_RocketDelay)
			{
				SDRC_ChopperEnemyHelper.SearchEnemyForRocket(owner);
				m_fTimeRocketDelay = 0;
			}
		}
		else
		{
			//Attack case:
			if (m_fTimeRocketDelay > m_RocketDelay)
			{
				SDRC_ChopperEnemyHelper.EnemyFoundForRocket(owner, m_vAttackPosition);
				m_fTimeRocketDelay = 0;
			}
		}
	}
}