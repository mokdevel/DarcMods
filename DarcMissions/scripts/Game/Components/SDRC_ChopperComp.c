//SDRC_ChopperComp.c

//Changed done in prefabs:
// - SCR_AIVehicleUsageComponent : Set true to Can Be Piloted

//------------------------------------------------------------------------------------------------
class SDRC_ChopperCompClass : ScriptGameComponentClass { }

//------------------------------------------------------------------------------------------------
enum SDRC_EHeliWaypointGenerationType
{
	NONE,
	RANDOM,		//Random flying for a helicopter
	PATROL,		//Fly around a certain area
//	GOTO,		//Fly to a given destination
	END,		//Fly out from the map - for mission ending
};

//------------------------------------------------------------------------------------------------
class SDRC_ChopperComp : ScriptGameComponent
{
	private static SDRC_ChopperComp s_Instance;	
	private ref array<vector> m_vSplinePoints = new array<vector>();
	private ref array<vector> m_vTangentPoints = new array<vector>();

	//Parameters accessible helicopter parameters
	float m_fRotorForceUp = 18.0;
	float m_fSpeedMin;				//Minimum speed
	float m_fSpeedMax;				//Maximum speed
	float m_fPower;					//Speed gain aka acceleration
	float m_fFlyHeightLow;			//Flight height low
	float m_fFlyHeightHigh;			//Flight height high
	SDRC_EHeliWaypointGenerationType m_fWpType; 
	float m_fDistanceLow;			//Distance for waypoint min
	float m_fDistanceHigh;			//..max
	
	//Speed management
	private const float SPEED_INTERVAL = 2;		
	private const float SPEED_GAIN = 1.1;
	private float m_fTimeSpeed = 0;

	private float m_fTimeBetweenPts = 1;
	private float m_fTimeBetweenPtsAvg = 1;
		
	//Turn
	const int TIME_TURN_INTERVAL_BASE = 40;			//Time to divide with speed to define the final turn time. Smaller value makes heli turn faster.
	
	//Pitch
	const float PITCH_ANGLE_RAD = 60 * Math.DEG2RAD;	//The pitch angle to use when calculating for speed effect. The faster the heli goes, the steeper the nose should be down.
	
	//Roll 
	const float ROLL_ANGLE_MUL = 2.4;//1.7;			//Multiplier for roll angle along the spline
	
	//Flight path
	const int SEGMENT_POINTS = 15;				//How many points to create for each segment
	const int POINTS_TO_NEW_DISTANCE = 2;		//How many spline points in to the future flight path is checked before adding new flight points.
	const int POINTS_TO_SPLINE_START = 6;		//Points to go back from m_iClosestIndex when creating a new flight path 
	const int DESTINATION_POINT_DIV = 12;		//How many points ahead to look for the destination. This is the divider for speed.
	const int TIME_FORCE_MOVE_POINT = 20;		//(seconds) Time to wait before force moving a point. This is to fix situations where the chopper gets stuck on a point.
	const float TIME_IN_INIT = 45;				//(seconds) Time to be in init state. During this time, we don't check for damage or similar things.
	const float ROTOR_FORCE_MUL = 0.9;			//Rotor force multiplier. Bigger value makes the heli react faster to up/down movement
	const float ROTOR_FORCE_MUL_PANIC = 1.4;	//Rotor force multiplier used when avoiding ground. 
	
	//Waypoint values
	const float WP_ANGLE = 40;					//Maximum angle for new destination
	
	//Runtime parameters
	private bool m_bInInit = true;				//While in init, consider the chopper to be flying.
	private bool m_bDestroyed = false;
	private int m_iDestinationPointAdd;
	private float m_fTimeTurnInterval;
	
	//Flight path runtime variables	
	private vector m_vOrigin;					//Current position
	private float m_fSpeed;						//Current speed
	private float m_fSpeedStart;				//Speed lerp start
	private float m_fSpeedTarget;				//Speed lerp target aka end
	private float m_fSpeedMul;					//Speed multiplier that depends on the turn
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
	
