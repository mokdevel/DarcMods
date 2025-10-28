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
				SDRC_DebugHelper.AddDebugSphere(pos, ARGB(40, 255, 32, 32), 3);			//Red
				
				if (i < (resultPoints.Count() - 2))
				{
					//Show direction vector
					vector direction = vector.Direction(resultPoints[i], resultPoints[i+1]);
					direction.Normalize();
					pos = resultPoints[i] + (direction * 30);
					SDRC_DebugHelper.AddDebugLine(resultPoints[i], pos, ARGB(40, 32, 255, 32));					//Green
	//				SDRC_DebugHelper.AddDebugLine(resultPoints[i], resultPoints[i+1], ARGB(40, 32, 255, 32));	//Green				
						
					//Show tangent
					direction = resultTangents[i];
					direction.Normalize();
					pos = resultPoints[i] + (direction * 40);
					SDRC_DebugHelper.AddDebugLine(resultPoints[i], pos, ARGB(40, 255, 0, 255));					//Purple
				}			
			}
		}
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
	
	// Calculate the tangent (first derivative of Catmull–Rom)
	static vector CatmullRomTangent(vector p0, vector p1, vector p2, vector p3, float t)
	{
		float t2 = t * t;

		vector term1 = (p2 - p0);
		vector term2 = (p0 * 2.0 - p1 * 5.0 + p2 * 4.0 - p3) * (2.0 * t);
		vector term3 = (-p0 + p1 * 3.0 - p2 * 3.0 + p3) * (3.0 * t2);

		return (term1 + term2 + term3) * 0.5;
	}	
	
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

class SplineHelpers
{
	// Cross product (component-wise)
	static vector Cross(vector a, vector b)
	{
		// component access using .x .y .z
		return Vector(a[1] * b[2] - a[2] * b[1],
		              a[2] * b[0] - a[0] * b[2],
		              a[0] * b[1] - a[1] * b[0]);
	}

	// Safe normalization (returns zero vector if input is near zero)
	static vector SafeNormalize(vector v)
	{
		float len = Math.Sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		if (len <= 0.000001) return "0 0 0";
		return v / len;
	}

	// Convert a direction (forward) and up vector to Euler angles (pitch, roll, yaw) in degrees.
	// - forwardDir should be a non-zero direction (it will be normalized).
	// - upVec is usually world up, e.g. "0 1 0".
	// Returns vector(pitch, roll, yaw) in degrees.
	static vector DirectionToAngles(vector forwardDir, vector upVec)
	{
		// constants
		const float RAD_TO_DEG = 180.0 / Math.PI;

		// normalize forward and up
		vector forward = SafeNormalize(forwardDir);
		vector up = SafeNormalize(upVec);

		// if forward is zero, return zero angles
		if (forward == "0 0 0")
			return "0 0 0";

		// Build orthonormal basis: forward, right, trueUp
		// right = cross(up, forward)  (this choice gives right pointing to entity's right-hand side)
		vector right = SafeNormalize(Cross(up, forward));

		// If up was parallel to forward, right may be zero; pick a fallback up (world Y)
		if (right == "0 0 0")
		{
			// try using world Y as up fallback
			vector worldUp = "0 1 0";
			right = SafeNormalize(Cross(worldUp, forward));

			// if still zero, use X axis fallback
			if (right == "0 0 0")
				right = SafeNormalize(Cross("1 0 0", forward));
		}

		// trueUp orthogonal to forward and right
		vector trueUp = Cross(forward, right); // already orthogonal, no need to normalize

		// Yaw: rotation around world up to align forward's projection on XZ plane
		// Use atan2(x, z) so yaw=0 when forward.z is positive (forward along +Z)
		float yaw = Math.Atan2(forward[0], forward[2]) * RAD_TO_DEG; // degrees

		// Pitch: rotation around right axis (tilt up/down)
		// Positive pitch -> looking upward (forward.y positive)
		float flatLen = Math.Sqrt(forward[0] * forward[0] + forward[2] * forward[2]); // length of projection onto XZ
		float pitch = Math.Atan2(forward[1], flatLen) * RAD_TO_DEG;

		// Roll: rotation around forward axis (bank). Compute how up vector aligns with trueUp/right.
		// Use atan2( dot(right, up), dot(trueUp, up) ) to get signed roll
		float dotRightUp = right[0] * up[0] + right[1] * up[1] + right[2] * up[2];
		float dotTrueUpUp = trueUp[0] * up[0] + trueUp[1] * up[1] + trueUp[2] * up[2];

		float roll = Math.Atan2(dotRightUp, dotTrueUpUp) * RAD_TO_DEG;

		// Final angles vector: (pitch, roll, yaw) — matches SetAngles expectation in many Enfusion projects
		return Vector(pitch, roll, yaw);
	}
};