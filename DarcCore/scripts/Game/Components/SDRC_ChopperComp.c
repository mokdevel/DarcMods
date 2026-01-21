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
//	GOTO,		//Fly to a given destination
};

//------------------------------------------------------------------------------------------------
enum SDRC_EFlyPathPointType
{
	FLY,
	FINAL,
	LAND,
}

class SDRC_FlyPathPoint
{
	vector pt;
	SDRC_EFlyPathPointType type;
	
	void SDRC_FlyPathPoint(vector pt_, SDRC_EFlyPathPointType type_ = SDRC_EFlyPathPointType.FLY)
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
	[Attribute(defvalue: "1.4", desc: "Throttle aka acceleration", params: "0.1 3.0 0.1")]	
	float m_fThrottle;
	float m_fThrottleOrig;
	[Attribute(defvalue: "3.0", desc: "Main rotor force", params: "0.1 10.0 0.1")]	
	float m_fRotorForce0;
	float m_fRotorForce0Orig;
	[Attribute(defvalue: "1.0", desc: "Rear rotor force", params: "0.1 2.0 0.1")]	
	float m_fRotorForce1;
	[Attribute(defvalue: "0 0 0", desc: "First destination to fly to.")]	
	vector m_fFirstDestination;
	
	[Attribute(defvalue: "10.0", desc: "Minimum speed", params: "1.0 100.0 0.1")]	
	float m_fSpeedMin;				//Minimum speed
	[Attribute(defvalue: "30.0", desc: "Maximum speed", params: "1.0 100.0 0.1")]	
	float m_fSpeedMax;				//Maximum speed
	[Attribute(defvalue: "40.0", desc: "Minimum fly height (from ground level)", params: "5 100.0 1")]	
	float m_fFlyHeightLow;			//Flight height low
	[Attribute(defvalue: "80.0", desc: "Maximum fly height (from ground level)", params: "5 100.0 1")]	
	float m_fFlyHeightHigh;			//Flight height high
	[Attribute(defvalue: "0.1", desc: "Minimum distance for waypoint", params: "0.1 1000.0 0.1")]	
	float m_fDistanceLow;			//Distance for waypoint min
	[Attribute(defvalue: "0.4", desc: "Maximum distance for waypoint", params: "0.1 1000.0 0.1")]	
	float m_fDistanceHigh;			//..max
	SDRC_EHeliWaypointGenerationType m_fWpType; 	

	//Original destination	
	private vector m_vOriginalDestination;
		
	//Speed management
	private const float SPEED_INTERVAL = 1.0;			//(seconds) Interval to modify speed of the helicopter
	private const float SPEED_GAIN = 1.0;
	private const int SPEED_TURN_DIV = 42;				//The divider that affects how much speed is decreased on sharp turns. The higher the value, the less brake.
	private float m_fTimeSpeed = 0;

	private float m_fTimeBetweenPts = 1;
	private float m_fTimeBetweenPtsAvg = 1;

	private float m_fTimeBetweenFixes = 30;
			
	//Turn
	private const int TIME_TURN_INTERVAL_BASE = 40;		//Time to divide with speed to define the final turn time. Smaller value makes heli turn faster.
	
	//Pitch
	private const float PITCH_ANGLE_RAD = 80 * Math.DEG2RAD;	//The pitch angle to use when calculating for speed effect. The faster the heli goes, the steeper the nose should be down.
	
	//Roll 
	private const float ROLL_ANGLE_MUL = 2.4;			//Multiplier for roll angle along the spline
	
	//Flight path
	private const int POINTS_TO_NEW_DISTANCE = 2;		//How many spline points in to the future flight path is checked before adding new flight points.
	private const int POINTS_TO_SPLINE_START = 4;		//Points to go back from m_iClosestIndex when creating a new flight path 
	private const int DESTINATION_POINT_DIV = 12;		//How many points ahead to look for the destination. This is the divider for speed.
//	private const int TIME_FORCE_MOVE_POINT = 20;		//(seconds) Time to wait before force moving a point. This is to fix situations where the chopper gets stuck on a point.
	private const int TIME_FIX_FLIGHT = 10;				//(seconds) Time to wait between flight fixes when chopper is pointing to the sky.
	private const float TIME_IN_INIT = 10;				//(seconds) Time to be in init state. During this time, we don't check for damage or similar things.
	
