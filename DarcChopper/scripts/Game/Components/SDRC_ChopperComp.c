//SDRC_ChopperComp.c

//Changes done in prefabs:
// - SCR_AIVehicleUsageComponent : Set true to Can Be Piloted

#define ENABLE_ROCKETS

//------------------------------------------------------------------------------------------------
class SDRC_ChopperCompClass : ScriptGameComponentClass { }

//------------------------------------------------------------------------------------------------
enum SDRC_EHeliWaypointGenerationType
{
	NONE,
	RANDOM,		//Random flying for a helicopter
	PATROL,		//Fly around a certain area
	SEARCH,		//Random flying search patrol. Once a player is found, mission ends.
	
	LANDING,	//Land the helicopter
	FLY_AWAY,	//Fly far away
};

//------------------------------------------------------------------------------------------------
enum SDRC_EFlyWayPointType
{
	UNDEFINED,
	FLY,
	FLY_AWAY,
	FLY_AWAY_IMMEDIATELY,
	LAND,
	WAIT,
	RAISE,
	HOVER,
	GET_OUT,
	END,
	
	//----	
	HOVER_UP,				//Does the action and goes to HOVER state
	STOP_ENGINE,			//Does the action and goes to WAIT state
	
	//Macro actions
	M_LAND_TROOPS,
}

enum SDRC_EHeliState
{
	UNKNOWN,
	FLY,
	FLY_AWAY,
	FLY_AWAY_IMMEDIATELY,	//NOTE: This is not a real state. When set, state will change to FLY_AWAY
	LAND,
	WAIT,					//Velocity disabled
	RAISE,
	HOVER,
	GET_OUT,
	END,
	
