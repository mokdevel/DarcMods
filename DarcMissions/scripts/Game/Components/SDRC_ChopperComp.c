//SDRC_ChopperComp.c

//------------------------------------------------------------------------------------------------
class SDRC_ChopperCompClass : ScriptGameComponentClass { }
//SDRC_RplGMCompClass g_RplGMCompClass;

//------------------------------------------------------------------------------------------------
class SDRC_ChopperComp : ScriptGameComponent
{
	private static SDRC_ChopperComp s_Instance;	
	private ref array<vector> m_vSplinePoints = new array<vector>();
	private ref array<vector> m_vTangentPoints = new array<vector>();
//	const float SPEED_INTERVAL = 0.2;		
//	private float m_fTimeSpeed = 0;
	private float m_fTimeTurn = 0;
	
	//Flight path
	const int SPLINE_POINT_DISTANCE = 25;
	const int TIME_TURN_INTERVAL_BASE = 30;
	const int DESTINATION_POINT_DIV = 15;
	
	//Setup parameters
	private float m_fGroundLow = 5;
	private float m_fGroundHigh = 40;
	private float m_fLen = 0;
	
	//Runtime parameters
	private int m_iSegments;
	private int m_iSegmentPoints;
	private int m_iDestinationPointAdd;
	private float m_fTimeTurnInterval;
	
	//Flight path runtime variables	
	private float m_fSpeed = 50;
	private float m_fSpeedTarget = m_fSpeed;
	private float m_fSpeedMul = 1;
	private float m_fSpeedChange = 0;
	private bool m_bDoTurn = true;
	private vector m_vRollTarget;
	private vector m_vAngTarget;
	private vector m_vRadRollVel;
	private vector m_vRadRollBack;
	private bool flip;
	
	int closestIndex;
	int newClosestIndex;
	vector m_vDestination;
	vector m_vDestinationFuture;
	
	override void OnPostInit(IEntity owner)
	{
		SDRC_Log.Add("[SDRC_ChopperComp] Starting SDRC_ChopperComp", LogLevel.NORMAL);
		s_Instance = this;				
		SetEventMask(owner, EntityEvent.FRAME | EntityEvent.POSTFRAME);
		Activate(owner);
		
		InitFlightPath();
		
		closestIndex = 3;
		newClosestIndex = closestIndex + 1;
		m_vDestination = m_vSplinePoints[closestIndex];

		//Set chopper initial position		
		owner.SetOrigin(m_vSplinePoints[0]);
		vector angles = vector.Direction(owner.GetOrigin(), m_vDestination);
		angles.Normalize();
		angles = angles.VectorToAngles();
		owner.SetYawPitchRoll(angles);

		SetVelocity(owner);
		SetTurn(owner, m_fTimeTurnInterval);
	}
 
	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to component
	*/
	static SDRC_ChopperComp GetInstance()
	{
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
//		m_fTimeSpeed += timeSlice;
		m_fTimeTurn += timeSlice;

		vector origin = owner.GetOrigin();

		//Adjust time depending on the time.
		m_fTimeTurnInterval = TIME_TURN_INTERVAL_BASE / m_fSpeed;
				
		//Count destintation addition along the spline which is dependent on the speed.
		m_iDestinationPointAdd = m_fSpeed / DESTINATION_POINT_DIV;
		m_iDestinationPointAdd = 1;
		if (m_iDestinationPointAdd < 1)
		{
			m_iDestinationPointAdd = 1;
		}
		
		//Draw where we are planning to go
		float distance = GetDistanceFromSpline(m_vSplinePoints, origin, newClosestIndex);		
		
		if ( (newClosestIndex - closestIndex) < m_iDestinationPointAdd)
		{		
			closestIndex = newClosestIndex + m_iDestinationPointAdd;
			m_bDoTurn = true;
		}
		
		if (newClosestIndex < closestIndex)
		{
			closestIndex++;
		}
		
		m_vDestination = m_vSplinePoints[closestIndex + m_iDestinationPointAdd];
		m_vDestinationFuture = m_vSplinePoints[closestIndex + (m_iDestinationPointAdd * 2)];
		
		SetVelocity(owner);

		DrawHelicopterVectors(owner);
						
/*		if (m_fTimeSpeed > SPEED_INTERVAL)
		{
			//SetTurn(owner);
			m_fTimeSpeed = m_fTimeSpeed - SPEED_INTERVAL;
		}*/

		if ( (m_fTimeTurn > m_fTimeTurnInterval) && (m_bDoTurn) )
		{
			SetTurn(owner, m_fTimeTurnInterval);
			m_fTimeTurn = m_fTimeTurn - m_fTimeTurnInterval * 1.1;
			m_bDoTurn = false;
		}
				
		float length = vector.Distance(m_vSplinePoints[closestIndex + 0], m_vSplinePoints[closestIndex + 1]);
		
		
/* From: SCR_HelicopterCinematicFlyComponent
		vector velOrig = GetOwner().GetPhysics().GetVelocity();
		vector rotVector = GetOwner().GetAngles();
		vector vel = {velOrig[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * forceMultiplier, velOrig[1], velOrig[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * forceMultiplier };
		GetOwner().GetPhysics().SetVelocity(vel);				
*/		
	}

