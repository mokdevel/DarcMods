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
}
