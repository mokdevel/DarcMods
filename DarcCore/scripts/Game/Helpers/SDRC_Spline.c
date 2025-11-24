//Helpers SDRC_Spline.c

//------------------------------------------------------------------------------------------------
/*!
Includes various small functions. 
*/

sealed class SDRC_Spline3D
{
	// Generates a list of points along a Catmull–Rom spline
	// Generates points and tangents along a Catmull–Rom spline
	static void GenerateSplinePoints(notnull array<vector> controlPoints, out array<vector> resultPoints, out array<vector> resultTangents, int samplesPerSegment = 20, bool showPath = false)
	{
		resultPoints.Clear();
		resultTangents.Clear();

		int count = controlPoints.Count();
		if (count < 3)
		{
			Print("Need at least 3 control points to create a spline.");
			return;
		}

		for (int i = 0; i < count - 1; i++)
		{
			vector p0;
			vector p1;
			vector p2;
			vector p3;

			// Handle start and end edges
			if (i == 0)
				p0 = controlPoints[i];
			else
				p0 = controlPoints[i - 1];

			p1 = controlPoints[i];
			p2 = controlPoints[i + 1];

			if (i + 2 < count)
				p3 = controlPoints[i + 2];
			else
				p3 = controlPoints[i + 1];

			// Sample between p1 and p2
			for (int j = 0; j < samplesPerSegment; j++)
			{
				float t = j / (float)samplesPerSegment;

				vector point = CatmullRom(p0, p1, p2, p3, t);
				vector tangent = CatmullRomTangent(p0, p1, p2, p3, t).Normalized();

				resultPoints.Insert(point);
				resultTangents.Insert(tangent);
			}
		}

		// Add final control point and tangent
		vector lastPoint = controlPoints[count - 1];
		vector lastTangent = (lastPoint - controlPoints[count - 2]).Normalized();

		resultPoints.Insert(lastPoint);
		resultTangents.Insert(lastTangent);
		
		if (showPath)
		{		
			foreach (int i, vector pos : resultPoints)
			{
				SDRC_DebugHelper.AddDebugSphere(pos, ARGB(10, 128, 64, 64), 0.5);			//Red
				
				if (i < (resultPoints.Count() - 2))
				{
					//Show direction vector
					vector direction = vector.Direction(resultPoints[i], resultPoints[i+1]);
					direction.Normalize();
					pos = resultPoints[i] + (direction * 10);
					SDRC_DebugHelper.AddDebugLine(resultPoints[i], pos, ARGB(40, 64, 64, 64));				
						
					//Show tangent
					direction = resultTangents[i];
					direction.Normalize();
					pos = resultPoints[i] + (direction * 10);
					SDRC_DebugHelper.AddDebugLine(resultPoints[i], pos, ARGB(40, 32, 32, 32));
				}			
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	// Calculate a Catmull–Rom position for t in [0,1]
	static vector CatmullRom(vector p0, vector p1, vector p2, vector p3, float t)
	{
		float t2 = t * t;
		float t3 = t2 * t;

		vector term1 = p1 * 2.0;
		vector term2 = (p2 - p0) * t;
		vector term3 = (p0 * 2.0 - p1 * 5.0 + p2 * 4.0 - p3) * t2;
		vector term4 = (-p0 + p1 * 3.0 - p2 * 3.0 + p3) * t3;

		return (term1 + term2 + term3 + term4) * 0.5;
	}
	
	//------------------------------------------------------------------------------------------------
	// Calculate the tangent (first derivative of Catmull–Rom)
	static vector CatmullRomTangent(vector p0, vector p1, vector p2, vector p3, float t)
	{
		float t2 = t * t;

		vector term1 = (p2 - p0);
		vector term2 = (p0 * 2.0 - p1 * 5.0 + p2 * 4.0 - p3) * (2.0 * t);
		vector term3 = (-p0 + p1 * 3.0 - p2 * 3.0 + p3) * (3.0 * t2);

		return (term1 + term2 + term3) * 0.5;
	}	

	//------------------------------------------------------------------------------------------------
	// \return degrees of roll
	// \return vector Axis of the roll
	static float ComputeSplineRoll(vector p0, vector p1, vector p2, out vector axis)
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
		
	//------------------------------------------------------------------------------------------------
	static void TestSpline()
	{
		array<vector> ctrl = {
			"300 100 1000",
			"800 300 1600",
			"1200 150 900",
			"300 150 500",
		};
	
		array<vector> splinePoints = new array<vector>();
		array<vector> resultTangents = new array<vector>();
		SDRC_Spline3D.GenerateSplinePoints(ctrl, splinePoints, resultTangents, 20);
		
/*		foreach (vector pos : splinePoints)
		{
			Print(pos);
//			pos = pos + SDRC_Misc.GetWorldCenter();
			SDRC_DebugHelper.AddDebugSphere(pos, ARGB(40, 255, 32, 32), 6);			//Red
		}*/
	}
	
	//------------------------------------------------------------------------------------------------
	ref static array<vector> m_ctrl = {
		"300 100 1000",
		"800 300 1600",
		"1200 150 900",
	};

	ref static array<vector> m_ctrlAdds = {
		"300 150 500",
		"600 50 800",
		"1000 150 1200",
		"700 100 900",
		"10 50 500",
	};		
	
	//------------------------------------------------------------------------------------------------
	static void TestSplineAdd(bool showDebug = true)
	{
		static int idx = 0;

		if (idx > m_ctrlAdds.Count() - 1)
		{
			return;
		}
				
		array<vector> splinePoints = new array<vector>();
		array<vector> resultTangents = new array<vector>();
		splinePoints.Clear();
		resultTangents.Clear();		
		SDRC_Spline3D.GenerateSplinePoints(m_ctrl, splinePoints, resultTangents, 20);
		
		//Prepare the next round
		int idx0 = SDRC_Misc.RandomInt((splinePoints.Count() / 2), splinePoints.Count() - 1);
		m_ctrl.Clear();
		m_ctrl.Insert(splinePoints[idx0]);
		m_ctrl.Insert(splinePoints[splinePoints.Count() - 1]);
		m_ctrl.Insert(m_ctrlAdds[idx]);
		idx++;
		
		if (showDebug)
		{
			foreach (vector pos : splinePoints)
			{
//				Print(pos);
//				pos = pos + SDRC_Misc.GetWorldCenter();
				SDRC_DebugHelper.AddDebugSphere(pos, ARGB(40, 255, 32, 32), 6);			//Red
			}
		}
	}	
}