	//------------------------------------------------------------------------------------------------	
	void SetVelocity(IEntity owner)
	{
		vector origin = owner.GetOrigin();
		
		//Define speed
//		float speedMul = vector.Distance(origin, m_vDestination);
		vector heliForward = owner.GetTransformAxis(2);
		vector heliDirection = vector.Direction(origin, m_vDestination);				
		vector heliDirectionFuture = vector.Direction(origin, m_vDestinationFuture);				
		vector heliVelocity = owner.GetPhysics().GetVelocity();
		float angle = GetAngleBetweenVectors(heliForward, heliDirectionFuture);
		m_fSpeedMul = Math.Clamp((angle * Math.RAD2DEG) - 10, 1, 90);	//A ten degree turn is not going to affect the speed
		m_fSpeedMul = 1.1 - (m_fSpeedMul / 90);
		
		m_fSpeed = m_fSpeed + m_fSpeedChange;
		
		string debugText = 	"Speedangle:" + angle * Math.RAD2DEG + "\n" +
						   	"Speed:" + m_fSpeed + "\n" +
						   	"SpeedChange:" + m_fSpeedChange + "\n" +
						   	"SpeedTarget:" + m_fSpeedTarget + "\n" +
						   	"SpeedMul:" + m_fSpeedMul + "\n";
		debugText = debugText + 
						   	"TurnInternal:" + m_fTimeTurnInterval + "\n" +
							"DestinationPointAdd: " + m_iDestinationPointAdd;
							
		DebugTextWorldSpace.Create(GetGame().GetWorld(), debugText, DebugTextFlags.ONCE, origin[0], origin[1], origin[2], 20);
		
		//Set velocity
		vector chopVector = vector.Direction(origin, m_vDestination);
		chopVector.Normalize();		
		chopVector = chopVector * m_fSpeed;// * (m_iSpeedAvg / 100);
//		chopVector = chopVector * speed * speedMul;//length;
		owner.GetPhysics().SetVelocity(chopVector);		
	}
	
	//------------------------------------------------------------------------------------------------	
	void SetTurn(IEntity owner, float deltaTime)
	{
		if (deltaTime == 0)
		{
			return;
		}
		
		//Set speed according to previous turns
		m_fSpeedTarget = m_fSpeed * m_fSpeedMul;
		m_fSpeedChange = (m_fSpeedTarget - m_fSpeed) / (deltaTime * 10);
		
		//Calculate roll
		int rollIdxStart = closestIndex - 1;//(m_iDestinationPointAdd / 2);
		if (rollIdxStart < 0)
		{
			rollIdxStart = 0;
		}
		int rollIdxEnd = closestIndex + (m_iDestinationPointAdd);
		if (rollIdxEnd <= closestIndex)
		{
			rollIdxEnd = closestIndex + 1;
		}
		
		float roll = ComputeSplineRoll(m_vSplinePoints[rollIdxStart], m_vSplinePoints[closestIndex], m_vSplinePoints[rollIdxEnd], m_vRollTarget);

		//Get heli position
		vector origin = owner.GetOrigin();
		//Get the heli forward vector
		vector heliForward = owner.GetTransformAxis(2);
		//Get chopper direction
		vector heliDirection = vector.Direction(origin, m_vDestination);
		
		//Count the angle from heli up vs world up. The heli should slowly move back to horizontal flight.
		vector heliUp = owner.GetTransformAxis(1);
		heliUp.Normalize();
		m_vRadRollBack = ComputeAngularVelocity(heliUp, vector.Up, deltaTime * 10);

		//See how steep we're turning. Roll the helicopter accordingly for more natural flight.
		vector heliVelocity = owner.GetPhysics().GetVelocity();
//		float angVelTurn = GetAngleBetweenVectors(heliForward, heliDirection);
		float angVelTurn = GetAngleBetweenVectors(heliVelocity, heliDirection);
//		float angVelTurn = GetAngleBetweenVectors(heliForward, heliVelocity);
		
		angVelTurn = angVelTurn + roll;
		angVelTurn = Math.Clamp(angVelTurn, -0.2, 0.2);
		m_vRadRollVel = "0 0 0";
		m_vRadRollVel[2] = -angVelTurn;
		
		float turnMul = 1;
		Print("SDRC angVelTurn: " + angVelTurn * Math.RAD2DEG + " , mul: " + turnMul );
		
		//Turn the roll according to spline
//		vector angularVel = ComputeAngularVelocity(heliForward, heliDirection, deltaTime);
		vector angularVel = ComputeAngularVelocity(heliForward, heliVelocity, deltaTime);
		
		m_vAngTarget = angularVel * Math.DEG2RAD;
		owner.GetPhysics().SetAngularVelocity(angularVel + m_vRadRollVel + m_vRadRollBack);		
	}
	
