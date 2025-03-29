//Helpers SCR_DC_Misc.c

//------------------------------------------------------------------------------------------------
/*!
Includes various small functions. 
*/

sealed class SCR_DC_Misc
{
	//------------------------------------------------------------------------------------------------
	/*!
	Return the biggest value in a vector. 
	Example: [10 12 8] returns 12
	*/
	static float FindMaxValue(vector vec)
	{
		float outfloat = vec[0];
	
		if (vec[1] > vec [0])
		{
			outfloat = vec[1];
		}

		if (vec[2] > outfloat)
		{
			outfloat = vec[2];
		}

		return outfloat;				
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Find a point on circle with given radius and angle in degrees (0-360)
	*/
    static vector GetCoordinatesOnCircle(vector vec, float radius, float angleInDegrees, float startAngle = 0)
    {
        // Convert the angle to radians
        float angleInRadians = AngleToRadians(startAngle + angleInDegrees);

        // Calculate the coordinates using trigonometric functions
        vec[0] = vec[0] + radius * Math.Cos(angleInRadians);
        vec[2] = vec[2] + radius * Math.Sin(angleInRadians);

		return vec;
    }	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Convert angle in degrees (0-360) to radians 
	*/
	static float AngleToRadians(float angle)
	{
		return (angle) * (Math.PI / 180.0);
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
	Returns the (max) size of the world
	*/	
	static int GetWorldSize()
	{
		int worldSize;
		//TBD: This should work, but returns incorrect size on dedicated server. Check with 1.3 if fixed.
		/*
		SCR_MapEntity m_MapEntity = SCR_MapEntity.GetMapInstance();
		worldSize = m_MapEntity.GetMapSizeX();		
		if (m_MapEntity.GetMapSizeY() > worldSize)
		{
			worldSize = m_MapEntity.GetMapSizeY();
		}*/

		vector mins, maxs;
		GetGame().GetWorld().GetBoundBox(mins, maxs);		
		worldSize = FindMaxValue(maxs);
		SCR_DC_Log.Add("[SCR_DC_Misc:GetWorldSize] Worldsize:" + worldSize, LogLevel.SPAM);
						
		return worldSize;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns the world name being played.
	\param filterGM Shall we remove GM_ from the worldname (GM_Arland -> Arland)
	Example: "path/to/worldfile/Arland.ent" will return "Arland"
	*/	
	static string GetWorldName(bool filterGM = false)
	{
		string worldName = "Unknown";

		worldName = GetGame().GetWorldFile();
		//The name is "path/to/worldfile/Arland.ent". Find the last slash, add one to it and cut the ".ent" from the end.
		int lastslash = worldName.LastIndexOf("/") + 1;
		worldName = worldName.Substring(lastslash, worldName.Length() - lastslash - 4);
		
		if(worldName.StartsWith("GM_"))
		{
			worldName.Replace("GM_", "");
		}
		
		return worldName;
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Find a random spot on the map.
	\param mustBeOnLand If position must be on land
	*/	
	static vector GetRandomWorldPos(bool mustBeOnLand = true)
	{
		int worldSize = SCR_DC_Misc.GetWorldSize();
		vector posTmp = "0 0 0";
		vector pos = "0 0 0";
		
		bool positionFound = false;
		int i = 0;
		
		for (i = 0; i <= 100; i++)
		{
			posTmp[0] = Math.RandomInt(0, worldSize);
			posTmp[2] = Math.RandomInt(0, worldSize);
			
			if (mustBeOnLand)
			{
				if (GetGame().GetWorld().GetOceanHeight(posTmp[0], posTmp[2]) == 0)
				{
					positionFound = true;
					break;
				}
			}
			else
			{
				positionFound = true;
				break;
			}			
		}

		if (positionFound)
		{
			pos = posTmp;			
			pos[1] = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]);
		}		
		
//		SCR_DC_Log.Add("[SCR_DC_MissionHelper:GetRandomWorldPos] Iterations: " + i, LogLevel.SPAM);			
								
		return pos;		
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
	    float angleInRadians = SCR_DC_Misc.AngleToRadians(angle);
	
	    // Calculate the new x and y coordinates
	    pos[0] = pos[0] + distance * Math.Cos(angleInRadians);
	    pos[2] = pos[2] + distance * Math.Sin(angleInRadians);
		
		return pos;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Move given position range meters away from the given position in X/Y.	
	*/	
	static vector RandomizePos(vector position, float range = 100)
	{
		vector posRnd = "0 0 0";
		vector newPos;
		posRnd[0] = Math.RandomFloat(-range, range);
		posRnd[2] = Math.RandomFloat(-range, range);
		
		newPos = position + posRnd;
		
		return newPos;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if two positions are within limit distance
	\param pos0,pos1 Positions to check if they're within limit
	\param limit How close the positions needs to be to return true
	*/	
	static bool IsPosNearPos(vector pos0, vector pos1, float limit = 10)
	{
		float distance = vector.DistanceXZ(pos0, pos1);
		
		if (distance < limit)
		{			
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Check if position is in water.
	*/	
	static bool IsPosInWater(vector pos)
	{
		float waterHeight = GetGame().GetWorld().GetOceanHeight(pos[0], pos[2]);
		
		if (waterHeight == 0)
		{			
			return false;
		}
		
		return true;
	}	
		
	//------------------------------------------------------------------------------------------------
	/*!
	Check if a class is available. This can be used to check if a mod has been loaded by checking a class
	\param classToTest The class to search
	TBD: For some reason uncommenting the code below results in WB crash and weird compilation errors
	*/	
/*	static bool IsClassAvailable(string classToTest)
	{
		string s = String(classToTest);
		typename var = s.ToType();
		if(var)
		{
			return true;
		}
		return false;
	}*/
}