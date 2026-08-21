//SDRC_ChopperCompModded.c

//------------------------------------------------------------------------------------------------
//class SDRC_ChopperComp : ScriptGameComponent
modded class SDRC_ChopperComp
{
	//------------------------------------------------------------------------------------------------
	/*!	
	Sets the next destination for an action. 
	- FLY will start to fly
	- Others will have some action bound to them.
	*/
	override private void SetNextState(IEntity owner, SDRC_EFlyWayPointType nextType = SDRC_EFlyWayPointType.WP_UNDEFINED, bool allowRemove = true)
	{
		//Reset the timer between points as we're setting new state with new points.
		m_fTimeBetweenPts = 0;
		
		//By default we remove the destination
		bool isRemoveDestination = false;
		
		//Check that next waypoint type is valid
		nextType = SDRC_ChopperHelper.GetNextWayPointType(owner, nextType);

		//Parameters needed below.
		int patrolCount = 8;				//Do one round for patrol by default (8*45 degrees)
		
		switch (nextType)
		{
			case SDRC_EFlyWayPointType.WP_UNDEFINED:
			{
				//Should never happen
				break;
			}
			case SDRC_EFlyWayPointType.WP_BRAKE:
				SetState(SDRC_EHeliState.BRAKE);			
				//NOTE: The final height will be set in SetFlightPointHeight
				m_bIsBraking = false;
				break;
			case SDRC_EFlyWayPointType.WP_LAND:
			{
				SDRC_ChopperCompCore.ResetOriginalValues(owner);		//Reset heli settings
				//Set the state so that when creating flight 
				SetState(SDRC_EHeliState.LAND);
				break;
			}
			case SDRC_EFlyWayPointType.WP_LAND_VERTICAL:
			{
				SDRC_ChopperCompCore.ResetOriginalValues(owner);		//Reset heli settings
				SetState(SDRC_EHeliState.LAND_VERTICAL);
				break;
			}
			case SDRC_EFlyWayPointType.WP_FLY:
			{
				SDRC_ChopperCompCore.ResetOriginalValues(owner);		//Reset heli settings
				SetState(SDRC_EHeliState.FLY);
				//Don't remove the destination as it has the next point where to fly
				break;
			}			
			case SDRC_EFlyWayPointType.WP_FLY_AWAY_IMMEDIATELY:	//NOTE: This is not a real state. When set, state will change to FLY_AWAY
			{
				//Don't remove the destination as it will be removed when creating a waypoint in CreateFlightPoints
			}
			case SDRC_EFlyWayPointType.WP_FLY_AWAY:
			{
				SDRC_ChopperCompCore.ResetOriginalValues(owner);		//Reset heli settings
				SetState(SDRC_EHeliState.FLY);
				//Fly for a while and then go to END state
				AddDestination(SDRC_EFlyWayPointType.WP_DESPAWN, owner.GetOrigin()); 
				break;
			}
			
			//These will remove the item from destination list. These are considered handled.
			case SDRC_EFlyWayPointType.WP_RAISE:
			{				
				SetState(SDRC_EHeliState.RAISE);
				
				//NOTE: We do not use AddDestination() for setting the flight. We just add points in the spline.				
				
				//Clear flight as we are adding the points ourselves.
				ResetFlight();
				
				//Reset heli settings
				SDRC_ChopperCompCore.ResetOriginalValues(owner);	//Reset heli settings				
				m_fSpeed = 2;										//Set a speed to start the raise from	
				m_fSpeedMax = m_fSpeedMin * 1.5;
				m_fSpeedMin = 2;
				m_fThrottle = 2.5;
				
				//m_vSplinePoints.Insert(owner.GetOrigin());
				
				//Fly forward
				if (m_vFlyDestinations[0].pt[0] == 0)
				{
					m_vFlyDestinations[0].pt[0] = 200;
				}
				vector pos = SDRC_ChopperHelper.GetDestinationForward(owner, m_vFlyDestinations[0].pt[0]);
				float height = m_vFlyDestinations[0].pt[1];
				if (height == -1)	//See docs
				{
					height = m_fFlyHeightLow + 5;
				}
				pos[1] = SDRC_Misc.GetSurfaceYWithWater(pos) + height;			//Fly to a point slightly above low fly point
				
				float pdiff = pos[1] - m_vOrigin[1];
				
				const int RAISE_POINT_COUNT = 12;
				
				for (int i = 0; i < RAISE_POINT_COUNT; i++)
				{	
					vector pt = vector.Lerp(owner.GetOrigin(), pos, i / RAISE_POINT_COUNT);
					pt[1] = m_vOrigin[1] + pdiff * SDRC_Math.HalfBell(i / RAISE_POINT_COUNT);
					m_vSplinePoints.Insert(pt);
//					m_vSplinePoints.Insert(vector.Lerp(owner.GetOrigin(), pos, i/RAISE_POINT_COUNT));
				}
				
				m_iClosestIndex = 3;				
				
				//Make a short flight forward to stabilize flight
				vector pos2 = SDRC_ChopperHelper.GetDestinationForward(owner, m_vFlyDestinations[0].pt[0] * 2);
				pos2[1] = pos[1];
				AddDestination(SDRC_EFlyWayPointType.WP_FLY, pos2, index: 1);
				
				//We enforce a time unless it has been defined
				int stateTime = m_vFlyDestinations[0].value;
				if (stateTime <= 0)	
				{
					stateTime = m_vFlyDestinations[0].pt[0] / 30;	//For 200m, it's around 7 secs
				}
				SetTimeInState(stateTime);				
				
				SDRC_ChopperDebug.DrawDebugPaths(owner);
				isRemoveDestination = true;
				break;
			}				
			case SDRC_EFlyWayPointType.WP_END:
			{
				SetState(SDRC_EHeliState.DESTROYED);
				m_vSplinePoints.Clear();
				isRemoveDestination = true;
				break;
			}
			case SDRC_EFlyWayPointType.WP_DESPAWN:
			{
				SetState(SDRC_EHeliState.DESPAWN);
				m_vSplinePoints.Clear();
				isRemoveDestination = true;
				break;
			}
			case SDRC_EFlyWayPointType.WP_HOVER:
			{
				SetState(SDRC_EHeliState.HOVER);
				SetTimeInState(m_vFlyDestinations[0].value);
				//Stop heli from moving
				m_fSpeed = 0.01;
				m_fSpeedMin = 0.01;
				m_fSpeedSlowingMul = 0.1;	//Make the heli stay upright
				//m_fSpeedMax = 0.2;
				isRemoveDestination = true;
				break;
			}
			case SDRC_EFlyWayPointType.WP_HOVER_UP:
			case SDRC_EFlyWayPointType.WP_HOVER_DOWN:
			{
				if (nextType == SDRC_EFlyWayPointType.WP_HOVER_DOWN)
				{
					SetState(SDRC_EHeliState.HOVER_DOWN);
				}
				if (nextType == SDRC_EFlyWayPointType.WP_HOVER_UP)
				{
					SetState(SDRC_EHeliState.HOVER_UP);
				}
				
				SetTimeInState(m_vFlyDestinations[0].value);
				
				//NOTE: We do not use AddDestination() for setting the flight. We just add points in the spline.
				
				//Clear flight as we are adding the points ourselves.
				ResetFlight();
				
				//Reset heli settings
				SDRC_ChopperCompCore.ResetOriginalValues(owner);
				
				//Stop heli from moving
				m_fSpeedMin = 0.3;
				m_fSpeedMax = 0.6;
				m_fSpeedSlowingMul = 0;
				
				vector pos = owner.GetOrigin();
				pos[1] = pos[1] + m_vFlyDestinations[0].pt[1];		//Hover above original point
				
				for (int i = 0; i < 3; i++)
				{
					m_vSplinePoints.Insert(pos);
				}
				m_iClosestIndex = 0;
				//CreateNewFlight(owner, firstDestination);
				
				SDRC_ChopperDebug.DrawDebugPaths(owner);
				isRemoveDestination = true;
				break;
			}
			case SDRC_EFlyWayPointType.WP_GET_OUT:
			{
				SDRC_ChopperCrewHelper.GetOut(owner);
				SetState(SDRC_EHeliState.GET_OUT);
				isRemoveDestination = true;
				break;
			}
			case SDRC_EFlyWayPointType.WP_STOP_ENGINE:
			{
				//Stop engine and wait
				//m_Helicopter_s.EngineStop();
				SetEngine(false, 0, 0, 0);
				SetState(SDRC_EHeliState.WAIT);
				SetTimeInState(30);
				isRemoveDestination = true;
				
				//ResetFlight();	//TBD: Check if lines are staying on screen after stop engine 
				break;
			}
			case SDRC_EFlyWayPointType.WP_WAIT:
			{
				//Just wait
				SetState(SDRC_EHeliState.WAIT);
				SetTimeInState(m_vFlyDestinations[0].value); 
				isRemoveDestination = true;
				break;
			}
			case SDRC_EFlyWayPointType.WP_WAIT_GETOUT:
			{
				//Wait for disembark. Time is dependent of crew count
				SetState(SDRC_EHeliState.WAIT);
				int crewCount = SDRC_ChopperCrewHelper.CountCrew(GetOwner());
				int time = 5 + crewCount * 4;	//Give N seconds per AI plus additional time
				SetTimeInState(time); 
				isRemoveDestination = true;
				break;
			}
			case SDRC_EFlyWayPointType.WP_PATROL:
			{
				patrolCount = SDRC_Misc.RandomInt(10, 25);
				//NOTE: This will fall through to WP_PATROL_ONCE 
			}
			case SDRC_EFlyWayPointType.WP_PATROL_ONCE:
			{
				//If request to patrol, create additional points around position. We will do _count_ amount of points around the area
				int degree = 45; 		//Degrees per patrolCount
				int sign = 1;			//SDRC_Misc.RandomSign(); <- does not work very well
				
				for (int i = 0; i < patrolCount; i++)
				{
					float value = m_vFlyDestinations[0].value;
					if (value <= 0)
					{
						value = params.patrolRadius;
					}
					float range = Math.RandomFloat(value * 0.7, value * 1.3);					
					//Make waypoints around the position to patrol.					
					vector dir = SDRC_Math.RotateAroundAxis(m_vHeliDirection, vector.Up, sign * i * degree * Math.DEG2RAD);
					dir.Normalize();
					vector pos = m_vFlyDestinations[0].pt + dir * range;						
					AddFlyPathPoint(pos);
					//SDRC_DebugHelper.AddDebugPos(pos, ARGB(255, 0, 0, 255), 2.0, m_sDid, 50 + i * 20);
				}
				break;
			}				
			default:
				SDRC_Log.Add("[SDRC_ChopperComp:SetNextState] State not defined: " + SCR_Enum.GetEnumName(SDRC_EHeliState, m_eHeliState), LogLevel.WARNING);
		}
		
		//Remove the destination if it was handled.	By default it is.
		if ( (isRemoveDestination) && (allowRemove) )
		{			
			if (!m_vFlyDestinations.IsEmpty())
			{
				m_vFlyDestinations.RemoveOrdered(0);
			}
		}
	}		
			
	//------------------------------------------------------------------------------------------------	
	// Special handling
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Handle landing
	*/
	override private void HandleLanding(IEntity owner, float timeSlice)
	{
		vector origin = owner.GetOrigin();
				
		vector lastPt = m_vSplinePoints[m_vSplinePoints.Count() - 1];
		float distance = vector.Distance(origin, lastPt);

		if (distance < m_fLandingDistance)
		{
			//float height = m_Helicopter_s.GetAltitudeAGL();
			float height = GetAltitude();
				
			if (!m_bIsLanding)
			{
				m_fSpeedLandingOrig = m_fSpeed;
				m_fSpeedMin = 0.001;
				m_fPositionLandingOrig = owner.GetOrigin();
								
				//We have started landing sequence so no need to count values
				m_bIsLanding = true;
				m_bFinalLanding = false;
			}
			
			if (!m_Helicopter_s.HasAnyGroundContact())
			{					
				float distMul = distance / m_fLandingDistance;
				float decrMul = origin[1] / lastPt[1];
				decrMul = Math.Clamp(decrMul, 0.1, 4.0);

				m_fSpeedTarget = m_fSpeedLandingOrig * distMul + 0.01;
								
				float decreasePower = -1.0;
				
				//Check if we're close to landing place, slow down and descent
				vector closePos = m_vSplinePoints[m_vSplinePoints.Count() - 4];
				//If we have passed the point, adjust values
				if (SDRC_Math.HasPassedPointXZ(m_fPositionLandingOrig, closePos, owner.GetOrigin()))
				{
					distance = vector.DistanceXZ(origin, m_fPositionLandingOrig);
					float distanceClose = vector.DistanceXZ(closePos, m_fPositionLandingOrig);
					float mulc = distanceClose / distance;
					
					m_fSpeedTarget = distance / 4;
					//	m_fSpeedTarget = Math.Clamp(decrMul, 1.0, 5.0);
					m_fSpeedTarget = 8 * distMul + 0.1;
					decreasePower = 2 * ((mulc + decrMul) / 2);
					m_bFinalLanding = true;
				}				

				m_fRotorForceMultiplier = m_fRotorForceMultiplier * 2.5 - decreasePower * decrMul;
											
				//If multiplier too small, enforce a higher value
				if (m_fRotorForceMultiplier > -3.0)
				{
					m_fRotorForceMultiplier = -5.0;
				}
				
				//This affects yaw-pitch-roll counting in SetTurn
				m_fSpeedSlowingMul = distMul;
			}
			else
			{
				//SDRC_Log.Add("[SDRC_ChopperComp:HandleLanding] Ground contact!", LogLevel.DEBUG);
				//Disable effect of rotors
		        m_Helicopter_s.RotorSetForceScaleState(0, 0);
		        m_Helicopter_s.RotorSetForceScaleState(1, 0);
		        m_Helicopter_s.SetThrottle(0);
				//Set values to stop moving
				m_fSpeedTarget = 0.0001;
				m_fSpeedSlowingMul = 0;
				m_fRotorForceMultiplier = 0;
				SetNextState(owner);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Handle braking
	*/
	override private void HandleBraking(IEntity owner, float timeSlice)
	{
		vector lastPt = m_vSplinePoints[m_vSplinePoints.Count() - 1];
		float distance = vector.Distance(m_vOrigin, lastPt);

		if (distance < m_fBrakingDistance)
		{
			if (!m_bIsBraking)
			{
				m_fSpeedBrakingOrig = m_fSpeed;
				m_fPositionBrakingOrig = m_vOrigin;
								
				//We have started landing sequence so no need to count values
				m_bIsBraking = true;
			}
			else
			{
				float distMul = distance / m_fBrakingDistance;
				
				//If we have passed the point, adjust values
				if (SDRC_Math.HasPassedPointXZ(m_fPositionBrakingOrig, lastPt, owner.GetOrigin()))
				{
					distMul = 0;
				}
				
				m_fSpeedTarget = m_fSpeedBrakingOrig * distMul + 0.01;
				m_fSpeedMin = 0.001;
				
				//This affects yaw-pitch-roll counting in SetTurn
				m_fSpeedSlowingMul = distMul;
				
				if (distMul < 0.01)
				{
					SetNextState(owner);
				}
			}
		}
	}	
}