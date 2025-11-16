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
	//TBD: Might be same as *RAD2DEG
	*/
	static float VectorToAngle(vector direction)
	{
    	float angle = Math.Atan2(direction[0], direction[2]);
    	float degrees = (180 * angle / Math.PI);
    	return degrees;
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