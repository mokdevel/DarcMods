//Helpers SDRC_Math.c

//------------------------------------------------------------------------------------------------
/*!
Includes various math/vector functions. 
*/

sealed class SDRC_Math
{
	//------------------------------------------------------------------------------------------------	
	// Vector math helpers
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	//\param angle in radians with sign
	static float GetAngleBetweenVectors(vector v1, vector v2)
	{
	    // Normalize both vectors
	    vector a = v1.Normalized();
	    vector b = v2.Normalized();
	
	    // Dot product
	    float dot = vector.Dot(a, b);
	
		// Cross product to get the sign
	    vector axis = a * b;
	    float sign = 1.0;
	    if (axis[1] < 0.0)
		{
	        sign = -1.0;
		}
		
	    // Clamp dot to avoid NaN from floating-point errors
	    dot = Math.Clamp(dot, -1.0, 1.0);
	
	    // Return angle in radians
	    return Math.Acos(dot) * sign;
	}	

	//------------------------------------------------------------------------------------------------	
	/*!	
	Get angle between (p1, p0) and (p1, p2).
	
		Find the angle A. 
		
 		 (p2)  dir1
			*<-----. (p1)
			     \_|
			     A | dir0
	               V
			       * (p0)
	*/	
	static float GetAngleBetweenThreePoints(vector p0, vector p1, vector p2, out vector dir0 = "0 0 0", out vector dir1 = "0 0 0")
	{
		//Use only ZX plane
		p0[1] = 0;
		p1[1] = 0;
		p2[1] = 0;
		//Current direction		
		dir0 = vector.Direction(p1, p0);
		//Direction towards destination
		dir1 = vector.Direction(p1, p2);
		float angle = SDRC_Math.GetAngleBetweenVectors(dir0, dir1) * Math.RAD2DEG;
		
		return angle;
	}	
			
	//------------------------------------------------------------------------------------------------	
	//\returns AngVel in radians
	static vector ComputeAngularVelocity(vector v1, vector v2, float deltaTime)
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
	
	    vector angVel = axis.Normalized() * (angle / deltaTime);
	    return angVel;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Rotate v1 around axis
	
	\param v1
	\param axis
	\param radians How many radians to turn
	*/
	static vector RotateAroundAxis(vector v1, vector axis, float radians)
	{
	    axis = axis.Normalized();
	    
	    float cosT = Math.Cos(radians);
	    float sinT = Math.Sin(radians);
	    
	    vector term1 = v1 * cosT;
	    vector term2 = (axis * v1) * sinT;
	    vector term3 = axis * vector.Dot(axis, v1) * (1.0 - cosT);
	    
	    return term1 + term2 + term3;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Convert a vector to angle in degrees (0-360) to radians 
	*/
	static float VectorToAngle(vector direction)
	{
    	float angle = Math.Atan2(direction[0], direction[2]);
		//TBD: Might be same as *RAD2DEG
    	float degrees = (180 * angle / Math.PI);
    	return degrees;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Convert a vector to angle in radians
	*/
	static float VectorToRadians(vector direction)
	{
    	float angle = Math.Atan2(direction[0], direction[2]);
    	return angle;
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Returns a position that has moved given distance along an angle from given position.
	\param pos Original position
	\param distance Distance to move
	\param angle Move towards this angle (degrees, 0-360).
	*/
	static vector MovePosToAngle(vector pos, float distance, float angle)
	{
	    float angleInRadians = SDRC_Misc.AngleToRadians(angle);
	
	    // Calculate the new x and y coordinates
	    pos[0] = pos[0] + distance * Math.Cos(angleInRadians);
	    pos[2] = pos[2] + distance * Math.Sin(angleInRadians);
		
		return pos;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get transform from the given position and rotation in XZ plane
	*/
	static void GetTransformFromPosAndRot(out vector transform[4], vector pos, float rotation, bool snap = true)
	{
		Math3D.MatrixIdentity3(transform);
		Math3D.AnglesToMatrix(Vector(rotation, 0, 0), transform);
		transform[3] = pos;
		if (snap)
		{
			SCR_TerrainHelper.SnapAndOrientToTerrain(transform);
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Rotate a position around a pivot point
	*/
	static vector RotatePosAroundPivot(vector pos, vector pivot, float rotation)
	{
		vector transform[4];
		vector newtransform[4];

		Math3D.MatrixIdentity3(transform);
		transform[3] = pos;
						
		SCR_Math3D.RotateAround(transform, pivot, "0 1 0", SDRC_Misc.AngleToRadians(rotation), newtransform);			
		
		return newtransform[3];		
	}		
}