	ON_GROUND,				//One frame state for touch down
	DESTROYED,
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class SDRC_FlyPathPoint
{
	[Attribute(defvalue: "1", uiwidget: UIWidgets.SearchComboBox, desc: "SDRC_EFlyWayPointType", enums: ParamEnumArray.FromEnum( SDRC_EFlyWayPointType ) )]
	SDRC_EFlyWayPointType type;
	
	[Attribute(defvalue: "0 0 0", desc: "Position, waiting time, hover height, etc..")]
	vector pt;

	[Attribute(defvalue: "0 0 0", desc: "Position, waiting time, hover height, etc..")]
	float value
	
	void Set(SDRC_EFlyWayPointType type_ = SDRC_EFlyWayPointType.FLY, vector pt_ = vector.Zero, float value_ = 0)
	{
		type = type_;
		pt = pt_;
		value = value_;		
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_ChopperComp : ScriptGameComponent
{
	private static SDRC_ChopperComp s_Instance;	
	private ref array<vector> m_vSplinePoints = new array<vector>();
	private static VehicleHelicopterSimulation m_Helicopter_s;
	
	//Parameters accessible helicopter parameters
	[Attribute(category: "Chopper", defvalue: "1", desc: "Autostart chopper")]	
	bool m_bAutoStart;
	[Attribute(category: "Chopper", defvalue: "1.2", desc: "Throttle aka acceleration", params: "0.1 3.0 0.1")]	
	float m_fThrottle;
	float m_fThrottleOrig;
	[Attribute(category: "Chopper", defvalue: "3.0", desc: "Main rotor force", params: "0.1 10.0 0.1")]	
	float m_fRotorForce0;
	float m_fRotorForce0Orig;
	[Attribute(category: "Chopper", defvalue: "1.0", desc: "Rear rotor force", params: "0.1 2.0 0.1")]	
	float m_fRotorForce1;
	float m_fRotorForce1Orig;
	
	[Attribute(category: "Chopper", defvalue: "10.0", desc: "Minimum speed", params: "1.0 100.0 0.1")]	
	float m_fSpeedMin;				//Minimum speed
	float m_fSpeedMinOrig;
	[Attribute(category: "Chopper", defvalue: "30.0", desc: "Maximum speed", params: "1.0 100.0 0.1")]	
	float m_fSpeedMax;				//Maximum speed
	float m_fSpeedMaxOrig;
	[Attribute(category: "Chopper", defvalue: "40.0", desc: "Minimum fly height (from ground level)", params: "5 100.0 1")]	
	float m_fFlyHeightLow;			//Flight height low
	[Attribute(category: "Chopper", defvalue: "80.0", desc: "Maximum fly height (from ground level)", params: "5 100.0 1")]	
	float m_fFlyHeightHigh;			//Flight height high
	[Attribute(category: "Chopper", defvalue: "0.1", desc: "Minimum distance for waypoint", params: "0.1 1000.0 0.1")]	
	float m_fDistanceLow;			//Distance for waypoint min
	[Attribute(category: "Chopper", defvalue: "0.4", desc: "Maximum distance for waypoint", params: "0.1 1000.0 0.1")]	
	float m_fDistanceHigh;			//..max
	SDRC_EHeliWaypointGenerationType m_fWpType = SDRC_EHeliWaypointGenerationType.RANDOM; 	

	//Crew
	[Attribute(category: "AI settings", defvalue: "", desc: "The faction to use")]	
	string m_sFaction;
	[Attribute(category: "AI settings", desc: "Characters to spawn in the chopper", params: "et")]
	ref array<ref SCR_DefaultOccupantData> m_aCrew;
	//Crew settings
//	[Attribute(defvalue: EAISkill.REGULAR.ToString(), UIWidgets.ComboBox, desc: "AI skill in combat", "", ParamEnumArray.FromEnum(EAISkill), category: "Common")]
	[Attribute(category: "AI settings", defvalue: typename.EnumToString(EAISkill, EAISkill.REGULAR), uiwidget: UIWidgets.ComboBox, desc: "AI skill", enumType: EAISkill)]	
	EAISkill m_AISkill;	
	[Attribute(category: "AI settings", defvalue: "1.0", desc: "AI perception", params: "0.1 3.0 0.1")]	
	float m_AIPerception;

	[Attribute(category: "Weapons", defvalue: "40.0", desc: "The sector where rockets may be shot", params: "1.0 45.0 1.0")]	
	float m_RocketSector;
	[Attribute(category: "Weapons", defvalue: "0.3", desc: "Delay between rockets", params: "0.1 30.0 0.1")]	
	float m_RocketDelay;
	float m_fTimeRocketDelay = 0;
	[Attribute(category: "Weapons", defvalue: "10 0 0", desc: "Rocket spawn position")]	
	vector m_RocketPosition;
//	[Attribute(category: "Weapons", params: "et", defvalue: "{EE65544BA845C458}Prefabs/Weapons/Ammo/Ammo_Rocket_S5_HEDP_S5KO.et", desc: "Rocket to use")]	
	[Attribute(category: "Weapons", params: "et", defvalue: "", desc: "Rocket to use")]	
	ref array<ref ResourceName> m_RocketPrefabs;	 
	ResourceName m_RocketPrefab = "";
		
	//Flight path
	ref array<ref SDRC_FlyPathPoint> m_vFlightPoints = {};
//	[Attribute(category: "Flight path", "", UIWidgets.Object, "Destinations")]	
	[Attribute("", UIWidgets.Object, "Destinations")]	
	ref array<ref SDRC_FlyPathPoint> m_vFlyDestinations;	//Requested destinations
	
	[Attribute(defvalue: "0", desc: "Show debugging information")]	
	bool m_bShowDebug;
	
	//Original destination	
	private vector m_vOriginalDestination;				//Used to know where to patrol
		
	//Speed management
	private const float SPEED_INTERVAL = 1.0;			//(seconds) Interval to modify speed of the helicopter
	private const float SPEED_GAIN = 1.0;
	private const int SPEED_TURN_DIV = 92;				//The divider that affects how much speed is decreased on sharp turns. The higher the value, the less brake.	Was: 42
	private float m_fTimeSpeed = 0;

	private float m_fTimeBetweenPts = 1;
	private float m_fTimeBetweenPtsAvg = 1;

	private float m_fTimeBetweenFixes = 30;
	
	private float m_fTimeInState = -1;					//The timer to stay in a certain state. This is only in effect when positive value.
	private bool  m_bTimeInStateEnabled = false;
			
	//Turn
	private const int TIME_TURN_INTERVAL_BASE = 40;		//Time to divide with speed to define the final turn time. Smaller value makes heli turn faster.
	
	//Pitch
	private const float PITCH_ANGLE_RAD = 11 * Math.DEG2RAD;		//The pitch angle to use when calculating for speed effect. The faster the heli goes, the steeper the nose should be down.
	private const float PITCH_ANGLE_FLAT_RAD = -45 * Math.DEG2RAD;	//The pitch angle when chopper is flying flat.
	
	//Roll 
	private const float ROLL_ANGLE_MUL = 2.4;			//Multiplier for roll angle along the spline
	
	//Flight path
	private const int POINTS_TO_NEW_DISTANCE = 2;		//How many spline points in to the future flight path is checked before adding new flight points.
	private const int POINTS_TO_SPLINE_START = 4;		//Points to go back from m_iClosestIndex when creating a new flight path 
	private const int DESTINATION_POINT_DIV = 12;		//How many points ahead to look for the destination. This is the divider for speed.
	private const float TIME_IN_INIT = 10;				//(seconds) Time to be in init state. During this time, we don't check for damage or similar things.

	private const int FLIGHT_FIX_TIME = 8;				//(seconds) Time to wait between flight fixes when chopper is pointing to the sky.
	private const int FLIGHT_FIX_ANGLE = 1.4;			//Angle that enforces 
	
	//Rotor force multipliers
	private const float ROTOR_FORCE_UP_MUL = 1.3;		//Rotor force multiplier in velocity counting. Bigger value makes the heli react faster to up/down movement but also starts stutter.
	
	//Waypoint values
	private const float WP_ANGLE = 60;					//Waypoint angle that is considered steep. This is the angle between current direction and new direction.
														//If chopper destination makes a too steep turn, we will add a few additional points.
	
	//Helistate
	private SDRC_EHeliState m_eHeliState;
	private bool m_bInInit;
	
	//Runtime parameters
	private int m_iDestinationPointAdd;
	private float m_fTimeTurnInterval;
	
	const int HEALTH_LIMIT = 1000;				//Limit to define the chopper to be heavily damaged. 
	
	//Flight path runtime variables	
	private vector m_vOrigin;					//Current position
	private float m_fSpeed;						//Current speed
	private float m_fSpeedStart;				//Speed lerp start
	private float m_fSpeedTarget;				//Speed lerp target aka end
	private float m_fSpeedMul;					//Speed multiplier that depends on the turn
	private float m_fSpeedLandingMul;			//Landing speed modifier
	private float m_fRotorForceMultiplier;		//Rotor force multiplier that simulates up/down throttle
	
	//Angular velocities
	private vector m_vAngularVel;
	private vector m_vRollTarget;
	private vector m_vRadRollVel;
	private vector m_vRadRollBack;
	private vector m_vRadRollPitch;

	//Heli directions
	private vector m_vHeliForward;
	private vector m_vHeliDirection;
	private vector m_vHeliDirectionFuture;
	
	//Enemy positions
	private const int ENEMY_FOUND_TIMEOUT = 10;		//Time between enemy position updates
	private const int ENEMY_FORGET_TIMEOUT = 30;	//Time to forget the enemy position
	private vector m_vEnemyPosition = vector.Zero;	//Position of last found enemy
	private int m_iEnemyFoundTimeOut;				//Time to wait to before allowing enemy position 
	private bool m_bSearchForEnemy;					//Enable/Disable enemy searching
	
	//Debug stuff
	private float m_fDbgAngle;
	private float m_fAnglePitch;
	private float m_fAngleRoll;
	private float m_fAngleRollBack;			//Remove from final
	
	//Runtime parameters
	private int m_iClosestIndex;				//Closest point on spline to heli
	private int m_iOldClosestIndex;
	private int m_iNextIndex;					//Next index to our m_iClosestIndex - depends on speed
	private int m_iFutureIndex;					//Where we are heading in the long run
	private vector m_vDestination;				//Lerped m_vDestination that keeps on moving along the spline
	private vector m_vDestinationFuture;		//Destination where we eventually plan to fly
	private vector m_vSplinePointBelow;			//The point below heli that is close to the spline
	
	//Id for debug items
	private string m_sDid;
	
	//Landing related
	private float m_fTimeLanding;
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		if (!GetGame().GetWorld())
		{
			return;
		}
		
		SDRC_Log.Add("[SDRC_ChopperComp] Starting SDRC_ChopperComp", LogLevel.NORMAL);
		
		SetEventMask(owner, EntityEvent.INIT);
		s_Instance = this;
		m_sDid = SDRC_Misc.GetCurrentTickTime().ToString() + Math.RandomInt(0, 10000);

		#ifndef SDRC_RELEASE
//			m_bShowDebug = true;
		#endif
		
		m_bInInit = true;
		SetState(SDRC_EHeliState.FLY);
		
		//Clear any existing path points
		ResetFlight();
		SetTimeInState(0);
		//Save the original values
		StoreOriginalValues();
		
		SDRC_ChopperEnemyHelper.GetWeapons(owner);
		
		m_Helicopter_s = VehicleHelicopterSimulation.Cast(GetOwner().GetRootParent().FindComponent(VehicleHelicopterSimulation));
		m_iEnemyFoundTimeOut = SDRC_Misc.GetCurrentTickTime() + ENEMY_FOUND_TIMEOUT;
		
		if (m_Helicopter_s)
		{
	        m_Helicopter_s.EngineStart();
	        m_Helicopter_s.SetThrottle(m_fThrottle);
	        m_Helicopter_s.RotorSetForceScaleState(0, m_fRotorForce0);
	        m_Helicopter_s.RotorSetForceScaleState(1, m_fRotorForce1);			
			SetHeli(m_fSpeedMin, m_fSpeedMax, m_fFlyHeightLow, m_fFlyHeightHigh, SDRC_EHeliWaypointGenerationType.RANDOM, m_fDistanceLow, m_fDistanceHigh);						

			if (m_bAutoStart)
			{
				//NOTE: This section is to be done in the mod
				SetSearchForEnemy(true);
				Ready(owner);
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_ChopperComp] VehicleHelicopterSimulation not found.", LogLevel.ERROR);
		}

		if (m_RocketPrefabs.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_ChopperComp] No rockets availabled.", LogLevel.NORMAL);
		}
		else
		{
			m_RocketPrefab = m_RocketPrefabs.GetRandomElement();
//			m_RocketPrefab = m_RocketPrefabs[0];
			SDRC_Log.Add("[SDRC_ChopperComp] Using rockets: " + SDRC_Misc.GetSimpleEntityName(m_RocketPrefab), LogLevel.NORMAL);
		}
				
		super.OnPostInit(owner);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~SDRC_ChopperComp()
	{
		Clear();		
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Do final clear up of things when heli is to be despawned.
	*/	
	void Clear()
	{
		SDRC_DebugHelper.DeleteDebugItems(m_sDid);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Set the helicopter to normal state
	*/	
	void InitDone(IEntity owner)
	{
		m_bInInit = false;
		
		if (SDRC_VehicleHelper.PilotCountAlive(owner) == 0)
		{
			SDRC_Log.Add("[SDRC_ChopperComp] Unable to set pilots.", LogLevel.WARNING);			
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Once all init things are done, activate the component after a small delay
	*/	
	void Ready(IEntity owner)
	{
		//Set ready in a few seconds
		GetGame().GetCallqueue().CallLater(ReadyDelayed, 1000, false, owner);
		#ifdef ENABLE_ROCKETS
//			GetGame().GetCallqueue().CallLater(HandleRocket, 10, true, owner);
		#endif
	}
	
	//------------------------------------------------------------------------------------------------
	void ReadyDelayed(IEntity owner)
	{
		// Some things needs to be done delayed
		if (m_bAutoStart)
		{
			//Init flight path
			InitFlight(owner, owner.GetOrigin());
			//Spawn crew 
			int crewCount = SDRC_ChopperHelper.SpawnCrew(owner, m_aCrew, m_sFaction, m_AISkill, m_AIPerception);
			SDRC_Log.Add("[SDRC_ChopperComp] Crew count: " + crewCount, LogLevel.DEBUG);
		}		

		DrawDebugPaths();
				
//		SetEventMask(owner, EntityEvent.FRAME | EntityEvent.POSTFRAME);
		SetEventMask(owner, EntityEvent.FRAME);
		Activate(owner);
		
		GetGame().GetCallqueue().CallLater(InitDone, TIME_IN_INIT * 1000, false, owner);
	}

	//------------------------------------------------------------------------------------------------
	void HandleRocket(IEntity owner)
	{
//		SDRC_ChopperEnemyHelper.SearchEnemyForRocket(owner);
	}	
					
	//------------------------------------------------------------------------------------------------
	void GivePoints(out array<vector> points, array<ref SDRC_FlyPathPoint> flyPathPoints)
	{
		points.Clear();
		
		foreach (SDRC_FlyPathPoint flyPathPoint : flyPathPoints)
		{
			points.Insert(flyPathPoint.pt);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to component
	*/
	SDRC_ChopperComp GetInstance()
	{
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------	
	// Flight model functionality
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		m_vOrigin = owner.GetOrigin();
	
		m_fTimeSpeed += timeSlice;
		m_fTimeBetweenPts += timeSlice;
		m_fTimeBetweenFixes -= timeSlice;
		m_fTimeInState -= timeSlice;		
		m_fTimeRocketDelay += timeSlice;		

		//Check if we're still working. This section is not needed every frame. //TBD: Could be done every x seconds - not that critical
		//---
		//If chopper is destroyed, let Reforger handle crash etc.
		if (m_eHeliState == SDRC_EHeliState.DESTROYED)
		{
			return;
		}
		
		if (!SDRC_ChopperHelper.IsStillWorking(owner, m_bInInit))
		{
			//Nope, we're done
			SetState(SDRC_EHeliState.DESTROYED);
			SDRC_DebugHelper.DeleteDebugItems(m_sDid);
		}
		
		//If the chopper is damaged, init is considered done.
		if (SDRC_ChopperHelper.GetHealth(owner) < HEALTH_LIMIT)
		{
			InitDone(owner);
		}				
		//---
		
		//Normal flying part
		
		//Adjust time depending on the speed.
		m_fTimeTurnInterval = TIME_TURN_INTERVAL_BASE / m_fSpeed;
		m_fTimeTurnInterval = Math.Clamp(m_fTimeTurnInterval, 0.6, 3);
				
		//If we've been stuck on a point, force new flight path. 
		//Sometimes the heli direction and path align so that the closest index does not update.
		//In these case the helicopter up vector and world up vector is big.
		bool bCreateNewPath = false;
		float heliUpAngleToWorld = SDRC_Math.GetAngleBetweenVectors(owner.GetTransformAxis(1), vector.Up);	
		if ( (heliUpAngleToWorld > FLIGHT_FIX_ANGLE) && (m_fTimeBetweenFixes < 0) )
		{
			#ifndef SDRC_RELEASE
				SDRC_Log.Add("[SDRC_ChopperComp] Fixing flight.", LogLevel.DEBUG);
			#endif
			bCreateNewPath = true;
		}
		
		//No need to do anything unless we are at the end of spline.
		if ((m_iClosestIndex + m_iDestinationPointAdd + POINTS_TO_NEW_DISTANCE >= m_vSplinePoints.Count() - 1) || bCreateNewPath)
		{
//			if (m_eHeliState != SDRC_EHeliState.LAND)
			if (m_eHeliState == SDRC_EHeliState.FLY)
			{
				//Define a new destination and create a new path
				CreateNewFlight(owner);
				m_fTimeBetweenFixes = FLIGHT_FIX_TIME;	//Time between tries to fix the flight
			}
		}
		
		//Count destintation addition along the spline which is dependent on the speed.
		m_iDestinationPointAdd = m_fSpeed / DESTINATION_POINT_DIV;
		m_iDestinationPointAdd = Math.ClampInt(m_iDestinationPointAdd, 1, 3);
		
		//Find where we're going
		float distance = SDRC_Spline3D.GetDistanceFromSpline(m_vSplinePoints, m_vOrigin, m_iClosestIndex, false);	//NOTE: This will set m_iClosestIndex

		if (m_iClosestIndex > m_iOldClosestIndex)
		{
			m_fTimeBetweenPtsAvg = m_fTimeBetweenPts;		//TBD: This is a static value of previous time instead of average 
			m_fTimeBetweenPts = 0;
			m_iOldClosestIndex = m_iClosestIndex;
		}
		
		//Destination point definition
		m_iFutureIndex = m_iClosestIndex + (m_iDestinationPointAdd * 2);
		m_iNextIndex = m_iClosestIndex + m_iDestinationPointAdd;

		if (m_iNextIndex > m_vSplinePoints.Count() - 1)
		{
			m_iNextIndex = m_vSplinePoints.Count() - 1;
		}
		
		if (m_iFutureIndex > m_vSplinePoints.Count() - 1)
		{
			m_iFutureIndex = m_vSplinePoints.Count() - 1;
		}
		
		//Lerped m_vDestination that keeps on moving along the spline
		float td = m_fTimeBetweenPts / m_fTimeBetweenPtsAvg;
		td = Math.Clamp(td, 0, 1);
		m_vDestination = vector.Lerp(m_vSplinePoints[m_iNextIndex], m_vDestinationFuture, td);
		
		//Destination where we eventually plan to fly
		m_vDestinationFuture = m_vSplinePoints[m_iFutureIndex];
		
		//Lerp speed		
		if (m_fTimeSpeed < SPEED_INTERVAL)
		{
			float ts = m_fTimeSpeed / SPEED_INTERVAL;
			m_fSpeed = Math.Lerp(m_fSpeedStart, m_fSpeedTarget, ts);
			m_fSpeed = Math.Clamp(m_fSpeed, m_fSpeedMin, m_fSpeedMax)
		}

		//Get chopper direction
		m_vHeliForward = owner.GetTransformAxis(2);
		m_vHeliDirection = vector.Direction(m_vOrigin, m_vDestination);		
		m_vHeliDirectionFuture = vector.Direction(m_vOrigin, m_vDestinationFuture);

		//Find the point on spline below the helicopter. This is not exact.
		if (m_vSplinePoints.Count() == 1)
		{
			m_iClosestIndex = 0;
		}
		int prevIndex = m_iClosestIndex - 1;	
		if (prevIndex < 0)
		{
			prevIndex = m_iClosestIndex;
		}
		m_vSplinePointBelow = vector.Lerp(m_vSplinePoints[prevIndex], m_vSplinePoints[m_iClosestIndex], td);
				
		//Set velocity 
		HandleRotorForce(owner);
		
		//Set turn
		SetTurn(owner, m_fTimeTurnInterval);
		
		HandleState(owner, timeSlice);

		SetVelocity(owner);
		
		//Search for enemies
		SearchForEnemy(owner);
		
		if (m_fTimeRocketDelay > m_RocketDelay)
		{
			SDRC_ChopperEnemyHelper.SearchEnemyForRocket(owner);
			m_fTimeRocketDelay = 0;
		}
		
		DrawHelicopterVectors(owner);
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Set turn for heli
	
	Multiple angular velocities are calculated.
	- Speed multiplier : The faster we're going, certain values needs to grow.
	- Roll pitch : The faster we go, the steeper the heli nose shall point down.
	- Roll on direction: The steeper the turn we're about to make, roll the heli left or right .
	- Roll back : Set the heli to return to normal flight position.
	*/
	private void SetTurn(IEntity owner, float deltaTime)
	{
		if (deltaTime == 0)
		{
			return;
		}

		//Get heli position
		vector origin = owner.GetOrigin();
		//Get the heli vectors
		vector heliVelocity = owner.GetPhysics().GetVelocity();
		vector heliPitch = owner.GetTransformAxis(0);
		vector heliUp = owner.GetTransformAxis(1);
		vector heliRoll = owner.GetTransformAxis(2);
		
		//SPEED: Set speed according to previous turns
		//Ignore height component
		float angle = Math.AbsFloat(SDRC_Math.GetAngleBetweenVectorsXZ(m_vHeliForward, m_vHeliDirectionFuture));
		m_fDbgAngle = angle;
		
		//Count the angle of the turn. The steeper the turn, the slower heli should be moving.
		m_fSpeedMul = Math.Clamp((angle * Math.RAD2DEG), 1, 90);											//Was 1,90
		m_fSpeedMul = m_fThrottle * (SPEED_GAIN - (m_fSpeedMul / SPEED_TURN_DIV));

		//In case we're landing, we need to modify the speed
		m_fSpeedMul = m_fSpeedMul * m_fSpeedLandingMul;
		
		//Set 
		m_fSpeedStart = m_fSpeed;
		m_fSpeedTarget = m_fSpeed * m_fSpeedMul;

		//If we're too close to ground, slow down the speed to allow time for climb
		float altitude = SDRC_ChopperHelper.GetAltitude(owner);
		const int ALTITUDE_ADD = 5;
		if ((altitude + ALTITUDE_ADD) < m_fFlyHeightLow)
		{		
//				float surfaceY = SDRC_Misc.GetSurfaceYWithWater(m_vOrigin);
//				float mul = (m_vOrigin[1] - surfaceY) / (m_fFlyHeightLow - surfaceY);
			float mul = (altitude + ALTITUDE_ADD) / m_fFlyHeightLow;
			mul = Math.Clamp(mul, 0, 1);
			m_fSpeedTarget = m_fSpeedTarget * mul;
		}

		m_fSpeedTarget = Math.Clamp(m_fSpeedTarget, m_fSpeedMin, m_fSpeedMax);
		m_fTimeSpeed = 0;	//Start to change speed
								
		//Handle yaw, pitch roll		
		if (m_fSpeedLandingMul > 0.95)
		{							
			//ROLL PITCH: Change pitch according to speed		
			m_fAnglePitch = PITCH_ANGLE_FLAT_RAD + PITCH_ANGLE_RAD * m_fSpeedMul;
//			m_fAnglePitch = Math.Clamp(m_fAnglePitch, -0.61, -0.45);
			
			if (m_eHeliState == SDRC_EHeliState.RAISE)
			{
				//Turn nose down
				m_fAnglePitch = -15 * Math.DEG2RAD;
			}
			
			m_vRadRollPitch = SDRC_Math.RotateAroundAxis(m_vHeliForward, heliPitch, m_fAnglePitch);
			m_vRadRollPitch = SDRC_Math.ComputeAngularVelocity(m_vHeliForward, m_vRadRollPitch, deltaTime * 0.5);
			
			//ROLL ON DIRECTION: See how steep we're turning. Roll the helicopter accordingly for more natural flight. We only care about ZX plane.
			m_fAngleRoll = SDRC_Math.GetAngleBetweenVectorsXZ(m_vHeliForward, m_vHeliDirectionFuture);
			m_fAngleRoll = Math.Clamp(m_fAngleRoll, -0.5, 0.5) * ROLL_ANGLE_MUL;
			m_vRadRollVel = SDRC_Math.RotateAroundAxis(m_vHeliForward, heliUp, m_fAngleRoll);
			m_vRadRollVel = SDRC_Math.ComputeAngularVelocity(heliUp, m_vRadRollVel, deltaTime);
			
			//Count the angular velocity
			m_vAngularVel = SDRC_Math.ComputeAngularVelocity(m_vHeliForward, m_vHeliDirection, deltaTime);
		}
		else
		{
			//Flatten the chopper when landing
			m_vAngularVel = vector.Zero;
			m_vRadRollVel = vector.Zero;
			m_vRadRollPitch = vector.Zero;
		}
			
		//ROLL UP (YAW): Count the angle from heli up vs world up. The heli should slowly move back to horizontal flight.
		m_fAngleRollBack = SDRC_Math.GetAngleBetweenVectors(heliUp, vector.Up);
		m_vRadRollBack = SDRC_Math.ComputeAngularVelocity(heliUp, vector.Up, deltaTime * 0.6);
		
		owner.GetPhysics().SetAngularVelocity(m_vAngularVel + m_vRadRollVel + m_vRadRollBack + m_vRadRollPitch);
	}

	//------------------------------------------------------------------------------------------------	
	/*!	
	Set velocity vector
	*/
	private void SetVelocity(IEntity owner)
	{
		//Set velocity
		vector velVector = vector.Zero;
		
		if (    (m_eHeliState != SDRC_EHeliState.ON_GROUND)
		     && (m_eHeliState != SDRC_EHeliState.WAIT)
		   )
		{
			velVector = m_vDestination;
			vector rotVector = owner.GetAngles();
			velVector.Normalize();
			float forceMultiplier = m_fSpeed;
			float forceRotorUp = m_fRotorForce0 * ROTOR_FORCE_UP_MUL * 10;
			
			velVector = {velVector[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * forceMultiplier, velVector[1] * forceRotorUp * m_fRotorForceMultiplier, velVector[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * forceMultiplier};
		}
				
		owner.GetPhysics().SetVelocity(velVector);
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Simulate the use of throttle.
	
	This will modify the rotor force up multiplier trying to keep the heli flying close to the spline points. 
	In case of too low altitude, panic mode is enabled. 
	*/
	private void HandleRotorForce(IEntity owner)
	{
		if (m_vDestination == vector.Zero)
		{
			return;		
		}

		float splineHeightFromGround = m_vSplinePointBelow[1];		
		float heliHeightFromGround = m_vOrigin[1] - 3;				//Move the origin slightly below the spline
				
		//The normal way to slowly go towards the spline
		int bigMul = 60;	//was 25

		
		switch (m_eHeliState)
		{
			case SDRC_EHeliState.HOVER:
			{
				//In HOVER state, do movemements slow
				bigMul = 14;
				break;
			}		
			case SDRC_EHeliState.RAISE:
			{
				//In RAISE state, do somewhat rapid climb slow
				bigMul = 150;
				break;
			}	
		}
				
		//In fly state, react to low flying
		if (m_eHeliState == SDRC_EHeliState.FLY)
		{
			//We're below the spline, let's raise bit more agressively
			if (heliHeightFromGround < splineHeightFromGround)
			{
				bigMul = 200;
			}
					
			//Modify if we're too close to the ground, raise very aggressively
			float altitude = SDRC_ChopperHelper.GetAltitude(owner);
			
			if (altitude < m_fFlyHeightLow)
			{
				bigMul = 300;
			}				
		}
		
		if (splineHeightFromGround == 0)
		{
			splineHeightFromGround = 0.01;
		}
		m_fRotorForceMultiplier = 0 - (bigMul * ( (heliHeightFromGround - splineHeightFromGround) / splineHeightFromGround ));	
	}
			
	//------------------------------------------------------------------------------------------------	
	// Flight path things
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Create the initial flight path 
	*/
	void InitFlight(IEntity owner, vector origin)	
	{
/*		if (!GetGame().GetWorld())
		{
			return;
		}*/
		
		//Store the original firstDestination
		m_vOriginalDestination = origin;
		
		//Create initial flypath. 		
		//We add two points to m_vFlightPoints that work as guide for the heli direction. 		
		AddFlyPathPoint(origin);	//point 1
		//AddDebugMarker(origin, ARGB(255, 0, 255, 00), 2.0, m_sDid, 10);
		
		//If no destination were defined, let's create a random one
		if (m_vFlyDestinations.IsEmpty())
		{
			vector destination = SDRC_Misc.GetCoordinatesOnCircle(owner.GetOrigin(), m_fDistanceLow, SDRC_Misc.RandomInt(0, 360));
			AddDestination(SDRC_EFlyWayPointType.FLY, destination);
		}

		//Add a point towards the first destination
		vector helipos = owner.GetOrigin();
		vector direction = vector.Direction(helipos, m_vFlyDestinations[0].pt);
		vector pos = owner.GetOrigin() + direction.Normalized() * 10;
		pos[1] = helipos[1];
		AddFlyPathPoint(pos);		//point 2 .. some meters towards the destination
		//AddDebugMarker(pos, ARGB(255, 0, 255, 00), 2.0, m_sDid, 10);
		
		//Set chopper to initial position in case an specific origin was provided
		if (m_bAutoStart)
		{		
			owner.SetOrigin(m_vFlightPoints[0].pt);
		}

		//Store the origin. This value is updated in EOnFrame, but needed already in calculations.
		m_vOrigin = owner.GetOrigin();				
		SDRC_Log.Add("[SDRC_ChopperComp:InitFlight] Chopper initial position: " + owner.GetOrigin(), LogLevel.DEBUG);
		
		//Turn chopper to face the first destination
		SDRC_Math.TurnEntityTowardsXZ(owner, m_vFlyDestinations[0].pt);
		
		if (!m_bAutoStart)	//With autostart, use the origin of the chopper spawn
		{
			SetFlightPointHeight();
		}
		
		//Create points for spline
		CreateFlightPoints(owner);
		
		array<vector> flyPathPoints = {};
		GivePoints(flyPathPoints, m_vFlightPoints);
		SDRC_Spline3D.GenerateSplinePoints(flyPathPoints, m_vSplinePoints, -1);
		
		//Set final values
		m_iClosestIndex = 2;
		m_iOldClosestIndex = m_iClosestIndex;
		
		//Check that points are above ground
		SetSplinePointsAboveGround();
		
		m_fSpeed = 0.1;
		m_fSpeedTarget = m_fSpeed;
		
		//NOTE: We draw the debug paths once the component is ready
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Create the runtime flight path with waypoint definition
	
	\param firstDestination 
	*/
	void CreateNewFlight(IEntity owner, vector firstDestination = vector.Zero)
	{
		//Clear any existing path points
		ResetFlight();
		
		//Add an initial flight position. 
		if (firstDestination == vector.Zero)
		{		
			AddFlyPathPoint(m_vDestinationFuture);
		}
		else
		{
			AddFlyPathPoint(firstDestination);
		}
		
		//Generate a destination point according to requested behaviour
		GenerateWayPoint(owner, m_fWpType);
	
		//Create points for spline		
		CreateFlightPoints(owner);
		
		SetFlightPointHeight();
		array<vector> flyPathPoints = {};
		GivePoints(flyPathPoints, m_vFlightPoints);
		SDRC_Spline3D.GenerateSplinePoints(flyPathPoints, m_vSplinePoints, -1);
		
		//Search the closest indes from the spline start
		m_iClosestIndex = 0;
		float distance = SDRC_Spline3D.GetDistanceFromSpline(m_vSplinePoints, owner.GetOrigin(), m_iClosestIndex, false);	//NOTE: This will set m_iClosestIndex
		m_iOldClosestIndex = m_iClosestIndex;
		
		//Check that points are above ground
		SetSplinePointsAboveGround();
		
		if (m_vSplinePoints.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_ChopperComp:CreateFlightPath] No points!", LogLevel.ERROR);
		}
		
		DrawDebugPaths();
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Clear the fly path as a preparation for a completely new path
	*/
	void ResetFlight()
	{
		m_vFlightPoints.Clear();
		m_vSplinePoints.Clear();
	}
	
	//------------------------------------------------------------------------------------------------	
	// FlyPoint fixing and sanity check
	//------------------------------------------------------------------------------------------------	
		
	//------------------------------------------------------------------------------------------------	
	/*!	
	Set the requested flight path points between min/max flying height.
	*/	
	private void SetFlightPointHeight()
	{
		float y = 0;
		
		foreach (int i, SDRC_FlyPathPoint flightPoint : m_vFlightPoints)
		{
			vector pt = flightPoint.pt;
			
			//Do not change height of two first points. These are the two points from the previous spline. 
			if (i < 2)
			{
				continue;
			}

			//Initial height will be on ground
			y = SDRC_Misc.GetSurfaceYWithWater(pt);
			float flyHeight = 0;
						
			if (flightPoint.type == SDRC_EHeliState.LAND)
			{
				//Do nothing .. height will be on ground due to y being set and flyHeigt is zero. See above.
			}
			else
			{
				flyHeight = SDRC_Misc.RandomFloat(m_fFlyHeightLow, m_fFlyHeightHigh);
			}

			pt[1] = 0;	//We may in the future use the provided Y coord for the points. For now we set it to 0.
			pt[1] = pt[1] + flyHeight + y;			
			m_vFlightPoints[i].pt = pt;
		}
	}
		
	//------------------------------------------------------------------------------------------------	
	/*!	
	Check that spline points are above ground. Raise the point if needed.
	*/	
	private void SetSplinePointsAboveGround()
	{	
		//If we change the Y values, we want to smooth the spline
		bool isSmoothingNeeded = false;
		int smoothCount = 0;				//How many points to smooth. 0 = all
		
		//Make sure the points are at minimum m_fFlyHeightLow from the ground.
		foreach (int i, vector pt : m_vSplinePoints)
		{
			float y = SDRC_Misc.GetSurfaceYWithWater(pt);

			if (pt[1] < (y + m_fFlyHeightLow))
			{
//				pt[1] = y + ( (m_fFlyHeightHigh + m_fFlyHeightLow) / 2 ) ;	//Make chopper fly higher for a moment
				pt[1] = y + m_fFlyHeightLow + 5;	//Make chopper fly higher for a moment
				m_vSplinePoints[i] = pt;
				
				isSmoothingNeeded = true;
			}
		}

		//If we're raising from the ground, put some of the points closer to ground
		if (m_eHeliState == SDRC_EHeliState.RAISE)
		{
			int count = m_vSplinePoints.Count() - 1;
			//Find high point, low point and difference
			float p0 = m_vOrigin[1];
			float p1 = m_vSplinePoints[m_vSplinePoints.Count() - 1][1];
			float pdiff = p1 - p0;

			//Create a Y spline to replace the given points to smooth the curve for raising
			foreach (int i, vector splinePoint : m_vSplinePoints)
			{
				float step = i / m_vSplinePoints.Count();
				splinePoint[1] = p0 + pdiff * SDRC_Math.HalfBell(step);
				m_vSplinePoints[i] = splinePoint;
			}
			
			isSmoothingNeeded = false;
		}
				
		//If we're landing set some of the last points close to the ground
		if (m_eHeliState == SDRC_EHeliState.LAND)
		{
			const int POINTS_TO_GROUND = 6;
			int lastIdx = m_vSplinePoints.Count() - 1;

			//Create a Y spline to replace the given points to smooth the curve for landing
			if (m_vSplinePoints.Count() - 1 > POINTS_TO_GROUND)
			{
				//Find high point, low point and difference
				float p0 = m_vSplinePoints[lastIdx - POINTS_TO_GROUND][1];
				float p1 = SDRC_Misc.GetSurfaceYWithWater(m_vSplinePoints[lastIdx]);
				float pdiff = p0 - p1;
				
				for (int i = 0; i < POINTS_TO_GROUND; i++)
				{					
					float step = 1 - (i / POINTS_TO_GROUND);	//NOTE: The step will not go from 1..0 but end a little earlier. The last point of the bell is ignored. Change to (POINTS_TO_GROUND -1) for full bell curve.
					
					vector pt = m_vSplinePoints[lastIdx - POINTS_TO_GROUND + i + 1];
					pt[1] = p1 + pdiff * SDRC_Math.HalfBell(step);
					m_vSplinePoints[lastIdx - POINTS_TO_GROUND + i + 1] = pt;
				}
				
				smoothCount = m_vSplinePoints.Count() - POINTS_TO_GROUND;
			}
			else
			{
				vector pt = m_vSplinePoints[lastIdx];
				float y = SDRC_Misc.GetSurfaceYWithWater(pt) + 5;	//Don't put it exactly to zero
				pt[1] = y;
				m_vSplinePoints[lastIdx] = pt;				
			}
		}		
		
		if (isSmoothingNeeded)
		{
			//Smooth the Up curve
			SDRC_Spline3D.SmoothSplineUpOnly(m_vSplinePoints, smoothCount);
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	// Waypoint functions
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Functionality to generate future destination(s) for the chopper
	*/
	private void GenerateWayPoint(IEntity owner, SDRC_EWaypointGenerationType wpGenType)
	{		
		vector pos = "0 0 0";

		if (m_vFlyDestinations.IsEmpty())
		{
			//Random flying for a helicopter
			if ( (wpGenType == SDRC_EHeliWaypointGenerationType.RANDOM) || 
			     (wpGenType == SDRC_EHeliWaypointGenerationType.SEARCH)
			   )
			{					
				pos = SDRC_ChopperHelper.GetRandomPosition(owner.GetOrigin(), m_fDistanceLow, m_fDistanceHigh);
				AddDestination(SDRC_EFlyWayPointType.FLY, pos);
				AddDebugMarker(pos, ARGB(255, 255, 00, 00), 2.0, m_sDid, 200);				
			}
			
			//Fly around a certain area
			if (wpGenType == SDRC_EHeliWaypointGenerationType.PATROL)
			{
				const int count = 3;
				const int degree = 45; 	// Degrees per count
				const int sign = 1;		//SDRC_Misc.RandomSign(); <- does not work very well
				
				for (int i = 0; i < count; i++)
				{			
					float range = Math.RandomFloat(m_fDistanceLow, m_fDistanceHigh);
					//Make waypoints around the position to patrol.					
					vector dir = SDRC_Math.RotateAroundAxis(m_vHeliDirection, vector.Up, sign * i * degree * Math.DEG2RAD);
					dir.Normalize();
					pos = m_vOriginalDestination + dir * range;
					
					AddDebugMarker(pos, ARGB(255, 0, 0, 255), 2.0, m_sDid, 50 + i * 20);
					
					AddFlyPathPoint(pos);
				}			
			}
			
			//Fly around a certain area
			if (wpGenType == SDRC_EHeliWaypointGenerationType.LANDING)
			{
				return;
			}
			
			//Fly around a certain area
			if (wpGenType == SDRC_EHeliWaypointGenerationType.FLY_AWAY)
			{
				return;
			}
		}
	}
		
	//------------------------------------------------------------------------------------------------	
	/*!	
	Create fly points
	Takes the points from m_vFlyDestinations and generates points to be used for spline creation
	*/	
	private void CreateFlightPoints(IEntity owner)
	{
		//Set the first point to same height as heli. 
		m_vFlightPoints[0].pt[1] = m_vOrigin[1];
		
		//Get vector from heli position to the first point to fly to.
		vector origin = owner.GetOrigin();
		vector direction = vector.Direction(origin, m_vFlightPoints[m_vFlightPoints.Count() - 1].pt);
		vector pos = owner.GetOrigin() + direction.Normalized() * 200;
		pos[1] = origin[1];
		AddFlyPathPoint(pos);
		AddDebugMarker(pos, ARGB(255, 0, 255, 00), 1.0, m_sDid, 30);
						
		//Add destinations .. if any
		int lastIdx = -1;
		
		foreach (int idx, SDRC_FlyPathPoint flyDestination : m_vFlyDestinations)
		{		
			AddDebugMarker(flyDestination.pt, ARGB(32, 255, 128, 64), 1.0, m_sDid, 50);
			
			//Distance of last flight point defined and the next destination
			float distance = vector.DistanceXZ(m_vFlightPoints[m_vFlightPoints.Count() - 1].pt, flyDestination.pt);
	
			//Get the angle for the destination
			vector dir0;	//Previous flight direction
			vector dir1;	//New flight direction
			vector p0 = m_vFlightPoints[m_vFlightPoints.Count() - 2].pt;
			vector p1 = m_vFlightPoints[m_vFlightPoints.Count() - 1].pt;
			vector p2 = flyDestination.pt;
			float heliAngle = SDRC_Math.GetRadiansBetweenThreePointsXZ(p0, p1, p2) * Math.RAD2DEG;

			SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Distance: " + distance + " - Angle: " + heliAngle, LogLevel.DEBUG);
			
			//Is the angle too steep? Re-route.
			if ( (Math.AbsFloat(heliAngle) < WP_ANGLE) && (distance > 200) )
			{				
				SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Heli direction angle is steep: " + heliAngle, LogLevel.SPAM);
				
				//Get the last point
				vector point = m_vFlightPoints[m_vFlightPoints.Count() - 1].pt;
				
				//We need to take a detour. Add an additional points outside of the line to make the route rounder				
				float lerpRnd = SDRC_Misc.RandomFloat(0.25, 0.65);
				float divRnd = SDRC_Misc.RandomFloat(1.5, 4);
				
				//Depending on the angle decide if we re-route left ot right				
				bool isOnLeft = SDRC_Math.IsPointOnLeft(p0, p1, p2);
								
				//Find a point along the fly path and move it away from the line along tangent
				vector vec = SDRC_Math.CreateOffsetMidPoint(point, flyDestination.pt, (distance / divRnd), lerpRnd, isOnLeft);
				AddFlyPathPoint(vec);
				
				AddDebugMarker(vec, ARGB(255, 0, 0, 0), 1.0, m_sDid, 500);
			}
			
			AddFlyPathPoint(flyDestination.pt, flyDestination.type);
			lastIdx = idx;
			
			if (flyDestination.type != SDRC_EFlyWayPointType.FLY)
			{
				break;
			}
		}

		//If only two points, add a mid point
		if (m_vFlightPoints.Count() <= 2)
		{
			vector p0 = m_vFlightPoints[0].pt;
			vector p1;
/*			if (m_vFlightPoints.Count() == 1)
			{
				p1 = m_vFlyDestinations[0].pt;
			}
			else
			{	
				p1 = m_vFlightPoints[1].pt;
			}*/
			p1 = m_vFlightPoints[1].pt;
			vector mid = vector.Lerp(p0, p1, 0.5);
			AddFlyPathPoint(mid, index: 1);
		}		
		
		//There should always be at least 3 points
		if (m_vFlightPoints.Count() < 3)
		{
			SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] This should never happen! ", LogLevel.WARNING);
		}
				
		SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Created " + m_vFlightPoints.Count() + " points.", LogLevel.SPAM);

		//Clear the destinations 
		for (int i = 0; i <= lastIdx; i++)
		{
			m_vFlyDestinations.RemoveOrdered(0);
		}		
		SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Destinations left: " + m_vFlyDestinations.Count(), LogLevel.DEBUG);
	}		

	//------------------------------------------------------------------------------------------------	
	/*!	
	Add a point to fly path. 
	*/
	private void AddFlyPathPoint(vector destination, SDRC_EFlyWayPointType type = SDRC_EFlyWayPointType.FLY, int index = -1)
	{
		if (type == SDRC_EFlyWayPointType.LAND)
		{
			destination = SDRC_Misc.SetPosToSurface(destination);
			SetState(SDRC_EHeliState.LAND);
			m_fTimeLanding = 0;
		}
		
		SDRC_FlyPathPoint fpp = new SDRC_FlyPathPoint();
		
		if (index == -1)
		{
			fpp.Set(type, destination);
			m_vFlightPoints.Insert(fpp);
		}
		else
		{
			fpp.Set(type, destination);
			m_vFlightPoints.InsertAt(fpp, index);
		}
	}		

	//------------------------------------------------------------------------------------------------	
	// States 
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Handle state (machine)
	*/
	private void HandleState(IEntity owner, float timeSlice)
	{	
		switch (m_eHeliState)
		{
			case SDRC_EHeliState.LAND:
			{
				HandleLanding(timeSlice);	
				break;
			}
			
			//If on ground, immediately set next state after touch down
			case SDRC_EHeliState.ON_GROUND:
			{
				SetNextState(owner);
				break;
			}
			
			//If on ground, immediately set next state after touch down
			case SDRC_EHeliState.GET_OUT:
			{
				array<SCR_AIGroup> groups = {};
				SDRC_VehicleHelper.GetOutVehicle(owner, groups);
		
				foreach (SCR_AIGroup group : groups)
				{
					SDRC_Log.Add("[SDRC_ChopperComp:HandleState] Create waypoint for AI group: " + group, LogLevel.DEBUG);			
					
					vector pos = SDRC_Misc.RandomizePos(owner.GetOrigin(), 50);			
					AddDebugMarker(pos);
					SDRC_WPHelper.CreateWaypoint(group, pos, SDRC_EWaypointMoveType.MOVE);
				}				
				
				SetNextState(owner);
				break;
			}
		}
		
		//Wait for the state timer to end and go to next state
		if ( (m_eHeliState != SDRC_EHeliState.FLY) && (m_fTimeInState < 0) && m_bTimeInStateEnabled) 
		{
			SetNextState(owner);
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Sets the next destination for an action. 
	- FLY will start to fly
	- Others will have some action bound to them.
	*/
	void SetNextState(IEntity owner)
	{
		SDRC_EFlyWayPointType nextType = SDRC_EFlyWayPointType.UNDEFINED;
		
		//By default we remove the destination
		bool isRemoveDestination = true;
		
		//If needed, create new flight path
		bool isCreateFlight = false;
		
		//First destination for flight. In most cases left at zero.
		vector firstDestination = vector.Zero;
		
		//If not destinations defined, start to fly		
		if (m_vFlyDestinations.IsEmpty())
		{
			nextType = SDRC_EFlyWayPointType.FLY;
		}
		else
		{
			nextType = m_vFlyDestinations[0].type;
		}

		switch (nextType)
		{
			case SDRC_EFlyWayPointType.FLY:
			{
				SetState(SDRC_EHeliState.FLY);
				ResetOriginalValues();
				//Don't remove the destination as it has the next point where to fly
				isRemoveDestination = false;
				break;
			}			
			case SDRC_EFlyWayPointType.FLY_AWAY:
			case SDRC_EFlyWayPointType.FLY_AWAY_IMMEDIATELY:
			{
				ResetOriginalValues();
				SetState(SDRC_EHeliState.FLY);
				//Fly for a while and then go to END state
				SetTimeInState(120);
				AddDestination(SDRC_EFlyWayPointType.END); 
				break;
			}		
			case SDRC_EFlyWayPointType.END:
			{
				SetState(SDRC_EHeliState.DESTROYED);
				break;
			}
			case SDRC_EFlyWayPointType.GET_OUT:
			{
				SetState(SDRC_EHeliState.GET_OUT);
				break;
			}
			case SDRC_EFlyWayPointType.WAIT:
			{
				//Just wait
				SetState(SDRC_EHeliState.WAIT);
				SetTimeInState(m_vFlyDestinations[0].value); 
				break;
			}
			case SDRC_EFlyWayPointType.RAISE:
			{
				//Reset heli settings
				ResetOriginalValues();
				
				//Create a first destination that is a short way to where we're planning to go. Smoothens the flight.
				firstDestination = SDRC_ChopperHelper.GetDestinationForward(owner, m_vFlyDestinations[0].pt[0] / 5);
				//Make it low, to get the helicopter nose down.
				firstDestination[1] = (firstDestination[1] / 2);
				//Then fly forward
				vector pos = SDRC_ChopperHelper.GetDestinationForward(owner, m_vFlyDestinations[0].pt[0]);				
				pos[1] = m_fFlyHeightLow + 5;	//Fly to a point slightly above low fly point				
				AddDestination(SDRC_EFlyWayPointType.RAISE, pos);
				SetState(SDRC_EHeliState.RAISE);
				
				float time = m_vFlyDestinations[0].value;
				if (time == -1)
				{
					//If no time defined, set it to 5
					time = 5;
				}
				
				SetTimeInState(time);
				isCreateFlight = true;			
				break;
			}	
			case SDRC_EFlyWayPointType.HOVER:
			{
				//NOTE: We do not use AddDestination() for setting the state
				
				//Reset heli settings
				ResetOriginalValues();
				
				//Stop heli from moving
				m_fSpeedMin = 0.3;
				m_fSpeedMax = 0.6;
				m_fSpeedLandingMul = 0;
				
				//For hovering, we add one point to the spline
				ResetFlight();
				
				vector pos = m_vOrigin;
				pos[1] = pos[1] + m_vFlyDestinations[0].pt[1];		//Hover above original point
				m_vSplinePoints.Insert(pos);
				
				//Just wait.
				SetState(SDRC_EHeliState.HOVER);
				SetTimeInState(m_vFlyDestinations[0].value);
				
				DrawDebugPaths();
				break;
			}
			case SDRC_EFlyWayPointType.STOP_ENGINE:
			{
				//Stop engine and wait
				m_Helicopter_s.EngineStop();
				SetState(SDRC_EHeliState.WAIT);
				SetTimeInState(30);
				break;
			}
		}
		
		//Remove the destination if it was handled.	By default it is.
		if (isRemoveDestination)
		{
			m_vFlyDestinations.RemoveOrdered(0);
		}
		
		//With new destinations, create new flight.
		if (isCreateFlight)
		{
			CreateNewFlight(owner, firstDestination);
		}
	}		

	//------------------------------------------------------------------------------------------------	
	void SetTimeInState(int seconds)	
	{
		if (seconds == -1)
		{
			seconds = 0;
		}
		
		m_fTimeInState = seconds;
		
		if (seconds == 0)
		{
			m_bTimeInStateEnabled = false;
		}
		else
		{
			m_bTimeInStateEnabled = true;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void SetState(SDRC_EHeliState state)
	{
		m_eHeliState = state;
		
		//If in normal flight mode, disable the TimeInState counter
		if (state == SDRC_EHeliState.FLY)
		{			
			SetTimeInState(0)
		}
		
		SDRC_Log.Add("[SDRC_ChopperComp:SetState] State: " + SCR_Enum.GetEnumName(SDRC_EHeliState, m_eHeliState), LogLevel.DEBUG);
	}
	
	//------------------------------------------------------------------------------------------------
	SDRC_EHeliState GetState()
	{
		return m_eHeliState;
	}
				
	//------------------------------------------------------------------------------------------------	
	/*!	
	Handle landing
	*/
	private void HandleLanding(float timeSlice)
	{
		const float LANDING_DISTANCE = 130;
		const float TIME_TO_LAND = 9;				//(seconds)
		const float LANDED_HEIGHT = 0.5;
		
		vector lastPt = m_vSplinePoints[m_vSplinePoints.Count() - 1];			
		float distance = vector.Distance(m_vOrigin, lastPt);

		if (distance < LANDING_DISTANCE)
		{
			if (!m_Helicopter_s.HasAnyGroundContact())
			{				
				//Set the last point on ground
				lastPt[1] = SDRC_Misc.GetSurfaceYWithWater(lastPt) + LANDED_HEIGHT;
				m_vSplinePoints[m_vSplinePoints.Count() - 1] = lastPt;
				
				m_fTimeLanding += timeSlice;
				m_fTimeLanding = Math.Clamp(m_fTimeLanding, 0, TIME_TO_LAND);
				float mul = 1 - m_fTimeLanding / TIME_TO_LAND;
				
//				m_fRotorForceMultiplier = -3.2 + -5.5 * SDRC_Math.HalfBell(mul);
				m_fRotorForceMultiplier = -3.0 + -4.0 * SDRC_Math.FullBell(mul);
				
				//Helicopter to descend
		        m_Helicopter_s.RotorSetForceScaleState(0, 0);
		        m_Helicopter_s.SetThrottle(0);
				
		        m_Helicopter_s.RotorSetForceScaleState(0, m_fRotorForce0Orig * mul * 0.01);
		        m_Helicopter_s.SetThrottle(m_fThrottleOrig * mul);
				float height = m_Helicopter_s.GetAltitudeAGL();
				m_fSpeedMin = distance / Math.Clamp(height, 0.1, 1000);
				m_fSpeedLandingMul = mul;
			}
			else
			{
				SDRC_Log.Add("[SDRC_ChopperComp:HandleLanding] Ground contact!", LogLevel.DEBUG);
				SetState(SDRC_EHeliState.ON_GROUND);
				m_fSpeedMin = 0;
				m_fSpeedLandingMul = 0;
				m_fRotorForceMultiplier = 0;
			}
		}
	}	
	
	//------------------------------------------------------------------------------------------------	
	// Destination settings
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Add a destination for future.
	\param destination Next position to fly to. Multiple destinations can be defined by calling multiple times.
	\param type Flying behaviour
	\param value Specific value for behaviour
	\param index At which index on to put the destination. By default, added to the end of array.
	*/
	void AddDestination(SDRC_EFlyWayPointType type = SDRC_EFlyWayPointType.FLY, vector destination = vector.Zero, float value = -1, int index = -1)
	{
		//In normal case, we just add a destination for future handling. 
		//Below are a few special cases where we need either react immediately of change some other params.

		bool addFlyPoint = true;
		
		switch (type)
		{
			case SDRC_EFlyWayPointType.FLY_AWAY:
			{
				//Fly away after all destinations have been handled
				SetState(SDRC_EHeliState.FLY_AWAY);
				m_fWpType = SDRC_EHeliWaypointGenerationType.FLY_AWAY;
				break;
			}
			case SDRC_EFlyWayPointType.FLY_AWAY_IMMEDIATELY:
			{
				//Fly away immediately
				ResetDestinations();
				SetState(SDRC_EHeliState.FLY_AWAY);
				m_fWpType = SDRC_EHeliWaypointGenerationType.FLY_AWAY;
				break;
			}		
			case SDRC_EFlyWayPointType.LAND:
			{
				//Land the chopper
				SetState(SDRC_EHeliState.LAND);			//Set state here to activate LANDing handling
				break;
			}	
			case SDRC_EFlyWayPointType.GET_OUT:
			{
				//Get out
				SetState(SDRC_EHeliState.GET_OUT);		//Set state here to activate GET_OUT handling
				break;
			}
			case SDRC_EFlyWayPointType.HOVER:
			{
				//Set hover position to be the current position
				destination = m_vOrigin;
				break;
			}
			case SDRC_EFlyWayPointType.HOVER_UP:
			{
				//Set hover position to according to destination[1]
				type = SDRC_EFlyWayPointType.HOVER;
				break;
			}		
			
			//Macro actions
			case SDRC_EFlyWayPointType.M_LAND_TROOPS:
			{
				AddDestination(SDRC_EFlyWayPointType.LAND, destination);
				AddDestination(SDRC_EFlyWayPointType.GET_OUT);
				AddDestination(SDRC_EFlyWayPointType.WAIT, value : value);
				AddDestination(SDRC_EFlyWayPointType.HOVER_UP, "0 25 0", 6);
				AddDestination(SDRC_EFlyWayPointType.RAISE, "200 0 0", 15);
				//All things are already added
				addFlyPoint = false;
				break;
			}			
		}

		if (addFlyPoint)
		{
			SDRC_FlyPathPoint fpp = new SDRC_FlyPathPoint();
			fpp.Set(type, destination, value);				
			
			if (index > -1)
			{
				m_vFlyDestinations.InsertAt(fpp, index);
			}
			else
			{
				m_vFlyDestinations.Insert(fpp);
			}
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Returns the last destination from the spline
	*/
	vector GetDestination()
	{
		return m_vSplinePoints[m_vSplinePoints.Count() - 1];
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Returns the destination where the helicopter is currently going. 
	NOTE: This is not the final destination on the spline
	*/
	vector GetCurrentDestination()
	{
		return m_vDestination;
	}	

	//------------------------------------------------------------------------------------------------
	/*!	
	Clear the destination as a preparation for a completely new path
	*/
	void ResetDestinations()
	{
		m_vFlyDestinations.Clear();
	}	
	
	//------------------------------------------------------------------------------------------------	
	// Helicopter settings
	//------------------------------------------------------------------------------------------------	
	
	void SetHeli(float speedMin, float speedMax, float flyHeightLow, float flyHeightHigh, SDRC_EHeliWaypointGenerationType wpType, float distanceLow, float distanceHigh)
	{
		SDRC_Log.Add("[SDRC_ChopperComp:SetHeli] Updating values.", LogLevel.DEBUG);
		
		m_fSpeedMin = speedMin;
		m_fSpeedMax = speedMax;
		m_fFlyHeightLow = flyHeightLow;
		m_fFlyHeightHigh = flyHeightHigh;
		m_fDistanceLow = distanceLow;
		m_fDistanceHigh = distanceHigh;
		m_fWpType =	wpType;
		
		m_fSpeed = m_fSpeedMin;
	}
	
	//------------------------------------------------------------------------------------------------
	private void StoreOriginalValues()
	{
		m_fThrottleOrig = m_fThrottle;
		m_fRotorForce0Orig = m_fRotorForce0;
		m_fRotorForce1Orig = m_fRotorForce1;
		m_fSpeedMinOrig = m_fSpeedMin;
		m_fSpeedMaxOrig = m_fSpeedMax;	
		
		//Reset runtime parameters
		m_fSpeedLandingMul = 1.0;
	}
	
	//------------------------------------------------------------------------------------------------
	private void ResetOriginalValues()
	{
		m_fThrottle = m_fThrottleOrig;
		m_fRotorForce0 = m_fRotorForce0Orig;
		m_fRotorForce1 = m_fRotorForce1Orig;
		m_fSpeedMin = m_fSpeedMinOrig;
		m_fSpeedMax = m_fSpeedMaxOrig;	
		
		//Reset runtime parameters
		m_fSpeedLandingMul = 1.0;
	}	
	
	//------------------------------------------------------------------------------------------------
	void SetSpeed(float min = -1, float max = -1)
	{
		if (min > -1)
		{
			m_fSpeedMin = min;
		}
		
		if (max > -1)
		{
			m_fSpeedMax = max;
		}			
	}

	//------------------------------------------------------------------------------------------------
	void SetAutostart(bool value)
	{
		m_bAutoStart = value;
	}
	
	//------------------------------------------------------------------------------------------------	
	// Enemy searching functionality
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Search for enemy and mark it. The knowledge will eventually be lost. 
	After a while, we may find another enemy to track.
	*/			
	private bool SearchForEnemy(IEntity owner)
	{
		bool found = false;
		
		if (!m_bSearchForEnemy)
		{
			return false;
		}
		
		if (m_iEnemyFoundTimeOut > SDRC_Misc.GetCurrentTickTime())
		{
			return false;
		}
		
		if ( (SDRC_Misc.GetCurrentTickTime() > m_iEnemyFoundTimeOut + ENEMY_FORGET_TIMEOUT) && (m_vEnemyPosition != "0 0 0") )
		{
			m_vEnemyPosition = "0 0 0";
			SDRC_Log.Add("[SDRC_ChopperComp:SearchForEnemy] Enemy position reset.", LogLevel.DEBUG);
		}
		
		m_vEnemyPosition = SDRC_ChopperEnemyHelper.SearchEnemy(owner);
		if (m_vEnemyPosition != vector.Zero)
		{
			found = true;
			m_iEnemyFoundTimeOut = SDRC_Misc.GetCurrentTickTime() + ENEMY_FOUND_TIMEOUT;
		}
		
		return found;
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!
	Enable/Disable enemy searching
	*/		
	void SetSearchForEnemy(bool value)
	{
		m_bSearchForEnemy = value;
	}

	//------------------------------------------------------------------------------------------------	
	/*!
	Get last known enemy position
	*/
	vector GetEnemyPosition()
	{
		return m_vEnemyPosition;
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!
	Reset enemy knowledge and timeout
	*/
	void EnemyHandled()
	{
		m_vEnemyPosition = "0 0 0";
		m_iEnemyFoundTimeOut = SDRC_Misc.GetCurrentTickTime() + ENEMY_FOUND_TIMEOUT;
	}	
		
	//------------------------------------------------------------------------------------------------	
	// Debugging things
	//------------------------------------------------------------------------------------------------	
			
	void DrawDebugPaths()
	{
		if (!m_bShowDebug)
		{
			return;
		}
		
		array<vector> flyPathPoints = {};
		GivePoints(flyPathPoints, m_vFlightPoints);
		
		SDRC_DebugHelper.DeleteDebugItems(m_sDid, marks: false);
		SDRC_DebugHelper.DrawPointList(m_vSplinePoints, m_sDid);		
		SDRC_DebugHelper.DrawPointList(flyPathPoints, m_sDid, ARGB(10, 64, 64, 192));
	}

	//------------------------------------------------------------------------------------------------	
	void AddDebugMarker(vector pos, int color = Color.RED, float radius = 1.0, string id = "NONE", int height = 300, bool snap = true)
	{	
		if (!m_bShowDebug)
		{
			return;
		}
		SDRC_DebugHelper.AddDebugPos(pos, color, radius, id, height, snap);	
	}	
		
	//------------------------------------------------------------------------------------------------	
	/*!
	Draw debugging details of helicopter
	*/	
	void DrawHelicopterVectors(IEntity owner)
	{
		if (!m_bShowDebug)
		{
			return;
		}
		
		vector origin = owner.GetOrigin();
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
		float health = damageManager.GetHealth();

		if (!m_Helicopter_s)
		{
			return;
		}

		vector heliUp = owner.GetTransformAxis(1);
		float angUp = SDRC_Math.GetAngleBetweenVectors(heliUp, vector.Up);
				
		if (DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_INFO))
		{		
			string debugText = 	//"Speedangle:" + angle * Math.RAD2DEG + "\n" +
								SCR_Enum.GetEnumName(SDRC_EHeliState, m_eHeliState) + "\n" + 
							   	"Speed:" + SDRC_Misc.FloatWithDecimals(m_fSpeed) + " " +
							   	"(" + SDRC_Misc.FloatWithDecimals(m_fSpeedStart) + "/" + SDRC_Misc.FloatWithDecimals(m_fSpeedTarget) + ") " +
	//						   	"Avg time:" + m_fTimeBetweenPtsAvg + "\n" +
							   	"mul:" + SDRC_Misc.FloatWithDecimals(m_fSpeedMul, 2) + " " + 
							   	"min:" + SDRC_Misc.FloatWithDecimals(m_fSpeedMin) + "\n" + 
								"";		
			debugText = debugText + 
							   	"Alt:" + 
								SDRC_Misc.FloatWithDecimals(origin[1] - SDRC_Misc.GetSurfaceYWithWater(origin)) + " " + 
							   	SDRC_Misc.FloatWithDecimals(m_Helicopter_s.GetAltitudeAGL()) + " " +
								"\n" + 
							   	"RotorForceMul:" + SDRC_Misc.FloatWithDecimals(m_fRotorForceMultiplier, 2) + "\n" +
	//						   	"SplinePoints:" + m_vSplinePoints.Count() + "\n" +
	//						   	"TurnInternal:" + m_fTimeTurnInterval + "\n" +
	//							"Angle: " + m_fDbgAngle * Math.RAD2DEG + "\n" +
//								"AnglePitch: " + m_fAnglePitch * Math.RAD2DEG + "\n" +
								"AnglePitch: " + SDRC_Misc.FloatWithDecimals(m_fAnglePitch) + "\n" +
	//							"AngleRoll: " + m_fAngleRoll * Math.RAD2DEG + "\n" +
	//							"AngleRollBack: " + m_fAngleRollBack * Math.RAD2DEG + "\n" +
	//							"DestinationPointAdd: " + m_iDestinationPointAdd + "\n" 
								"";
			debugText = debugText + 
//								"Init:" + m_bInInit + ", " +
//								"Pilots::" + SDRC_VehicleHelper.PilotCountAlive(owner) + "\n" +
//								"Working:" + SDRC_VehicleHelper.IsWorking(owner) + " - " + 
//								"Health: " + health + "\n" + 
	//							"Is piloted:" + SDRC_VehicleHelper.IsPiloted(owner) + "\n" +
								"";

			if (angUp > 1.0)
			{
				debugText = debugText + "AngleUp: ******** " + angUp + " ********";
			}
					
			DebugTextWorldSpace.Create(GetGame().GetWorld(), debugText, DebugTextFlags.ONCE, origin[0], origin[1], origin[2], 20);
		}
			
		if (!DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_LINES))
		{		
			return;
		}
		
		int idx = m_iClosestIndex;
		if (idx > m_vSplinePoints.Count() - 1)
		{
			idx = m_vSplinePoints.Count() - 1;			
			//SDRC_Log.Add("[SDRC_ChopperComp:DrawHelicopterVectors] Index fixed.", LogLevel.WARNING);
		}
						
		//Planned destination
		SDRC_ChopperHelper.DrawLine(origin, m_vSplinePoints[idx], Color.GRAY);		
		
		//Chopper destination direction vector
		vector vFwd = vector.Direction(origin, m_vDestination);
//		vFwd.Normalize();
//		SDRC_ChopperHelper.DrawLine(origin, origin + (vFwd * 20), Color.WHITE);
//		SDRC_ChopperHelper.DrawLine(origin, origin + vFwd, Color.WHITE);
		SDRC_ChopperHelper.DrawLine(origin, m_vDestination, Color.WHITE);

		//Chopper future destination direction vector
		vFwd = vector.Direction(origin, m_vDestinationFuture);
//		vFwd.Normalize();
//		DrawLine(origin, origin + (vFwd * 50), Color.BLACK);		
		SDRC_ChopperHelper.DrawLine(origin, origin + vFwd, Color.BLACK);		

		//Point on spline below the helicopter
		SDRC_ChopperHelper.DrawLine(origin, m_vSplinePointBelow, Color.RED);		
		
		//Draw vectors
		vector vDir2 = owner.GetTransformAxis(2);	//Forward
//		SDRC_ChopperHelper.DrawLine(origin, origin + (vDir2 * 30), Color.CYAN);		

		vector vDir0 = owner.GetTransformAxis(0);	//Side
//		SDRC_ChopperHelper.DrawLine(origin, origin + (vDir0 * 10), Color.DARK_CYAN);

		vector vDir1 = owner.GetTransformAxis(1);	//Up
		SDRC_ChopperHelper.DrawLine(origin, origin + (vDir1 * 15), Color.MAGENTA);		
		
		vector vUp = vector.Up;						//World Up
		SDRC_ChopperHelper.DrawLine(origin, origin + (vUp * 10), Color.MAGENTA);		
		
		//Roll vector
		vector vRoll = m_vRollTarget * Math.DEG2RAD;
		vRoll.Normalize();
//		SDRC_ChopperHelper.DrawLine(origin, origin + (vRoll * 15), Color.BLUE);		

		//RollPitch vector
		vRoll = m_vRadRollPitch * Math.DEG2RAD;
		vRoll.Normalize();
//		SDRC_ChopperHelper.DrawLine(origin, origin + (vRoll * 15), Color.BLUE);		
		
/*		vector vVec = m_vRadRollVel;
		vVec[1] = -vVec[2];
		vVec[2] = vVec[0];
		vVec[0] = 0;
		vVec.Normalize();
		SDRC_ChopperHelper.DrawLine(origin, origin + (vVec * 45), Color.WHITE);

		vVec = m_vRadRollBack;
		vVec.Normalize();
		SDRC_ChopperHelper.DrawLine(origin, origin + (vVec * 35), Color.WHITE);*/
								
		//Draw velocity vector
		vector vVel = owner.GetPhysics().GetVelocity();
		vVel.Normalize();
//		float currentSpeed = vVel.Length();
		SDRC_ChopperHelper.DrawLine(origin, origin + (vVel * m_fSpeed), Color.GRAY_75);			
		
		//Enemy stuff
		
		//Draw current enemy sighting		
		if (m_vEnemyPosition != "0 0 0")
		{
			SDRC_ChopperHelper.DrawLine(origin, m_vEnemyPosition, Color.PINK);
		}
		
		//Draw eyesight		
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(owner.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		array<IEntity> occupants = {};
		scr_compartmentManager.GetOccupants(occupants);

		foreach (IEntity occupant : occupants)
		{
			SCR_AICombatComponent aicc = SCR_AICombatComponent.Cast(occupant.FindComponent(SCR_AICombatComponent));
			if (aicc)
			{
				BaseTarget bt = aicc.GetCurrentTarget();
				if (bt)
				{
					IEntity target = bt.GetTargetEntity();
//					if (EntityUtils.IsPlayer(target))
//					{
						SDRC_ChopperHelper.DrawLine(occupant.GetOrigin(), target.GetOrigin(), Color.RED);
//					}
				}
			}
		}
	}
}