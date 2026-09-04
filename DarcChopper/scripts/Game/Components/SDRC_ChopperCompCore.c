//SDRC_ChopperCompCore.c

//Core classes/functions from SDRC_ChopperComp to shorten the file

//------------------------------------------------------------------------------------------------
class SDRC_ChopperParams
{
	SDRC_EChopperType type = SDRC_EChopperType.UNKNOWN;
	
	//Turn
	int turnSpeedDivider;							//The divider that affects how much speed is decreased on sharp turns. The higher the value, the less brake.	Was: 42	
	int turnSpeedDegreeMin;							//Min/Max angles that affects the speed decrease on turns.
	int turnSpeedDegreeMax;
	int turnTimeIntervalBase;						//Time to divide with speed to define the final turn time. Smaller value makes vehicle turn faster.

	//Roll 
	float rollAngleMul;								//Multiplier for roll angle along the spline. 
	
	//Pitch
	float pitchAngleRad;							//The pitch angle to use when calculating for speed effect. The faster the vehicle goes, the steeper the nose should be down.
	float pitchAngleRadFlat;						//The pitch angle when chopper is flying flat. This value shall be between pitchNoseAngleDown and pitchNoseAngleUp
	float pitchNoseAngleDown;						//Maximum angle to turn the vehicle nose down when in high speed.
	float pitchNoseAngleUp;							//Maximum angle to turn the vehicle nose up when braking.
	
	//Rotor force multipliers
	float fRotorForceMulUp							//Rotor force multiplier in velocity counting. Bigger value makes the vehicle react faster to up/down movement but also starts stutter.
	int   iRotorForceNormal; 						//.. normal flight situation
	int   iRotorForceRaise; 						//.. when raising from stand still
	int   iRotorForceHover; 						//.. when hovering in one place
	int   iRotorForceCrash; 						//.. when crashin. Bigger value causes a deeper curve
	
	//Obstacle awareness
	float rayLenFront;								//Length of the ray to detect obstacles in front of vehicle
	float rayDown;									//Distance to point the ray end downward 
	
	//Flight parameters
	float detourLerpPosition;						//Value that is reduced from the detour lerp position (0.65). Shall not be bigger than default.
	float detourDivider;							//Divider addition to defaul value (1.5). 
	
	//Attack and enemy related
	int rayLenEnemy;								//Max distance to see an enemy
	int timeSearchAndDestroy;						//(seconds) Time to be in search and destroy behaviour
	float attackHeightMul;							//Height multiplier to use for attack position. This will take m_fFlyHeightLow and multiply with this value.
	int attackDefaultTime;							//(seconds) How long to attack a location
	
	//Damage levels
	float damageHeavy;
	float damageMedium;
	float damageLight;
	
	//Waypoint values
	float wpSteepAngle;								//Waypoint angle that is considered steep. This is the angle between current direction and new direction.
													//If chopper destination makes a too steep turn, we will add a few additional points.
	int destinationForwardInitial;					//Distance to fly forward on first fligth at init
	float destinationForward;						//Distance to add points forward when creating new flight path
	
	//Flight pattern related
	int patrolRadius;								//Default radius for WP_PATROL and WP_PATROL_ONCE commands. The radius will be between 70% - 130
}

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
	WP_LAND,					//7  - DO NOT USE - use WP_M_LAND instead. Left for compatibility.
	WP_LAND_VERTICAL,			//8  - Landing vertically down
	WP_WAIT,					//9  - 
	WP_WAIT_GETOUT,				//10 - Wait during get out. Sets the time and goes to WAIT
	WP_RAISE,					//11 - 
	WP_HOVER,					//12 - Heli is hovering in one place
	WP_HOVER_UP,				//13 - Heli is hovering up
	WP_HOVER_DOWN,				//14 - Heli is hovering down
	WP_BRAKE,					//15 - Slow the heli down to stand still
	WP_END,						//16 - 
	WP_DESPAWN,					//17 - 
	WP_CRASH,					//18 - 
	
	//One shot commands
	WP_GET_OUT,					//19 - Orders AI to get out
	WP_STOP_ENGINE,				//20 - Does the action and goes to WAIT state
	WP_ATTACK,					//21 - Sets attack position and time and then FLY
	WP_SEARCH_DESTROY,			//   - Search for enemy by patroling an area. If enemy is found, attack the location.
	WP_RESET,					//   - Reset destinations and flight patg. Cut the current flight planned and pick the next destination in the list.
	WP_CUT,						//   - Cut the current flight planned and pick the next destination in the list.
	
	//Macro actions
	WP_M_LAND,					//50 - Land to a position by doing WP_BRAKE and WP_LAND
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
	FLY,					// 1 - Normal state
	FLY_AWAY,				// 2 - In this state, when all destinations have been flown through, we fly away and end.
	FLY_AWAY_IMMEDIATELY,	// 3 - NOTE: This is not a real state. When set, state will change to FLY_AWAY
	LAND,					// 4 - Chopper is landing
	LAND_VERTICAL,			// 5 - Chopper is landing vertically
	BRAKE,					// 6 - Chopper is braking
	WAIT,					// 7 - NOTE: Velocity disabled
	RAISE,					// 8 - Chopper is raising and moving forward to given position
	HOVER,					// 9 - Chopper is hovering at given height
	HOVER_UP,				//10 - Chopper is hovering upwards
	HOVER_DOWN,				//11 - Chopper is hovering upwards
	GET_OUT,				//12 - One frame state to order AI to get out
	ATTACK,					//13 - Added when WP_ATTACK is handled to inform that spline height checking can go below minimum height	
	CRASH,					//14 - Chopper has a target to crash
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

enum SDRC_EChopperSubType
{
	DEFAULT
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
	NORMAL_BEHAVIOUR,				//Normal state which changes to S&D if an enemy is seen.
	SEARCH_AND_DESTROY_BEHAVIOUR,	//S&D looking for enemies. Returns to NORMAL if enemies are lost.
	EVAC_BEHAVIOUR,					//Chopper is doing an evac or landing
	PASSIVE_BEHAVIOUR,				//Just flying around
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

		ResetOriginalValuesComp(chopperComp);
	}
	
	//------------------------------------------------------------------------------------------------
	static void ResetOriginalValuesComp(SDRC_ChopperComp chopperComp)
	{
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
		
		//Allow other movement
		chopperComp.m_bOnlyVerticalMovement = false;
	}		
}