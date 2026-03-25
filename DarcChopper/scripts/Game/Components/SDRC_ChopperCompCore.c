//SDRC_ChopperCompCore.c

//Core functions from SDRC_ChopperComp to shorten the file

//------------------------------------------------------------------------------------------------
enum SDRC_EFlyWayPointType
{
	WP_UNDEFINED,				//0 
	WP_FLY,						//1 - Fly, mormal flight pattern
	WP_FLY_IMMEDIATELY,			//2 - Fly, but remove all already added destinations
	WP_FLY_AWAY,				//3 - Fly away as a last move
	WP_FLY_AWAY_IMMEDIATELY,	//4 - Fly away immediately removing the previous destinations
	WP_PATROL,					//5 - Patrol around an area
	WP_LAND,					//6 - 
	WP_WAIT,					//7 - 
	WP_RAISE,					//8 - 
	WP_HOVER,					//9 - 
	WP_GET_OUT,					//10 - 
	WP_END,						//11 - 
	
	//----	
	WP_HOVER_UP,				//12 - Does the action and goes to HOVER state
	WP_STOP_ENGINE,				//13 - Does the action and goes to WAIT state
	WP_ATTACK,					//14 - Sets attack position and time and then FLY
	
	//Macro actions
	WP_M_RESET = 30,			//30 - Reset destinations
	WP_M_LAND_TROOPS,			//   - Drop of troops to exact position, wait and leave
	WP_M_LAND_TO_FREE_SPOT,		//   - Drop of troops, but search for empty spot
	WP_M_EVAC_TROOPS,			//   - Drop troops to a safe spot and stop engine
	WP_M_ATTACK,				//   - Bomb run on the area without the need to have an enemy
	WP_M_SEARCH_DESTROY,		//   - Search for enemy by patroling an area. If enemy is found, attack the location.
	WP_M_TESTING,				//Just for testing
}

enum SDRC_EHeliState
{
	UNKNOWN,
	FLY,
	FLY_AWAY,
	FLY_AWAY_IMMEDIATELY,	//NOTE: This is not a real state. When set, state will change to FLY_AWAY
	LAND,
	WAIT,					//NOTE: Velocity disabled
	RAISE,
	HOVER,
	GET_OUT,				//One frame state to order AI to get out
	END,
	
	ON_GROUND,				//One frame state for touch down
	DESTROYED,
}

enum SDRC_EHeliCargoSeatFill
{
	NONE,
	RANDOM,
	LOW,
	HALF,
	HIGH,
	FULL,
}

enum SDRC_EChopperType
{
	UNKNOWN,
	HELICOPTER,
	DRONE,
	FIXEDWING,
}

enum SDRC_EChopperDamageLevel
{
	UNKNOWN,
	UNDAMAGED,
	LIGHT,
	MEDIUM,
	HEAVY,
	DESTROYED,	
}


//------------------------------------------------------------------------------------------------
class SDRC_ChopperCompCore
{
	//------------------------------------------------------------------------------------------------	
	// Helicopter settings
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	static void StoreOriginalValues(IEntity owner)
	{
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}
		
		chopperComp.m_fThrottleOrig = chopperComp.m_fThrottle;
		chopperComp.m_fRotorForce0Orig = chopperComp.m_fRotorForce0;
		chopperComp.m_fRotorForce1Orig = chopperComp.m_fRotorForce1;
		chopperComp.m_fSpeedMinOrig = chopperComp.m_fSpeedMin;
		chopperComp.m_fSpeedMaxOrig = chopperComp.m_fSpeedMax;	
		
		//Reset runtime parameters
		chopperComp.m_fSpeedLandingMul = 1.0;
	}
	
	//------------------------------------------------------------------------------------------------
	static void ResetOriginalValues(IEntity owner)
	{
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}
		
		chopperComp.m_fThrottle = chopperComp.m_fThrottleOrig;
		chopperComp.m_fRotorForce0 = chopperComp.m_fRotorForce0Orig;
		chopperComp.m_fRotorForce1 = chopperComp.m_fRotorForce1Orig;
		chopperComp.m_fSpeedMin = chopperComp.m_fSpeedMinOrig;
		chopperComp.m_fSpeedMax = chopperComp.m_fSpeedMaxOrig;	
		
		VehicleHelicopterSimulation m_Helicopter_s = VehicleHelicopterSimulation.Cast(owner.GetRootParent().FindComponent(VehicleHelicopterSimulation));
		if (m_Helicopter_s)
		{
	        m_Helicopter_s.SetThrottle(chopperComp.m_fThrottle);
	        m_Helicopter_s.RotorSetForceScaleState(0, chopperComp.m_fRotorForce0);
	        m_Helicopter_s.RotorSetForceScaleState(1, chopperComp.m_fRotorForce1);
		}
		
		//Reset runtime parameters
		chopperComp.m_fSpeedLandingMul = 1.0;
	}	
}