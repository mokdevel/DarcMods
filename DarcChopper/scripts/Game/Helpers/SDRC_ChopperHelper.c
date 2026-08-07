//SDRC_ChopperHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_ChopperHelper
{
	//------------------------------------------------------------------------------------------------	
	/*!	
	Calculate a position in front of current heli position, along it's axis. 
	*/
	static vector GetDestinationForward(IEntity owner, float distance)
	{
		vector direction = owner.GetTransformAxis(2);
		direction.Normalize();
		direction[1] = 0;
		vector origin = owner.GetOrigin();
		vector position = origin + direction * distance;
		//Keep the flying flat
		position[1] = origin[1];
		return position;
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Creates a random destination point
	*/	
	static vector GetRandomPosition(vector pos, float distanceLow, float distanceHigh)
	{
		float distance = SDRC_Misc.RandomFloat(distanceLow, distanceHigh);
		vector newpos;
		
		//If distance is under 1.0, use world percentage
		if (distance < 1.0)
		{
			newpos = SDRC_Misc.GetRandomWorldPosPercentage(distance);
		}
		else
		{
			newpos = SDRC_Misc.GetCoordinatesOnCircle(pos, distance, SDRC_Misc.RandomInt(0, 360));
		}
				
		return newpos;
	}	

	//------------------------------------------------------------------------------------------------	
	/*!	
	Tries to find a safe landing spot
	*/	
	static bool GetSafeLandingPosition(out vector landingSpot, float emptySize = 40, int tries = 3, int searchSize = 100)
	{
		bool foundLandingSpot = false;

		for (int i = 0; i < tries; i++)
		{
			if (SDRC_SpawnHelper.FindEmptyPos(landingSpot, searchSize + i * searchSize, emptySize))
			{
				foundLandingSpot = true;
				break;
			}
		}
		
		return foundLandingSpot;
	}
	
	//------------------------------------------------------------------------------------------------
	// Waypoint stuff
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Functionality to generate future destination(s) for the chopper
	
	\param owner ..
	\param orig The position to use for waypoint generation.
	*/
	static void GenerateWayPoint(IEntity owner, vector orig = vector.Zero)
	{		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}
		
		if (orig == vector.Zero)
		{
			orig = owner.GetOrigin();
		}
		
		if (SDRC_Misc.RandomFloat(0, 1) < 0.2)
		{
			//Occasionally fly towards middle to avoid sliding out of the map
			orig = SDRC_Misc.GetWorldCenter();
		}
			
		vector pos;
		
		//Create a random position to fly to. Try to avoid very steep turns by doing iterations.
		for (int i = 0; i < 3; i++)
		{
			pos = SDRC_ChopperHelper.GetRandomPosition(orig, chopperComp.m_fDistanceLow, chopperComp.m_fDistanceHigh);	
			vector dir1 = vector.Direction(owner.GetOrigin(), pos);
			float angle = SDRC_Math.GetAngleBetweenVectorsXZ(dir1, chopperComp.m_vHeliDirectionFuture);
			if (Math.AbsFloat(angle) < (110 * Math.DEG2RAD) )
			{				
				break;
			}			
		}
		
		chopperComp.AddDestination(SDRC_EFlyWayPointType.WP_FLY, pos);
		//SDRC_DebugHelper.AddDebugPos(pos, ARGB(255, 255, 00, 00), 2.0, chopperComp.m_sDid, 200);
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Returns the next recommended waypoint type defined in destinations. Does not modify the destination list.
	
	WP_FLY is returned as default if destination list is empty.
	*/
	static SDRC_EFlyWayPointType GetNextWayPointType(IEntity owner, SDRC_EFlyWayPointType nextType = SDRC_EFlyWayPointType.WP_UNDEFINED)
	{
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return SDRC_EFlyWayPointType.WP_UNDEFINED;
		}		

		//If destinations is empty, set nextType as WP_UNDEFINED		
		if (chopperComp.m_vFlyDestinations.IsEmpty())
		{
			nextType = SDRC_EFlyWayPointType.WP_UNDEFINED;
		}

		//If nextType is WP_UNDEFINED, return WP_FLY as the default recommendation.
		if (nextType == SDRC_EFlyWayPointType.WP_UNDEFINED)
		{		
			//If not destinations defined, start to fly		
			if (chopperComp.m_vFlyDestinations.IsEmpty())
			{
				nextType = SDRC_EFlyWayPointType.WP_FLY;
			}
			else
			{
				//There is a next one defined. Let's provide it.
				nextType = chopperComp.m_vFlyDestinations[0].type;
			}
		}		
				
		return nextType;
	}	
				
	//------------------------------------------------------------------------------------------------
	/*!
	Handle waypoints set
	*/
	static void HandleWaypoints(IEntity owner)
	{
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}

		array<AIWaypoint> waypoints = {};
				
		foreach (AIGroup group : chopperComp.m_aGroups)
		{
			if (group != null)
			{
				group.GetWaypoints(waypoints);
				
				//If group has no waypoints, check the next group
				if (waypoints.Count() == 0)
				{
					continue;
				}

				//Find if it is a cycle				
				foreach (AIWaypoint wp : waypoints)
				{
					//Skip cycle waypoints
					if (AIWaypointCycle.Cast(wp) != null)
					{
						continue;
					}
					
					if (AIWaypoint.Cast(wp) != null)
					{
						vector pos = wp.GetOrigin();
						
						EntityPrefabData prefabData = wp.GetPrefabData();
						ResourceName resourceName = prefabData.GetPrefabName();
						resourceName = SDRC_Misc.GetSimpleEntityName(resourceName);
						SDRC_Log.Add("[SDRC_ChopperHelper:HandleWaypoints] Waypoint " + resourceName + " found at: " + pos, LogLevel.DEBUG);						
						
						switch (resourceName)
						{
							case "E_AIWaypoint_Move":
							{
								chopperComp.AddDestination(SDRC_EFlyWayPointType.WP_FLY, pos);
								break;
							}						
							case "E_AIWaypoint_ForcedMove":
							{
								chopperComp.AddDestination(SDRC_EFlyWayPointType.WP_FLY_IMMEDIATELY, pos);
								break;
							}						
							case "E_AIWaypoint_Patrol":
							{
								chopperComp.AddDestination(SDRC_EFlyWayPointType.WP_PATROL, pos);
								break;
							}
							case "E_AIWaypoint_GetOut":
							{
								chopperComp.AddDestination(SDRC_EFlyWayPointType.WP_M_LAND_TROOPS, pos);
								break;
							}
							case "E_AIWaypoint_Defend":
							{
								chopperComp.AddDestination(SDRC_EFlyWayPointType.WP_M_LAND_TO_FREE_SPOT, pos);
								break;
							}
							case "E_AIWaypoint_GetIn":
							{
								break;
							}
							case "E_AIWaypoint_Wait":
							case "E_AIWaypoint_Suppress_Editor":
							{
								chopperComp.AddDestination(SDRC_EFlyWayPointType.WP_M_SUPPRESSIVE, pos);
								break;
							}
							case "E_AIWaypoint_ArtillerySupport":
							{
								chopperComp.AddDestination(SDRC_EFlyWayPointType.WP_ATTACK, pos, 120);
								break;
							}
							case "E_AIWaypoint_SearchAndDestroy":
							{
								chopperComp.AddDestination(SDRC_EFlyWayPointType.WP_SEARCH_DESTROY, pos, chopperComp.params.timeSearchAndDestroy);	
								break;
							}
						}
					}
				}
				
				//Clear all waypoints
				SDRC_WPHelper.RemoveWaypoints(group);	
			}
		}
	}

	//------------------------------------------------------------------------------------------------	
	// FlyPoint fixing and sanity check
	//------------------------------------------------------------------------------------------------	

	//------------------------------------------------------------------------------------------------	
	/*!	
	Set a point between min/max *ABOVE* the surface/object on surface
	*/	
	static float SetPointHeight(vector pos, float min, float max)
	{
		float height = pos[1];
		
		//Make sure we're on proper flight height.
		float y = SDRC_Misc.GetSurfaceYWithWater(pos, true);
		height = Math.Clamp(height, (y + min), (y + max) ); 
		
		return height;
	}	
			
	//------------------------------------------------------------------------------------------------	
	/*!	
	Set the requested flight path points between min/max flying height.
	*/	
	static void SetFlightPointHeight(IEntity owner)
	{
		if (!owner)
		{
			return;
		}
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}
		
		float y = 0;
		
		foreach (int i, SDRC_FlyPathPoint flightPoint : chopperComp.m_vFlightPoints)
		{
			vector pt = flightPoint.pt;

			//Do not change height of two first points. These are the two points from the previous spline. 
			if (i < 2)
			{
				continue;
			}
			
			//Initial height will be on ground
			y = SDRC_Misc.GetSurfaceYWithWater(pt, true);
						
			float flyHeight = 0;
						
			if (flightPoint.type == SDRC_EFlyWayPointType.WP_LAND)
			{
				//Do nothing .. height will be on ground due to y being set and flyHeigt is zero. See above.
			}
			else if (flightPoint.type == SDRC_EFlyWayPointType.WP_ATTACK)
			{
				//Do nothing .. height is defined for the attack point.
				//TBD: When spline is created, the height is still set to minimum. We maybe should have a specific 
				//	   ATTACK state and the height handling specific for it.
			}
			else
			{
				flyHeight = SDRC_Misc.RandomFloat(chopperComp.m_fFlyHeightLow, chopperComp.m_fFlyHeightHigh);
			}
			
			pt[1] = 0;	//We may in the future use the provided Y coord for the points. For now we set it to 0.
			pt[1] = pt[1] + flyHeight + y;
			chopperComp.m_vFlightPoints[i].pt = pt;
		}
	}	
		
	//------------------------------------------------------------------------------------------------	
	// Spline functions
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	static int FindNextSplinePointIndex(vector origin, int currentIndex, array<vector> splinePoints)
	{
		if (splinePoints.IsEmpty())
		{
			return 0;
		}
		
		float distance = vector.Distance(origin, splinePoints[currentIndex]);
		int newIndex = currentIndex;
		
		for (int i = (currentIndex + 1); i < splinePoints.Count() - 1; i++)
		{
			if (vector.Distance(origin, splinePoints[i]) < distance)
			{
				newIndex = i;
			}
			else
			{
				break;
			}
		}
		
		return newIndex;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Cut spline to create the new destination almost immediately. Remove points from the end.
	*/
	static void CutSplineTail(out array<vector> splinePoints, int closestIndex)
	{
		const int POINTS_TO_NEW_DISTANCE = 3;		//How many spline points in to the future flight path is checked before adding new flight points.
		
		//Take the last one, reduce current one and reduce the point count for new distance. Add a safe margin of 2.
		int toBeRemoved = (splinePoints.Count() - 1) - closestIndex - POINTS_TO_NEW_DISTANCE - 2;
		for (int i = 0; i < toBeRemoved; i++)
		{
			splinePoints.RemoveOrdered(splinePoints.Count() - 1);
		}
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Cut spline from the beginning to closestIndex. Remove points from the start.
	*/
	static void CutSplineHead(out array<vector> splinePoints, int closestIndex)
	{
		for (int i = 0; i < closestIndex; i++)
		{
			splinePoints.RemoveOrdered(0);
		}
	}	
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Check that spline points are above ground. Raise the point if needed.
	\param owner Chopper entity
	\param skipCount How many points to skip from the beginning.
	*/	
	static void SetSplinePointsAboveGround(IEntity owner, int skipCount = 0)
	{	
		if (!owner)
		{
			return;
		}
		
		vector origin = owner.GetOrigin();
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (chopperComp)
		{
			//If we change the Y values, we want to smooth the spline
			bool isSmoothingNeeded = false;
			int smoothCount = 0;				//How many points to smooth. 0 = all
			
			//Make sure the points are at minimum m_fFlyHeightLow from the ground.
			foreach (int i, vector pt : chopperComp.m_vSplinePoints)
			{
				if (i < skipCount)
				{
					continue;
				}
				
				float y = SDRC_Misc.GetSurfaceYWithWater(pt, true);
	
				//If we're low, fix the point a bit above the low fly height
				if (pt[1] < (y + chopperComp.m_fFlyHeightLow))
				{
					pt[1] = y + chopperComp.m_fFlyHeightLow + 5;	//Make chopper fly higher for a moment
					chopperComp.m_vSplinePoints[i] = pt;
					
					isSmoothingNeeded = true;
				}
			}
					
			//If we're braking set points towards the last point
			if (chopperComp.GetState() == SDRC_EHeliState.BRAKE)
			{
				int lastIdx = chopperComp.m_vSplinePoints.Count() - 1;

				//Find high point, low point and difference
				vector v0 = chopperComp.m_vSplinePoints[chopperComp.m_iClosestIndex];
				vector v1 = chopperComp.m_vSplinePoints[lastIdx];
				
				//Count a landing (bell) curve
				float p0 = v0[1];
				float p1 = v1[1];
				float pdiff = p0 - p1;
									
				//Create a Y spline to replace the given points to smooth the curve for braking
				int points = lastIdx - chopperComp.m_iClosestIndex;
				for (int i = 0; i < points; i++)
				{					
					float step = 1 - (i / (points - 1));	//NOTE: The step will not go from 1..0 but end a little earlier. The last point of the bell is ignored. Change to (pointsToGround -1) for full bell curve.

					vector pt = vector.Lerp(v1, v0, step);
					pt[1] = p1 + pdiff * SDRC_Math.HalfBell(step);
					chopperComp.m_vSplinePoints[lastIdx - points + i] = pt;
					chopperComp.m_vSplinePoints[lastIdx - points + i + 1] = pt;	//Ugly hack to make sure the last point is also modified
				}
				
				isSmoothingNeeded = false;
			}					
			
			//If we're landing set some of the last points close to the ground
			if (chopperComp.GetState() == SDRC_EHeliState.LAND)
			{
				//Decide a point on the spline for the initial landing to start
				//int pointsToGround = chopperComp.m_fSpeed / 1.2;
				
				int lastIdx = chopperComp.m_vSplinePoints.Count() - 1;
				int pointsToGround = lastIdx - 1;
				if (pointsToGround > 10)
				{
					pointsToGround = 10;
				}
				
				//Check that point is within limits
				SDRC_Log.Add("[SDRC_ChopperHelper:SetSplinePointsAboveGround] pointsToGround: " + pointsToGround, LogLevel.DEBUG);			

				//Find high point, low point and difference
				vector v0 = chopperComp.m_vSplinePoints[lastIdx - pointsToGround];
				vector v1 = chopperComp.m_vSplinePoints[lastIdx];
				//The destination point will take any objects like buildings in to account
				v1[1] = SDRC_Misc.GetSurfaceYWithWater(v1, true);
				
				//Define the distance from where to start landing sequence				
				chopperComp.m_fLandingDistance = vector.Distance(v0, v1);

				//Count a landing (bell) curve
				float p0 = v0[1];
				float p1 = v1[1];
				float pdiff = p0 - p1;
									
				//Create a Y spline to replace the given points to smooth the curve for landing
				if (chopperComp.m_vSplinePoints.Count() - 1 > pointsToGround)
				{
					for (int i = 0; i < pointsToGround; i++)
					{					
						float step = 1 - (i / (pointsToGround - 1));	//NOTE: The step will not go from 1..0 but end a little earlier. The last point of the bell is ignored. Change to (pointsToGround -1) for full bell curve.

						vector pt = vector.Lerp(v1, v0, step);
						pt[1] = p1 + pdiff * SDRC_Math.HalfBell(step);
						chopperComp.m_vSplinePoints[lastIdx - pointsToGround + i + 1] = pt;
					}
				}
				
				isSmoothingNeeded = false;
			}		
			
			if (isSmoothingNeeded)
			{
				//Smooth the Up curve
				SDRC_Spline3D.SmoothSplineUpOnly(chopperComp.m_vSplinePoints, smoothCount);
			}

			//After smoothing, set heli original height to points in the beginning. This is to avoid jumping.
			for (int i = 0; i < skipCount; i++)
			{
				vector pt = chopperComp.m_vSplinePoints[i];
				pt[1] = origin[1];
				chopperComp.m_vSplinePoints[i] = pt;
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Set the entity flat on XZ plane.
	*/
	static void SetHorizontal(IEntity owner, float timeSlice)
	{
			//Flatten the helicopter while being in init
			vector ownerUp = owner.GetTransformAxis(1);
			vector straightenAngle = SDRC_Math.ComputeAngularVelocity(ownerUp, vector.Up, timeSlice);
			
			owner.GetPhysics().SetAngularVelocity(straightenAngle);
	}
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Returns the final destination from the spline
	*/
/*	vector GetFinalSplineDestination()
	{
		return m_vSplinePoints[m_vSplinePoints.Count() - 1];
	}		*/
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Returns the destination where the helicopter is currently going. 
	NOTE: This is not the final destination on the spline
	*/
/*	vector GetCurrentSplineDestination()
	{
		return m_vDestination;
	}		*/		
}