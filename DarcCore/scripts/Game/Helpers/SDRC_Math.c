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
	//\param angle in radians with sign
	static float GetAngleBetweenVectorsXZ(vector v1, vector v2)
	{
		//Set Y to zero to set the XZ plane
		v1[1] = 0;
		v2[1] = 0;
		
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
	static float GetRadiansBetweenThreePointsXZ(vector p0, vector p1, vector p2, out vector dir0 = vector.Zero, out vector dir1 = vector.Zero)
	{
		//Use only XZ plane
		p0[1] = 0;
		p1[1] = 0;
		p2[1] = 0;
		//Current direction		
		dir0 = vector.Direction(p1, p0);
		//Direction towards destination
		dir1 = vector.Direction(p1, p2);
		float angle = SDRC_Math.GetAngleBetweenVectors(dir0, dir1);
		
		return angle;
	}	
				

	//------------------------------------------------------------------------------------------------	
	/*!	
	Check if point p2 is on the left side of vector p0-p1
	*/
	static float IsPointOnLeft(vector p0, vector p1, vector p2)
	{
		float side = (p1[0] - p0[0]) * (p2[2] - p0[2])
		           - (p1[2] - p0[2]) * (p2[0] - p0[0]);	
		
		//Are we on left side?
		if (side > 0)
		{
			return true;
		}
		
		return false;	
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
	Checks if target is inside a forward-facing cone

	\param position Position from where to check
	\param forward Forward vector of position. Must be normalized!
	\param targetPos Position of target
	\param maxAngleDeg half-angle of the cone
	*/
	static bool IsTargetInSector(vector position, vector forward, vector targetPos, float maxAngleDeg
	)
	{
	    // Direction from heli to target
	    vector toTarget = targetPos - position;
	
	    float dist = toTarget.Length();
		
	    if (dist < 0.0001)
	        return true; // target is on us
	
	    toTarget.Normalize();
	
	    // Dot product
	    float dot = vector.Dot(forward, toTarget);
	
	    // Convert angle to cosine
	    float cosLimit = Math.Cos(maxAngleDeg * Math.DEG2RAD);
	
	    return dot >= cosLimit;
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Checks if target is in line of sight

	\param traceStartPos Where to start the trace
	\param traceEndPos Where to start the trace
	\param owner The entity to skip for the trace. In case it's a big one, the trace may hit it.
	\return position where enemy was found. vector.Zero returned if no enemies found.	
	*/
	static bool IsTargetInLos(vector traceStartPos, vector traceEndPos, IEntity owner = null)
	{
		bool inLos = true;
		
		TraceParam param = new TraceParam();
		{					
			param.Start = traceStartPos;
			param.End = traceEndPos;
			param.Exclude = owner;
			//param.Flags = TraceFlags.DEFAULT | TraceFlags.ENTS | TraceFlags.ANY_CONTACT;
			param.Flags = TraceFlags.DEFAULT | TraceFlags.ANY_CONTACT;
			param.LayerMask = EPhysicsLayerDefs.Projectile;
		}					
	
		BaseWorld world = GetGame().GetWorld();
		float traceDistance = world.TraceMove(param, null);
		//float traceDistance = world.TracePosition(param, null);
		
		if (traceDistance > 0.99)
		{
			inLos = true;
			SDRC_Log.Add("[SDRC_Math:doLosTrace] target in los: " + traceEndPos, LogLevel.DEBUG);
		}	
		
		return inLos;
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

	//------------------------------------------------------------------------------------------------	
	/*!
	Turn the entity towards target
	*/
	static void TurnEntityTowards(IEntity owner, vector target)
	{
		vector p0 = owner.GetOrigin();
		vector angles = vector.Direction(p0, target);
		angles.Normalize();
		angles = angles.VectorToAngles();
		owner.SetYawPitchRoll(angles);		
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!
	Turn the entity towards target on XZ plane
	*/
	static void TurnEntityTowardsXZ(IEntity owner, vector target)
	{
		vector p0 = owner.GetOrigin();
		p0[1] = 10;
		target[1] = 10;		
		vector angles = vector.Direction(p0, target);
		angles.Normalize();
		angles = angles.VectorToAngles();
		owner.SetYawPitchRoll(angles);		
	}	
	
	//------------------------------------------------------------------------------------------------
	// Half bell curve (Gaussian-based)
	// t in range [0..1]
	// NOTE: float y = Math.Sin(Math.Sin(t * Math.PI * 0.5) * Math.PI * 0.5); would work for t in range [-1..1]
	static float HalfBell(float t)
	{
		t = t * 2 - 1;
		float y = (1 + Math.Sin(Math.Sin(t * Math.PI * 0.5) * Math.PI * 0.5)) / 2;
		return y;
	}
	
	//------------------------------------------------------------------------------------------------
	// Full bell curve (Gaussian-based)
	// t in range [0..1]
	static float FullBell(float t)
	{
//		t = t * 2 - 1;
		
//		float y = (1 + Math.Sin(Math.Sin(t * Math.PI) * Math.PI * 0.5)) / 2;
		float y = Math.Sin(Math.Sin(t * Math.PI) * Math.PI * 0.5);
		return y;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Create a distance offset point p2 between p0 and p1.
	
			x = distanceMid aka the point between p0 and p1. 0.5 is in the middle, 1.0 at the end
			d = distance
		
			* (p1)
			|
			| 	 	 
			x---d---> output vector (p2) 
			|
			|
			* (p0)
	*/
	static vector CreateOffsetMidPoint(vector p0, vector p1, float distance, float distanceMid = 0.5, bool leftSide = true)
	{
	    //Midpoint
		vector mid = vector.Lerp(p0, p1, distanceMid);
	
	    //Tangent direction
	    vector tangent = p1 - p0;
	    tangent.Normalize();
	
	    //Perpendicular (side) direction
	    vector side = SCR_Math3D.Cross(vector.Up, tangent);
	    side.Normalize();
	
	    //Choose left or right
	    if (!leftSide)
	    {
	        side = side * -1.0;
	    }
	
		//SDRC_DebugHelper.AddDebugPos(mid, ARGB(255, 0, 0, 255), 1.0, "", 140);
		
	    //Offset midpoint
	    vector p2 = mid + side * distance;
	
	    return p2;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Get rotation vector for two points
	*/
	static vector GetRotationVector(vector fromDir, vector toDir)
	{
	    fromDir.Normalize();
	    toDir.Normalize();
	
	    vector axis = SCR_Math3D.Cross(fromDir, toDir);
	    float sinAngle = axis.Length();
	    float cosAngle = vector.Dot(fromDir, toDir);
	
	    if (sinAngle < 0.0001)
	        return "0 0 0"; // no rotation
	
	    axis.Normalize();
	    float angle = Math.Atan2(sinAngle, cosAngle);
	
	    // Rotation vector = axis * angle (radians)
	    return axis * angle;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Check if a moving point has passed target position in XZ plane
	*/
	static bool HasPassedPointXZ(vector startPos, vector targetPos, vector currentPos)
	{
	    // Movement direction
	    vector dir = targetPos - startPos;
	    dir[1] = 0;
	
	    // Vector from target to current position
	    vector after = currentPos - targetPos;
	    after[1] = 0;
	
	    float d = vector.Dot(dir, after);
	
	    if (d > 0)
	        return true;
	
	    return false;
	}	
}