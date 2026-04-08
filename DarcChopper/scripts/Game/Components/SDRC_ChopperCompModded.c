//SDRC_ChopperCompModded.c

//------------------------------------------------------------------------------------------------
//class SDRC_ChopperComp : ScriptGameComponent
modded class SDRC_ChopperComp
{
	//------------------------------------------------------------------------------------------------	
	// Helicopter settings
	//------------------------------------------------------------------------------------------------	

	//------------------------------------------------------------------------------------------------
	void SetSpeed(float min = -1, float max = -1)
	{
		if (min > -1)
		{
			m_fSpeedMin = min;
		}
		
		if (max > -1)
		{
			m_fSpeedMax = max;
		}			
	}

	//------------------------------------------------------------------------------------------------
	void SetAutostart(bool value)
	{
		m_bAutoStart = value;
	}
	
	//------------------------------------------------------------------------------------------------	
	override void SetHeli(float speedMin, float speedMax, float flyHeightLow, float flyHeightHigh, float distanceLow, float distanceHigh)
	{
		SDRC_Log.Add("[SDRC_ChopperComp:SetHeli] Updating values.", LogLevel.DEBUG);
	
		m_fSpeedMin = speedMin;
		m_fSpeedMax = speedMax;
		m_fFlyHeightLow = flyHeightLow;
		m_fFlyHeightHigh = flyHeightHigh;
		m_fDistanceLow = distanceLow;
		m_fDistanceHigh = distanceHigh;
		
		//Save the original values
		SDRC_ChopperCompCore.StoreOriginalValues(GetOwner());
		
		m_fSpeed = m_fSpeedMin;
	}	

	//------------------------------------------------------------------------------------------------	
	override void SetEngine(bool engine, float throttle, float rotorForce0, float rotorForce1)
	{
		VehicleHelicopterSimulation heliSimulation = VehicleHelicopterSimulation.Cast(GetOwner().GetRootParent().FindComponent(VehicleHelicopterSimulation));
		if (heliSimulation)
		{
			if (engine)
			{
				heliSimulation.EngineStart();
			}
			else
			{
				heliSimulation.EngineStop();
			}
	        
	        heliSimulation.SetThrottle(throttle);
	        heliSimulation.RotorSetForceScaleState(0, rotorForce0);
	        heliSimulation.RotorSetForceScaleState(1, rotorForce1);		
		}
	}
		
	//------------------------------------------------------------------------------------------------	
	// Enemy related
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	/*!
	Enable/Disable enemy searching
	*/		
	override void SetEnemySearchType(SDRC_EHeliEnemySearchType type)
	{
		m_EnemySearchType = type;
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!
	Get last known enemy position
	*/
	vector GetEnemyPosition()
	{
		return m_vEnemyPosition;
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!
	Reset enemy knowledge and timeout
	*/
	void EnemyHandled()
	{
		m_vEnemyPosition = "0 0 0";
		m_iEnemyFoundTime = SDRC_Misc.GetCurrentTickTime() + m_iEnemyFoundTimeout;
	}	
	
	//------------------------------------------------------------------------------------------------	
	// Damage settings
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	override bool IsStillWorking(IEntity owner, bool inInit)
	{
		//If still in init, don't care if no pilots etc yet set.
		if (inInit)
		{
			return true;
		}

		//If working and at least one pilot, all good
		int pilotCount = SDRC_VehicleHelper.PilotCountAlive(owner);
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));		
		if (chopperComp)
		{
			if (chopperComp.m_bUnpiloted)
			{
				pilotCount = 1;
			}
		}

		float health;
		GetHealthScaled(owner, health);
		if ( health < 0.93 )
		{
			m_eDamageLevel = SDRC_EHeliDamageLevel.HEAVY;
		}	
		else if ( health < 0.97)
		{
			m_eDamageLevel = SDRC_EHeliDamageLevel.MEDIUM;
		}	
		else if ( health < 0.99)
		{
			m_eDamageLevel = SDRC_EHeliDamageLevel.LIGHT;
		}			
				
		switch (m_EntityType)
		{
			case SDRC_EChopperType.HELICOPTER:		
			{
				//If damage is high, evac!
				if ( ( (m_eDamageLevel == SDRC_EHeliDamageLevel.MEDIUM) || (m_eDamageLevel == SDRC_EHeliDamageLevel.HEAVY) ) && (GetBehaviour() != SDRC_EHeliBehaviour.EVAC) )
				{
					SetBehaviour(SDRC_EHeliBehaviour.EVAC, -1);
					AddDestination(SDRC_EFlyWayPointType.WP_M_RESET);
					AddDestination(SDRC_EFlyWayPointType.WP_M_EVAC_TROOPS, SDRC_Misc.RandomizePos(owner.GetOrigin(), 600));
				}		
				
				if ( SCR_AIVehicleUsability.VehicleCanMove(owner) && (pilotCount > 0) )
				{
					return true;	//Still working, RETURN!
				}
				break;
			}
			case SDRC_EChopperType.DRONE:		
			{
				if ( m_eDamageLevel != SDRC_EHeliDamageLevel.HEAVY )
				{
					return true;	//Still working, RETURN!
				}
				break;
			}
		}
		
		//If we get here, the damage is critical
		HandleDamageFinal(owner);
		
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (m_BaseGameMode)
		{
	 		if (m_BaseGameMode.chopperFrame)
			{
				m_BaseGameMode.chopperFrame.RemoveChopperFromList(owner);					
			}
		}
		
		return false;
	}	
	
	//------------------------------------------------------------------------------------------------	
	// Destination settings
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Add a destination for future.
	\param destination Next position to fly to. Multiple destinations can be defined by calling multiple times.
	\param type Flying behaviour
	\param value Specific value for behaviour
	\param index At which index on to put the destination. By default, added to the end of array.
	*/
	override void AddDestination(SDRC_EFlyWayPointType type = SDRC_EFlyWayPointType.WP_FLY, vector destination = vector.Zero, float value = -1, int index = -1)
	{
		//In normal case, we just add a destination for future handling. 
		//Below are a few special cases where we need either react immediately of change some other params.

		bool addDestinationPoint = true;
		
		switch (type)
		{
			//------------------------------------------------------------------------------------------------	
			// Normal actions
			//------------------------------------------------------------------------------------------------	
			case SDRC_EFlyWayPointType.WP_FLY_IMMEDIATELY:
			{
				//Fly immediately to a destination
				//Remove any existing destination
				ResetDestinations();
				SDRC_ChopperHelper.CutSpline(m_vSplinePoints, m_iClosestIndex);
				type = SDRC_EFlyWayPointType.WP_FLY;
				SetState(SDRC_EHeliState.FLY);
				break;
			}
			case SDRC_EFlyWayPointType.WP_FLY_AWAY_IMMEDIATELY:
			{
				//Fly away immediately
				ResetDestinations();
				SDRC_ChopperHelper.CutSpline(m_vSplinePoints, m_iClosestIndex);
				//NOTE: Will drop through WP_FLY_AWAY
			}		
			case SDRC_EFlyWayPointType.WP_FLY_AWAY:
			{
				//Fly away after all destinations have been handled
				if (destination == vector.Zero)
				{
					vector direction = vector.Direction(SDRC_Misc.GetWorldCenter(), GetOwner().GetOrigin() );
					destination = GetOwner().GetOrigin() + (direction.Normalized() * (float)SDRC_Misc.GetWorldSize());
				}
				
				SetState(SDRC_EHeliState.FLY_AWAY);
				break;
			}
			case SDRC_EFlyWayPointType.WP_ATTACK:
			{
				//Do attack on low altitude.				
				m_vAttackPosition = destination;			//Where to attack

				//With default attack time, set it to 60 seconds
				if (value == -1)
				{
					value = 60;
				}
								
				if (value < 30)
				{
					SDRC_Log.Add("[SDRC_ChopperComp:AddDestination] Time assigned to WP_ATTACK is very short: " + value + " seconds.", LogLevel.WARNING);
				}
				
				m_fTimerAttack = value;						//For how long to continue attacks
				break;
			}
			
			//These just fall through
			case SDRC_EFlyWayPointType.WP_GET_OUT:			//Handled in HandleState()
			case SDRC_EFlyWayPointType.WP_STOP_ENGINE:
			case SDRC_EFlyWayPointType.WP_LAND:
			case SDRC_EFlyWayPointType.WP_END:
			case SDRC_EFlyWayPointType.WP_PATROL:
			{
				break;
			}
			case SDRC_EFlyWayPointType.WP_SEARCH_DESTROY:
			{
				m_eHeliBehaviour = SDRC_EHeliBehaviour.SEARCH_AND_DESTROY;
				m_fTimerBehaviour = value;
				break;
			}
			
			//------------------------------------------------------------------------------------------------	
			//Macro actions
			//------------------------------------------------------------------------------------------------	
			case SDRC_EFlyWayPointType.WP_M_RESET:
			{
				ResetDestinations();
				SDRC_ChopperHelper.CutSpline(m_vSplinePoints, m_iClosestIndex);
				addDestinationPoint = false;
				break;
			}
			case SDRC_EFlyWayPointType.WP_M_LAND_TO_FREE_SPOT:
			case SDRC_EFlyWayPointType.WP_M_EVAC_TROOPS:
			{
				if (SDRC_ChopperHelper.GetSafeLandingPosition(destination, SAFE_LANDING_SIZE))
				{
					//Safe landing position found
					SDRC_DebugHelper.AddDebugPos(destination, ARGB(32, 64, 255, 64), SAFE_LANDING_SIZE, m_sDid, 10.0);				
					AddDestination(SDRC_EFlyWayPointType.WP_LAND, destination);
					AddDestination(SDRC_EFlyWayPointType.WP_GET_OUT);
					AddDestination(SDRC_EFlyWayPointType.WP_STOP_ENGINE);
					AddDestination(SDRC_EFlyWayPointType.WP_END);
				}
				else
				{
					//No safe landing position found
					AddDestination(SDRC_EFlyWayPointType.WP_FLY_AWAY_IMMEDIATELY);
				}
				//All things are already added
				addDestinationPoint = false;
				break;
			}
			case SDRC_EFlyWayPointType.WP_M_LAND_TROOPS:
			{
				AddDestination(SDRC_EFlyWayPointType.WP_LAND, destination);
				AddDestination(SDRC_EFlyWayPointType.WP_GET_OUT);
				
				int crewCount = SDRC_ChopperCrewHelper.CountCrew(GetOwner());
				crewCount = 5 + crewCount * 4;	//Give N seconds per AI plus additional time				
				
				AddDestination(SDRC_EFlyWayPointType.WP_WAIT, value : crewCount);
				vector hoverPos = vector.Zero;
				hoverPos[1] = m_fFlyHeightLow;
				AddDestination(SDRC_EFlyWayPointType.WP_HOVER_UP, hoverPos, 8);
				AddDestination(SDRC_EFlyWayPointType.WP_RAISE, "150 0 0");
				
				//All things are already added
				addDestinationPoint = false;
				break;
			}
			case SDRC_EFlyWayPointType.WP_M_ATTACK:
			{
				SDRC_Log.Add("[SDRC_ChopperComp:AddDestination] For modders: WP_M_ATTACK behaviour will change in the future. Use: WP_M_SUPPRESSIVE", LogLevel.WARNING);
			}
			case SDRC_EFlyWayPointType.WP_M_SUPPRESSIVE:
			{
				if (value == -1)
				{
					value = TIME_ATTACK_RUN;
				}
				
				//Do one attack
				AddDestination(SDRC_EFlyWayPointType.WP_ATTACK, destination, value);
				//Add random count of bombing runs
				int runCount = SDRC_Misc.RandomInt(0, 4);
				
				//Do multiple ones if requested
				for (int i = 0; i < runCount; i++)
				{
					float angle = SDRC_Misc.RandomFloat(0, 360);
					float distance = SDRC_Misc.RandomFloat(100, 200);
					vector rndPos = SDRC_Misc.GetCoordinatesOnCircle(destination, distance, angle);
					AddDestinationPoint(SDRC_EFlyWayPointType.WP_FLY, rndPos, value);
					distance = SDRC_Misc.RandomFloat(200, 400);
					rndPos = SDRC_Misc.GetCoordinatesOnCircle(destination, distance, angle + SDRC_Misc.RandomFloat(-120, 120));
					AddDestinationPoint(SDRC_EFlyWayPointType.WP_FLY, rndPos, value);
					AddDestinationPoint(SDRC_EFlyWayPointType.WP_ATTACK, destination, value);
				}
				m_vAttackPosition = destination;			//Where to attack
				//All things are already added
				addDestinationPoint = false;
				break;
			}
			case SDRC_EFlyWayPointType.WP_M_TESTING:
			{
				break;
			}			
		}

		if (addDestinationPoint)
		{
			AddDestinationPoint(type, destination, value, index);
		}
	}	

	//------------------------------------------------------------------------------------------------	
	/*!	
	Add a destination
	*/
	private void AddDestinationPoint(SDRC_EFlyWayPointType type, vector destination, float value, int index = -1)
	{
		SDRC_FlyPathPoint fpp = new SDRC_FlyPathPoint();
		fpp.Set(type, destination, value);				
		
		if (index > -1)
		{
			m_vFlyDestinations.InsertAt(fpp, index);
		}
		else
		{
			m_vFlyDestinations.Insert(fpp);
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
			case SDRC_EHeliState.LAND:
			{
				HandleLanding(owner, timeSlice);	
				break;
			}
			
			case SDRC_EHeliState.GET_OUT:
			{
				SetNextState(owner);
				break;				
			}			
		}
		
		//Wait for the state timer to end and go to next state
		if ( (m_eHeliState != SDRC_EHeliState.FLY) && (m_fTimeInState < 0) && m_bTimeInStateEnabled) 
//		if ( (m_fTimeInState < 0) && (m_bTimeInStateEnabled) )
		{
			SetNextState(owner);
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Handle behaviour
	- Normal case: Fly and react normally
	- Attack case: 
	*/
	override private void HandleBehaviour(IEntity owner)
	{
		if ( (m_eHeliBehaviour == SDRC_EHeliBehaviour.NORMAL) || (m_eHeliBehaviour == SDRC_EHeliBehaviour.EVAC) )
		{
			return;
		}
		
		if (m_fTimerBehaviour < 0)	
		{
			//Normal case:
			m_eHeliBehaviour = SDRC_EHeliBehaviour.NORMAL;
			return;
		}
		
		if (m_fTimerBehaviourCycle > 0)
		{
			return;
		}
		
		m_fTimerBehaviourCycle = 3;	//10 seconds between cycles
		
		switch (m_eHeliBehaviour)
		{
			case SDRC_EHeliBehaviour.SEARCH_AND_DESTROY:
			{
				vector enemyPos = SDRC_ChopperEnemyHelper.SearchEnemy(owner);
				
				if (enemyPos != vector.Zero)
				{
					SDRC_Log.Add("[SDRC_ChopperComp:HandleBehaviour] S&D set to: " + enemyPos, LogLevel.NORMAL);
					
					AddDestination(SDRC_EFlyWayPointType.WP_M_RESET);
					AddDestination(SDRC_EFlyWayPointType.WP_ATTACK, enemyPos);
					AddDestination(SDRC_EFlyWayPointType.WP_PATROL, enemyPos);
					m_fTimerBehaviourCycle = 30;
				}
				
				break;
			}
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Sets the next destination for an action. 
	- FLY will start to fly
	- Others will have some action bound to them.
	*/
	override private void SetNextState(IEntity owner, SDRC_EFlyWayPointType nextType = SDRC_EFlyWayPointType.WP_UNDEFINED, bool allowRemove = true)
	{
		//By default we remove the destination
		bool isRemoveDestination = false;
		
		nextType = SDRC_ChopperHelper.GetNextWayPointType(owner, nextType);

		switch (nextType)
		{
			case SDRC_EFlyWayPointType.WP_UNDEFINED:
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
				//NOTE: We do not use AddDestination() for setting the flight. We just add one point in the spline
				
				SDRC_ChopperCompCore.ResetOriginalValues(owner);		//Reset heli settings
				//For raise, we add points to the spline
				ResetFlight();
				
				m_vSplinePoints.Insert(owner.GetOrigin());
				
				//Fly forward
				vector pos = SDRC_ChopperHelper.GetDestinationForward(owner, m_vFlyDestinations[0].pt[0]);				
				pos[1] = SDRC_Misc.GetSurfaceYWithWater(pos) + m_fFlyHeightLow + 5;			//Fly to a point slightly above low fly point
				
				for (int i = 0; i < 10; i++)
				{
					m_vSplinePoints.Insert(vector.Lerp(owner.GetOrigin(), pos, i/10));
				}
				m_iClosestIndex = 3;				
				SetState(SDRC_EHeliState.FLY);
				
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
				SetState(SDRC_EHeliState.DESTROYED);
				m_vSplinePoints.Clear();
				isRemoveDestination = true;
				//TBD: DESPAWN!!
				break;
			}
			case SDRC_EFlyWayPointType.WP_GET_OUT:
			{
				SDRC_ChopperCrewHelper.GetOut(owner);
				SetState(SDRC_EHeliState.GET_OUT);
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
			case SDRC_EFlyWayPointType.WP_HOVER_UP:
			case SDRC_EFlyWayPointType.WP_HOVER:
			{
				//NOTE: We do not use AddDestination() for setting the flight. We just add one point in the spline
				
				//Reset heli settings
				SDRC_ChopperCompCore.ResetOriginalValues(owner);
				
				//Stop heli from moving
				m_fSpeedMin = 0.3;
				m_fSpeedMax = 0.6;
				m_fSpeedLandingMul = 0;
				
				//For hovering, we add points to the spline
				ResetFlight();
				
				vector pos = owner.GetOrigin();
				pos[1] = pos[1] + m_vFlyDestinations[0].pt[1];		//Hover above original point
				
				for (int i = 0; i < 10; i++)
				{
					m_vSplinePoints.Insert(pos);
				}
				m_iClosestIndex = 0;
				//CreateNewFlight(owner, firstDestination);
				
				//Just wait.
				SetState(SDRC_EHeliState.HOVER);
				SetTimeInState(m_vFlyDestinations[0].value);
				
				SDRC_ChopperDebug.DrawDebugPaths(owner);
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
				break;
			}
		}
		
		//Remove the destination if it was handled.	By default it is.
		if ( (isRemoveDestination) && (allowRemove) )
		{
			m_vFlyDestinations.RemoveOrdered(0);
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
				m_fSpeedLandingMul = distMul;
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
				m_fSpeedLandingMul = 0;
				m_fRotorForceMultiplier = 0;
				SetNextState(owner);
			}
		}
	}		
}