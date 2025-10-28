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
		
		if (m_fTime < MEASURE_INTERVAL)
		{
			return;
		}
		
		m_fTime = 0;
		
		static int closestIndex;
		
		VehicleHelicopterSimulation m_Vehicle_s = VehicleHelicopterSimulation.Cast(owner.FindComponent(VehicleHelicopterSimulation));
		
		if (!m_Vehicle_s)
		{
			Print("SDRC_ChopperComp No VehicleHelicopterSimulation");
		}

		vector origin = owner.GetOrigin();
				
		//Draw velicity vector
		vector vVel = owner.GetPhysics().GetVelocity();
		float currentSpeed = vVel.Length();
//		vVel.Normalize();
		DrawLine(origin, origin - (vVel * currentSpeed), Color.RED);
				
		//Draw angle vector
//		vector vAng = owner.GetAngles();
//		vAng.Normalize();				
//		DrawLine(origin, origin + (vAng * 30), Color.GREEN);
		
		//Draw yaw vector
		vector vYaw = origin.FromYaw(0);
		vYaw.Normalize();
		vYaw.Perpend();
		DrawLine(origin, origin + (vYaw * 30), Color.BLUE);
		
		//Draw where we are planning to go
		int newClosestIndex = -1;
		float distance = GetDistanceFromSpline(m_vSplinePoints, origin, newClosestIndex);		
		
		if (newClosestIndex > closestIndex)		
		{
			closestIndex = newClosestIndex;
		}

		vector rotVector m_vTangentPoints[closestIndex];
		
		//Set velocity
		vector chopVector = vector.Direction(origin, m_vSplinePoints[closestIndex]);
		chopVector.Normalize();		
		chopVector = chopVector * 30;
		owner.GetPhysics().SetVelocity(chopVector);
		DrawLine(origin, m_vSplinePoints[closestIndex], Color.GRAY);
		
		//Turn the chopper
		vector angles = vector.Direction(origin, m_vSplinePoints[closestIndex]);
		angles.Normalize();
		DrawLine(origin, origin + (angles * 10), Color.BLACK);
		angles = angles.VectorToAngles();
		owner.SetYawPitchRoll(angles);
	}

	void DrawLine(vector p0, vector p1, int color = Color.RED)
	{
		int shapeFlags = ShapeFlags.ONCE;
		vector p[2];
		p[0] = p0;
		p[1] = p1;		
		Shape.CreateLines(color, shapeFlags, p, 2);		
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
			
		SDRC_Spline3D.GenerateSplinePoints(pathPoints, m_vSplinePoints, m_vTangentPoints, 6);
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