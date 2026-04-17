//SDRC_ChopperCompCore.c

//Core functions from SDRC_ChopperComp to shorten the file

//------------------------------------------------------------------------------------------------
enum SDRC_EFlyWayPointType
{
	WP_UNDEFINED,				//0 
	WP_FLY,						//1  - Fly, mormal flight pattern
	WP_FLY_IMMEDIATELY,			//2  - Fly, but remove all already added destinations
	WP_FLY_AWAY,				//3  - Fly away as a last move
	WP_FLY_AWAY_IMMEDIATELY,	//4  - Fly away immediately removing the previous destinations
	WP_PATROL,					//5  - Patrol around an area 
	WP_PATROL_ONCE,				//6  - Do one patrol round around an area.
	WP_LAND,					//   - 
	WP_WAIT,					//   - 
	WP_WAIT_GETOUT,				//   - Wait during get out. Sets the time and goes to WAIT
	WP_RAISE,					//   - 
	WP_HOVER,					//   - Heli is hovering in one place
	WP_HOVER_UP,				//   - Heli is hovering up
	WP_GET_OUT,					//   - 
	WP_BRAKE,					//   - Slow the heli down to stand still
	WP_END,						//   - 
	WP_DESPAWN,					//   - 
	
	//One shot commands
	WP_STOP_ENGINE,				//   - Does the action and goes to WAIT state
	WP_ATTACK,					//   - Sets attack position and time and then FLY
	WP_SEARCH_DESTROY,			//   - Search for enemy by patroling an area. If enemy is found, attack the location.
	
	//Macro actions
	WP_M_RESET = 30,			//30 - Reset destinations. Cut the current flight planned and pick the next destination in the list.
	WP_M_CUT,					//   - Cut the current flight planned and pick the next destination in the list.
	WP_M_LAND_TROOPS,			//   - Drop of troops to exact position, wait and leave
	WP_M_LAND_TO_FREE_SPOT,		//   - Drop of troops, but search for empty spot. Stop engine.
	WP_M_EVAC_TROOPS,			//   - Drop troops to a safe spot and stop engine
	WP_M_ATTACK,				//   - Bomb run on the area without the need to have an enemy (TO BE CHANGED!)
	WP_M_SUPPRESSIVE,			//   - Bomb run on the area without the need to have an enemy
	WP_M_TESTING,				//Just for testing
}

enum SDRC_EHeliState
{
	UNKNOWN,
	FLY,					//Normal state
	FLY_AWAY,				//In this state, when all destinations have been flown through, we fly away and end.
	FLY_AWAY_IMMEDIATELY,	//NOTE: This is not a real state. When set, state will change to FLY_AWAY
	LAND,					//Chopper is landing
	BRAKE,					//Chopper is braking
	WAIT,					//NOTE: Velocity disabled
	RAISE,					//Heli is raising and moving forward to given position
	HOVER,					//Heli is hovering at given height
	HOVER_UP,				//Heli is hovering upwards
	GET_OUT,				//One frame state to order AI to get out
	END,
	
	ON_GROUND,				//One frame state for touch down
	DESTROYED,
	DESPAWN,
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

enum SDRC_EHeliDamageLevel
{
	UNKNOWN,
	UNDAMAGED,
	LIGHT,
	MEDIUM,
	HEAVY,
	DESTROYED,	
}

enum SDRC_EHeliBehaviour
{
	UNKNOWN_BEHAVIOUR,
	NORMAL_BEHAVIOUR,
	SEARCH_AND_DESTROY_BEHAVIOUR,
	EVAC_BEHAVIOUR,
}

//------------------------------------------------------------------------------------------------
//NOTE: This is specific for DarcMissions Chopper mission. Here for compatibility for other mods.

enum SDRC_EHeliWaypointGenerationType
{
	NONE,
	RANDOM,					//Random flying for a helicopter
	PATROL,					//Fly around a certain area
	SEARCH,					//Random flying search patrol. Once a player is found, mission ends.
	SEARCH_AND_DESTROY,		//Search for players and engage in attack if found.
	
	LANDING,				//Land the helicopter
};	

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
		chopperComp.m_fSpeedSlowingMul = 1.0;
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
		
		chopperComp.SetEngine(true, chopperComp.m_fThrottle, chopperComp.m_fRotorForce0, chopperComp.m_fRotorForce1);
		
		//Reset runtime parameters
		chopperComp.m_fSpeedSlowingMul = 1.0;
	}	
}