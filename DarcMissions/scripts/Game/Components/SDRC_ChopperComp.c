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
	const float SPEED_INTERVAL = 0.2;		
	const float TURN_INTERVAL = 0.5;		
	private float m_fTimeSpeed = 0;
	private float m_fTimeTurn = 0;
	private float m_fSpeed = 12;
	private int m_iSplinePoints = 12;
	private int m_iDestinationPointAdd = m_fSpeed / 3;
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
		
		SetVelocity(owner);
		SetTurn(owner, TURN_INTERVAL);
		
		
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
		m_fTimeSpeed += timeSlice;
		m_fTimeTurn += timeSlice;

		vector origin = owner.GetOrigin();
				
		//Draw velocity vector
		vector vVel = owner.GetPhysics().GetVelocity();
		float currentSpeed = vVel.Length();
//		vVel.Normalize();
//		DrawLine(origin, origin + (vVel * currentSpeed), Color.PINK);
				
/*		//Draw angle vector
		vector vAng = owner.GetAngles();
		vAng.Normalize();				
		DrawLine(origin, origin + (vAng * 10), Color.GREEN);

		//Draw yaw vector
		vector vYaw = origin.FromYaw(0);
//		vYaw.Normalize();
//		vYaw.Perpend();
		DrawLine(origin, origin + (vYaw * 10), Color.BLUE);		*/

		//Draw chopper direction vector
		vector vDir = owner.GetTransformAxis(2);
		DrawLine(origin, origin + (vDir * 10), Color.WHITE);		

		//Destination vector
		vector chopVector = vector.Direction(origin, m_vDestination);
		chopVector.Normalize();		
		chopVector = chopVector * 20;
		DrawLine(origin, m_vSplinePoints[closestIndex], Color.GRAY);		
		
		//Chopper destination direction vector
		vector angles = vector.Direction(origin, m_vDestination);
		angles.Normalize();
		DrawLine(origin, origin + (angles * 10), Color.BLACK);		
		
		VehicleHelicopterSimulation m_Vehicle_s = VehicleHelicopterSimulation.Cast(owner.FindComponent(VehicleHelicopterSimulation));		
		if (!m_Vehicle_s)
		{
			Print("SDRC_ChopperComp No VehicleHelicopterSimulation");
		}

		//Draw where we are planning to go
		float distance = GetDistanceFromSpline(m_vSplinePoints, origin, newClosestIndex);		
		
		closestIndex = newClosestIndex + m_iDestinationPointAdd;
		
/*		if ( (distance < 40) && (newClosestIndex > (closestIndex + 3) ) )
		{
			if (newClosestIndex > closestIndex)
			{
				closestIndex = newClosestIndex;
				closestIndex++;
				closestIndex++;
				closestIndex++;
			}
		}

		if (newClosestIndex == closestIndex)
		{
			closestIndex++;
			closestIndex++;
			closestIndex++;
		}*/
		
		m_vDestination = m_vSplinePoints[closestIndex];
		
//		owner.GetPhysics().SetVelocity(chopVector);
//		vVel = owner.GetPhysics().GetVelocity();
//		owner.GetPhysics().SetVelocity(vVel);		

		bool bDoTurn = false;

		SetVelocity(owner);
				
		if (m_fTimeSpeed > SPEED_INTERVAL)
		{
//			SetVelocity(owner);
			//bDoTurn = true;
			//SetTurn(owner);
			m_fTimeSpeed = m_fTimeSpeed - SPEED_INTERVAL;
		}

		if ( (m_fTimeTurn > TURN_INTERVAL) || (bDoTurn) )
		{
//			SetVelocity(owner);
			SetTurn(owner, m_fTimeTurn);
			m_fTimeTurn = m_fTimeTurn - TURN_INTERVAL;
		
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
		float speed = vector.Distance(origin, m_vDestination);
		float speedMul = 1.0;

/*		vector tangent = m_Spline.GetTangentAt(m_Param).Normalized();				
		vector targetVel = tangent * m_Speed;		
		m_CurrentVelocity = SmoothVelocity(m_CurrentVelocity, targetVel, m_VelocitySmoothFactor);		*/
		
		//Set velocity
		vector chopVector = vector.Direction(origin, m_vDestination);
		chopVector.Normalize();		
		chopVector = chopVector * m_fSpeed;
//		chopVector = chopVector * speed * speedMul;//length;
		owner.GetPhysics().SetVelocity(chopVector);		
	}
	
	//------------------------------------------------------------------------------------------------	
	void SetTurn(IEntity owner, float deltaTime)
	{
		//Get chopper direction
		vector vDir = owner.GetTransformAxis(2);
		vector origin = owner.GetOrigin();
		
		//Turn the chopper
		vector angles = vector.Direction(origin, m_vDestination);
		angles = ComputeAngularVelocity(vDir, angles, deltaTime);
		
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
		
		float roll = ComputeSplineRoll(m_vSplinePoints[rollIdxStart], m_vSplinePoints[closestIndex], m_vSplinePoints[rollIdxEnd]);
		angles[2] = angles[2] - roll;
		
		owner.GetPhysics().SetAngularVelocity(angles);		
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
	    vector a = v1.Normalized();
	    vector b = v2.Normalized();
	
	    vector axis = a * b; // cross product (rotation axis)
	    float dot = vector.Dot(a, b);
	    dot = Math.Clamp(dot, -1.0, 1.0);
	
	    float angle = Math.Acos(dot); // radians between directions
	
	    // avoid division by zero
	    if (Math.AbsFloat(angle) < 0.0001)
	        return "0 0 0";
	
	    vector angVel = axis.Normalized() * (angle / deltaTime);
	    return angVel;
	}	
	
	//------------------------------------------------------------------------------------------------
	float ComputeSplineRoll(vector p0, vector p1, vector p2)
	{
	    // Compute forward tangents between points
	    vector t0 = (p1 - p0).Normalized();
	    vector t1 = (p2 - p1).Normalized();
	
	    // Compute curve axis (cross product)
	    vector axis = t0 * t1;
	    float axisLen = axis.Length();
	
	    // Handle nearly straight segments
	    if (axisLen < 0.0001)
	        return 0.0;
	
	    // Compute angle between tangents
//	    float dot = Math.Clamp(vector.Dot(t0,t1), -1.0, 1.0);
	    float dot = vector.Dot(t0, t1);
	    dot = Math.Clamp(dot, -1.0, 1.0);
	    float angle = Math.Acos(dot);
	
	    // Determine roll direction (sign)
	    float rollSign = 1.0;
	    if (axis[1] < 0.0)
	        rollSign = -1.0;
	
	    // Convert to degrees and apply sign
	    float rollDeg = rollSign * angle;// * Math.RAD2DEG;
	    return rollDeg;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	
	*/
	void InitFlightPath()	
	{
		array<vector> pathPoints = {
			"0000 010 000",
			"0060 040 100",
			"0030 050 200",
			"0100 030 240",
			"0200 030 160",
			"0220 030 140",
			"0120 020 080",
		};
			
		SDRC_Spline3D.GenerateSplinePoints(pathPoints, m_vSplinePoints, m_vTangentPoints, m_iSplinePoints, true);
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