	//------------------------------------------------------------------------------------------------	
	void DrawHelicopterVectors(IEntity owner)
	{
		if (!DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_LINES))
		{		
			return;
		}
			
		vector origin = owner.GetOrigin();
		
		//Planned destination
		DrawLine(origin, m_vSplinePoints[closestIndex], Color.GRAY);		
		
		//Chopper destination direction vector
		vector vFwd = vector.Direction(origin, m_vDestination);
		vFwd.Normalize();
		DrawLine(origin, origin + (vFwd * 10), Color.BLACK);

		//Chopper future destination direction vector
		vFwd = vector.Direction(origin, m_vDestinationFuture);
		vFwd.Normalize();
		DrawLine(origin, origin + (vFwd * 40), Color.BLACK);		

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

		vector vVec = m_vAngTarget;
		vVec.Normalize();
		DrawLine(origin, origin + (vVec * 15), Color.GRAY);

		vVec = m_vRadRollVel;
		vVec[1] = -vVec[2];
		vVec[2] = vVec[0];
		vVec.Normalize();
		DrawLine(origin, origin + (vVec * 25), Color.WHITE);

		vVec = m_vRadRollBack;
		vVec.Normalize();
		DrawLine(origin, origin + (vVec * 35), Color.WHITE);
		
								
		//Draw velocity vector
		vector vVel = owner.GetPhysics().GetVelocity();
		float currentSpeed = vVel.Length();
//		vVel.Normalize();
		DrawLine(origin, origin + (vVel * currentSpeed), Color.GRAY_75);			
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

	//------------------------------------------------------------------------------------------------	
	//\param angle in radians
	float GetAngleBetweenVectors(vector v1, vector v2)
	{
	    // Normalize both vectors
	    vector a = v1.Normalized();
	    vector b = v2.Normalized();
	
	    // Dot product
	    float dot = vector.Dot(a, b);
	
	    // Clamp dot to avoid NaN from floating-point errors
	    dot = Math.Clamp(dot, -1.0, 1.0);
	
	    // Return angle in radians
	    return Math.Acos(dot);
	}	
		
	//------------------------------------------------------------------------------------------------	
	//\returns AngVel in radians
	vector ComputeAngularVelocity(vector v1, vector v2, float deltaTime)
	{
		if (deltaTime == 0)
		{
			return "0 0 0";
		}
		
	    vector a = v1.Normalized();
	    vector b = v2.Normalized();
	
	    vector axis = a * b; // cross product (rotation axis)
	    float dot = vector.Dot(a, b);
	    dot = Math.Clamp(dot, -1.0, 1.0);
	
	    float angle = Math.Acos(dot); // radians between directions
	
	    // avoid division by zero
/*	    if (Math.AbsFloat(angle) < 0.0001)
	        return "0 0 0"; */
	
	    vector angVel = axis.Normalized() * (angle / deltaTime);
	    return angVel;
	}	
	
