//SDRC_ChopperCompModded.c

//------------------------------------------------------------------------------------------------
//class SDRC_ChopperComp : ScriptGameComponent
modded class SDRC_ChopperComp
{
	const int DEFAULT_ATTACK_TIME = 60;			//(seconds) The time to stay in attack mode
	const int DEFAULT_BRAKE_DISTANCE = 200;		//Default distance to brake
	
	//------------------------------------------------------------------------------------------------	
	// Chopper setup
	//------------------------------------------------------------------------------------------------	

	//------------------------------------------------------------------------------------------------
	/*!
	Do setup. This is delayed in case another mod has disabled autostart
	Executed only on server!
	*/	
	override void Setup(IEntity owner)
	{
		GetGame().GetCallqueue().CallLater(Setup_Delayed, TIME_DELAY_READY * 1000, false, owner);		
		
	}	
	
	void Setup_Delayed(IEntity owner)
	{
		if (m_bSetupDone)
		{
			return;
		}
		
		m_bSetupDone = true;
		
		TypeSetup(owner);
		
		if (m_bAutoStart)
		{					
			Ready(owner);
		}
		
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (m_BaseGameMode)
		{
	 		if (m_BaseGameMode.chopperFrame)
			{
				m_BaseGameMode.chopperFrame.AddChopperToList(owner);
			}
		}		
		
		//IMPORTANT: In a mod, you need to call Ready() yourself after Setup() in case you don't have autostart enabled!!!
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Once all init things are done, activate the component after a small delay
	*/	
	void Ready(IEntity owner)
	{
		SDRC_Log.Add("[SDRC_ChopperComp:Ready] Called..", LogLevel.DEBUG);
		
		//Add fly destinations added to prefab to the beginning of array.
/*		array<ref SDRC_FlyPathPoint> flyDestinationsTemp = {};
		foreach(SDRC_FlyPathPoint fpp : m_vFlyDestinationsOnPrefab)
		{
			flyDestinationsTemp.Insert(fpp);
		}*/
		
		// Some things needs to be done delayed
		if (m_bAutoStart)
		{
			//Init flight path
			InitFlight(owner);
		}

		//Any assigned destination on the prefab will be added to the end
		foreach(SDRC_FlyPathPoint fpp : m_vFlyDestinationsOnPrefab)
		{
			AddDestination(fpp.type, fpp.pt, fpp.value);
		}		

		//Prepare the first flight spline
		SDRC_ChopperHelper.SetFlightPointHeight(owner);
		
		//Create points for spline
		CreateFlightPoints(owner, true);
		
		array<vector> flyPathPoints = {};
		SDRC_ChopperDebug.GivePoints(flyPathPoints, m_vFlightPoints);
		SDRC_Spline3D.GenerateSplinePoints(flyPathPoints, m_vSplinePoints, -1);
		
		//Set final values		
		m_iClosestIndex = 5;
		m_iOldClosestIndex = m_iClosestIndex;
		
		//Check that points are above ground
		SDRC_ChopperHelper.SetSplinePointsAboveGround(owner);
		
		m_fSpeed = 0.1;
		m_fSpeedTarget = m_fSpeed;		
						
		SDRC_ChopperDebug.DrawDebugPaths(owner);		

		SetEventMask(owner, EntityEvent.FRAME);
		Activate(owner);
				
		//Set ready in a few seconds
		GetGame().GetCallqueue().CallLater(ReadyDelayed, TIME_DELAY_READY * 1000, false, owner);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Delayed spawn of AI crew	
	*/
	void ReadyDelayed(IEntity owner)
	{
		// Some things needs to be done delayed
		if (m_bAutoStart)
		{
			//Spawn crew 
			if (!m_bUnpiloted)
			{
				int crewCount = SDRC_ChopperCrewHelper.SpawnCrew(owner, m_CargoSeatFill, m_aCrew, m_sFaction, m_AISkill, m_AIPerception);
				SDRC_Log.Add("[SDRC_ChopperComp] Crew count: " + crewCount, LogLevel.DEBUG);
			}
			else
			{
				//NOTE: SDRC_ChopperCrewHelper.SpawnCrew will set the faction. Make sure you set 
				//		faction for unpiloted ones somewhere else.
			}
		}		
		
		GetGame().GetCallqueue().CallLater(ReadyDelayed_2, TIME_DELAY_READY * 1000, false, owner);		
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Delayed group counting, flight init and activation	
	*/
	void ReadyDelayed_2(IEntity owner)
	{
		if (!owner)
		{
			return;
		}
		
		if (owner)	//In case the owner was deleted before we ended up here
		{
			SetEventMask(owner, EntityEvent.INIT);
			
			GetGame().GetCallqueue().CallLater(InitDone, TIME_IN_INIT * 1000, false, owner);
		}
	}		
	
	//------------------------------------------------------------------------------------------------	
	// Helicopter deletion and despawn
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to component
	*/
	override void OnDelete(IEntity owner)
	{
		//TBD: Remove from helilist
		
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (m_BaseGameMode)
		{
	 		if (m_BaseGameMode.chopperFrame)
			{
				m_BaseGameMode.chopperFrame.RemoveChopperFromList(owner);
			}
		}		
		
		SDRC_Log.Add("[SDRC_ChopperComp:OnDelete] Deleting: " + owner, LogLevel.DEBUG);
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Do despawn related things
	*/	
	override void DeSpawn(IEntity owner)
	{
		//Despawn AI
		SDRC_ChopperCrewHelper.DespawnCrew(owner);		
		Deactivate(owner);
		SDRC_Log.Add("[SDRC_ChopperComp:DeSpawn] Despawning: " + owner, LogLevel.DEBUG);
		delete owner;
	}
	
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
		
		if (!m_bPilotCountTrusted)
		{
			if (pilotCount == 0)
			{
				pilotCount = 1;
			}
			else
			{
				//We found one, we can trust the pilot count
				m_bPilotCountTrusted = true;
				SDRC_Log.Add("[SDRC_ChopperComp:InitDone] Pilot count can be trusted.", LogLevel.DEBUG);
				//Recalculate the groups
				if (m_aGroups.IsEmpty())
				{
					SDRC_VehicleHelper.GroupFindAll(owner, m_aGroups);
				}
			}
		}

		if (m_bUnpiloted)
		{
			pilotCount = 1;
			m_bPilotCountTrusted = true;
		}
				
/*		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));		
		if (chopperComp)
		{
			if (chopperComp.m_bUnpiloted)
			{
				pilotCount = 1;
			}
		}*/

		float health;
		TypeGetHealthScaled(owner, health);
		
		if ( health <= params.damageHeavy )
		{
			m_eDamageLevel = SDRC_EHeliDamageLevel.HEAVY;
		}	
		else if ( health < params.damageMedium)
		{
			m_eDamageLevel = SDRC_EHeliDamageLevel.MEDIUM;
		}	
		else if ( health < params.damageLight)
		{
			m_eDamageLevel = SDRC_EHeliDamageLevel.LIGHT;
		}			
				
		switch (m_EntityType)
		{
			case SDRC_EChopperType.HELICOPTER:		
			{
				//If damage is high, evac!
				if ( ( (m_eDamageLevel == SDRC_EHeliDamageLevel.MEDIUM) || (m_eDamageLevel == SDRC_EHeliDamageLevel.HEAVY) ) && (GetBehaviour() != SDRC_EHeliBehaviour.EVAC_BEHAVIOUR) )
				{
					SetBehaviour(SDRC_EHeliBehaviour.EVAC_BEHAVIOUR, -1);
					AddDestination(SDRC_EFlyWayPointType.WP_RESET);
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
			case SDRC_EChopperType.FIXEDWING:
			{
				if ( m_eDamageLevel != SDRC_EHeliDamageLevel.HEAVY )
				{
					return true;	//Still working, RETURN!
				}
				break;
			}
		}
		
		//If we get here, the damage is critical
		TypeHandleDamageFinal(owner);
		
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
			// Normal actions without the need for any additional handling
			// These just fall through and do a basic AddDestinationPoint()
			//------------------------------------------------------------------------------------------------				
			case SDRC_EFlyWayPointType.WP_GET_OUT:			//Handled in HandleState()
			case SDRC_EFlyWayPointType.WP_END:
			case SDRC_EFlyWayPointType.WP_DESPAWN:
			case SDRC_EFlyWayPointType.WP_STOP_ENGINE:
			case SDRC_EFlyWayPointType.WP_LAND_VERTICAL:
			case SDRC_EFlyWayPointType.WP_PATROL:
			case SDRC_EFlyWayPointType.WP_PATROL_ONCE:
			case SDRC_EFlyWayPointType.WP_RAISE:
			case SDRC_EFlyWayPointType.WP_WAIT_GETOUT:
			case SDRC_EFlyWayPointType.WP_CRASH:
			{
				break;
			}
			case SDRC_EFlyWayPointType.WP_WAIT:
			case SDRC_EFlyWayPointType.WP_HOVER:
			case SDRC_EFlyWayPointType.WP_HOVER_UP:
			case SDRC_EFlyWayPointType.WP_HOVER_DOWN:
			{
				if (value < 1)
				{
					SDRC_Log.Add("[SDRC_ChopperComp:AddDestination] Time assigned to " + SCR_Enum.GetEnumName(SDRC_EFlyWayPointType, type) + " is very short: " + value + " seconds.", LogLevel.WARNING);					
				}
				break;
			}
						
			//------------------------------------------------------------------------------------------------	
			// Normal actions
			//------------------------------------------------------------------------------------------------	
			case SDRC_EFlyWayPointType.WP_FLY_IMMEDIATELY:
			{
				//Fly immediately to a destination
				//Remove any existing destination
				ResetDestinations();
				SDRC_ChopperCompCore.ResetOriginalValuesComp(this);
				SDRC_ChopperHelper.CutSplineTail(m_vSplinePoints, m_iClosestIndex);
				type = SDRC_EFlyWayPointType.WP_FLY;
				SetState(SDRC_EHeliState.FLY);
				break;
			}
			case SDRC_EFlyWayPointType.WP_FLY_AWAY_IMMEDIATELY:
			{
				//Fly away immediately
				ResetDestinations();
				SDRC_ChopperCompCore.ResetOriginalValuesComp(this);
				SDRC_ChopperHelper.CutSplineTail(m_vSplinePoints, m_iClosestIndex);
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
				
				SetBehaviour(SDRC_EHeliBehaviour.PASSIVE_BEHAVIOUR, -1);	//Become passive once flying away.
				SetState(SDRC_EHeliState.FLY_AWAY);
				break;
			}
			case SDRC_EFlyWayPointType.WP_ATTACK:
			{
				//Set attack position. At this stage, it could be at 0 height
				m_vAttackPosition = destination;
				
				//For drone, the attack position needs to a bit further than the one defined. We want a fly by towards or over the player.
				if (params.type == SDRC_EChopperType.DRONE)
				{				
					//Move it along the flight path.
					vector direction = vector.Direction(m_vSplinePoints[m_vSplinePoints.Count() - 1], destination);
					direction.Normalize();
					direction[1] = 0;		//Move only on XZ plane
					m_vAttackPosition = destination + (direction * 100);
					destination = m_vAttackPosition;
				}				

				//Set attack position on ground, unless some other height was defined.
				if (m_vAttackPosition[1] == 0)
				{
					m_vAttackPosition[1] = SDRC_Misc.GetSurfaceYWithWater(destination, true);
				}
				
				//With default attack time, set it to DEFAULT_ATTACK_TIME seconds
				if (value == -1)
				{
					value = DEFAULT_ATTACK_TIME;
				}
								
				if (value < 30)
				{
					SDRC_Log.Add("[SDRC_ChopperComp:AddDestination] Time assigned to WP_ATTACK is very short: " + value + " seconds.", LogLevel.WARNING);
				}
				
				m_fTimerAttack = value;						//For how long to continue attacks
				break;
			}
			case SDRC_EFlyWayPointType.WP_SEARCH_DESTROY:
			{
				value = params.timeSearchAndDestroy;
				m_vAttackPosition = destination;			//Where to attack
				break;
			}								
			case SDRC_EFlyWayPointType.WP_BRAKE:
			{
				if (value == -1)
				{
					if (!m_vFlyDestinations.IsEmpty() )
					{
						vector pos = m_vFlyDestinations[m_vFlyDestinations.Count() - 1].pt;
						value = Math.AbsFloat(vector.DistanceXZ(pos, destination)) / 1.5;
					}
					else					
					{
						value = DEFAULT_BRAKE_DISTANCE;
					}
				}
				
				if (value < 50)
				{
					SDRC_Log.Add("[SDRC_ChopperComp:AddDestination] The distance (value) for WP_BRAKE is very short: " + value, LogLevel.WARNING);
				}
				
				m_fBrakingDistance = value;
				break;
			}
			case SDRC_EFlyWayPointType.WP_CUT:
			{
				SDRC_ChopperHelper.CutSplineTail(m_vSplinePoints, m_iClosestIndex);
				addDestinationPoint = false;
				break;
			}
			case SDRC_EFlyWayPointType.WP_RESET:
			{
				ResetDestinations();
				SDRC_ChopperCompCore.ResetOriginalValuesComp(this);
				SDRC_ChopperHelper.CutSplineTail(m_vSplinePoints, m_iClosestIndex);
				addDestinationPoint = false;
				break;
			}
			
			//------------------------------------------------------------------------------------------------	
			//Macro actions
			//------------------------------------------------------------------------------------------------	
			case SDRC_EFlyWayPointType.WP_LAND:
				SDRC_Log.Add("[SDRC_ChopperComp:AddDestination] WP_LAND is deprecated. Use WP_M_LAND instead.", LogLevel.WARNING);
			case SDRC_EFlyWayPointType.WP_M_LAND:
			{
				//Get the distance from last spline point to the braking destination
				float distance = DEFAULT_BRAKE_DISTANCE;
				if (!m_vSplinePoints.IsEmpty())
				{
					distance = vector.DistanceXZ(m_vSplinePoints[m_vSplinePoints.Count() - 1], destination);
				}
				AddDestination(SDRC_EFlyWayPointType.WP_BRAKE, destination, distance);
				AddDestination(SDRC_EFlyWayPointType.WP_LAND_VERTICAL, destination, value);
//				SDRC_ChopperHelper.CutSplineTail(m_vSplinePoints, m_iClosestIndex);
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
					AddDestination(SDRC_EFlyWayPointType.WP_M_LAND, destination);
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
				AddDestination(SDRC_EFlyWayPointType.WP_M_LAND, destination);
				AddDestination(SDRC_EFlyWayPointType.WP_GET_OUT);
				AddDestination(SDRC_EFlyWayPointType.WP_WAIT_GETOUT);
				vector hoverPos = vector.Zero;
				hoverPos[1] = m_fFlyHeightLow * 0.7;		//We don't want to raise to exactly same position
				AddDestination(SDRC_EFlyWayPointType.WP_HOVER_UP, hoverPos, 5);
				vector raisePos = "300 0 0";
				raisePos[1] = m_fFlyHeightHigh;
				AddDestination(SDRC_EFlyWayPointType.WP_RAISE, raisePos);
				
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
					rndPos[1] = 0;	//Zero height to get a random height in SetFlightPointHeight()
					AddDestinationPoint(SDRC_EFlyWayPointType.WP_FLY, rndPos, value);
					distance = SDRC_Misc.RandomFloat(200, 400);
					rndPos = SDRC_Misc.GetCoordinatesOnCircle(destination, distance, angle + SDRC_Misc.RandomFloat(-120, 120));
					rndPos[1] = 0;	//Zero height to get a random height in SetFlightPointHeight()
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
		//!!!!
		//NOTE: If height (destination[1]) is 0, make it to current heli height
		//!!!!
		if (destination[1] == -999)
		{
			vector origin = GetOwner().GetOrigin();
			destination[1] = origin[1];
		}
		
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
	/*!	
	Handle behaviour
	- Normal case: Fly and react normally
	- Active case: A behaviour cycle is run every BEHAVIOUR_CHECK_CYCLE seconds. If we're in a behaviour, 
				after this time, we check if there is a need to change the behaviour. This is quite rapid 
				checking
	*/
	override private void HandleBehaviour(IEntity owner)
	{
		const int BEHAVIOUR_CHECK_CYCLE = 2;
		
		//When in EVAC or PASSIVE mode, stay there.
		if ( (m_eHeliBehaviour == SDRC_EHeliBehaviour.PASSIVE_BEHAVIOUR) || (m_eHeliBehaviour == SDRC_EHeliBehaviour.EVAC_BEHAVIOUR) )
		{
			return;
		}
		
		//Only when we're flying, do things.
		if (GetState() != SDRC_EHeliState.FLY)
		{
			return;
		}
		
		if ( (m_fTimerBehaviour < 0) && (GetBehaviour() != SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR) )
		{
			//Normal case:
			m_eHeliBehaviour = SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR;
			return;
		}
		
		if (m_fTimerBehaviourCycle > 0)
		{
			return;
		}
		
		m_fTimerBehaviourCycle = BEHAVIOUR_CHECK_CYCLE;

		//Let's check if we have an enemy near by. 
		if (m_vEnemyPosition != vector.Zero)
		{
			//If yes, become aggressive and/or reset timer.
			SetBehaviour(SDRC_EHeliBehaviour.SEARCH_AND_DESTROY_BEHAVIOUR, params.timeSearchAndDestroy);
		}			
						
		switch (m_eHeliBehaviour)
		{
			case SDRC_EHeliBehaviour.SEARCH_AND_DESTROY_BEHAVIOUR:
			{
				//Make sure we have a proper patrol position. 
				//The priority is m_vAttackPosition (last ordered attack position) -> m_vEnemyPosition (last seen enemy) -> around itself 
				vector patrolPos = m_vAttackPosition;
				if (patrolPos == vector.Zero)
				{
					patrolPos = m_vEnemyPosition;
				}
				if (patrolPos == vector.Zero)
				{
					patrolPos = owner.GetOrigin();
				}
				
				if (m_vEnemyPosition != vector.Zero)
				{
					SDRC_Log.Add("[SDRC_ChopperComp:HandleBehaviour] S&D: Enemy found, attacking: " + m_vEnemyPosition, LogLevel.NORMAL);
					
					AddDestination(SDRC_EFlyWayPointType.WP_CUT);
					//Add WP_ATTACK and WP_PATROL to the list of next destinations. These are added as first items in the list and
					//have to be added in reverse order to have WP_ATTACK as the first item.
					AddDestination(SDRC_EFlyWayPointType.WP_PATROL_ONCE, patrolPos, index: 0);
					AddDestination(SDRC_EFlyWayPointType.WP_ATTACK, m_vEnemyPosition, index: 0);
					m_fTimerBehaviourCycle = params.timeSearchAndDestroy;
				}
				else
				{
					//If no enemy found, add another patrol round in case one is already in the list.
					if (SDRC_ChopperHelper.GetNextWayPointType(owner) != SDRC_EFlyWayPointType.WP_PATROL_ONCE)
					{
						AddDestination(SDRC_EFlyWayPointType.WP_PATROL_ONCE, patrolPos, index: 0);
					}
				}
				
				break;
			}
		}
	}	
	
	//------------------------------------------------------------------------------------------------	
	// Misc
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Add the chopper to chopper frame list
	*/	
	override void AddChopperToList(IEntity owner)
	{	
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (m_BaseGameMode)
		{
	 		if (m_BaseGameMode.chopperFrame)
			{
				m_BaseGameMode.chopperFrame.AddChopperToList(owner);
				SDRC_Log.Add("[SDRC_ChopperComp:AddChopperToList] Chopper added to list.", LogLevel.DEBUG);
			}
			else
			{
				GetGame().GetCallqueue().CallLater(AddChopperToList, 1000, false, owner);	
			}
		}		
	}			
}