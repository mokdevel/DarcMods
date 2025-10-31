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
	const int SPLINE_POINT_DISTANCE = 30;
	private float m_fGroundLow = 5;
	private float m_fGroundHigh = 40;
	private float m_fLen = 0;
	private int m_iSegments = 0;
	private int m_iSegmentPoints = 0;
	private int m_iDestinationPointAdd = 2;
	private float m_fTimeTurnInterval;
	
	//Flight path runtime variables	
	private float m_fSpeed = 60;
	private int m_iSpeedAvgCount = 0;
	private float m_iSpeedAvg = 0;
	private bool m_bDoTurn = true;
	private vector m_vRollTarget;
	
	int closestIndex;
	int newClosestIndex;
	vector m_vDestination;
	
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

		m_fTimeTurnInterval = m_fSpeed / 100;
				
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
				
		//Draw velocity vector
		vector vVel = owner.GetPhysics().GetVelocity();
		float currentSpeed = vVel.Length();

		//Draw chopper direction vector
		vector vDir = owner.GetTransformAxis(2);

		//Destination vector
		vector chopVector = vector.Direction(origin, m_vDestination);
		chopVector.Normalize();		
		chopVector = chopVector * 20;
				
		VehicleHelicopterSimulation m_Vehicle_s = VehicleHelicopterSimulation.Cast(owner.FindComponent(VehicleHelicopterSimulation));		
		if (!m_Vehicle_s)
		{
			Print("SDRC_ChopperComp No VehicleHelicopterSimulation");
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
		
		SetVelocity(owner);

		DrawHelicopterVectors(owner);
						
/*		if (m_fTimeSpeed > SPEED_INTERVAL)
		{
			//SetTurn(owner);
			m_fTimeSpeed = m_fTimeSpeed - SPEED_INTERVAL;
		}*/

		if ( (m_fTimeTurn > m_fTimeTurnInterval) && (m_bDoTurn) )
		{
//			SetVelocity(owner);
			SetTurn(owner, m_fTimeTurnInterval);
//			m_fTimeTurn = m_fTimeTurn - (m_fTimeTurnInterval * 1.1);
			m_fTimeTurn = m_fTimeTurn - m_fTimeTurnInterval * 1.1;
			m_bDoTurn = false;
		
			//Set the chopper YPR			
/*			float roll = ComputeSplineRoll(m_vSplinePoints[closestIndex - 1], m_vSplinePoints[closestIndex + 0], m_vSplinePoints[closestIndex + 1]);
			angles = owner.GetYawPitchRoll();
			angles[2] = roll;
	//		angles[1] = -10;
			owner.SetYawPitchRoll(angles);*/
			
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
		float speedMul = vector.Distance(origin, m_vDestination);
		
		m_iSpeedAvgCount++;
		m_iSpeedAvg = m_iSpeedAvg + ( (speedMul - m_iSpeedAvg) / m_iSpeedAvgCount) ;
		
//		Print("avg:" + m_iSpeedAvg + " / " + speedMul);

/*		vector tangent = m_Spline.GetTangentAt(m_Param).Normalized();				
		vector targetVel = tangent * m_Speed;		
		m_CurrentVelocity = SmoothVelocity(m_CurrentVelocity, targetVel, m_VelocitySmoothFactor);		*/
		
		//Set velocity
		vector chopVector = vector.Direction(origin, m_vDestination);
		chopVector.Normalize();		
		chopVector = chopVector * m_fSpeed * (m_iSpeedAvg / 100);
//		chopVector = chopVector * speed * speedMul;//length;
		owner.GetPhysics().SetVelocity(chopVector);		
	}
	
	//------------------------------------------------------------------------------------------------	
	void SetTurn(IEntity owner, float deltaTime)
	{
		//Calculate roll
		int rollIdxStart = closestIndex - 1;//(m_iDestinationPointAdd / 2);
		if (rollIdxStart < 0)
		{
			rollIdxStart = 0;
		}
		int rollIdxEnd = closestIndex + (m_iDestinationPointAdd / 3);
		if (rollIdxEnd <= closestIndex)
		{
			rollIdxEnd = closestIndex + 1;
		}
		
		float roll = ComputeSplineRoll(m_vSplinePoints[rollIdxStart], m_vSplinePoints[closestIndex], m_vSplinePoints[rollIdxEnd], m_vRollTarget);
//		float roll = ComputeSplineRoll(origin, m_vSplinePoints[closestIndex], m_vSplinePoints[rollIdxEnd]);

		//Get chopper direction
		vector vDir = owner.GetTransformAxis(2);
		vector origin = owner.GetOrigin();
		
		//Count the angle from heli up vs world up
		vector vDir1 = owner.GetTransformAxis(1);// * Math.RAD2DEG;	//Up from chopper (MAGENTA)
		vDir1 = vector.Direction(origin, vDir1);
		vDir1.Normalize();
		vector vUp = vector.Up;// * Math.RAD2DEG;	//Up
		vUp.Normalize();
		vDir1 = vDir1 - vUp;
		Print("diff: " + vDir1[2]);
				
		//Get the heli forward vector
		vector angles = vector.Direction(origin, m_vDestination);
//		vector currAngV = owner.GetPhysics().GetAngularVelocity();						
//		angles = angles + currAngV;

		//Turn the roll towards up
		angles[2] = angles[2] - vDir1[2];
		//Turn the roll according to spline
		angles[2] = angles[2] - (roll * 2);
		angles = ComputeAngularVelocity(vDir, angles, deltaTime);
		
		owner.GetPhysics().SetAngularVelocity(angles);		
	}
	
	//------------------------------------------------------------------------------------------------	
	void DrawHelicopterVectors(IEntity owner)
	{
		vector origin = owner.GetOrigin();
		
		//Planned destination
		DrawLine(origin, m_vSplinePoints[closestIndex], Color.GRAY);		
		
		//Chopper destination direction vector
		vector vFwd = vector.Direction(origin, m_vDestination);
		vFwd.Normalize();
		DrawLine(origin, origin + (vFwd * 10), Color.BLACK);		

		//Draw vectors
		vector vDir2 = owner.GetTransformAxis(2);	//Forward
		DrawLine(origin, origin + (vDir2 * 10), Color.CYAN);		

		vector vDir0 = owner.GetTransformAxis(0);	//Side
		DrawLine(origin, origin + (vDir0 * 10), Color.DARK_CYAN);		

		vector vDir1 = owner.GetTransformAxis(1);	//Up
		DrawLine(origin, origin + (vDir1 * 10), Color.MAGENTA);		
		
		vector vUp = vector.Up;						//World Up
		DrawLine(origin, origin + (vUp * 10), Color.GRAY);		
		
		
		//Roll vector
		vector vRoll = m_vRollTarget;
		vRoll.Normalize();
		DrawLine(origin, origin + (vRoll * 10), Color.BLUE);		
				
		//Draw velocity vector
		vector vVel = owner.GetPhysics().GetVelocity();
		float currentSpeed = vVel.Length();
//		vVel.Normalize();
		DrawLine(origin, origin + (vVel * currentSpeed), Color.GRAY_75);
				
/*		//Draw angle vector
		vector vAng = owner.GetAngles();
		vAng.Normalize();				
		DrawLine(origin, origin + (vAng * 10), Color.GREEN);

		//Draw yaw vector
		vector vYaw = origin.FromYaw(0);
//		vYaw.Normalize();
//		vYaw.Perpend();
		DrawLine(origin, origin + (vYaw * 10), Color.BLUE);		*/		
				
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
	        rollSign = -1.0;

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
			"1500 010 2000",
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