	//Rotor force multipliers
	private const float ROTOR_FORCE_MUL = 1.0;			//Rotor force multiplier. Bigger value makes the heli react faster to up/down movement
	private const float ROTOR_FORCE_UP_MUL = 1.2;		//Rotor force multiplier in velocity counting
	private const float ROTOR_FORCE_MUL_PANIC = 5.0;	//Rotor force multiplier used when avoiding ground. 
	
	//Waypoint values
	private const float WP_ANGLE = 60;					//Waypoint angle that is considered steep. This is the angle between current direction and new direction.
														//If chopper destination makes a too steep turn, we will add a few additional points.
	
	//Runtime parameters
	private bool m_bInInit = true;						//While in init, consider the chopper to be flying.
	private bool m_bDestroyed = false;					//If true, helicopter is considered destroyed and will not generate anymore fly points. AR physics handle the rest.
	private bool m_bFinalDestination = false;			//If true, once reaching the last point, m_bDestroyed is set true. Heli will stop flying.
	private bool m_bFinalDestinationReached = false;
	private bool m_bDoLanding = false;					//If true, we're doing a landing at the destination
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
	private float m_fDbgAnglePitch;
	private float m_fDbgAngleRoll;
	private float m_fDbgAngleRollBack;			//Remove from final
	
	//Runtime parameters
	private int m_iClosestIndex;				//Closest point on spline to heli
	private int m_iOldClosestIndex;
	private int m_iNextIndex;					//Next index to our m_iClosestIndex - depends on speed
	private int m_iFutureIndex;					//Where we are heading in the long run
	private vector m_vDestination;				//Lerped m_vDestination that keeps on moving along the spline
	private vector m_vDestinationFuture;		//Destination where we eventually plan to fly

	//Flight path
	ref array<ref SDRC_FlyPathPoint> m_vFlyPathPoints = {};
	ref array<ref SDRC_FlyPathPoint> m_vFlyDestinations = {};	//Requested destinations
		
	//Id for debug items
	private string m_sDid;
	
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

		m_Helicopter_s = VehicleHelicopterSimulation.Cast(GetOwner().GetRootParent().FindComponent(VehicleHelicopterSimulation));
		m_iEnemyFoundTimeOut = SDRC_Misc.GetCurrentTickTime() + ENEMY_FOUND_TIMEOUT;
		
