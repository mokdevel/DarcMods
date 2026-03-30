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
		turnTimeIntervalBase = 40;
	
		//Roll 
		rollAngleMul = 2.4;
		
		//Pitch
		pitchAngleRad 	 =  11 * Math.DEG2RAD;
		pitchAngleRadFlat  = -45 * Math.DEG2RAD;
		pitchNoseAngleDown = -30 * Math.DEG2RAD;
		pitchNoseAngleUp   =  20 * Math.DEG2RAD;
		
		//Rotor force multipliers
		rotorForceMulUp = 1.3 * 10;
		
		//Waypoint values
		wpSteepAngle = 60;
														
		destinationForwardInitial = 500;
		destinationForward = 300;
	}
}

//------------------------------------------------------------------------------------------------
modded class SDRC_ChopperComp
{
	//------------------------------------------------------------------------------------------------
	/*!
	This sets up the flight model params for a specific SDRC_EChopperType. Override this function for other types.
	This is called immediately when component is initialized.
	*/	
	override void SetupTypeParams(IEntity owner)
	{
		super.SetupTypeParams(owner);
		
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
	*/
	override void SetupType(IEntity owner)
	{
		super.SetupType(owner);
		
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
	Type specific things within EOnFrame. Override this function in other types.
	*/	
	override void TypeEOnFrame(IEntity owner, float timeSlice)
	{
		super.SetupTypeParams(owner);
		
		if (m_EntityType != SDRC_EChopperType.HELICOPTER)
		{
			return;
		}
		
		/* NOTHING NEEDED FOR HELICOPTER */
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Handle attacks
	- Normal case: If enemy is seen, consider shooting
	- Attack case: The location to bomb has been assigned. (m_vAttackPosition)
	*/
	override void HandleAttack(IEntity owner)
	{
		super.HandleAttack(owner);
		
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