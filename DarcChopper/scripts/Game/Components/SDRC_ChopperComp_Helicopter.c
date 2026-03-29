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
	This is the setup for a specific SDRC_EChopperType. Override this function in other types.
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
	This sets up the flight model params for a specific SDRC_EChopperType. Override this function for other types.
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
}