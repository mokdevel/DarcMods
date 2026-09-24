//SDRC_ChopperCompModdedNextState.c

//------------------------------------------------------------------------------------------------
//class SDRC_ChopperComp : ScriptGameComponent
modded class SDRC_ChopperComp
{
	const int VERTICAL_SPLINE_POINTS = 3;		//How many vertical spline points to create
	
	//------------------------------------------------------------------------------------------------	
	// States 
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	override SDRC_EHeliState GetState()
	{
		return m_eHeliState;
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetState(SDRC_EHeliState state)
	{
		m_eHeliState = state;

		switch (state)
		{
			case SDRC_EHeliState.UNKNOWN:
				break;
			case SDRC_EHeliState.ATTACK:
				break;
			case SDRC_EHeliState.FLY:
			{
				//Disable the TimeInState counter
				SetTimeInState(0);			
				//Set normal behaviour
				SetBehaviour(SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR, -1);
				//Reset attack
				ResetAttack();
				break;
			}
			case SDRC_EHeliState.FLY_AWAY:
			case SDRC_EHeliState.FLY_AWAY_IMMEDIATELY:
			case SDRC_EHeliState.LAND_VERTICAL:
			case SDRC_EHeliState.BRAKE:
			case SDRC_EHeliState.WAIT:
			case SDRC_EHeliState.RAISE:
			case SDRC_EHeliState.HOVER:
			case SDRC_EHeliState.HOVER_UP:
			case SDRC_EHeliState.HOVER_DOWN:
			case SDRC_EHeliState.GET_OUT:
			case SDRC_EHeliState.CRASH:
			case SDRC_EHeliState.END:
			case SDRC_EHeliState.ON_GROUND:
			case SDRC_EHeliState.DESTROYED:
			case SDRC_EHeliState.DESPAWN:
			{
				SetBehaviour(SDRC_EHeliBehaviour.PASSIVE_BEHAVIOUR, -1);
				break;
			}
		}
		
		SDRC_Log.Add("[SDRC_ChopperComp:SetState] State: " + SCR_Enum.GetEnumName(SDRC_EHeliState, m_eHeliState), LogLevel.SPAM);
	}
	
	//------------------------------------------------------------------------------------------------	
	override void SetTimeInState(int seconds)	
	{
		if (seconds == -1)
		{
			seconds = 0;
		}

		//Store original time
		m_fTimeInStateOrig = seconds;
		//Set the time to the default. Value is decreased in EOnFrame
		m_fTimeInStateLeft = seconds;
		//Reset the timer we've been in the state. Value is increased in EOnFrame
		m_fTimeInStateBeen = 0;
		
		//If a time was requested, mark that we have a state with a timer
		if (seconds == 0)
		{
			m_bTimeInStateEnabled = false;
		}
		else
		{
			m_bTimeInStateEnabled = true;
		}
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Sets the next destination for an action. 
	- FLY will start to fly
	- Others will have some action bound to them.
	*/
	override private void SetNextState(IEntity owner, SDRC_FlyPathPoint flyDestination = null, bool allowRemove = true)
	{
		//Reset the timer between points as we're setting new state with new points.
		m_fTimeBetweenPts = 0;
		
		//By default we don't remove the destination point from m_vFlyDestinations
		bool isRemoveDestination = false;
		
		SDRC_EFlyWayPointType nextType = SDRC_EFlyWayPointType.WP_UNDEFINED;
		if (flyDestination)
		{
			nextType = flyDestination.type;
		}		
		
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
			case SDRC_EFlyWayPointType.WP_ATTACK:
			{
				//NOTE: SetAttackPosition() has been set in CreateFlightPoints() because we add some additional FlyPathPoints 
				//      to make a better attack path.
				SetState(SDRC_EHeliState.ATTACK);				
				break;
			}
			case SDRC_EFlyWayPointType.WP_FLY:
			{
				SDRC_ChopperCompCore.ResetOriginalValues(owner);		//Reset heli settings
				SetState(SDRC_EHeliState.FLY);
				//Don't remove the destination as it has the next point where to fly
				break;
			}			
			case SDRC_EFlyWayPointType.WP_FLY_IMMEDIATELY:
				//Handled in AddDestination() as this interrupts previous flight
				ResetAttack();
				break;
			case SDRC_EFlyWayPointType.WP_FLY_AWAY:
			{
				SDRC_ChopperCompCore.ResetOriginalValues(owner);		//Reset heli settings
				SetState(SDRC_EHeliState.FLY);
				//Fly for a while and then go to END state
				AddDestination(SDRC_EFlyWayPointType.WP_DESPAWN, owner.GetOrigin()); 
				break;
			}
			case SDRC_EFlyWayPointType.WP_FLY_AWAY_IMMEDIATELY:	//NOTE: This is not a real state. When set, state will change to FLY_AWAY
			{
				//Handled in AddDestination() as this interrupts previous flight
				ResetAttack();
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
				
				//If we have a known enemy position, fly to it.
				if (m_vAttackPosition != vector.Zero)
				{
					AddFlyPathPoint(m_vAttackPosition);
				}
				
				break;
			}
			case SDRC_EFlyWayPointType.WP_LAND:
				break;
			case SDRC_EFlyWayPointType.WP_LAND_VERTICAL:
			{
				SDRC_ChopperCompCore.ResetOriginalValues(owner);		//Reset heli settings
				
				SetState(SDRC_EHeliState.LAND_VERTICAL);
				SetTimeInState(60);								//Just set some timer value. One minute for the max time.

				//Clear flight as we are adding the points ourselves.
				ResetFlight();

				vector pos = m_vOrigin;
				pos[1] = SDRC_Misc.GetSurfaceYWithWater(m_vOrigin, true, owner) - 1;	//Set the point slightly below surface level
				//Set the landing distance to be from the helicopter height to slightly below ground. This is needed for the touch down check.
				m_fLandingDistance = vector.Distance(m_vOrigin, pos);
				
				//Stop heli from moving
				m_bOnlyVerticalMovement = true;
/*				m_fSpeedMin = 0.01;
				m_fSpeedMax = 0.01;*/
				m_fSpeedSlowingMul = 0.1;	//Make the heli stay upright
				
				for (int i = 0; i < VERTICAL_SPLINE_POINTS; i++)
				{
					m_vSplinePoints.Insert(pos);
				}
				m_iClosestIndex = 0;
				isRemoveDestination = true;
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
			case SDRC_EFlyWayPointType.WP_RAISE:
			{				
				SetState(SDRC_EHeliState.RAISE);
				
				//NOTE: We do not use AddDestination() for setting the flight. We just add points in the spline.				
				
				//Clear flight as we are adding the points ourselves.
				ResetFlight();
				
				//Reset heli settings
				SDRC_ChopperCompCore.ResetOriginalValues(owner);	//Reset heli settings				
				SetSpeedToChange(1.0, m_fSpeedMin);
				m_fThrottle = 2.4;
				
				//Fly forward
				if (m_vFlyDestinations[0].pt[0] == 0)
				{
					m_vFlyDestinations[0].pt[0] = params.destinationForward;
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
			case SDRC_EFlyWayPointType.WP_HOVER:
				//Fall through
			case SDRC_EFlyWayPointType.WP_HOVER_UP:
				//Fall through
			case SDRC_EFlyWayPointType.WP_HOVER_DOWN:
			{
				switch(nextType)
				{
					case SDRC_EFlyWayPointType.WP_HOVER:
					{
						SetState(SDRC_EHeliState.HOVER);
						break;
					}
					case SDRC_EFlyWayPointType.WP_HOVER_DOWN:
					{
						SetState(SDRC_EHeliState.HOVER_DOWN);
						break;
					}
					case SDRC_EFlyWayPointType.WP_HOVER_UP:
					{
						SetState(SDRC_EHeliState.HOVER_UP);
						break;
					}
				}
				
				SetTimeInState(m_vFlyDestinations[0].value);
				
				//NOTE: We do not use AddDestination() for setting the flight. We just add points in the spline.
				
				//Clear flight as we are adding the points ourselves.
				ResetFlight();
				
				//Reset heli settings
				SDRC_ChopperCompCore.ResetOriginalValues(owner);
				
				//Stop heli from moving
				m_bOnlyVerticalMovement = true;
				m_fSpeedSlowingMul = 0.1;	//Make the heli stay upright
				
				for (int i = 0; i < VERTICAL_SPLINE_POINTS; i++)
				{
					vector pos;
					//Add a few points forward
					pos = SDRC_ChopperHelper.GetDestinationForward(owner, 0.5 * i);
					pos[1] = pos[1] + m_vFlyDestinations[0].pt[1];		//Hover above original point
					m_vSplinePoints.Insert(pos);
				}
				m_iClosestIndex = 0;
				
				isRemoveDestination = true;
				break;
			}
			case SDRC_EFlyWayPointType.WP_BRAKE:
			{
				SetState(SDRC_EHeliState.BRAKE);			
				m_bIsBraking = false;
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
			case SDRC_EFlyWayPointType.WP_CRASH:
			{
				SetState(SDRC_EHeliState.CRASH);
				//NOTE: The final height will be set in SetFlightPointHeight
				m_bIsCrashing = false;
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
			case SDRC_EFlyWayPointType.WP_SEARCH_DESTROY:
				break;
			case SDRC_EFlyWayPointType.WP_RESET:
				break;
			case SDRC_EFlyWayPointType.WP_CUT:
				break;
			case SDRC_EFlyWayPointType.WP_DESTROY:
				TypeSetHealthScaled(owner, 0);
				break;
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
	// States 
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Handle state (machine)
	*/
	override private void HandleState(IEntity owner, float timeSlice)
	{	
		switch (m_eHeliState)
		{
			
			case SDRC_EHeliState.ATTACK:
			case SDRC_EHeliState.FLY:
			{
				TypeHandleAttack(owner);
				break;
			}
			case SDRC_EHeliState.LAND_VERTICAL:
			{
				HandleLandingVertical(owner, timeSlice);	
				break;
			}
			case SDRC_EHeliState.BRAKE:
			{
				HandleBraking(owner, timeSlice);	
				break;
			}
			case SDRC_EHeliState.CRASH:
			{
				HandleCrashing(owner, timeSlice);	
				break;
			}
			case SDRC_EHeliState.GET_OUT:
			{
				SetNextState(owner);
				break;				
			}			
		}
		
		//Wait for the state timer to end and go to next state
		if (    (m_eHeliState != SDRC_EHeliState.FLY) 			//We do not automatically change state when flying
		     //&& (m_eHeliState != SDRC_EHeliState.RAISE) 		//..or raising
		     && (m_fTimeInStateLeft < 0) && m_bTimeInStateEnabled) 
		{
				SetNextState(owner);
		}
	}	
	
	//------------------------------------------------------------------------------------------------	
	// Behaviour
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	override SDRC_EHeliBehaviour GetBehaviour()
	{
		return m_eHeliBehaviour;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Sets the behaviour.
	\param time (seconds) How long to stay in behaviour. -1 = infinite
	*/	
	override void SetBehaviour(SDRC_EHeliBehaviour behaviour, int time)
	{
		//If we're in evac state, this is considered to be the last state. Do not change anything.
		if (GetBehaviour() == SDRC_EHeliBehaviour.EVAC_BEHAVIOUR)
		{
			return;
		}
		
		m_eHeliBehaviour = behaviour;
		
		//Reset timer for NORMAL
		if (behaviour == SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR)
		{			
			time = 0;
			m_fTimerBehaviourCycle = params.behaviourCycleTime;
		}
				
		//If time is set as -1, make time veeeeery long.
		if (time == -1)
		{
			time = 10000000;
		}
		
		SDRC_Log.Add("[SDRC_ChopperComp:SetBehaviour] Setting: " + SCR_Enum.GetEnumName(SDRC_EHeliBehaviour, behaviour), LogLevel.SPAM);
		
		m_fTimerBehaviour = time;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Handle behaviour
	- Normal case Fly and react normally
	- Active case A behaviour cycle is run every BEHAVIOUR_CHECK_CYCLE seconds. If we're in a behaviour, 
				after this time, we check if there is a need to change the behaviour. This is quite rapid 
				checking
	*/
	override private void HandleBehaviour(IEntity owner)
	{
		//Return to normal state
		if ( (m_fTimerBehaviour < 0) && (GetBehaviour() != SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR) )
		{
			//Normal case
			SetBehaviour(SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR, -1);
//			m_eHeliBehaviour = SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR;
			ResetAttack();
			return;
		}

		//When in EVAC or PASSIVE behaviour, stay there.
		if ( (m_eHeliBehaviour == SDRC_EHeliBehaviour.PASSIVE_BEHAVIOUR) 
		  || (m_eHeliBehaviour == SDRC_EHeliBehaviour.EVAC_BEHAVIOUR) 
		   )
		{
			return;
		}		
				
		//Handle behaviour in cycles of behaviourCycleTime seconds
		if (m_fTimerBehaviourCycle > 0)
		{
			return;
		}
		
		//Reset behaviour cycle timeout
		m_fTimerBehaviourCycle = params.behaviourCycleTime;

		//Do enemy search
		SetAttackPosition(vector.Zero);	//NOTE: This will not reset m_fAttackPositionSetTime if we're still S&D state
		SDRC_ChopperEnemyHelper.SearchForEnemy(owner);
		
		//If enemy found, enter S&D behaviour in case we're in normal behaviour. 
		//If we're passive, doing evac or .. we don't want S&D to happen.
		if ( (m_vAttackPosition != vector.Zero) && (GetBehaviour() == SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR) )
		{
			//If yes, become aggressive and/or reset timer.
			SetBehaviour(SDRC_EHeliBehaviour.SEARCH_AND_DESTROY_BEHAVIOUR, params.timeSearchAndDestroy);
			//When an enemy is found, set the behaviour timer to enemyKnownTime. This way a new enemy is searched only after it's forgotten.
			//m_fTimerBehaviourCycle is set slightly after the enemy is forgotten.
			m_fTimerBehaviourCycle = params.enemyKnownTime + 1;
			SDRC_Log.Add("[SDRC_ChopperComp:HandleBehaviour] Enemy found. Changing to S&D behaviour." + m_vAttackPosition, LogLevel.DEBUG);
		}			
						
		switch (GetBehaviour())
		{
			case SDRC_EHeliBehaviour.SEARCH_AND_DESTROY_BEHAVIOUR:
			{
				//Set attack position. This also resets the m_fAttackPositionSetTime		
				//SetAttackPosition(m_vAttackPosition);
				
				if ( (m_vAttackPosition != vector.Zero) && (m_fAttackPositionSetTime == params.enemyKnownTime) )
				{
					TypeAttackSetup(owner, m_vAttackPosition);
					SDRC_Log.Add("[SDRC_ChopperComp:HandleBehaviour] S&D: Enemy found, attacking: " + m_vAttackPosition, LogLevel.NORMAL);
				}
				else
				{
					if ( (m_fAttackPositionSetTime <= 0) && (m_vAttackPositionOld != vector.Zero) )
					{					
						//If no enemy, add another patrol round
						if (SDRC_ChopperHelper.GetNextWayPointType(owner) != SDRC_EFlyWayPointType.WP_PATROL_ONCE)
						{
							AddDestination(SDRC_EFlyWayPointType.WP_PATROL_ONCE, m_vAttackPositionOld, index: 0);		//NOTE: This is set as first waypoint							
						}
						//We consider the spot 
						m_fTimerBehaviourCycle = params.enemyKnownTime;
					}
				}
				break;
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
	override private void HandleLandingVertical(IEntity owner, float timeSlice)
	{
		vector origin = owner.GetOrigin();
				
		vector lastPt = m_vSplinePoints[m_vSplinePoints.Count() - 1];
		float distance = vector.Distance(origin, lastPt);
		
		if (distance < m_fLandingDistance)
		{
			if (m_Helicopter_s.HasAnyGroundContact())
			{			
				HandleGroundContact(owner);		
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
		float distance = vector.DistanceXZ(m_vOrigin, lastPt);
		
		if (distance < m_fBrakingDistance)
		{
			if (!m_bIsBraking)
			{
				m_fSpeedBrakingOrig = m_fSpeed * 0.7;	//0.7 is just a good value :-)
				m_fPositionBrakingOrig = m_vOrigin;
							
				//This value is calculated depending on the speed. The higher the speed, the bigger the multiplier needs to be.
				// <30 = value smaller than 1.0
				//	30 = 1.00 (orig: 1.00)
				//	40 = 1.49 (orig: 1.50)
				//	50 = 2.93 (orig: 2.20)
				//	60 = 4.78 (orig: 4.80)
				
				const float MAGIC_NUMBER = 21;	// 21 = 30 * 0.7
				
				if (m_fSpeedBrakingOrig < MAGIC_NUMBER)
				{
					m_fSpeedBrakingMul = m_fSpeedBrakingOrig / MAGIC_NUMBER;
				}
				else
				{
					float var = (m_fSpeedBrakingOrig - MAGIC_NUMBER) / MAGIC_NUMBER;			
					m_fSpeedBrakingMul = 1 + 4.5 * Math.Sin(var * var);		//The formula creates a semi exponentially growing value
				}
				
				//SDRC_Log.Add("[SDRC_ChopperComp:HandleBraking] m_fSpeedBrakingMul: " + m_fSpeedBrakingMul, LogLevel.DEBUG);
				
				//m_fSpeedBrakingMul = 3.15;
				
				//We have started landing sequence so no need to count values
				m_bIsBraking = true;
			}
			else
			{
				
				float distMul = (distance / (m_fBrakingDistance * m_fSpeedBrakingMul) ) * (m_fSpeed / m_fSpeedBrakingOrig);
				m_fSpeedTarget = m_fSpeedBrakingOrig * distMul;
				m_fSpeedTarget = Math.Clamp(m_fSpeedTarget, 1.0, 100);	//Clamp to have some speed forward
				
				//If we have passed the point, adjust values
				if (SDRC_Math.HasPassedPointXZ(m_fPositionBrakingOrig, lastPt, owner.GetOrigin()))
				{
					m_fSpeedTarget = 0.001;
					distMul = 0;
				}
				
				m_fSpeedMin = 0.001;
				
				//This affects yaw-pitch-roll counting in SetTurn
				m_fSpeedSlowingMul = distMul;
									
				if ( (distMul < 0.001) || (distance < BRAKING_DISTANCE_END) )
				{
					SetNextState(owner);
				}
			}
		}
	}	
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Handle crashing
	*/
	override private void HandleCrashing(IEntity owner, float timeSlice)
	{
		if (!m_bIsCrashing)
		{
			m_fPositionCrashingOrig = m_vOrigin;

			//Disable effect of rotors
			if (m_Helicopter_s)
			{
		        m_Helicopter_s.RotorSetForceScaleState(0, 0);
		        m_Helicopter_s.RotorSetForceScaleState(1, 0);
		        m_Helicopter_s.SetThrottle(0);
			}
						
			//We have started landing sequence so no need to count values
			m_bIsCrashing = true;
		}
		else
		{
			//If we have passed the point, go to next state
			if (SDRC_Math.HasPassedPointXZ(m_fPositionCrashingOrig, m_vSplinePoints[m_vSplinePoints.Count() - 1], owner.GetOrigin()))
			{
				SetNextState(owner);				
			}			
			else if (m_Helicopter_s)
			{
				//Test ground contact
				if (m_Helicopter_s.HasAnyGroundContact())
				{			
					SetNextState(owner);			
				}			
			}
			else 
			{
				//If no component to use for ground contact, check with altitude
				if ( m_vOrigin[1] < (SDRC_Misc.GetSurfaceYWithWater(m_vOrigin) + 1.0) )
				{
					SetNextState(owner);			
				}				
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Handle ground contact
	*/
	private void HandleGroundContact(IEntity owner)
	{
		//SDRC_Log.Add("[SDRC_ChopperComp:HandleLanding] Ground contact!", LogLevel.DEBUG);
		//Disable effect of rotors
		if (m_Helicopter_s)
		{		
	        m_Helicopter_s.RotorSetForceScaleState(0, 0);
	        m_Helicopter_s.RotorSetForceScaleState(1, 0);
	        m_Helicopter_s.SetThrottle(0);
		}
		//Set values to stop moving
		m_fSpeedTarget = 0.0001;
		m_fSpeedSlowingMul = 0;
		m_fRotorForceMultiplier = 0;
		SetNextState(owner);
	}		
}