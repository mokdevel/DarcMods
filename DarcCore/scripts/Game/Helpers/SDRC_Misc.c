//Helpers SDRC_Misc.c

//------------------------------------------------------------------------------------------------
/*!
Includes various small functions. 
*/

sealed class SDRC_Misc
{
	//------------------------------------------------------------------------------------------------
	/*!
	Are we server (master) or not.
	*/
	static float IsMaster()
	{
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());		
		return gameMode.IsMaster();
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Returns random float between min and max. 
	This function just to get rid of an error when min/max are the same.
	*/
	static float RandomFloat(float min, float max)
	{
		if (min == max)
		{
			return min;
		}
		return Math.RandomFloatInclusive(min, max);	
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Returns random int between min and max. 
	This function just to get rid of an error when min/max are the same.
	*/
	static int RandomInt(int min, int max)
	{
		if (min == max)
		{
			return min;
		}
		return Math.RandomIntInclusive(min, max);	
	}
			
	//------------------------------------------------------------------------------------------------
	/*!
	Returns random sign as an int. 
	*/
	static int RandomSign()
	{
		array<int> signs = {1, -1};
		return signs.GetRandomElement();
	}
	
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
	Returns the available mods
	*/	
	static void GetAddonList(out array<string> addonList, bool printList = false)
	{
		addonList = SCR_AddonTool.GetAllAddonFileSystems();
		if (printList)
		{
			foreach (string addon : addonList)
			{
				SDRC_Log.Add("[SDRC_Misc:GetAddonList] Mod found: " + addon, LogLevel.NORMAL);
			}
		}
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Is addon with the a specific name loaded. Mod name has '$' at the beginning and ':' at the end.
	
	Example:
		SDRC_Misc.IsAddonLoaded("$DarcMissions:"))
	*/	
	static bool IsAddonLoaded(string addonName)
	{
		array<string> addonList;
		GetAddonList(addonList);
		
		if (addonList.Contains(addonName))
		{
			return true;
		}
		
		return false;
	}		
		
	//------------------------------------------------------------------------------------------------
	/*!
	Returns the (max) size of the world bound box
	
	NOTE: Some maps may have the bounding box larger than the actualy play area (GenericTerrainEntity). 
	In these cases the worldsize not correct.	
	*/	
	static int GetWorldSize()
	{
		int worldSize = -1;
		
		if (GetGame().GetWorld())
		{		
			vector mins, maxs;
			GetGame().GetWorld().GetBoundBox(mins, maxs);		
			worldSize = FindMaxValue(maxs);
			SDRC_Log.Add("[SDRC_Misc:GetWorldSize] Worldsize:" + worldSize, LogLevel.SPAM);
		}
						
		return worldSize;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Returns the radius of the world from center to corner.
	*/	
	static float GetWorldSizeRadius()
	{
		float worldSize = SDRC_Misc.GetWorldSize();		
		float radius = worldSize * 0.707;	//Approximation 
		return radius;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Returns a position at the outer edge of the map
	*/	
	static vector GetRandomWorldEdgePosition(float sizeMultiplier = 1)
	{
		return SDRC_Math.MovePosToAngle(SDRC_Misc.GetWorldCenter(), SDRC_Misc.GetWorldSizeRadius() * sizeMultiplier, SDRC_Misc.RandomFloat(0, 360));
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns the center of the world
	*/	
	static vector GetWorldCenter()
	{
		float worldSize = SDRC_Misc.GetWorldSize();		
		vector pos = "0 0 0";
		pos[0] = worldSize/2;
		pos[2] = pos[0];
		return pos;
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
		
		if (filterGM)
		{
			if (worldName.StartsWith("GM_"))
			{
				worldName.Replace("GM_", "");
			}
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
		int worldSize = SDRC_Misc.GetWorldSize();
		vector posTmp = vector.Zero;
		vector pos = vector.Zero;
		
		bool positionFound = false;
		int i = 0;
		
		for (i = 0; i <= 100; i++)
		{
			posTmp[0] = SDRC_Misc.RandomInt(0, worldSize);
			posTmp[2] = SDRC_Misc.RandomInt(0, worldSize);

			//SDRC_Log.Add("[SDRC_MissionHelper:GetRandomWorldPos] pos: " + posTmp, LogLevel.DEBUG);			
			
			posTmp[1] = GetGame().GetWorld().GetSurfaceY(posTmp[0], posTmp[2]);
			
			//If map has ocean, check if position on land
			if (GetGame().GetWorld().IsOcean())
			{
				if (mustBeOnLand)
				{
					if (GetGame().GetWorld().GetOceanHeight(posTmp[0], posTmp[2]) == 0)
					{
						positionFound = true;
					}
				}
				else
				{
					positionFound = true;
				}
			}
			else
			{
				//If no ocean present, we check that it's not under map
				if (posTmp[1] >= 0)
				{
					positionFound = true;
				}
			}
			
			if (positionFound)
			{
				break;
			}
		}

		if (positionFound)
		{
			pos = posTmp;			
		}		
		
//		SDRC_Log.Add("[SDRC_MissionHelper:GetRandomWorldPos] Iterations: " + i, LogLevel.SPAM);			
								
		return pos;		
	}		

	//------------------------------------------------------------------------------------------------
	/*!
	Find a position from the world center with a distance percentage. 
	*/	
	static vector GetRandomWorldPosPercentage(float distancePercentage, vector pos = "-1 -1 -1")
	{
		if (pos == "-1 -1 -1")
		{
			pos[0] = SDRC_Misc.GetWorldSize()/2;
			pos[2] = SDRC_Misc.GetWorldSize()/2;
		}
		
		pos = SDRC_Misc.GetCoordinatesOnCircle(pos, SDRC_Misc.GetWorldSize() * distancePercentage, SDRC_Misc.RandomInt(0, 360));
		
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
		posRnd[0] = SDRC_Misc.RandomFloat(-range, range);
		posRnd[2] = SDRC_Misc.RandomFloat(-range, range);
		
		newPos = position + posRnd;
		
		return newPos;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Return surface height either on land or water.
	\param position The position to find Y
	\param doTrace If true, an additional ray cast is done to find the highest point on the location. For example, there could be a building.
	\param traceHeight From which height the trace should start
	*/	
	static float GetSurfaceYWithWater(vector position, bool doTrace = false, float traceHeightMod = 200)
	{
		float y = 0;
		
		if (GetGame().GetWorld())
		{		
			y = GetGame().GetWorld().GetSurfaceY(position[0], position[2]);
			if (SDRC_Misc.IsPosInWater(position))	//Is it under water?
			{
				y = GetGame().GetWorld().GetOceanHeight(position[0], position[2]);
			}
			
			if (doTrace)
			{
				//Trace if there is an entity, like house, blocking. Trace will also stop on vehicles and other temporary obstacles.
				vector traceStartPos = position;
				//If trace start would be below current Y or zero, set the current known Y as the starting point
				if ( (traceStartPos[1] < y) || (traceStartPos[1] == 0) )
				{
			 		traceStartPos[1] = y + 0.1;
				}				
				traceStartPos[1] = traceStartPos[1] + traceHeightMod;
				
				vector traceEndPos = position;
			 	traceEndPos[1] = y;
				TraceParam param = new TraceParam();
				{					
					param.Start = traceStartPos;
					param.End = traceEndPos;
//					param.TargetLayers = EPhysicsLayerDefs.Navmesh;
					param.Flags = TraceFlags.ENTS | TraceFlags.WORLD;
					param.LayerMask = EPhysicsLayerPresets.Projectile;
				}					
	
				BaseWorld world = GetGame().GetWorld();
				float traceDistance = world.TraceMove(param, null);
				float traceHeight = vector.Distance(traceStartPos, traceEndPos);
				
				vector newPos = position;
				newPos[1] = traceStartPos[1] - (traceHeight * traceDistance);
				y = newPos[1];
				
				//SDRC_DebugHelper.AddDebugSphere(newPos, ARGB(40, 255, 32, 32), 6);			//Red
			}						
		}
		return y;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Sets the current position to surface height
	*/	
	static vector SetPosToSurface(vector position)
	{
		float yp = SDRC_Misc.GetSurfaceYWithWater(position);
		position[1] = yp;
		
		return position;
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
		if (!GetGame().GetWorld().IsOcean())
		{
			return false;
		}
		
		float waterHeight = GetGame().GetWorld().GetOceanHeight(pos[0], pos[2]);
		
		if (waterHeight == 0)
		{			
			return false;
		}
		
		return true;
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Check if position under map. 
	The case happens when the Y-coordinate is negative so we consider it to be under the map.
	*/	
	static bool IsPosUnderMap(vector pos)
	{
		if (GetGame().GetWorld().GetSurfaceY(pos[0] , pos[2]) < 0)
		{
			return true;
		}
		
		return false;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns the resourceName in human readable format
	*/	
	static string GetSimpleEntityName(ResourceName resourceName)
	{
		string name = SCR_StringHelper.FormatResourceNameToUserFriendly(resourceName);
		name = SCR_StringHelper.ReplaceRecursive(name, " ", "_");
		
		return name;
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Get current tick time
	*/	
	static int GetCurrentTickTime()
	{
		return (System.GetTickCount() / 1000);
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Format a float to a value for n decimals
	
	Original: SCR_EditorSettingsSubMenu.GetSliderText()
	*/
	static string FloatWithDecimals(float value, int decimals = 1)
	{
		float coef = Math.Pow(10, decimals);
		value = Math.Round(value * coef);
		string valueText = value.ToString();
		if (decimals > 0)
		{
			for (int i = 0, count = decimals - valueText.Length() + 1; i < count; i++)
			{
				valueText = "0" + valueText;
			}
			int length = valueText.Length();
			valueText = valueText.Substring(0, length - decimals) + "." + valueText.Substring(length - decimals, decimals);
		}

		return valueText;
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Pauses the game so that you can fly in GM to check things
	*/
	static void PauseGame()
	{
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!gameMode)
			return;
		gameMode.PauseGame(true, SCR_EPauseReason.MENU);		
	}
			
	//----------------------------------------------------------------------------------------------
	static string GetPlatformName()
	{		
		//GetGame().IsPlatformGameConsole())
		EPlatform platform = System.GetPlatform();
		return SCR_Enum.GetEnumName(EPlatform, platform);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if a class is available. This can be used to check if a mod has been loaded by checking a class
	\param classToTest The class to search
	TBD: For some reason uncommenting the code below results in WB crash and weird compilation errors
	*/	
	/*static bool IsClassAvailable(string classToTest)
	{
		string s = String(classToTest);
		typename var = s.ToType();
		if (var)
		{
			return true;
		}
		return false;
	}*/
	
	//------------------------------------------------------------------------------------------------
	/* Where the GM camera is facing
		//Set first destination to where we're looking
		SCR_CameraEditorComponent cameraManager = SCR_CameraEditorComponent.Cast(SCR_CameraEditorComponent.GetInstance(SCR_CameraEditorComponent));
		if (cameraManager)
		{
			SCR_ManualCamera GMCamera = cameraManager.GetCamera();				
			if (GMCamera)
			{
				vector transform[4];
				GMCamera.GetTransform(transform);
				vector angle = transform[2];
				angle.Normalized();
//				m_vFirstDestination = owner.GetOrigin() + angle * m_fDistanceLow;
			}
		}		
	
	*/
	
}