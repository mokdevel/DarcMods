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
														
		destinationForwardInitial = 150;
		destinationForward = 100;
	}
}

//------------------------------------------------------------------------------------------------
modded class SDRC_ChopperComp
{
	private bool m_bRegistered = false;
	
	//------------------------------------------------------------------------------------------------
	/*!
	This is the setup for a specific SDRC_EChopperType. Override this function in other types
	*/
	override void SetupType(IEntity owner)
	{
		super.SetupType(owner);
		
		if (m_EntityType != SDRC_EChopperType.DRONE)
		{
			return;
		}

		//DRONE specific
		
		SAL_DroneControllerComponent droneControllerComponent = SAL_DroneControllerComponent.Cast(owner.FindComponent(SAL_DroneControllerComponent));		
		if (droneControllerComponent)
		{
			droneControllerComponent.ArmDrone();
			droneControllerComponent.m_bIsActive = true;
			droneControllerComponent.m_bIsArmed = true;			
			droneControllerComponent.m_iOwner = GetGame().GetPlayerController();
			if (droneControllerComponent.m_iOwner == -1)
			{
				droneControllerComponent.m_iOwner = 0;
			}
			//droneControllerComponent.m_InputManager.SetActionValue("DroneUp", 3.0);
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	This sets up the flight model params for a specific SDRC_EChopperType. Override this function for other types.
	*/	
	override void SetupTypeParams(IEntity owner)
	{
		super.SetupTypeParams(owner);
		
		if (m_EntityType != SDRC_EChopperType.DRONE)
		{
			return;
		}
		
		params = new SDRC_ChopperParams_Drone();
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Type specific things within EOnFrame. Override this function in other types.
	*/	
	override void TypeEOnFrame(IEntity owner, float timeSlice)
	{
		super.SetupTypeParams(owner);
		
		if (m_EntityType != SDRC_EChopperType.DRONE)
		{
			return;
		}
		
		if (!m_bRegistered)
		{
			SAL_DroneControllerComponent droneControllerComponent = SAL_DroneControllerComponent.Cast(owner.FindComponent(SAL_DroneControllerComponent));		
			if (droneControllerComponent)
			{
				if (droneControllerComponent.m_DroneId != -1)
				{
					SDRC_Log.Add("[SDRC_ChopperComp_Drone:SetupType] Registering DroneId: " + droneControllerComponent.m_DroneId, LogLevel.DEBUG);
					droneControllerComponent.m_DroneManager.m_aActiveDrones.Insert(droneControllerComponent.m_DroneId);
					m_bRegistered = true;
				}
			}
		
		}
		
		InputManager m_InputManager = GetGame().GetInputManager();
		m_InputManager.SetActionValue("DroneUp", 1.0);		
	}	
}