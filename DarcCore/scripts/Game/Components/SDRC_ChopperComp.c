//SDRC_ChopperComp.c

//Changes done in prefabs:
// - SCR_AIVehicleUsageComponent : Set true to Can Be Piloted

#ifndef SDRC_RELEASE
//	#define HELI_TESTING

	#ifdef HELI_TESTING
//		#define HELI_TESTING_AIRPORT
//		#define HELI_TESTING_HILL
		#define HELI_TESTING_LANDING
	#endif
#endif

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
	STOP_ENGINE,
	END,
}

enum SDRC_EHeliState
{
	UNKNOWN,
	FLY,
	DESTROYED,
	LAND,
	ON_GROUND,				//One frame state
	WAIT,					//Velocity disabled
	GETOUT,					//One frame state
	RAISE,
	
	FLY_AWAY,
	FLY_AWAY_IMMEDIATELY,	//NOTE: This is not a real state. When set, state will change to FLY_AWAY
	END,
	
//	LANDING			= LAND + WAIT + GETOUT,
//	FLYING			= FLY + FLY_AWAY + RAISE,
}

//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class SDRC_FlyPathPoint
{
	[Attribute(defvalue: "0 0 0", desc: "Destination")]
	vector pt;
	
	[Attribute( defvalue: "1", uiwidget: UIWidgets.SearchComboBox, desc: "SDRC_EFlyWayPointType", enums: ParamEnumArray.FromEnum( SDRC_EFlyWayPointType ) )]
	SDRC_EFlyWayPointType type;
	
	void SDRC_FlyPathPoint(vector pt_, SDRC_EFlyWayPointType type_ = SDRC_EFlyWayPointType.FLY)
	{
		pt = pt_;
		type = type_;
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_ChopperComp : ScriptGameComponent
{
	private static SDRC_ChopperComp s_Instance;	
	private ref array<vector> m_vSplinePoints = new array<vector>();
	private static VehicleHelicopterSimulation m_Helicopter_s;
	
	//Parameters accessible helicopter parameters
	[Attribute(defvalue: "0", desc: "Autostart chopper")]	
	bool m_bAutoStart;
	[Attribute(defvalue: "1.2", desc: "Throttle aka acceleration", params: "0.1 3.0 0.1")]	
	float m_fThrottle;
	float m_fThrottleOrig;
	[Attribute(defvalue: "3.0", desc: "Main rotor force", params: "0.1 10.0 0.1")]	
	float m_fRotorForce0;
	float m_fRotorForce0Orig;
	[Attribute(defvalue: "1.0", desc: "Rear rotor force", params: "0.1 2.0 0.1")]	
	float m_fRotorForce1;
	float m_fRotorForce1Orig;
	
	[Attribute(defvalue: "10.0", desc: "Minimum speed", params: "1.0 100.0 0.1")]	
	float m_fSpeedMin;				//Minimum speed
	float m_fSpeedMinOrig;
	[Attribute(defvalue: "30.0", desc: "Maximum speed", params: "1.0 100.0 0.1")]	
	float m_fSpeedMax;				//Maximum speed
	float m_fSpeedMaxOrig;
	[Attribute(defvalue: "40.0", desc: "Minimum fly height (from ground level)", params: "5 100.0 1")]	
	float m_fFlyHeightLow;			//Flight height low
	[Attribute(defvalue: "80.0", desc: "Maximum fly height (from ground level)", params: "5 100.0 1")]	
	float m_fFlyHeightHigh;			//Flight height high
	[Attribute(defvalue: "0.1", desc: "Minimum distance for waypoint", params: "0.1 1000.0 0.1")]	
	float m_fDistanceLow;			//Distance for waypoint min
	[Attribute(defvalue: "0.4", desc: "Maximum distance for waypoint", params: "0.1 1000.0 0.1")]	
	float m_fDistanceHigh;			//..max
	SDRC_EHeliWaypointGenerationType m_fWpType; 	

	//Flight path
	ref array<ref SDRC_FlyPathPoint> m_vFlightPoints = {};
	[Attribute("", UIWidgets.Object, "Destinations.")]	
	ref array<ref SDRC_FlyPathPoint> m_vFlyDestinations = {};	//Requested destinations
	
	//Original destination	
	private vector m_vOriginalDestination;				//Used to know where to patrol
		
	//Speed management
	private const float SPEED_INTERVAL = 1.0;			//(seconds) Interval to modify speed of the helicopter
	private const float SPEED_GAIN = 1.0;
	private const int SPEED_TURN_DIV = 42;				//The divider that affects how much speed is decreased on sharp turns. The higher the value, the less brake.
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
//	private const int TIME_FORCE_MOVE_POINT = 20;		//(seconds) Time to wait before force moving a point. This is to fix situations where the chopper gets stuck on a point.
	private const float TIME_IN_INIT = 10;				//(seconds) Time to be in init state. During this time, we don't check for damage or similar things.

	private const int FLIGHT_FIX_TIME = 5;				//(seconds) Time to wait between flight fixes when chopper is pointing to the sky.
	private const int FLIGHT_FIX_ANGLE = 1.35;			//Angle that enforces 
	
	//Rotor force multipliers
	private const float ROTOR_FORCE_MUL = 1.0;			//Rotor force multiplier. Bigger value makes the heli react faster to up/down movement
	private const float ROTOR_FORCE_UP_MUL = 1.2;		//Rotor force multiplier in velocity counting
	private const float ROTOR_FORCE_MUL_PANIC = 5.0;	//Rotor force multiplier used when avoiding ground. 
	
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
		
		s_Instance = this;
		m_sDid = SDRC_Misc.GetCurrentTickTime().ToString();

		m_bInInit = true;
		SetState(SDRC_EHeliState.FLY);
		
		//Clear any existing path points
		ResetFlight();
		SetTimeInState(0);
		//Save the original values
		StoreOriginalValues();
		
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
				//TBD: Read values from the provided array
				vector destination = SDRC_Misc.GetCoordinatesOnCircle(owner.GetOrigin(), m_fDistanceLow, SDRC_Misc.RandomInt(0, 360));
				
				InitFlight(owner, owner.GetOrigin());
				Ready(owner);
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_ChopperComp] VehicleHelicopterSimulation not found.", LogLevel.WARNING);						
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnDeactivate(IEntity owner)
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
	Once all init things are done, activate the component
	*/	
	void Ready(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.FRAME | EntityEvent.POSTFRAME);
		Activate(owner);
		
		if (!m_bAutoStart)
		{
			GetGame().GetCallqueue().CallLater(InitDone, TIME_IN_INIT * 1000, false, owner);
		}
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
	Do final clear up of things when heli is to be despawned.
	*/	
	void Clear()
	{
		SDRC_DebugHelper.DeleteDebugItems(m_sDid);
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

		//If chopper is destroyed, let Reforger handle crash etc.
		//Check if we're still working. Not needed every frame. //TBD: Could be done every x seconds - not that critical
		if ( (m_eHeliState == SDRC_EHeliState.DESTROYED) || (!IsStillWorking(owner)) )
		{
			return;
		}
		
		if (!HandleState())
		{
			return;
		}
				
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
			SDRC_Log.Add("[SDRC_ChopperComp] Fixing flight.", LogLevel.DEBUG);
			bCreateNewPath = true;
		}
		
		//No need to do anything unless we are at the end of spline.
		if ((m_iClosestIndex + m_iDestinationPointAdd + POINTS_TO_NEW_DISTANCE >= m_vSplinePoints.Count() - 1) || bCreateNewPath)
		{
//			if (m_eHeliState != SDRC_EHeliState.LAND)
			if (m_eHeliState == SDRC_EHeliState.FLY)
			{
				//Define a new destination and create a new path
				CreateNewFlight(m_vOrigin);
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
		
		//Set velocity 
		HandleRotorForce();
		
		//Set turn
		SetTurn(owner, m_fTimeTurnInterval);
		HandleLanding(timeSlice);

		SetVelocity(owner);
				
		//Search for enemies
		SearchForEnemy(owner);
		
		#ifndef SDRC_RELEASE
			DrawHelicopterVectors(owner);
		#endif		
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
		
		m_fSpeedStart = m_fSpeed;
		m_fSpeedTarget = m_fSpeed * m_fSpeedMul;
		m_fSpeedTarget = Math.Clamp(m_fSpeedTarget, m_fSpeedMin, m_fSpeedMax);
		m_fTimeSpeed = 0;	//Start to change speed
						
		if (m_fSpeedLandingMul > 0.95)
		{							
			//ROLL PITCH: Change pitch according to speed		
			m_fAnglePitch = PITCH_ANGLE_FLAT_RAD + PITCH_ANGLE_RAD * m_fSpeedMul;
//			m_fAnglePitch = Math.Clamp(m_fAnglePitch, -0.61, -0.45);
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
	private void HandleRotorForce()
	{
		if (m_vDestination == vector.Zero)
		{
			return;
		}
		
		float splineHeightFromGround = m_vDestination[1];
		float heliHeightFromGround = m_vOrigin[1];
		
		m_fRotorForceMultiplier = 1 - (25 * ( (heliHeightFromGround - splineHeightFromGround) / splineHeightFromGround ));
		
/*		if (m_vOrigin[1] - surfaceY - m_fFlyHeightLow < 0)
		{
			float mul = (m_vOrigin[1] - surfaceY) / m_fFlyHeightLow;
			//Slow down the speed to allow time for climb
			m_fSpeed = Math.Clamp(m_fSpeed * mul, m_fSpeedMin, m_fSpeedMax)
		}*/
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Handle landing
	*/
	private void HandleLanding(float timeSlice)
	{
		const float LANDING_DISTANCE = 150;
		const float TIME_TO_LAND = 12;				//(seconds)
		const float HOVER_HEIGHT = 3;
		const float LANDED_HEIGHT = 0.5;

		if (m_eHeliState == SDRC_EHeliState.LAND)
		{		
			vector lastPt = m_vSplinePoints[m_vSplinePoints.Count() - 1];			
			float distance = vector.Distance(m_vOrigin, lastPt);

			if (distance < LANDING_DISTANCE)
			{
				if (!m_Helicopter_s.HasAnyGroundContact())
				{				
					m_fTimeLanding += timeSlice;
					m_fTimeLanding = Math.Clamp(m_fTimeLanding, 0, TIME_TO_LAND);
	
					//Set the last point on ground
					lastPt[1] = SDRC_Misc.GetSurfaceYWithWater(lastPt) + LANDED_HEIGHT;
					
					float height = m_Helicopter_s.GetAltitudeAGL() - 1;					
					m_fRotorForceMultiplier = -1 * Math.Clamp(height, 0, 10);
					
					float mul = 1 - m_fTimeLanding / TIME_TO_LAND;
	
					//Helicopter to descend
			        m_Helicopter_s.RotorSetForceScaleState(0, m_fRotorForce0Orig * mul);
			        m_Helicopter_s.SetThrottle(m_fThrottleOrig * mul);
					m_fSpeedMin = distance/height;
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
	}

	//------------------------------------------------------------------------------------------------	
	/*!	
	Handle state (machine)
	
	\return true Handle flight model calculations
	\return false Skip flight model calculations
	*/
	bool HandleState()
	{	
		//If on ground, let's see what to do next
		if (m_eHeliState == SDRC_EHeliState.ON_GROUND) 
		{
			SetNextState();	
		}
		
		//Wait for the state timer to end and go to next state
		if ( (m_eHeliState != SDRC_EHeliState.FLY) && (m_fTimeInState < 0) && m_bTimeInStateEnabled) 
		{
			SetNextState();
		}
		
		return true;
	}	

	//------------------------------------------------------------------------------------------------
	/*!	
	Sets the next destination for an action. 
	- FLY will start to fly
	- Others will have some action bound to them.
	*/
	void SetNextState()
	{
		SDRC_EFlyWayPointType nextType = SDRC_EFlyWayPointType.UNDEFINED;
		
		//By default we remove the destination
		bool removeDestination = true;
		
		//If not destinations defined, start to fly		
		if (m_vFlyDestinations.IsEmpty())
		{
			nextType = SDRC_EFlyWayPointType.FLY
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
				removeDestination = false;
				break;
			}			
			case SDRC_EFlyWayPointType.FLY_AWAY:
			case SDRC_EFlyWayPointType.FLY_AWAY_IMMEDIATELY:
			{
				SetState(SDRC_EHeliState.FLY);
				ResetOriginalValues();
				//Fly for a while and then go to END state
				SetTimeInState(120);
				AddDestination(vector.Zero, SDRC_EFlyWayPointType.END); 
				break;
			}		
			case SDRC_EFlyWayPointType.END:
			{
				SetState(SDRC_EHeliState.DESTROYED);
				break;
			}
			case SDRC_EFlyWayPointType.WAIT:
			{
				//Just wait
				SetState(SDRC_EHeliState.WAIT);
				SetTimeInState(2); 
				break;
			}
			case SDRC_EFlyWayPointType.RAISE:
			{
				SetState(SDRC_EHeliState.RAISE);
				SetTimeInState(3);
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
		
		//Remove the destination is it was handled.		
		if (removeDestination)
		{
			m_vFlyDestinations.RemoveOrdered(0);			
		}
	}		

	//------------------------------------------------------------------------------------------------	
	void SetTimeInState(int seconds)	
	{
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
	#ifdef HELI_TESTING		
		#ifdef HELI_TESTING_LANDING
			//Start near airfield
			origin = "800 50 2800";
			destination = "990 0 2560";		
//			m_eHeliState == SDRC_EHeliState.LAND;
		#endif
		
		#ifdef HELI_TESTING_AIRPORT
			//Start near airfield
			origin = "800 50 2800";
			destination = "1060 0 2450";		
		#endif
		
		#ifdef HELI_TESTING_HILL
			//Fly over hill	
			origin = "2000 0 1800";
			destination = "3000 0 2800";
		#endif
	#endif
		
		int worldSize = SDRC_Misc.GetWorldSize();
		
		//Set height for start and destination points		
		//With autostart, use the origin of the chopper spawn
		if (m_bAutoStart)	
		{
			//TBD: Destination handling for helis
		}
		else		
		{
			//Without autostart, randomize start height
			origin[1] = SDRC_Misc.RandomFloat(m_fFlyHeightLow, m_fFlyHeightHigh) + SDRC_Misc.GetSurfaceYWithWater(origin);
		}

		//Store the original firstDestination
		m_vOriginalDestination = origin;
		
		//Create initial flypath
		//Add points to path. Normally we would use AddDestination, but for the initial flight, we need points in m_vFlightPoints.
		AddFlyPathPoint(origin);
		
		//Cases:
		// - Only origin defined -> error
		// - Origin with multiple destinations -> Ok, normal case
		
		if (m_vFlyDestinations.IsEmpty())
		{		
			SDRC_Log.Add("[SDRC_ChopperComp:InitFlight] No destination defined.", LogLevel.ERROR);
			return;			
		}
		
		//If only one destination defined, add an additional point
		if (m_vFlyDestinations.Count() == 1)
		{
			AddFlyPathPoint(vector.Lerp(origin, m_vFlyDestinations[0].pt, 0.5));
			SDRC_Log.Add("[SDRC_ChopperComp:InitFlight] Adding a mid point.", LogLevel.DEBUG);
		}
		//AddDestination(firstDestination, fpType);

		if (!m_bAutoStart)	//With autostart, use the origin of the chopper spawn
		{
			SetFlightPointHeight();
		}

		//Create points for spline
		CreateFlightPoints();
		
		array<vector> flyPathPoints = {};
		GivePoints(flyPathPoints, m_vFlightPoints);
		SDRC_Spline3D.GenerateSplinePoints(flyPathPoints, m_vSplinePoints, -1);
		
		//Set final values
		m_iClosestIndex = 2;
		m_iOldClosestIndex = m_iClosestIndex;
		
		//Check that points are above ground
		SetSplinePointsAboveGround();
		
		//Smooth the Up curve
		SDRC_Spline3D.SmoothSplineUpOnly(m_vSplinePoints);
		
		m_fSpeed = 0.1;
		m_fSpeedTarget = m_fSpeed;
		
		//Set chopper initial position
		owner.SetOrigin(m_vSplinePoints[0]);
		vector angles = vector.Direction(owner.GetOrigin(), m_vSplinePoints[m_iClosestIndex]);
		angles.Normalize();
		angles = angles.VectorToAngles();
		owner.SetYawPitchRoll(angles);

		SetVelocity(owner);
		
		#ifndef SDRC_RELEASE
			SDRC_DebugHelper.DrawPointList(m_vSplinePoints, m_sDid);		
			SDRC_DebugHelper.DrawPointList(flyPathPoints, m_sDid, ARGB(10, 64, 64, 192));		
		#endif
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Create the runtime flight path with waypoint definition
	
	\param origin The middle point of the path. Typically the helicopter position
	\param force - False: Create normal fly path. - True: Stop everything and fly immediately to the last position in m_vFlightPoints.
	*/
	void CreateNewFlight(vector origin)
	{
		//Clear any existing path points
		ResetFlight();

//		SDRC_EHeliWaypointGenerationType wpType = m_fWpType;
		
		//If we are the final destination, we stop creating waypoints and stop flying.
/*		if (m_eHeliState == SDRC_EHeliState.END)
		{
			SetState(SDRC_EHeliState.DESTROYED);
			return;
		}
		
		//If final destination was requested, create the last waypoints.
		if (m_eHeliState == SDRC_EHeliState.FLY_AWAY)
		{
			SetState(SDRC_EHeliState.END);
			wpType = SDRC_EHeliWaypointGenerationType.FLY_AWAY;
		} */
		
		AddFlyPathPoint(m_vDestinationFuture);
		GenerateWayPoint(origin, m_fWpType);
	
		//Create points for spline		
		CreateFlightPoints();
		
		SetFlightPointHeight();
		array<vector> flyPathPoints = {};
		GivePoints(flyPathPoints, m_vFlightPoints);
		SDRC_Spline3D.GenerateSplinePoints(flyPathPoints, m_vSplinePoints, -1);
		
		//Search the closest indes from the spline start
		m_iClosestIndex = 0;
		float distance = SDRC_Spline3D.GetDistanceFromSpline(m_vSplinePoints, origin, m_iClosestIndex, false);	//NOTE: This will set m_iClosestIndex
		m_iOldClosestIndex = m_iClosestIndex;
		
		//Check that points are above ground
		//SDRC_DebugHelper.DrawPointList(m_vSplinePoints, m_sDid);
		SetSplinePointsAboveGround();
		
		//Smooth the Up curve
		SDRC_Spline3D.SmoothSplineUpOnly(m_vSplinePoints);
		
		if (m_vSplinePoints.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_ChopperComp:CreateFlightPath] No points!", LogLevel.ERROR);
		}
		
		#ifndef SDRC_RELEASE
			SDRC_DebugHelper.DeleteDebugItems(m_sDid);
			SDRC_DebugHelper.DrawPointList(m_vSplinePoints, m_sDid);		
			SDRC_DebugHelper.DrawPointList(flyPathPoints, m_sDid, ARGB(10, 64, 64, 192));
		#endif
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Clear the fly path as a preparation for a completely new path
	*/
	void ResetFlight()
	{
		m_vFlightPoints.Clear();
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
		//Make sure the points are at minimum m_fFlyHeightLow from the ground.
		foreach (int i, vector pt : m_vSplinePoints)
		{
			float y = SDRC_Misc.GetSurfaceYWithWater(pt);

			if (pt[1] < (y + m_fFlyHeightLow))
			{
//				pt[1] = y + ( (m_fFlyHeightHigh + m_fFlyHeightLow) / 2 ) ;	//Make chopper fly higher for a moment
				pt[1] = y + m_fFlyHeightLow + 5;	//Make chopper fly higher for a moment
				m_vSplinePoints[i] = pt;
			}
		}
		
		//If we're landing set some of the last points close to the ground
		if (m_eHeliState == SDRC_EHeliState.LAND)
		{
			const int POINTS_TO_GROUND = 6;
			int lastIdx = m_vSplinePoints.Count() - 1;
			
			//Create a Y spline to replace the given points to smooth the curve for landing
			if (m_vSplinePoints.Count() - 1 > POINTS_TO_GROUND)
			{
				//Take the high point and low point. 
				float p0 = m_vSplinePoints[lastIdx - POINTS_TO_GROUND][1];
				float p2 = SDRC_Misc.GetSurfaceYWithWater(m_vSplinePoints[lastIdx]);
				//Add a point that is slightly above the low point
				float p1 = p2 + p0 * 0.1;
				
				array<float> smoothedPoints = SDRC_Spline3D.CreateYSpline(p0, p1, p2, POINTS_TO_GROUND);
				
				for (int i = 0; i < POINTS_TO_GROUND; i++)
				{
					float newY = smoothedPoints[i];					
					vector pt = m_vSplinePoints[lastIdx - POINTS_TO_GROUND + i];
					pt[1] = newY;
					m_vSplinePoints[lastIdx - POINTS_TO_GROUND + i] = pt;
				}
			}
			else
			{
				vector pt = m_vSplinePoints[lastIdx];
				float y = SDRC_Misc.GetSurfaceYWithWater(pt) + 5;	//Don't put it exactly to zero
				pt[1] = y;
				m_vSplinePoints[lastIdx] = pt;				
			}	
		}		
	}
	
	//------------------------------------------------------------------------------------------------	
	// Waypoint functions
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Functionality to generate future destination(s) for the chopper
	*/
	private void GenerateWayPoint(vector origin, SDRC_EWaypointGenerationType wpGenType)
	{		
		vector pos = "0 0 0";

		if (m_vFlyDestinations.IsEmpty())
		{
			//Random flying for a helicopter
			if ( (wpGenType == SDRC_EHeliWaypointGenerationType.RANDOM) || 
			     (wpGenType == SDRC_EHeliWaypointGenerationType.SEARCH)
			   )
			{	
				pos = GetRandomPosition(origin);
				AddFlyPathPoint(pos);
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
					
					SDRC_DebugHelper.AddDebugPos(pos, ARGB(255, 255, 00, 00), 2.0, m_sDid, 50 + i * 20);
					
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
	Creates a random destination point
	*/	
	private vector GetRandomPosition(vector pos)
	{
		float distance = SDRC_Misc.RandomFloat(m_fDistanceLow, m_fDistanceHigh);
		vector newpos;
		
		//If distance is under 1.0, use world percentage
		if (distance < 1.0)
		{
			newpos = SDRC_Misc.GetRandomWorldPosPercentage(distance);
		}
		else
		{
			newpos = SDRC_Misc.GetCoordinatesOnCircle(pos, distance, SDRC_Misc.RandomInt(0, 360));
		}
				
		return newpos;
	}
		
	//------------------------------------------------------------------------------------------------	
	/*!	
	Create fly points
	Takes the points from m_vFlyDestinations and generates points to be used for spline creation
	*/	
	private void CreateFlightPoints()
	{
		#ifdef HELI_TESTING			
			#ifdef HELI_TESTING_AIRPORT	
				//Replace the provided destination for testing purposes
			//	m_vFlightPoints.RemoveOrdered(m_vFlightPoints.Count() - 1);
//				m_vFlyDestinations.RemoveOrdered(m_vFlyDestinations.Count() - 1);
				m_vFlyDestinations.Clear();
				//Testing near air field
				vector dest;	
			//	dest = "1600 0 2600";	//113 degrees 
				dest = "1300 0 2900";	//64 degrees 
			//	dest = "1000 0 2800";	//34 degrees 
			//	dest = "900 0 2800";		//12 degrees 
			//	dest = "500 0 2000";		//-94 degrees 
			//	dest = "700 0 2700";		//-22 degrees 
				AddDestination(dest);
				dest = "1800 0 2800";
				AddDestination(dest);
			#endif
		#endif

		//If only two points, add a mid point
		if (m_vFlightPoints.Count() <= 2)
		{
			vector p0 = m_vFlightPoints[0].pt;
			vector p1;
			if (m_vFlightPoints.Count() == 1)
			{
				p1 = m_vFlyDestinations[0].pt;
			}
			else
			{	
				p1 = m_vFlightPoints[1].pt;
			}
			vector mid = vector.Lerp(p0, p1, 0.5);
			AddFlyPathPoint(mid, index: 1);
		}
				
		//Add destinations .. if any
		int lastIdx = -1;
		
		foreach (int idx, SDRC_FlyPathPoint flyDestination : m_vFlyDestinations)
		{		
			float distance = vector.DistanceXZ(m_vFlightPoints[m_vFlightPoints.Count() - 1].pt, flyDestination.pt);
			
			SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Distance: " + distance, LogLevel.SPAM);
	
			//TBD: If distance between two points is too short, should we skip?
			
			vector dir0;	//Previous flight direction
			vector dir1;	//New flight direction
			vector p0 = m_vFlightPoints[m_vFlightPoints.Count() - 2].pt;
			vector p1 = m_vFlightPoints[m_vFlightPoints.Count() - 1].pt;
			vector p2 = flyDestination.pt;
			float heliAngle = SDRC_Math.GetAngleBetweenThreePointsXZ(p0, p1, p2, dir0, dir1);

			SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Angle: " + heliAngle, LogLevel.SPAM);
							
		#ifdef HELI_TESTING				
			//For some reason two points on the path are the same. Should not happen. Maybe an issue in spline generation?
			if (Math.AbsFloat(heliAngle) == 90)
			{
				SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Why 90?", LogLevel.DEBUG);
			}
		#endif		
			
			//Is the angle too steep? Re-route.
			if ( (Math.AbsFloat(heliAngle) < WP_ANGLE) && (distance > 200) )
			{				
				SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Heli direction angle is very STEEP: " + heliAngle, LogLevel.SPAM);
				
				//Remove the last point
				vector point = m_vFlightPoints[idx].pt;
//				m_vFlightPoints.RemoveOrdered(m_vFlightPoints.Count() - 1);
				
				//We need to take a detour. Add an additional points outside of the line to make the route rounder
				dir0 = SDRC_Math.RotateAroundAxis(dir0, vector.Up, heliAngle);
				dir0.Normalize();
				
				float lerpRnd = SDRC_Misc.RandomFloat(0.15, 0.33);
				vector mid = vector.Lerp(point, flyDestination.pt, lerpRnd);
				vector vec = mid + dir0 * (distance / 6);
				AddFlyPathPoint(vec);
				SDRC_DebugHelper.AddDebugPos(vec, ARGB(32, 128, 128, 64), 1.0, m_sDid, vec[1]);
				
				//If the angle is very steep, add a second point	
				if (Math.AbsFloat(heliAngle) < (WP_ANGLE / 2))
				{				
					lerpRnd = SDRC_Misc.RandomFloat(0.4, 0.6);			
					mid = vector.Lerp(point, flyDestination.pt, lerpRnd);
					vec = mid + dir0 * (distance / 4);
					AddFlyPathPoint(vec);
					SDRC_DebugHelper.AddDebugPos(vec, ARGB(32, 128, 128, 64), 1.0, m_sDid, vec[1]);
				}
			}
			
			AddFlyPathPoint(flyDestination.pt, flyDestination.type);
			lastIdx = idx;
			SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Heli direction angle: " + heliAngle + " - Distance: " + distance, LogLevel.SPAM);
			
			if (flyDestination.type != SDRC_EFlyWayPointType.FLY)
			{
				break;
			}
		}

		//There should always be at least 3 points
		if (m_vFlightPoints.Count() < 3)
		{
			SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] This should never happen! ", LogLevel.WARNING);
/*			vector p0 = m_vFlightPoints[0].pt;
			vector p1 = m_vFlightPoints[1].pt;
			vector mid = vector.Lerp(p0, p1, 0.5);
			AddFlyPathPoint(mid, index: 1);*/
		}
				
		SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Created " + m_vFlightPoints.Count() + " points.", LogLevel.SPAM);

		//Clear the destinations 
		for (int i = 0; i <= lastIdx; i++)
		{
			m_vFlyDestinations.RemoveOrdered(0);
		}		
		SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Destinations left: " + m_vFlyDestinations.Count(), LogLevel.DEBUG);
		//m_vFlyDestinations.Clear();
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
		
		if (index == -1)
		{
			m_vFlightPoints.Insert(new SDRC_FlyPathPoint(destination, type));
		}
		else
		{
			m_vFlightPoints.InsertAt(new SDRC_FlyPathPoint(destination, type), index);
		}
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
	// Destination settings
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Clear the destination as a preparation for a completely new path
	*/
	void ResetDestinations()
	{
		m_vFlyDestinations.Clear();
	}	
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Add a destination for future.
	\param destination Next position to fly to. Multiple destinations can be defined by calling multiple times.
	\param type How to fly .. kinda. If set as FINAL, once reaching the destination, helicopter will stop flying. 
	*/
	void AddDestination(vector destination, SDRC_EFlyWayPointType type = SDRC_EFlyWayPointType.FLY)
	{
		//Fly away after all destinations have been handled
		if (type == SDRC_EFlyWayPointType.FLY_AWAY)
		{
			SetState(SDRC_EHeliState.FLY_AWAY);
			m_fWpType = SDRC_EHeliWaypointGenerationType.FLY_AWAY;
		}

		//Fly away immediately
		if (type == SDRC_EFlyWayPointType.FLY_AWAY_IMMEDIATELY)
		{
			ResetDestinations();
			SetState(SDRC_EHeliState.FLY_AWAY);
			m_fWpType = SDRC_EHeliWaypointGenerationType.FLY_AWAY;
		}
		
		//Land the chopper
		if (type == SDRC_EFlyWayPointType.LAND)
		{
			SetState(SDRC_EHeliState.LAND);
		}		
				
		m_vFlyDestinations.Insert(new SDRC_FlyPathPoint(destination, type));		
	}	

	//------------------------------------------------------------------------------------------------
	/*!	
	Returns the last destination.
	*/
	vector GetDestination()
	{
		return m_vSplinePoints[m_vSplinePoints.Count() - 1];
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Returns the destination where the helicopter is currently going. 
	*/
	vector GetCurrentDestination()
	{
		return m_vDestination;
	}	
	
	//------------------------------------------------------------------------------------------------	
	// Damage settings
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	void SetDamage(IEntity owner)
	{
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(owner.FindComponent(DamageManagerComponent));
		float health = SDRC_Misc.RandomFloat(0.0001, 0.0005);
		if (damageManager)
		{
			damageManager.SetHealthScaled(health);
		}
		
		SDRC_Log.Add("[SDRC_ChopperComp:SetDamage] Setting health: " + health, LogLevel.DEBUG);
	}

	//------------------------------------------------------------------------------------------------
	bool IsStillWorking(IEntity owner)
	{
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
		float health = damageManager.GetHealth();
		
		//If the chopper is damaged, init is considered done.
		if (health < HEALTH_LIMIT)
		{
			InitDone(owner);
		}

		//If still in init, don't care if no pilots etc yet set.		
		if (m_bInInit)
		{
			return true;
		}

		if ( (SDRC_VehicleHelper.IsWorking(owner)) && (SDRC_VehicleHelper.PilotCountAlive(owner) > 0) )
		{
			return true;
		}
		
		//Nope, we're done
		SetState(SDRC_EHeliState.DESTROYED);
		SDRC_DebugHelper.DeleteDebugItems(m_sDid);
		
		//Set damage so it should be destroyed on crash
		float damage = SDRC_Misc.RandomFloat(0, 0.05);
		if (damageManager)
		{
			damageManager.SetHealthScaled(damage);		
		}
		
		//Make the chopper fly unsteadily
		float force = SDRC_Misc.RandomFloat(0, 0.1);
        m_Helicopter_s.RotorSetForceScaleState(0, force);
		force = SDRC_Misc.RandomFloat(0.1, 2.5);
        m_Helicopter_s.RotorSetForceScaleState(1, force);
		force = SDRC_Misc.RandomFloat(0.0, 0.1);
		m_Helicopter_s.SetThrottle(force);
		
		return false;
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
		
		//Enemy stuff		
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(owner.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		array<IEntity> occupants = {};
		scr_compartmentManager.GetOccupants(occupants);

		foreach(IEntity occupant : occupants)
		{
			SCR_AICombatComponent aicc = SCR_AICombatComponent.Cast(occupant.FindComponent(SCR_AICombatComponent));
			if (aicc)
			{
				BaseTarget bt = aicc.GetCurrentTarget();
				if (bt)
				{
					IEntity target = bt.GetTargetEntity();
					if (EntityUtils.IsPlayer(target))
					{
						found = true;
						m_vEnemyPosition = target.GetOrigin();
						m_iEnemyFoundTimeOut = SDRC_Misc.GetCurrentTickTime() + ENEMY_FOUND_TIMEOUT;
						SDRC_Log.Add("[SDRC_ChopperComp:SearchForEnemy] Enemy found at " + m_vEnemyPosition, LogLevel.DEBUG);
						break;
					}
				}
			}
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
			
	//------------------------------------------------------------------------------------------------	
	/*!
	Draw debugging details of helicopter
	*/	
	void DrawHelicopterVectors(IEntity owner)
	{
	#ifndef SDRC_RELEASE
		
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
							   	"Speed:" + Math.Round(10*m_fSpeed)/10 + " - " +
							   	"Start/Target:" + Math.Round(10*m_fSpeedStart)/10 + "/" + Math.Round(10*m_fSpeedTarget)/10 + "\n" +
	//						   	"Avg time:" + m_fTimeBetweenPtsAvg + "\n" +
							   	"SpeedMul:" + Math.Round(10*m_fSpeedMul)/10 + "\n" + 
							   	"SpeedMin:" + Math.Round(10*m_fSpeedMin)/10 + "\n" + 
								"";		
			debugText = debugText + 
							   	"Height: " + Math.Round(10*(origin[1] - SDRC_Misc.GetSurfaceYWithWater(origin)))/10 + " - " + 
							   	"Height: " + m_Helicopter_s.GetAltitudeAGL() + " - " + 			
							   	"RotorForceMul:" + m_fRotorForceMultiplier + "\n" +
	//						   	"SplinePoints:" + m_vSplinePoints.Count() + "\n" +
	//						   	"TurnInternal:" + m_fTimeTurnInterval + "\n" +
	//							"Angle: " + m_fDbgAngle * Math.RAD2DEG + "\n" +
//								"AnglePitch: " + m_fAnglePitch * Math.RAD2DEG + "\n" +
								"AnglePitch: " + m_fAnglePitch + "\n" +
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
		DrawLine(origin, m_vSplinePoints[idx], Color.GRAY);		
		
		//Chopper destination direction vector
		vector vFwd = vector.Direction(origin, m_vDestination);
//		vFwd.Normalize();
//		DrawLine(origin, origin + (vFwd * 20), Color.WHITE);
//		DrawLine(origin, origin + vFwd, Color.WHITE);
		DrawLine(origin, m_vDestination, Color.WHITE);

		//Chopper future destination direction vector
		vFwd = vector.Direction(origin, m_vDestinationFuture);
//		vFwd.Normalize();
//		DrawLine(origin, origin + (vFwd * 50), Color.BLACK);		
		DrawLine(origin, origin + vFwd, Color.BLACK);		

		//Draw vectors
		vector vDir2 = owner.GetTransformAxis(2);	//Forward
//		DrawLine(origin, origin + (vDir2 * 30), Color.CYAN);		

		vector vDir0 = owner.GetTransformAxis(0);	//Side
//		DrawLine(origin, origin + (vDir0 * 10), Color.DARK_CYAN);

		vector vDir1 = owner.GetTransformAxis(1);	//Up
		DrawLine(origin, origin + (vDir1 * 15), Color.MAGENTA);		
		
		vector vUp = vector.Up;						//World Up
		DrawLine(origin, origin + (vUp * 10), Color.MAGENTA);		
		
		//Roll vector
		vector vRoll = m_vRollTarget * Math.DEG2RAD;
		vRoll.Normalize();
//		DrawLine(origin, origin + (vRoll * 15), Color.BLUE);		

		//RollPitch vector
		vRoll = m_vRadRollPitch * Math.DEG2RAD;
		vRoll.Normalize();
//		DrawLine(origin, origin + (vRoll * 15), Color.BLUE);		
		
/*		vector vVec = m_vRadRollVel;
		vVec[1] = -vVec[2];
		vVec[2] = vVec[0];
		vVec[0] = 0;
		vVec.Normalize();
		DrawLine(origin, origin + (vVec * 45), Color.WHITE);

		vVec = m_vRadRollBack;
		vVec.Normalize();
		DrawLine(origin, origin + (vVec * 35), Color.WHITE);*/
								
		//Draw velocity vector
		vector vVel = owner.GetPhysics().GetVelocity();
		vVel.Normalize();
//		float currentSpeed = vVel.Length();
		DrawLine(origin, origin + (vVel * m_fSpeed), Color.GRAY_75);			
		
		//Enemy stuff
		
		//Draw current enemy sighting		
		if (m_vEnemyPosition != "0 0 0")
		{
			DrawLine(origin, m_vEnemyPosition, Color.PINK);
		}
		
		//Draw eyesight		
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(owner.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		array<IEntity> occupants = {};
		scr_compartmentManager.GetOccupants(occupants);

		foreach(IEntity occupant : occupants)
		{
			SCR_AICombatComponent aicc = SCR_AICombatComponent.Cast(occupant.FindComponent(SCR_AICombatComponent));
			if (aicc)
			{
				BaseTarget bt = aicc.GetCurrentTarget();
				if (bt)
				{
					IEntity target = bt.GetTargetEntity();
					if (EntityUtils.IsPlayer(target))
					{
						DrawLine(occupant.GetOrigin(), target.GetOrigin(), Color.RED);
					}
				}
			}
		}
	#endif			
	}
	
	//------------------------------------------------------------------------------------------------	
	void DrawLine(vector p0, vector p1, int color = Color.RED)
	{
		int shapeFlags = ShapeFlags.ONCE;
		vector p[2];
		p[0] = p0;
		p[1] = p1;		
		Shape.CreateLines(color, shapeFlags, p, 2);		
	}	
}