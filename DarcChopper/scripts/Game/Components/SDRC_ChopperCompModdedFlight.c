//SDRC_ChopperCompModdedFlight.c

//------------------------------------------------------------------------------------------------
modded class SDRC_ChopperComp : ScriptComponent
{
	//------------------------------------------------------------------------------------------------	
	// Flight path things
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Create the initial flight path 
	\param owner The chopper entity
	\param destination The first destination to fly to.if
	
	The first destination priority is:
	- If on a low altitude, set to raise
	- If no destinations set, create one infront of chopper. Turn the chopper towards it.
	- The first fly destination if assigned
	- Random point in front of heli, if nothing is pre defined
	*/
	override void InitFlight(IEntity owner)
	{
/*		if (!GetGame().GetWorld())
		{
			return;
		}*/

		vector destination = vector.Zero;
		
		//Store the origin. This value is updated in EOnFrame, but needed already in calculations.
		m_vOrigin = owner.GetOrigin();	
		float y = SDRC_Misc.GetSurfaceYWithWater(m_vOrigin, true, owner);
				
		//If we're on low altitude, wait for a moment and then hover to start flight
		if ( m_vOrigin[1] < (y + 3) )
		{	
			m_vOrigin[1] = y + 0.1;
			owner.SetOrigin(m_vOrigin);
			
			#ifdef WORKBENCH
				AddDestination(SDRC_EFlyWayPointType.WP_HOVER, value: 2);
			#else
				AddDestination(SDRC_EFlyWayPointType.WP_HOVER, value: 30);
			#endif
			vector hoverPos = vector.Zero;
			hoverPos[1] = m_fFlyHeightLow;
			AddDestination(SDRC_EFlyWayPointType.WP_HOVER_UP, hoverPos, 3);
			hoverPos[1] = (m_fFlyHeightLow + m_fFlyHeightHigh) / 2;
			AddDestination(SDRC_EFlyWayPointType.WP_RAISE, hoverPos);
		}

		destination = SDRC_ChopperHelper.GetDestinationForward(owner, params.destinationForwardInitial);
		//Make sure we're on proper flight height.
		destination[1] = SDRC_ChopperHelper.SetPointHeight(destination, m_fFlyHeightLow, m_fFlyHeightHigh); 
		AddDestination(SDRC_EFlyWayPointType.WP_FLY, destination);			
		
		//Turn chopper to face the first destination
		SDRC_Math.TurnEntityTowardsXZ(owner, destination);							
		
		SDRC_Log.Add("[SDRC_ChopperComp:InitFlight] Chopper initial position: " + owner.GetOrigin(), LogLevel.DEBUG);
				
		//NOTE: We draw the debug paths once the component is ready
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Create the runtime flight path with waypoint definition
	*/
	override void CreateNewFlight(IEntity owner)
	{	
		if (m_vSplinePoints.IsEmpty())
		{
			//If no spline points defined, add a few to get an initial flight direction. 
			//This should only happen at init.
			vector firstPoint = SDRC_ChopperHelper.GetDestinationForward(owner, params.destinationForwardInitial / 3);
			m_vSplinePoints.Insert(firstPoint);
			firstPoint = SDRC_ChopperHelper.GetDestinationForward(owner, params.destinationForwardInitial / 2);
			m_vSplinePoints.Insert(firstPoint);
			SDRC_Log.Add("[SDRC_ChopperComp:CreateNewFlight] Setting initial spline points.", LogLevel.DEBUG);
			return;
		}
		
		//Take the height of the current spline point to set a couple of points to it.
		vector oldHeight = m_vSplinePoints[m_iClosestIndex];
		
		// 1. Clear any existing path points. 
		ResetFlight();
		
		// 2. Add a point in front
		
		SDRC_DebugHelper.DeleteDebugPos(m_sDid + "line");
		vector newPoint = SDRC_ChopperHelper.GetDestinationForward(owner, params.destinationForward * 0.4);
		newPoint[1] = oldHeight[1];
		AddFlyPathPoint(newPoint);
		SDRC_DebugHelper.AddDebugPos(newPoint, ARGB(255, 0, 128, 0), 1.0, m_sDid + "line", 50);		
		newPoint = SDRC_ChopperHelper.GetDestinationForward(owner, params.destinationForward * 0.7);
		newPoint[1] = oldHeight[1];
		AddFlyPathPoint(newPoint);
		SDRC_DebugHelper.AddDebugPos(newPoint, ARGB(255, 0, 128, 0), 1.0, m_sDid + "line", 50);
		newPoint = SDRC_ChopperHelper.GetDestinationForward(owner, params.destinationForward);
		newPoint[1] = oldHeight[1];
		AddFlyPathPoint(newPoint);
		SDRC_DebugHelper.AddDebugPos(newPoint, ARGB(255, 0, 128, 0), 1.0, m_sDid + "line", 50);
		
		//3. Create flight points. These are the main points on the path which are then used for spline	creation.
		//   The points may be below the flight height (e.g. landing).
		//   This will also set the next state for the chopper.
		CreateFlightPoints(owner);
		
		//Flight point management is skipped if there are none. This could happen at init stage were the points are set straight to the spline.
		if (!m_vFlyPathPoints.IsEmpty())
		{
			//4. By default, check that flight points are above the minimum flight height.
			SDRC_ChopperHelper.SetFlightPointHeight(owner);
			
			//5. Set first flight points to same height as the helicopter. This smooths the flight.
			vector origin = owner.GetOrigin();
			//m_vFlyPathPoints[0].pt[1] = origin[1];
			//m_vFlyPathPoints[1].pt[1] = origin[1];
			//m_vFlyPathPoints[0].pt[1] = oldHeight[1];
			//m_vFlyPathPoints[1].pt[1] = oldHeight[1];
			m_vFlyPathPoints[0].pt[1] = m_vSplinePointBelow[1];// - SDRC_Misc.GetSurfaceYWithWater(m_vSplinePointBelow, true, owner);
			m_vFlyPathPoints[1].pt[1] = m_vSplinePointBelow[1];// - SDRC_Misc.GetSurfaceYWithWater(m_vSplinePointBelow, true, owner);
					
			//6. Generate the spline
			array<vector> flyPathPoints = {};
			SDRC_ChopperDebug.GivePoints(flyPathPoints, m_vFlyPathPoints);
			SDRC_Spline3D.GenerateSplinePoints(flyPathPoints, m_vSplinePoints, -1);
		}
				
		//7. Set the closest index from the spline start. This will set the near future destination.
		m_iClosestIndex = 1;
		m_iOldClosestIndex = m_iClosestIndex;
				
		//8. Check that points are above ground. 
		//   This will also handle special spline handling depending on the state assigned for the chopper.
		//SDRC_ChopperHelper.SetSplinePointsAboveGround(owner, 6);	//Skip some of the points at start
		SDRC_ChopperHelper.SetSplinePointsAboveGround(owner, 0);
		
		if (m_vSplinePoints.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_ChopperComp:CreateFlightPath] No points!", LogLevel.ERROR);
		}
		
		SDRC_ChopperDebug.DrawDebugPaths(owner);
	}

	//------------------------------------------------------------------------------------------------	
	// Fly point handling
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Create fly points for spline creation
	Takes the points from m_vFlyDestinations and generates points to be used for spline creation
	\param fixHeight Fix the height of the two first points. This is needed at startup.
	*/	
	override private void CreateFlightPoints(IEntity owner, bool fixHeight = false)
	{
		//Add destinations .. if any
		int lastIdx = 0;
		
		//Generate a random destination point if needed
		vector pos = SDRC_ChopperHelper.GetDestinationForward(owner, params.destinationForward / 2);
		if (m_vFlyDestinations.IsEmpty())
		{		
			SDRC_ChopperHelper.GenerateWayPoint(owner, pos);
		}

		//Add a point towards our next destination
/*		if (!m_vFlyPathPoints.IsEmpty())
		{
			vector lastFlightPoint = m_vFlyPathPoints[m_vFlyPathPoints.Count() - 1].pt; 
			vector newPoint = vector.Lerp(lastFlightPoint, m_vFlyDestinations[0].pt, 0.5);
			AddFlyPathPoint(newPoint);

			SDRC_DebugHelper.AddDebugPos(newPoint, ARGB(255, 0, 128, 0), 1.0, m_sDid + "line", 150);
		}*/
		
		bool firstDestinationHandled = false;
		bool oneShotHandled = false;
		
		//Handle destinations
		foreach (int idx, SDRC_FlyPathPoint flyDestination : m_vFlyDestinations)
		{		
			//FLY points are handled in a serie. Others one at a time.
			if (flyDestination.type != SDRC_EFlyWayPointType.WP_FLY)
			{			
				//If we're doing something else than flying, just do it once.
				oneShotHandled = true;
				
				if (firstDestinationHandled)
				{
					break;
				}
			}
			
			//SDRC_DebugHelper.AddDebugPos(flyDestination.pt, ARGB(32, 255, 128, 64), 1.0, m_sDid, 50);
			
			bool destinationHandled = false;	//Set true, if destination was added
			int patrolCount = 8;				//Do one round for patrol by default (8*45 degrees)
			
			switch (flyDestination.type)
			{
				case SDRC_EFlyWayPointType.WP_HOVER:
				case SDRC_EFlyWayPointType.WP_PATROL:
				case SDRC_EFlyWayPointType.WP_PATROL_ONCE:
				{
					//Special case where the flight points are added in SetNextState()
					
					SetNextState(owner, flyDestination.type, false);					
					destinationHandled = true;
					break;
				}		
			}
					
			//If destination has already been set, skip the re-routing etc.
			if (!m_vFlyPathPoints.IsEmpty())
			{
				if (!destinationHandled)
				{	
					//Distance of last flight point defined and the next destination
					float distance = vector.DistanceXZ(m_vFlyPathPoints[m_vFlyPathPoints.Count() - 1].pt, flyDestination.pt);
			
					//Get the angle for the destination
					vector p0 = m_vFlyPathPoints[m_vFlyPathPoints.Count() - 2].pt;
					vector p1 = m_vFlyPathPoints[m_vFlyPathPoints.Count() - 1].pt;
					vector p2 = flyDestination.pt;
					float heliAngle = SDRC_Math.GetRadiansBetweenThreePointsXZ(p0, p1, p2) * Math.RAD2DEG;
		
					SDRC_DebugHelper.AddDebugPos(p2, ARGB(255, 0, 128, 0), 1.0, m_sDid + "line", 200);
					
					SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Distance: " + distance + " - Angle: " + heliAngle, LogLevel.DEBUG);
					
					//Is the angle too steep? Re-route.
					if (Math.AbsFloat(heliAngle) < params.wpSteepAngle)
					{				
						// Rerouting creates points CD for a path ABE
						//
						//       _C__B 
						//      /    |
						//     D     |
						//    |      A (usually origin)
						//    E      
						//
						SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Heli direction angle is steep: " + heliAngle, LogLevel.SPAM);
						
						//Get the last point
						vector point = m_vFlyPathPoints[m_vFlyPathPoints.Count() - 1].pt;
						
						//We need to take a detour. Add an additional points outside of the line to make the route rounder				
						float lerpRnd = SDRC_Misc.RandomFloat(params.detourLerpPosition * 0.5, params.detourLerpPosition * 1.5);
						float divRnd = SDRC_Misc.RandomFloat(params.detourDivider * 0.5, params.detourDivider * 1.5);
											
						//Depending on the angle decide if we re-route left ot right				
						bool isOnLeft = SDRC_Math.IsPointOnLeft(p0, p1, p2);
	
						//Find a point along the fly path and move it away from the line along tangent					
						vector vec2 = SDRC_Math.CreateOffsetMidPoint(point, flyDestination.pt, (distance / divRnd), lerpRnd, isOnLeft);
						//Find a similar point but now between the start and vec2
						vector vec1 = SDRC_Math.CreateOffsetMidPoint(point, vec2, (distance / (divRnd * 1.5)), 0.5, isOnLeft);
						AddFlyPathPoint(vec1);									
						AddFlyPathPoint(vec2);
						SDRC_DebugHelper.DeleteDebugPos(m_sDid + "detour");
						SDRC_DebugHelper.AddDebugPos(vec1, ARGB(255, 0, 0, 0), 1.0, m_sDid + "detour", 500);
						SDRC_DebugHelper.AddDebugPos(vec2, ARGB(255, 0, 0, 0), 1.0, m_sDid + "detour", 500);
					}
					
					AddFlyPathPoint(flyDestination.pt, flyDestination.type, flyDestination.value);
					
					SetNextState(owner, flyDestination.type, false);
				}
			}
					
			lastIdx = idx;
			firstDestinationHandled = true;
						
			if (oneShotHandled)
			{
				break;
			}
		}

		//If only two points, add a mid point. 
		//NOTE: We ignore the cases where there is no points or only 1. Not sure if this causes issues.
		if (m_vFlyPathPoints.Count() == 2)
		{
			vector p0 = m_vFlyPathPoints[0].pt;
			vector p1 = m_vFlyPathPoints[1].pt;
			vector mid = vector.Lerp(p0, p1, 0.5);
			AddFlyPathPoint(mid, index: 1);
		}		
		
		SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Created " + m_vFlyPathPoints.Count() + " points.", LogLevel.SPAM);
		
		//Remove the destinations that have been handled
		for (int i = 0; i <= lastIdx; i++)
		{
			if (!m_vFlyDestinations.IsEmpty())			//Destination may have been deleted SetNextState 
			{
				m_vFlyDestinations.RemoveOrdered(0);
			}
		}		
		SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Destinations left: " + m_vFlyDestinations.Count(), LogLevel.SPAM);		
	}		

	//------------------------------------------------------------------------------------------------	
	/*!	
	Add a point to fly path. 
	*/
	private void AddFlyPathPoint(vector destination, SDRC_EFlyWayPointType type = SDRC_EFlyWayPointType.WP_FLY, float value = 0, int index = -1)
	{
		switch (type)
		{
			case SDRC_EFlyWayPointType.WP_ATTACK:
			{
				//Attack to be on low altitude. This will be set in SDRC_ChopperHelper.SetSplinePointsAboveGround()
				SetState(SDRC_EHeliState.ATTACK);
				//NOTE: m_vAttackPosition has been set in AddDestination
				break;
			}
			case SDRC_EFlyWayPointType.WP_SEARCH_DESTROY:
			{	
				SetBehaviour(SDRC_EHeliBehaviour.SEARCH_AND_DESTROY_BEHAVIOUR, value);
				//NOTE: m_vAttackPosition has been set in AddDestination
				break;
			}
			case SDRC_EFlyWayPointType.WP_M_LAND:
			{
				destination = SDRC_Misc.SetPosToSurface(destination);
				//SetState(SDRC_EHeliState.LAND);
				m_bIsLanding = false;
				break;
			}
		}

		//Add the point to flypath				
		SDRC_FlyPathPoint fpp = new SDRC_FlyPathPoint();
		
		if (index == -1)
		{
			fpp.Set(type, destination, value);
			m_vFlyPathPoints.Insert(fpp);
		}
		else
		{
			fpp.Set(type, destination, value);
			m_vFlyPathPoints.InsertAt(fpp, index);
		}
	}		
}