		if (m_Helicopter_s)
		{
	        m_Helicopter_s.EngineStart();
	        m_Helicopter_s.SetThrottle(m_fThrottle);
	        m_Helicopter_s.RotorSetForceScaleState(0, m_fRotorForce0);
	        m_Helicopter_s.RotorSetForceScaleState(1, m_fRotorForce1);			
			SetHeli(m_fSpeedMin, m_fSpeedMax, m_fFlyHeightLow, m_fFlyHeightHigh, SDRC_EHeliWaypointGenerationType.RANDOM, m_fDistanceLow, m_fDistanceHigh);						
			
			//Save the original values
			m_fThrottleOrig = m_fThrottle;
			m_fRotorForce0Orig = m_fRotorForce0;
			m_fSpeedLandingMul = 1.0;
			
			if (m_bAutoStart)
			{
				vector destination = m_fFirstDestination;
				if (m_fFirstDestination == "0 0 0")
				{
					destination = SDRC_Misc.GetCoordinatesOnCircle(owner.GetOrigin(), m_fDistanceLow, SDRC_Misc.RandomInt(0, 360));
				}
				
				InitFlyPath(owner, owner.GetOrigin(), destination);
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
		
		#ifndef SDRC_RELEASE		
			SDRC_DebugHelper.DrawPointList(m_vSplinePoints, m_sDid);
		
			array<vector> flyPathPoints = {};
			GivePoints(flyPathPoints, m_vFlyPathPoints);
			SDRC_DebugHelper.DrawPointList(flyPathPoints, m_sDid, ARGB(10, 64, 64, 192));
		#endif
		
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

		//If chopper is destroyed, let Reforger handle crash etc.
		//Check if we're still working. Not needed every frame. //TBD: Could be done every x seconds - not that critical
		if ( (m_bDestroyed) || (!IsStillWorking(owner)) )
		{
			return;
		}
		
		m_fTimeSpeed += timeSlice;
		m_fTimeBetweenPts += timeSlice;
		m_fTimeBetweenFixes -= timeSlice;

		//Adjust time depending on the speed.
		m_fTimeTurnInterval = TIME_TURN_INTERVAL_BASE / m_fSpeed;
		m_fTimeTurnInterval = Math.Clamp(m_fTimeTurnInterval, 0.6, 3);
				
		//If we've been stuck on a point, force new flight path. 
		//Sometimes the heli direction and path align so that the closest index does not update.
		//In these case the helicopter up vector and world up vector is big.
		bool bCreateNewPath = false;
		float heliUpAngleToWorld = SDRC_Math.GetAngleBetweenVectors(owner.GetTransformAxis(1), vector.Up);	
		if ( (heliUpAngleToWorld > 1.2) && (m_fTimeBetweenFixes < 0) )
		{
			SDRC_Log.Add("[SDRC_ChopperComp] Fixing flight.", LogLevel.DEBUG);						
			m_fTimeBetweenFixes = TIME_FIX_FLIGHT;	//Time between tries to fix the flight
			bCreateNewPath = true;
		}
		
		//No need to do anything unless we are at the end of spline.
		if ((m_iClosestIndex + m_iDestinationPointAdd + POINTS_TO_NEW_DISTANCE >= m_vSplinePoints.Count() - 1) || bCreateNewPath)
		{
			if (!m_bDoLanding)
			{
				//Define a new destination and create a new path
				CreateFlyPath(m_vOrigin);
			}
		}
		
		//Count destintation addition along the spline which is dependent on the speed.
		m_iDestinationPointAdd = m_fSpeed / DESTINATION_POINT_DIV;
		m_iDestinationPointAdd = Math.ClampInt(m_iDestinationPointAdd, 1, 3);
		
		//Find where we're going
		float distance = SDRC_Spline3D.GetDistanceFromSpline(m_vSplinePoints, m_vOrigin, m_iClosestIndex, false);	//NOTE: This will set m_iNewClosestIndex

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
		HandleLanding();

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
		//Consider height
//		float angle = Math.AbsFloat(SDRC_Math.GetAngleBetweenVectors(m_vHeliForward, m_vHeliDirectionFuture));
		//Ignore height component
		float angle = Math.AbsFloat(SDRC_Math.GetAngleBetweenVectorsXZ(m_vHeliForward, m_vHeliDirectionFuture));
		m_fDbgAngle = angle;
		//Count the angle of the turn. The steeper the turn, the slower heli should be moving.
		m_fSpeedMul = Math.Clamp((angle * Math.RAD2DEG), 1, 90);
		m_fSpeedMul = m_fThrottle * (SPEED_GAIN - (m_fSpeedMul / SPEED_TURN_DIV));
//		m_fSpeedMul = m_fPower * (SPEED_GAIN - (m_fSpeedMul / 25));	//45
		
		//In case we're landing, we need to modify the speed
		m_fSpeedMul = m_fSpeedMul * m_fSpeedLandingMul;
		
		m_fSpeedStart = m_fSpeed;
		m_fSpeedTarget = m_fSpeed * m_fSpeedMul;
		m_fSpeedTarget = Math.Clamp(m_fSpeedTarget, m_fSpeedMin, m_fSpeedMax);
		m_fTimeSpeed = 0;	//Start to change speed

		//ROLL PITCH: Change pitch according to speed		
		m_fDbgAnglePitch = (m_fSpeedMul - 4) * PITCH_ANGLE_RAD;			//No idea why 4 is a good value. :-)
		m_fDbgAnglePitch = Math.Clamp(m_fDbgAnglePitch, -0.7, 0.7);		
		m_vRadRollPitch = SDRC_Math.RotateAroundAxis(m_vHeliForward, heliPitch, m_fDbgAnglePitch);
		m_vRadRollPitch = SDRC_Math.ComputeAngularVelocity(m_vHeliForward, m_vRadRollPitch, deltaTime * 0.5);
						
		//ROLL ON DIRECTION: See how steep we're turning. Roll the helicopter accordingly for more natural flight. We only care about ZX plane.
//		m_fDbgAngleRoll = SDRC_Math.GetAngleBetweenVectorsXZ(heliVelocity, m_vHeliDirectionFuture);
		m_fDbgAngleRoll = SDRC_Math.GetAngleBetweenVectorsXZ(m_vHeliForward, m_vHeliDirectionFuture);
//		m_fDbgAngleRoll = SDRC_Math.GetAngleBetweenVectorsXZ(m_vHeliDirection, m_vHeliDirectionFuture);
//		m_fDbgAngleRoll = SDRC_Math.GetAngleBetweenVectorsXZ(m_vHeliForward, m_vHeliDirection);
		
/*		vector vec0 = m_vHeliForward;
		vector vec1 = m_vHeliDirectionFuture;
		vec0[1] = 0;
		vec1[1] = 0;
		m_fDbgAngleRoll = SDRC_Math.GetAngleBetweenVectors(vec0, vec1);*/
		
		m_fDbgAngleRoll = Math.Clamp(m_fDbgAngleRoll, -0.5, 0.5) * ROLL_ANGLE_MUL;
		m_vRadRollVel = SDRC_Math.RotateAroundAxis(m_vHeliForward, heliUp, m_fDbgAngleRoll);
		m_vRadRollVel = SDRC_Math.ComputeAngularVelocity(heliUp, m_vRadRollVel, deltaTime);
		
		//ROLL UP (YAW): Count the angle from heli up vs world up. The heli should slowly move back to horizontal flight.
		m_fDbgAngleRollBack = SDRC_Math.GetAngleBetweenVectors(heliUp, vector.Up);
		m_vRadRollBack = SDRC_Math.ComputeAngularVelocity(heliUp, vector.Up, deltaTime * 0.6);
	
		//Dummy
//		m_vRadRollVel = "0 0 0";
//		m_vRadRollBack = "0 0 0";
//		m_vRadRollPitch = "0 0 0";
		
		//Count the angular velocity
		m_vAngularVel = SDRC_Math.ComputeAngularVelocity(m_vHeliForward, m_vHeliDirection, deltaTime);
		
		owner.GetPhysics().SetAngularVelocity(m_vAngularVel + m_vRadRollVel + m_vRadRollBack + m_vRadRollPitch);
	}

	//------------------------------------------------------------------------------------------------	
	/*!	
	Set velocity vector
	*/
	private void SetVelocity(IEntity owner)
	{
		//Set velocity
		vector velVector = m_vDestination;
		vector rotVector = owner.GetAngles();
		velVector.Normalize();
		float forceMultiplier = m_fSpeed;
		float forceRotorUp = m_fRotorForce0 * ROTOR_FORCE_UP_MUL * 10;
		
		velVector = {velVector[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * forceMultiplier, velVector[1] * forceRotorUp * m_fRotorForceMultiplier, velVector[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * forceMultiplier};
		
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
	private void HandleLanding()
	{
		const float LANDING_DISTANCE = 150;
		
		if (m_bDoLanding)
		{		
			float distance = vector.Distance(m_vOrigin, m_vSplinePoints[m_vSplinePoints.Count() - 1]);
			
			if (distance < LANDING_DISTANCE)
			{
				float mul = distance / LANDING_DISTANCE;
				//m_fRotorForce0 = -1;//m_fRotorForce0Orig * mul * 0.25;
				m_fRotorForceMultiplier = -10 * (1 - mul);
		        m_Helicopter_s.RotorSetForceScaleState(0, m_fRotorForce0 * mul);
				m_fSpeedLandingMul = mul;
				m_fSpeedMin = 0;
			}
		}
	}		
	
	//------------------------------------------------------------------------------------------------	
	// Flight path things
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Create the initial flight path 
	*/
	void InitFlyPath(IEntity owner, vector origin, vector destination)	
	{
/*		if (!GetGame().GetWorld())
		{
			return;
		}*/
	#ifdef HELI_TESTING
		
		#ifdef HELI_TESTING_LANDING
			//Start near airfield
			origin = "800 50 2800";
			destination = "1060 0 2450";		
			m_bDoLanding = true;
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
		
		//Clear any existing path points
		ResetFlyPath();
		
		//Set height for start and destination points
		
		//With autostart, use the origin of the chopper spawn
		if (m_bAutoStart)	
		{
			if (destination[1] == 0)
			{
				destination[1] = SDRC_Misc.RandomFloat(m_fFlyHeightLow, m_fFlyHeightHigh);
			}
		}
		
		//Without autostart, randomize start height
		if (!m_bAutoStart)	
		{
			origin[1] = SDRC_Misc.RandomFloat(m_fFlyHeightLow, m_fFlyHeightHigh);
			destination[1] = SDRC_Misc.RandomFloat(m_fFlyHeightLow, m_fFlyHeightHigh);
		}

		//Store the original destination
		m_vOriginalDestination = destination;
				
		//Add points to path. Normally we would use AddDestination, but for the initial flight, we need points in m_vFlyPathPoints.
		SDRC_EFlyPathPointType fpType = SDRC_EFlyPathPointType.FLY;
		
		#ifdef HELI_TESTING_LANDING
			SDRC_EFlyPathPointType fpType = SDRC_EFlyPathPointType.LAND;
		#endif
		
		AddFlyPathPoint(origin);
		AddFlyPathPoint(vector.Lerp(origin, destination, 0.5), fpType);
		AddFlyPathPoint(destination, fpType);

		if (!m_bAutoStart)	//With autostart, use the origin of the chopper spawn
		{
			SetFlyPathHeight(origin);
		}

		//Create points for spline
		CreateFlyPathPoints();
		
		array<vector> flyPathPoints = {};
		GivePoints(flyPathPoints, m_vFlyPathPoints);
		SDRC_Spline3D.GenerateSplinePoints(flyPathPoints, m_vSplinePoints, -1);
		
		//Set final values
		m_iClosestIndex = 2;
		m_iOldClosestIndex = m_iClosestIndex;
		
		//Check that points are above ground
		CheckSplinePoints(origin);
		
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
	\param force - False: Create normal fly path. - True: Stop everything and fly immediately to the last position in m_vFlyPathPoints.
	*/
	void CreateFlyPath(vector origin, bool force = false)
	{
		//Clear any existing path points
		ResetFlyPath();

		//If we are the final destination, we stop creating waypoints and stop flying.
		if (m_bFinalDestinationReached)
		{
			m_bDestroyed = true;
			return;
		}
		
		//If final destination was requested, create the last waypoints.
		if (m_bFinalDestination)
		{
			m_bFinalDestinationReached = true;
		}
		
		if (!force)
		{
			//Take two points from old spline. This smoothens the spline.
			int splineStartIdx = m_vSplinePoints.Count() - POINTS_TO_SPLINE_START - 1;
			if (splineStartIdx < 1)
			{
				splineStartIdx = 1;
			}
			AddFlyPathPoint(m_vSplinePoints[splineStartIdx - 1]);
			AddFlyPathPoint(m_vSplinePoints[splineStartIdx]);
			GenerateWayPoint(origin, m_fWpType);
		}
		else
		{
			//Force flying to the last point ignoring other possible points. Take current origin, add a mid point and then destination
			AddFlyPathPoint(origin);
			vector destination = m_vFlyPathPoints[m_vFlyPathPoints.Count() - 1].pt;
			AddFlyPathPoint(vector.Lerp(origin, destination, 0.5));
			AddFlyPathPoint(destination);
			GenerateWayPoint(origin, SDRC_EHeliWaypointGenerationType.RANDOM);
		}
	
		//Create points for spline		
		CreateFlyPathPoints();
		
		SetFlyPathHeight(origin);
		array<vector> flyPathPoints = {};
		GivePoints(flyPathPoints, m_vFlyPathPoints);
		SDRC_Spline3D.GenerateSplinePoints(flyPathPoints, m_vSplinePoints, -1);
		//Search the closest indes from the spline start
		m_iClosestIndex = 0;
		float distance = SDRC_Spline3D.GetDistanceFromSpline(m_vSplinePoints, origin, m_iClosestIndex, false);	//NOTE: This will set m_iClosestIndex
		m_iOldClosestIndex = m_iClosestIndex;
		
		//Check that points are above ground
		//SDRC_DebugHelper.DrawPointList(m_vSplinePoints, m_sDid);
		CheckSplinePoints(origin);		
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
	void ResetFlyPath()
	{
		m_vFlyPathPoints.Clear();
	}
	
	//------------------------------------------------------------------------------------------------	
	// FlyPoint fixing and sanity check
	//------------------------------------------------------------------------------------------------	
		
	//------------------------------------------------------------------------------------------------	
	/*!	
	Set the requested flight path points between min/max flying height.
	*/	
	private void SetFlyPathHeight(vector origin)
	{
		float y = 0;
		
		foreach (int i, SDRC_FlyPathPoint flyPathPoint : m_vFlyPathPoints)
		{
			vector pt = flyPathPoint.pt;
			
			//Do not change height of two first points. These are the two points from the previous spline. 
			if (i < 2)
			{
				continue;
			}

			y = SDRC_Misc.GetSurfaceYWithWater(pt);
			float flyHeight = 0;
						
			if (flyPathPoint.type == SDRC_EFlyPathPointType.LAND)
			{
				//Do nothing
				y = y;
			}
			else
			{
				flyHeight = SDRC_Misc.RandomFloat(m_fFlyHeightLow, m_fFlyHeightHigh);
				pt[1] = 0;	//We may in the future use the provided Y coord for the points. For now we set it to 0.
			}
			
			pt[1] = pt[1] + flyHeight + y;			
			m_vFlyPathPoints[i].pt = pt;
		}
	}
		
	//------------------------------------------------------------------------------------------------	
	/*!	
	Check that spline points are above ground. Raise the point if needed.
	*/	
	private void CheckSplinePoints(vector origin)
	{	
		//Make sure the points are at minimum m_fFlyHeightLow from the ground.
		foreach (int i, vector pt : m_vSplinePoints)
		{
			float y = SDRC_Misc.GetSurfaceYWithWater(pt);

			if (pt[1] < (y + m_fFlyHeightLow))
			{
				pt[1] = y + m_fFlyHeightLow;	//Make chopper fly higher for a moment
				m_vSplinePoints[i] = pt;
			}
		}
		
		//If landing, let's put last point on ground
		if (m_bDoLanding)
		{
			int lastIdx = m_vSplinePoints.Count() - 1;
			m_vSplinePoints[lastIdx] = SDRC_Misc.SetPosToSurface(m_vSplinePoints[lastIdx]) + "0 10 0";
		}
				
/*		if (m_bDoLanding)
		{
			int lastIdx = m_vSplinePoints.Count() - 1;
			float yp = SDRC_Misc.GetSurfaceYWithWater(m_vSplinePoints[lastIdx]);
			
			for (int i = m_vSplinePoints.Count() - 5; i < m_vSplinePoints.Count(); i++)
			{	
				m_vSplinePoints[i] = SDRC_Misc.SetPosToSurface(m_vSplinePoints[i]) + 10;
			}
		}*/
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
				pos = GetRandomDestination(origin);
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
		}
	}

	//------------------------------------------------------------------------------------------------	
	/*!	
	Creates a random destination point
	*/	
	private vector GetRandomDestination(vector pos)
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
	private void CreateFlyPathPoints()
	{

		#ifdef HELI_TESTING			
			#ifndef HELI_TESTING_LANDING	
				//Replace the provided destination for testing purposes
			//	m_vFlyPathPoints.RemoveOrdered(m_vFlyPathPoints.Count() - 1);
				m_vFlyDestinations.RemoveOrdered(m_vFlyDestinations.Count() - 1);
			
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
		
		foreach (int idx, SDRC_FlyPathPoint flyDestination : m_vFlyDestinations)
		{		
			float distance = vector.DistanceXZ(m_vFlyPathPoints[m_vFlyPathPoints.Count() - 1].pt, flyDestination.pt);
			
			SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Distance: " + distance, LogLevel.SPAM);
	
			//TBD: If distance between two points is too short, should we skip?
			
			vector dir0;	//Previous flight direction
			vector dir1;	//New flight direction
			vector p0 = m_vFlyPathPoints[m_vFlyPathPoints.Count() - 2].pt;
			vector p1 = m_vFlyPathPoints[m_vFlyPathPoints.Count() - 1].pt;
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
				vector point = m_vFlyPathPoints[idx].pt;
//				m_vFlyPathPoints.RemoveOrdered(m_vFlyPathPoints.Count() - 1);
				
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
			
			SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Heli direction angle: " + heliAngle + " - Distance: " + distance, LogLevel.SPAM);
		}
		
		SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Created " + m_vFlyPathPoints.Count() + " points.", LogLevel.SPAM);
		
		//Clear the destinations 
		m_vFlyDestinations.Clear();
	}		

	//------------------------------------------------------------------------------------------------	
	/*!	
	Add a point to fly path. 
	*/
	void AddFlyPathPoint(vector destination, SDRC_EFlyPathPointType type = SDRC_EFlyPathPointType.FLY)
	{
		if (type == SDRC_EFlyPathPointType.LAND)
		{
//			destination = SDRC_Misc.SetPosToSurface(destination);
			m_bDoLanding = true;
		}
		
		m_vFlyPathPoints.Insert(new SDRC_FlyPathPoint(destination, type));		
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
	/*!	
	Add a destination for future.
	\param destination Next position to fly to. Multiple destinations can be defined by calling multiple times.
	\param type How to fly .. kinda. If set as FINAL, once reaching the destination, helicopter will stop flying. 
	*/
	void AddDestination(vector destination, SDRC_EFlyPathPointType type = SDRC_EFlyPathPointType.FLY)
	{
		m_vFlyDestinations.Insert(new SDRC_FlyPathPoint(destination, type));
		
		if (type == SDRC_EFlyPathPointType.FINAL)
		{
			m_bFinalDestination	= true;
		}
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
	void SetAutostart(bool value)
	{
		m_bAutoStart = value;
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
		
		m_bDestroyed = true;
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
							   	"SpeedMul:" + Math.Round(100*m_fSpeedMul)/100 + "\n" + 
								"";		
			debugText = debugText + 
							   	"Height: " + Math.Round(10*(origin[1] - SDRC_Misc.GetSurfaceYWithWater(origin)))/10 + " - " + 
							   	"Height: " + m_Helicopter_s.GetAltitudeAGL() + " - " + 			
							   	"RotorForceMul:" + m_fRotorForceMultiplier + "\n" +
	//						   	"SplinePoints:" + m_vSplinePoints.Count() + "\n" +
	//						   	"TurnInternal:" + m_fTimeTurnInterval + "\n" +
	//							"DbgAngle: " + m_fDbgAngle * Math.RAD2DEG + "\n" +
	//							"DbgAnglePitch: " + m_fDbgAnglePitch * Math.RAD2DEG + "\n" +
	//							"DbgAngleRoll: " + m_fDbgAngleRoll * Math.RAD2DEG + "\n" +
	//							"DbgAngleRollBack: " + m_fDbgAngleRollBack * Math.RAD2DEG + "\n" +
	//							"DestinationPointAdd: " + m_iDestinationPointAdd + "\n" 
								"";
			debugText = debugText + 
//								"Init:" + m_bInInit + ", " +
//								"Pilots::" + SDRC_VehicleHelper.PilotCountAlive(owner) + "\n" +
//								"Working:" + SDRC_VehicleHelper.IsWorking(owner) + " - " + 
								"Health: " + health + "\n" + 
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
			SDRC_Log.Add("[SDRC_ChopperComp:DrawHelicopterVectors] Index fixed.", LogLevel.WARNING);
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