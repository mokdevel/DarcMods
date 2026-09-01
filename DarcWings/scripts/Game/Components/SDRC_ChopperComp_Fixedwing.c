//SDRC_ChopperParams_Fixedwing.c

//This is the modified component for fixed wing use

modded enum SDRC_EChopperSubType
{
	WINGS_C130
}

//------------------------------------------------------------------------------------------------
class SDRC_ChopperParams_Fixedwing : SDRC_ChopperParams
{
	void SDRC_ChopperParams_Fixedwing()
	{
		type = SDRC_EChopperType.FIXEDWING;
		//NOTE: SubType has been set in the prefab
				
		//Turn
		turnSpeedDivider = 92;
		turnSpeedDegreeMin = 1;
		turnSpeedDegreeMax = 90;
		turnTimeIntervalBase = 80;
	
		//Roll 
		rollAngleMul = 0.8;
		
		//Pitch
		pitchAngleRad 	   =  0.001 * Math.DEG2RAD;
		pitchAngleRadFlat  = -85 * Math.DEG2RAD;	//0 points XX degress downwards
		pitchNoseAngleDown = pitchAngleRadFlat + (10 * Math.DEG2RAD);
		pitchNoseAngleUp   = pitchAngleRadFlat - (60 * Math.DEG2RAD);
		
		//Rotor force multipliers
		fRotorForceMulUp = 1.3 * 10;
		iRotorForceNormal = 30;
		iRotorForceRaise = 40;
		iRotorForceHover = 0;
		iRotorForceCrash = 4;
		
		//Obstacle awareness
		rayLenFront = 700;
		rayDown = 50;
				
		//Flight parameters
		detourLerpPosition = 0.4;
		detourDivider = 1.0;
		
		//Attack and enemy related
		rayLenEnemy = 1000;
		timeSearchAndDestroy = 600;
		attackHeightMul = 1.0;
		
		//Damage levels
		damageHeavy = 0.90;
		damageMedium = 0.96;
		damageLight = 0.98;
		
		//Waypoint values
		wpSteepAngle = 140;
														
		destinationForwardInitial = 400;
		destinationForward = 500;
		
		//Flight pattern related
		patrolRadius = 500;	
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
		
		if (m_EntityType != SDRC_EChopperType.FIXEDWING)
		{
			return;
		}
		
		params = new SDRC_ChopperParams_Fixedwing();
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
		
		if (m_EntityType != SDRC_EChopperType.FIXEDWING)
		{
			return;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Type specific init finalization
	*/	
	override void TypeInitDone(IEntity owner)
	{
		super.TypeInitDone(owner);
		
		if (m_EntityType != SDRC_EChopperType.FIXEDWING)
		{
			return;
		}
		
		/* NOTHING NEEDED FOR FIXEDWING */
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Type specific things within EOnFrame. Override this function in other types.
	*/	
	override void TypeEOnFrame(IEntity owner, float timeSlice)
	{
		super.TypeEOnFrame(owner, timeSlice);
		
		if (m_EntityType != SDRC_EChopperType.FIXEDWING)
		{
			return;
		}
		
		/* NOTHING NEEDED FOR FIXEDWING */
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
		
		if (m_EntityType != SDRC_EChopperType.FIXEDWING)
		{
			return;
		}

		health = 1.0;
//		health = SDRC_VehicleHelper.GetHealthScaled(owner, false);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Handle the final parts after damage that breaks flying
	*/	
	override void TypeHandleDamageFinal(IEntity owner)
	{
		super.TypeHandleDamageFinal(owner);
		
		if (m_EntityType != SDRC_EChopperType.FIXEDWING)
		{
			return;
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
		
		if (m_EntityType != SDRC_EChopperType.FIXEDWING)
		{
			return;
		}		
	}
}