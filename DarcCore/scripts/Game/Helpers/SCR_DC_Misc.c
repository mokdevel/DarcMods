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
//        float angleInRadians = (startAngle + angleInDegrees) * (Math.PI / 180.0);
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
	static float AngleToRadians(float angleInRadians)
	{
		return (angleInRadians) * (Math.PI / 180.0);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Returns the (max) size of the world
	*/	
	static int GetWorldSize()
	{
		SCR_MapEntity m_MapEntity = SCR_MapEntity.GetMapInstance();
		int worldSize = m_MapEntity.GetMapSizeX();
		if (m_MapEntity.GetMapSizeY() > worldSize)
		{
			worldSize = m_MapEntity.GetMapSizeY();
		}

		SCR_DC_Log.Add("[SCR_DC_Misc:GetWorldSize] Worldsize:" + worldSize, LogLevel.SPAM);
						
		return worldSize;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns the world name being played.
	Example: "path/to/worldfile/Arland.ent" will return "Arland"
	*/	
	static string GetWorldName()
	{
		string worldName = "Unknown";

		worldName = GetGame().GetWorldFile();
		//The name is "path/to/worldfile/Arland.ent". Find the last slash, add one to it and cut the ".ent" from the end.
		int lastslash = worldName.LastIndexOf("/") + 1;
		worldName = worldName.Substring(lastslash, worldName.Length() - lastslash - 4);
		
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
/*			#ifndef RELEASE
				SCR_DC_MapMarkersUI.AddMarker("DUMMY_", pos, "", DC_EMarkerType.DEBUG);
			#endif*/
		}		
		
//		SCR_DC_Log.Add("[SCR_DC_MissionHelper:GetRandomWorldPos] Iterations: " + i, LogLevel.SPAM);			
								
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
}