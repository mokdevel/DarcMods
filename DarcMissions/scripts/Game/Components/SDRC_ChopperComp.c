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
	const float MEASURE_INTERVAL = 1.0;		
	private float m_fTime = 0;
	int closestIndex = 1;
	int newClosestIndex = 1;
	vector m_vDestination;
	
	override void OnPostInit(IEntity owner)
	{
		SDRC_Log.Add("[SDRC_ChopperComp] Starting SDRC_ChopperComp", LogLevel.NORMAL);
		s_Instance = this;				
		SetEventMask(owner, EntityEvent.FRAME | EntityEvent.POSTFRAME);
		Activate(owner);
		
		InitFlightPath();		
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
		m_fTime += timeSlice;

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
		chopVector = chopVector * 10;
		DrawLine(origin, m_vSplinePoints[closestIndex], Color.GRAY);		
		
		//Chopper direction vector
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
		
		if ( (distance < 20) && (newClosestIndex == (closestIndex + 1) ) )
		{
			if (newClosestIndex > closestIndex)
			{
				closestIndex = newClosestIndex;
				closestIndex++;
			}
		}

		if (newClosestIndex == closestIndex)
		{
			closestIndex++;
		}
		
		m_vDestination = m_vSplinePoints[closestIndex];
		
		
		//Turn the chopper
/*		angles = vector.Direction(origin, m_vDestination);
		angles.Normalize();
		angles = angles.VectorToAngles();
		owner.SetYawPitchRoll(angles);			*/
		
//		owner.GetPhysics().SetVelocity(chopVector);
//		vVel = owner.GetPhysics().GetVelocity();
//		owner.GetPhysics().SetVelocity(vVel);
		
		
		if (m_fTime < MEASURE_INTERVAL)
		{
			return;
		}
		
		m_fTime = 0;

		//Set velocity
		chopVector = vector.Direction(origin, m_vDestination);
		chopVector.Normalize();		
		chopVector = chopVector * 30;//length;
		owner.GetPhysics().SetVelocity(chopVector);
						
		//Turn the chopper
		angles = vector.Direction(origin, m_vDestination);
		angles = ComputeAngularVelocity(vDir, angles, 0.5);
		owner.GetPhysics().SetAngularVelocity(angles);

		//Turn the chopper
		angles = owner.GetYawPitchRoll();
		angles[2] = 0;
		owner.SetYawPitchRoll(angles);
		
		float length = vector.Distance(m_vSplinePoints[closestIndex + 0], m_vSplinePoints[closestIndex + 1]);
		
		
/* From: SCR_HelicopterCinematicFlyComponent
		vector velOrig = GetOwner().GetPhysics().GetVelocity();
		vector rotVector = GetOwner().GetAngles();
		vector vel = {velOrig[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * forceMultiplier, velOrig[1], velOrig[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * forceMultiplier };
		GetOwner().GetPhysics().SetVelocity(vel);				
*/		
	}

	void DrawLine(vector p0, vector p1, int color = Color.RED)
	{
		int shapeFlags = ShapeFlags.ONCE;
		vector p[2];
		p[0] = p0;
		p[1] = p1;		
		Shape.CreateLines(color, shapeFlags, p, 2);		
	}
	
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
	/*!	
	
	*/
	void InitFlightPath()	
	{
		array<vector> pathPoints = {
			"0000 050 000",
			"0200 020 100",
			"0300 060 400",
			"0100 080 200",
			"0300 020 250",
		};
			
		SDRC_Spline3D.GenerateSplinePoints(pathPoints, m_vSplinePoints, m_vTangentPoints, 8, true);
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