	//------------------------------------------------------------------------------------------------
	// \return degrees of roll
	// \return vector Axis of the roll
	float ComputeSplineRoll(vector p0, vector p1, vector p2, out vector axis)
	{
	    // Compute forward tangents between points
	    vector t0 = (p1 - p0).Normalized();
	    vector t1 = (p2 - p1).Normalized();
	
	    // Compute curve axis (cross product)
	    //vector axis = t0 * t1;
	    axis = t0 * t1;
	    float axisLen = axis.Length();
	
	    // Handle nearly straight segments
	    if (axisLen < 0.0001)
	        return 0.0;
	
	    // Compute angle between tangents
	    float dot = vector.Dot(t0, t1);
	    dot = Math.Clamp(dot, -1.0, 1.0);
	    float angle = Math.Acos(dot);
	
	    // Determine roll direction (sign)
	    float rollSign = 1.0;
	    if (axis[1] < 0.0)
		{
	        rollSign = -1.0;
		}

		axis = axis * rollSign;
		
	    // Convert to degrees and apply sign
	    float rollDeg = rollSign * angle * Math.RAD2DEG;
	    return rollDeg;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	
	*/
	void InitFlightPath()	
	{
		//Structures
/*		array<vector> pathPoints = {
			"0000 010 000",
			"0060 040 100",
			"0030 050 200",
			"0100 030 240",
			"0200 030 160",
			"0220 030 140",
			"0120 020 080",
		};*/

		//Arland
		array<vector> pathPoints = {
			"1500 020 2000",
			"1400 030 2200",
			"1600 020 2300",
			"2300 040 2500",
			"2500 010 2250",	//Timber Ridge
			"3100 030 2800",	//Beauregard
			"2400 030 1600",
			"1900 000 1300",
			"1500 000 2200",
			"2200 020 2200",
		};
		
		//Count flight path length - straight lines
		for (int i = 0; i < pathPoints.Count() - 2; i++)
		{
			m_fLen = m_fLen + vector.Distance(pathPoints[i], pathPoints[i + 1]);
		}
		
		m_iSegments = pathPoints.Count() - 1;
		m_iSegmentPoints = (m_fLen/m_iSegments) / SPLINE_POINT_DISTANCE;
		
		SDRC_Log.Add("[SDRC_ChopperComp:InitFlightPath] Flight path length: " + m_fLen + " , segments: " + m_iSegments + " fpSegmentPoints: " + m_iSegmentPoints, LogLevel.DEBUG);
		
		m_fGroundLow = 5;
		m_fGroundHigh = 40;
		
		foreach (int i, vector pt : pathPoints)
		{
			float y = GetGame().GetWorld().GetSurfaceY(pt[0], pt[2]);
			if (SDRC_Misc.IsPosInWater(pt))	//Is it under water?
			{
				y = GetGame().GetWorld().GetOceanHeight(pt[0], pt[2]);;
			}
			y = pt[1] + y + Math.RandomFloat(m_fGroundLow, m_fGroundHigh); 
			vector newPt = pt;
			newPt[1] = y;
			pathPoints[i] = newPt;
		}
					
		SDRC_Spline3D.GenerateSplinePoints(pathPoints, m_vSplinePoints, m_vTangentPoints, m_iSegmentPoints, true);
		m_vDestination = m_vSplinePoints[1];
	}
	
	//------------------------------------------------------------------------------------------------	
	//! Gets the shortest 3D distance between a point and a spline
	//! Extended from SCR_Math3D function
	//! \param[in] points array of all points forming the spline, minimum 1 point
	//! \param[in] point point that is being checked
	//! \return distance from spline, -1 if no points are provided
	static float GetDistanceFromSpline(notnull array<vector> points, vector point, out int index = 0)
	{		
		int count = points.Count();
		if (count < 1)
			return -1;

		if (count == 1)
			return vector.Distance(point, points[0]);

		float tempDistanceSq;
		vector segmentStart = points[0];
		float minDistanceSq = vector.DistanceSq(point, segmentStart);

		foreach (int i, vector segmentEnd : points)
		{
			if (i == 0)
				continue;

			tempDistanceSq = Math3D.PointLineSegmentDistanceSqr(point, segmentStart, segmentEnd);
			if (tempDistanceSq < minDistanceSq)
			{
				index = i;
				minDistanceSq = tempDistanceSq;
			}

			segmentStart = segmentEnd;
		}

		return Math.Sqrt(minDistanceSq);
	}	
}