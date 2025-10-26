//Helpers SDRC_Spline.c

//------------------------------------------------------------------------------------------------
/*!
Includes various small functions. 
*/

sealed class SDRC_Spline3D
{
	// Generates a list of points along a Catmull–Rom spline
	static void GenerateSplinePoints(notnull array<vector> controlPoints, out array<vector> result, int samplesPerSegment = 20)
	{
		result.Clear();

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

			// Sample points between p1 and p2
			for (int j = 0; j < samplesPerSegment; j++)
			{
				float t = j / (float)samplesPerSegment;
				vector point = CatmullRom(p0, p1, p2, p3, t);
				result.Insert(point);
			}
		}

		// Add final control point
		result.Insert(controlPoints[count - 1]);
	}

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
	
	static void TestSpline()
	{
		array<vector> ctrl = {
			"000 000 000",
			"200 100 300",
			"400 400 1600",
			"700 150 900"
		};
	
		array<vector> splinePoints = new array<vector>();
		SDRC_Spline3D.GenerateSplinePoints(ctrl, splinePoints, 20);
		
		foreach (vector pos : splinePoints)
		{
			Print(pos);
			pos = pos + SDRC_Misc.GetWorldCenter();
			SDRC_DebugHelper.AddDebugSphere(pos, ARGB(40, 192, 192, 192), 10);			//Gray
		}
	}	
}