	//Debug stuff
	private float m_fDbgAngle;
	private float m_fDbgAnglePitch;
	private float m_fDbgAngleRoll;
	
	private int m_iClosestIndex;				//Closest point on spline to heli
	private int m_iNewClosestIndex;
	private int m_iNextIndex;					//Next index to our m_iClosestIndex - depends on speed
	private int m_iFutureIndex;					//Where we are heading in the long run
	private vector m_vDestination;
	private vector m_vDestinationFuture;

	//Flight path
	ref array<vector> m_vPathPoints = {};
	ref array<vector> m_vFlyDestinations = {};	//Requested destinations
		
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SDRC_Log.Add("[SDRC_ChopperComp] Starting SDRC_ChopperComp", LogLevel.NORMAL);
		s_Instance = this;				
		
		GetGame().GetCallqueue().CallLater(InitDone, TIME_IN_INIT * 1000);
	}
 
	//------------------------------------------------------------------------------------------------
	void InitDone()
	{
		m_bInInit = false;
	}

	//------------------------------------------------------------------------------------------------
	// Once all init things are done, activate the component
	void Ready(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.FRAME | EntityEvent.POSTFRAME);
		Activate(owner);
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

		//Adjust time depending on the speed.
		m_fTimeTurnInterval = TIME_TURN_INTERVAL_BASE / m_fSpeed;
		m_fTimeTurnInterval = Math.Clamp(m_fTimeTurnInterval, 0.6, 3);
				
		//If we've been stuck on a point, force new flight path. 
		//Sometimes the heli direction and path align so that the closest index does not update.
		bool bCreateNewPath = false;
		if (m_fTimeBetweenPts > TIME_FORCE_MOVE_POINT)
		{
			bCreateNewPath = true;
		}
		
		//No need to do anything unless we are at the end of spline.
		if ((m_iClosestIndex + m_iDestinationPointAdd + POINTS_TO_NEW_DISTANCE >= m_vSplinePoints.Count() - 1) || bCreateNewPath)
		{
			//Define a new destination and create a new path
			CreateFlightPath(m_vOrigin);
		}
		
		//Count destintation addition along the spline which is dependent on the speed.
		m_iDestinationPointAdd = m_fSpeed / DESTINATION_POINT_DIV;
		m_iDestinationPointAdd = Math.ClampInt(m_iDestinationPointAdd, 1, 3);
		
		//Find where we're going
		float distance = SDRC_Spline3D.GetDistanceFromSpline(m_vSplinePoints, m_vOrigin, m_iNewClosestIndex);	//NOTE: This will set m_iNewClosestIndex

		if (m_iNewClosestIndex > m_iClosestIndex)
		{
			m_iClosestIndex = m_iNewClosestIndex;
			m_fTimeBetweenPtsAvg = m_fTimeBetweenPts;		//TBD: This is a static value of previous time instead of average 
			m_fTimeBetweenPts = 0;
		}
		else if (m_iNewClosestIndex == m_iClosestIndex)
		{
			m_iClosestIndex++;
			m_fTimeBetweenPtsAvg = m_fTimeBetweenPts;		//TBD: This is a static value of previous time instead of average 
			m_fTimeBetweenPts = 0;
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
		
		m_vDestinationFuture = m_vSplinePoints[m_iFutureIndex];
		
		//Lerped m_vDestination that keeps on moving along the spline
		float td = m_fTimeBetweenPts / m_fTimeBetweenPtsAvg;
		td = Math.Clamp(td, 0, 1);
		m_vDestination = vector.Lerp(m_vSplinePoints[m_iNextIndex], m_vDestinationFuture, td);

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
		SetVelocity(owner);
		
		//Set turn
		SetTurn(owner, m_fTimeTurnInterval);				
		
		#ifndef SDRC_RELEASE
			DrawHelicopterVectors(owner);
		#endif		
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
		
		velVector = {velVector[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * forceMultiplier, velVector[1] * m_fRotorForceUp * m_fRotorForceMultiplier, velVector[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * forceMultiplier};
		
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
		m_fRotorForceMultiplier = (10 * ROTOR_FORCE_MUL) * ( (m_vSplinePoints[m_iClosestIndex][1] - m_vOrigin[1]) / m_vSplinePoints[m_iClosestIndex][1]);

		//If too low, turn the rotor force up
		if (m_vOrigin[1] < (SDRC_Misc.GetSurfaceYWithWater(m_vOrigin) + m_fFlyHeightLow) )
		{
			m_fRotorForceMultiplier = ROTOR_FORCE_MUL_PANIC;
		}
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
		float angle = Math.AbsFloat(SDRC_Math.GetAngleBetweenVectors(m_vHeliForward, m_vHeliDirectionFuture));
		m_fDbgAngle = angle;
		//Count the angle of the turn. The steeper the turn, the slower heli should be moving.
		m_fSpeedMul = Math.Clamp((angle * Math.RAD2DEG), 1, 90);
		m_fSpeedMul = m_fPower * (SPEED_GAIN - (m_fSpeedMul / 45));	//90
		m_fSpeedStart = m_fSpeed;
		m_fSpeedTarget = m_fSpeed * m_fSpeedMul;
		m_fSpeedTarget = Math.Clamp(m_fSpeedTarget, m_fSpeedMin, m_fSpeedMax);
		m_fTimeSpeed = 0;	//Start to change speed

		//ROLL PITCH: Change pitch according to speed		
//		m_fDbgAnglePitch = (m_fSpeedMul - 4) * PITCH_ANGLE * Math.DEG2RAD;	//No idea why 3 is a good value. :-)
		m_fDbgAnglePitch = (m_fSpeedMul - 4) * PITCH_ANGLE_RAD;	//No idea why 3 is a good value. :-)
		m_fDbgAnglePitch = Math.Clamp(m_fDbgAnglePitch, -0.7, 0.7);		
		m_vRadRollPitch = SDRC_Math.RotateAroundAxis(m_vHeliForward, heliPitch, m_fDbgAnglePitch);
		m_vRadRollPitch = SDRC_Math.ComputeAngularVelocity(m_vHeliForward, m_vRadRollPitch, deltaTime * 0.5);
						
		//ROLL UP (YAW): Count the angle from heli up vs world up. The heli should slowly move back to horizontal flight.
		m_vRadRollBack = SDRC_Math.ComputeAngularVelocity(heliUp, vector.Up, deltaTime * 0.3);
	
		//ROLL ON DIRECTION: See how steep we're turning. Roll the helicopter accordingly for more natural flight.
//		m_fDbgAngleRoll = SDRC_Math.GetAngleBetweenVectors(heliVelocity, m_vHeliDirectionFuture);
		m_fDbgAngleRoll = SDRC_Math.GetAngleBetweenVectors(m_vHeliForward, m_vHeliDirectionFuture);
//		m_fDbgAngleRoll = SDRC_Math.GetAngleBetweenVectors(m_vHeliDirection, m_vHeliDirectionFuture);
//		m_fDbgAngleRoll = SDRC_Math.GetAngleBetweenVectors(m_vHeliForward, m_vHeliDirection);
		m_fDbgAngleRoll = Math.Clamp(m_fDbgAngleRoll, -0.5, 0.5) * ROLL_ANGLE_MUL;
		m_vRadRollVel = SDRC_Math.RotateAroundAxis(m_vHeliForward, heliUp, m_fDbgAngleRoll);
		m_vRadRollVel = SDRC_Math.ComputeAngularVelocity(heliUp, m_vRadRollVel, deltaTime);
		
		//Dummy
//		m_vRadRollVel = "0 0 0";
//		m_vRadRollBack = "0 0 0";
//		m_vRadRollPitch = "0 0 0";
		
		//Count the angular velocity
		m_vAngularVel = SDRC_Math.ComputeAngularVelocity(m_vHeliForward, m_vHeliDirection, deltaTime);
		
		owner.GetPhysics().SetAngularVelocity(m_vAngularVel + m_vRadRollVel + m_vRadRollBack + m_vRadRollPitch);
	}

	//------------------------------------------------------------------------------------------------	
	// Flight path things
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Create the initial flight path 
	*/
	void InitFlightPath(IEntity owner, vector origin, vector destination)	
	{
		if (!GetGame().GetWorld())
		{
			return;
		}
		
		//Clear any existing path points
		m_vPathPoints.Clear();
		
		//Set height for initial starting point
		origin[1] = SDRC_Misc.RandomFloat(m_fFlyHeightLow, m_fFlyHeightHigh);
		
		//Add points to path
		m_vPathPoints.Insert(origin);		
		m_vPathPoints.Insert(vector.Lerp(origin, destination, 0.2));
		m_vPathPoints.Insert(vector.Lerp(origin, destination, 0.6));
		m_vPathPoints.Insert(destination);

		SetFlyPathHeight(origin);
		SDRC_Spline3D.GenerateSplinePoints(m_vPathPoints, m_vSplinePoints, m_vTangentPoints, SEGMENT_POINTS, true);
		//Set final values
		m_iClosestIndex = 2;
		m_iNewClosestIndex = m_iClosestIndex + 1;
		//Check that points are above ground
		CheckSplinePoints(origin);
		SDRC_Spline3D.DrawSplinePoints(m_vSplinePoints);
		
		m_fSpeed = 0.1;
		m_fSpeedTarget = m_fSpeed;
		
		//Set chopper initial position		
		owner.SetOrigin(m_vSplinePoints[0]);
		vector angles = vector.Direction(owner.GetOrigin(), m_vSplinePoints[m_iClosestIndex]);
		angles.Normalize();
		angles = angles.VectorToAngles();
		owner.SetYawPitchRoll(angles);

		SetVelocity(owner);
		
		DrawSplinePoints(m_vPathPoints);		
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Create the runtime flight path with waypoint definition
	*/
	void CreateFlightPath(vector origin)
	{
		if (!GetGame().GetWorld())
		{
			return;
		}

		//Clear any existing path points
		m_vPathPoints.Clear();
		
		//Take two points from old spline. This smoothens the spline.
		int splineStartIdx = m_iClosestIndex  - POINTS_TO_SPLINE_START;
		if (splineStartIdx < 0)
		{
			splineStartIdx = 0;
		}
		m_vPathPoints.Insert(m_vSplinePoints[splineStartIdx]);
		m_vPathPoints.Insert(m_vSplinePoints[m_iClosestIndex]);
		
		//Use requested destinations or generate a waypoint.
		if (m_vFlyDestinations.IsEmpty())
		{
			GenerateWayPoint(origin, m_fWpType);
		}
		else
		{
			m_vPathPoints.InsertAll(m_vFlyDestinations);
			m_vFlyDestinations.Clear();
		}
	
		SetFlyPathHeight(origin);
		SDRC_Spline3D.GenerateSplinePoints(m_vPathPoints, m_vSplinePoints, m_vTangentPoints, SEGMENT_POINTS, true);
		float distance = SDRC_Spline3D.GetDistanceFromSpline(m_vSplinePoints, origin, m_iClosestIndex);	//NOTE: This will set m_iNewClosestIndex
		m_iNewClosestIndex = m_iClosestIndex + 1;
		//Check that points are above ground
		CheckSplinePoints(origin);
		SDRC_Spline3D.DrawSplinePoints(m_vSplinePoints);
		
		if (m_vSplinePoints.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_ChopperComp:CreateFlightPath] No points!", LogLevel.ERROR);
		}
		
		DrawSplinePoints(m_vPathPoints);
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
		float yp = 0;
		
		foreach (int i, vector pt : m_vPathPoints)
		{
			//Do not change height of two first points. These are the two points from the previous spline. 
			if (i < 2)
			{
				continue;
			}
			
			pt[1] = 0;	//We may in the future use the provided Y coord for the points. For now we set it to 0.
			
			float flyHeight = SDRC_Misc.RandomFloat(m_fFlyHeightLow, m_fFlyHeightHigh);
			y = SDRC_Misc.GetSurfaceYWithWater(pt);
			
			pt[1] = pt[1] + flyHeight + y;
			m_vPathPoints[i] = pt;
		}
	}
		
	//------------------------------------------------------------------------------------------------	
	/*!	
	Check that spline points are above ground and smooth new segments.
	*/	
	private void CheckSplinePoints(vector origin)
	{	
/*		const int DIFF_DISTANCE = 7;
		
		//Smooth the segments		
		int diffPt = Math.ClampInt(m_vSplinePoints.Count() - 1 - DIFF_DISTANCE - m_iClosestIndex, 1, DIFF_DISTANCE);
		
		//Let's lerp the heights somewhat together for smoother raise/lower
		for (int i = 0; i < diffPt; i++)
		{
			float l = Math.Lerp(origin[1], m_vSplinePoints[m_iClosestIndex + diffPt][1], i / diffPt);
			m_vSplinePoints[m_iClosestIndex + i][1] = l;
		}*/
		
		//Make sure the points are at minimum m_fFlyHeightLow from the ground.
		foreach (int i, vector pt : m_vSplinePoints)
		{
			float y = SDRC_Misc.GetSurfaceYWithWater(pt);

			if (pt[1] < (y + m_fFlyHeightLow))
			{
				pt[1] = y + m_fFlyHeightLow * 1.2;	//Make chopper fly higher for a moment
				m_vSplinePoints[i] = pt;
				
				//TBD: Should be flying higher for a few dots
				if (i > 0)
				{
					m_vSplinePoints[i - 1] = pt;
				}
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
		array<vector> positions = {};
		vector pos = "0 0 0";
		
		//Random flying for a helicopter
		if (wpGenType == SDRC_EHeliWaypointGenerationType.RANDOM)
		{	
			pos = GetRandomDestination(origin);
			positions.Insert(pos);
			
			CreateFlyPoints(positions);
		}
		
		//Fly around a certain area
		if (wpGenType == SDRC_EHeliWaypointGenerationType.PATROL)
		{
		}
		
/*		//Fly to a given destination
		if (wpGenType == SDRC_EHeliWaypointGenerationType.GOTO)
		{
			pos = "3100 0 2800";									//TBD: Remove
			positions.Insert(pos);
	
			CreateFlyPoints(positions);
		}*/
		
		//Fly out from the map - for mission ending
		if (wpGenType == SDRC_EHeliWaypointGenerationType.END)
		{		
			
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
		
		for (int i = 0; i < 5; i++)
		{
			//If distance is under 1.0, use world percentage
			if (distance < 1.0)
			{
//				newpos = SDRC_Misc.GetRandomWorldPosPercentage(0.2);
				newpos = SDRC_Misc.GetRandomWorldPosPercentage(distance);
			}
			else
			{
				newpos = SDRC_Misc.GetCoordinatesOnCircle(pos, distance, SDRC_Misc.RandomInt(0, 360));
			}

			SDRC_Log.Add("[SDRC_ChopperComp:GetRandomDestination] Pos: " + newpos, LogLevel.DEBUG);
				
			float angle = GetDestinationAngle(newpos);
			
			SDRC_Log.Add("[SDRC_ChopperComp:GetRandomDestination] Angle: " + angle, LogLevel.DEBUG);

			if (Math.AbsFloat(angle) < (WP_ANGLE * 2.0))
			{
				break;
			}			
		}
				
		return newpos;
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Get angle between destination and current path direction.
	*/	
	private float GetDestinationAngle(vector pos, out vector dir0 = "0 0 0")
	{
		int idx = m_vPathPoints.Count() - 1;
		//Current direction		
		dir0 = vector.Direction(m_vPathPoints[idx - 1], m_vPathPoints[idx]);
		//Direction towards destination
		vector dir1 = vector.Direction(m_vPathPoints[idx], pos);
		float angle = SDRC_Math.GetAngleBetweenVectors(dir0, dir1) * Math.RAD2DEG;
		
		return angle;
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Create fly points
	
	NOTE: Currently uses only one point of the array
	*/	
	private void CreateFlyPoints(array<vector> positions)
	{
		//TBD: Future improvement. NOTE: Untested code below
		/*
		//Add mid points for too long distances
		int cnt = m_vPathPoints.Count() - 1;
		for (int i = 0; i < cnt; i++)
		{
			float dist = vector.DistanceXZ(m_vPathPoints[i], m_vPathPoints[i + 1]);
			if (dist > 100)
			{
				vector mid = vector.Lerp(m_vPathPoints[i], m_vPathPoints[i + 1], 0.5);
				m_vPathPoints.InsertAt(mid, i);
				i++;
				cnt++;					
			}
		}
		*/
		
		vector pos = positions[0];
		
		int idx = m_vPathPoints.Count() - 1;
		float distance = vector.DistanceXZ(m_vPathPoints[idx], pos);
		
		//Current direction		
		vector dir0;// = vector.Direction(m_vPathPoints[idx - 1], m_vPathPoints[idx]);
		
/*		//Direction towards destination
		vector dir1 = vector.Direction(m_vPathPoints[idx], pos);
		
		float heliAngle = SDRC_Math.GetAngleBetweenVectors(dir0, dir1) * Math.RAD2DEG;
*/
		float heliAngle = GetDestinationAngle(pos, dir0);
		
		if (Math.AbsFloat(heliAngle) > (WP_ANGLE * 2))
		{				
			SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Heli direction angle is very STEEP: " + heliAngle, LogLevel.WARNING);
		}
				
		if (Math.AbsFloat(heliAngle) > WP_ANGLE)
		{				
			//We need to take a detour
			dir0 = SDRC_Math.RotateAroundAxis(dir0, vector.Up, heliAngle * 1.5);
			vector vec = m_vPathPoints[idx] + dir0.Normalized() * distance / 3;
			
			m_vPathPoints.Insert(vec);
			
			SDRC_DebugHelper.AddDebugPos(vec, ARGB(128, 128, 128, 64), 5.0);
		}
		
		m_vPathPoints.Insert(pos);
		
		SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Heli direction angle: " + heliAngle + " - Distance: " + distance, LogLevel.DEBUG);
	}		

	//------------------------------------------------------------------------------------------------	
	/*!	
	Add a destination for future.
	*/
	void AddDestination(vector destination)
	{
		m_vFlyDestinations.Insert(destination);
	}	
		
	//------------------------------------------------------------------------------------------------	
	// Helicopter things
	//------------------------------------------------------------------------------------------------	
	
	void SetHeli(float rotorForceUp, float speedMin, float speedMax, float power, float flyHeightLow, float flyHeightHigh, SDRC_EHeliWaypointGenerationType wpType, float distanceLow, float distanceHigh)
	{
		SDRC_Log.Add("[SDRC_ChopperComp:SetHeli] Updating values.", LogLevel.DEBUG);
		
		m_fRotorForceUp = rotorForceUp * 100;
		m_fSpeedMin = speedMin;
		m_fSpeedMax = speedMax;
		m_fPower = power;
		m_fFlyHeightLow = flyHeightLow;
		m_fFlyHeightHigh = flyHeightHigh;
		m_fDistanceLow = distanceLow;
		m_fDistanceHigh = distanceHigh;
		m_fWpType =	wpType;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetDamage(IEntity owner)
	{
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(owner.FindComponent(DamageManagerComponent));
		float health = 0.02;//SDRC_Misc.RandomFloat(0, 0.15);
		if (damageManager)
		{
			damageManager.SetHealthScaled(health);
		}
		
		SDRC_Log.Add("[SDRC_ChopperComp:SetDamage] Setting health: " + health, LogLevel.DEBUG);
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsStillWorking(IEntity owner)
	{
		if (m_bInInit)
		{
			return true;
		}

		if ( (SDRC_VehicleHelper.IsWorking(owner)) && (SDRC_VehicleHelper.PilotCountAlive(owner) > 0) )
		{
			return true;
		}
		
		m_bDestroyed = true;
		
		//Set damage so it should be destroyed on crash
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(owner.FindComponent(DamageManagerComponent));
		float damage = SDRC_Misc.RandomFloat(0, 0.15);
		if (damageManager)
		{
			damageManager.SetHealthScaled(damage);		
		}
		
		//Make the chopper while unsteadily
		VehicleHelicopterSimulation owner_s = VehicleHelicopterSimulation.Cast(owner.FindComponent(VehicleHelicopterSimulation));
		float force = SDRC_Misc.RandomFloat(0.2, 0.7);
        owner_s.RotorSetForceScaleState(0, force);
		force = SDRC_Misc.RandomFloat(0.3, 2.5);
        owner_s.RotorSetForceScaleState(1, force);
		
		return false;
	}

	//------------------------------------------------------------------------------------------------	
	// Debugging things
	//------------------------------------------------------------------------------------------------	

	//------------------------------------------------------------------------------------------------	
	static void DrawSplinePoints(array<vector> resultPoints)
	{
	#ifndef SDRC_RELEASE
		foreach (int i, vector pos : resultPoints)
		{
			SDRC_DebugHelper.AddDebugSphere(pos, ARGB(40, 128, 64, 64), 1.0);
			
			if (i < (resultPoints.Count() - 1))
			{
				SDRC_DebugHelper.AddDebugLine(resultPoints[i], resultPoints[i + 1], ARGB(40, 256, 64, 64));
			}			
		}
	#endif
	}
			
	//------------------------------------------------------------------------------------------------	
	void DrawHelicopterVectors(IEntity owner)
	{
		if (!DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_LINES))
		{		
			return;
		}
			
		vector origin = owner.GetOrigin();
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
		float health = damageManager.GetHealth();

		string debugText = 	//"Speedangle:" + angle * Math.RAD2DEG + "\n" +
						   	"Speed:" + Math.Round(10*m_fSpeed)/10 + " - " +
						   	"Start/Target:" + Math.Round(10*m_fSpeedStart)/10 + "/" + Math.Round(10*m_fSpeedTarget)/10 + "\n" +
//						   	"Avg time:" + m_fTimeBetweenPtsAvg + "\n" +
						   	"Height: " + (origin[1] - SDRC_Misc.GetSurfaceYWithWater(origin)) + "\n" + 
						   	"SpeedMul:" + m_fSpeedMul + "\n" + 
							"";		
		debugText = debugText + 
						   	"RotorForceMul:" + m_fRotorForceMultiplier + "\n" +
						   	"SplinePoints:" + m_vSplinePoints.Count() + "\n" +
//						   	"TurnInternal:" + m_fTimeTurnInterval + "\n" +
//							"DbgAngle: " + m_fDbgAngle * Math.RAD2DEG + "\n" +
//							"DbgAnglePitch: " + m_fDbgAnglePitch * Math.RAD2DEG + "\n" +
//							"DbgAngleRoll: " + m_fDbgAngleRoll * Math.RAD2DEG + "\n" +
//							"DestinationPointAdd: " + m_iDestinationPointAdd + "\n" 
							"";
		debugText = debugText + 
							"In Init:" + m_bInInit + ", " +
							"Is working:" + SDRC_VehicleHelper.IsWorking(owner) + "\n" +
							"Pilot count:" + SDRC_VehicleHelper.PilotCountAlive(owner) + "\n" +
							//"Is piloted:" + SDRC_VehicleHelper.IsPiloted(owner) + "\n" +
							"Health: " + health + "\n" + 
							"";

		DebugTextWorldSpace.Create(GetGame().GetWorld(), debugText, DebugTextFlags.ONCE, origin[0], origin[1], origin[2], 20);
				
		//Planned destination
		DrawLine(origin, m_vSplinePoints[m_iClosestIndex], Color.GRAY);		
		
		//Chopper destination direction vector
		vector vFwd = vector.Direction(origin, m_vDestination);
//		vFwd.Normalize();
//		DrawLine(origin, origin + (vFwd * 20), Color.WHITE);
		DrawLine(origin, origin + vFwd, Color.WHITE);

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