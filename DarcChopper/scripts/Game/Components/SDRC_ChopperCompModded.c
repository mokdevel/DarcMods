//SDRC_ChopperCompModded.c

//------------------------------------------------------------------------------------------------
//class SDRC_ChopperComp : ScriptGameComponent
modded class SDRC_ChopperComp
{
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
		
		//IMPORTANT: In a mod, you need to call Ready() yourself after Setup()!!!
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Once all init things are done, activate the component after a small delay
	*/	
	void Ready(IEntity owner)
	{
		SDRC_Log.Add("[SDRC_ChopperComp:Ready] Called..", LogLevel.DEBUG);
		
		//Add fly destinations added to prefab to the array correctly.
		array<ref SDRC_FlyPathPoint> flyDestinationsTemp = {};
		foreach(SDRC_FlyPathPoint fpp : m_vFlyDestinations)
		{
			flyDestinationsTemp.Insert(fpp);
		}
		ResetDestinations();
		foreach(SDRC_FlyPathPoint fpp : flyDestinationsTemp)
		{
			AddDestination(fpp.type, fpp.pt, fpp.value);
		}		
		
		// Some things needs to be done delayed
		if (m_bAutoStart)
		{
			//Init flight path
			InitFlight(owner);
		}
		
		SDRC_ChopperDebug.DrawDebugPaths(owner);		

		SetEventMask(owner, EntityEvent.FRAME);
//		SetEventMask(owner, EntityEvent.FIXEDFRAME | EntityEvent.FRAME | EntityEvent.SIMULATE);
//		SetEventMask(owner, EntityEvent.FRAME | EntityEvent.POSTFRAME);
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
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));		
		if (chopperComp)
		{
			if (chopperComp.m_bUnpiloted)
			{
				pilotCount = 1;
			}
		}

		float health;
		TypeGetHealthScaled(owner, health);
		
		if ( health <= chopperComp.params.damageHeavy )
		{
			m_eDamageLevel = SDRC_EHeliDamageLevel.HEAVY;
		}	
		else if ( health < chopperComp.params.damageMedium)
		{
			m_eDamageLevel = SDRC_EHeliDamageLevel.MEDIUM;
		}	
		else if ( health < chopperComp.params.damageLight)
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
				
				SetState(SDRC_EHeliState.FLY_AWAY);
				break;
			}
			case SDRC_EFlyWayPointType.WP_ATTACK:
			{
				m_vAttackPosition = destination;			//Where to attack
				//Attack flying to be on low altitude. The height will be modified in CreateFlightPoints()!
				//destination[1] = m_fFlyHeightLow * params.attackHeightMul;	//TBD: 

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
			case SDRC_EFlyWayPointType.WP_SEARCH_DESTROY:
			{
				value = params.timeSearchAndDestroy;
				m_vAttackPosition = destination;			//Where to attack
				break;
			}								
			case SDRC_EFlyWayPointType.WP_BRAKE:
			{
				if (value == 0 )
				{
					value = 200;
				}
				m_fBrakingDistance = value;
				break;
			}

			//These just fall through
			case SDRC_EFlyWayPointType.WP_WAIT:
			case SDRC_EFlyWayPointType.WP_WAIT_GETOUT:
			case SDRC_EFlyWayPointType.WP_RAISE:
			case SDRC_EFlyWayPointType.WP_HOVER:
			case SDRC_EFlyWayPointType.WP_HOVER_UP:
			case SDRC_EFlyWayPointType.WP_GET_OUT:			//Handled in HandleState()
			case SDRC_EFlyWayPointType.WP_END:
			case SDRC_EFlyWayPointType.WP_DESPAWN:
			case SDRC_EFlyWayPointType.WP_STOP_ENGINE:
			case SDRC_EFlyWayPointType.WP_LAND:
			case SDRC_EFlyWayPointType.WP_PATROL:
			case SDRC_EFlyWayPointType.WP_PATROL_ONCE:
			{
				break;
			}
			
			//------------------------------------------------------------------------------------------------	
			//Macro actions
			//------------------------------------------------------------------------------------------------	
			case SDRC_EFlyWayPointType.WP_M_CUT:
			{
				SDRC_ChopperHelper.CutSplineTail(m_vSplinePoints, m_iClosestIndex);
				addDestinationPoint = false;
				break;
			}
			case SDRC_EFlyWayPointType.WP_M_RESET:
			{
				ResetDestinations();
				SDRC_ChopperCompCore.ResetOriginalValuesComp(this);
				SDRC_ChopperHelper.CutSplineTail(m_vSplinePoints, m_iClosestIndex);
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
				AddDestination(SDRC_EFlyWayPointType.WP_WAIT_GETOUT);
				vector hoverPos = vector.Zero;
				//hoverPos[1] = (m_fFlyHeightLow + m_fFlyHeightHigh)/2;	//We don't want to raise to exactly same position
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
		//If height is 0, make it to current heli height
		if (destination[1] == 0)
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

			case SDRC_EHeliState.BRAKE:
			{
				HandleBraking(owner, timeSlice);	
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
		     && (m_fTimeInState < 0) && m_bTimeInStateEnabled) 
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
					
					AddDestination(SDRC_EFlyWayPointType.WP_M_CUT);
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
		
		nextType = SDRC_ChopperHelper.GetNextWayPointType(owner, nextType);

		switch (nextType)
		{
			case SDRC_EFlyWayPointType.WP_UNDEFINED:
/*			case SDRC_EFlyWayPointType.WP_LAND:
			{
				SDRC_ChopperCompCore.ResetOriginalValues(owner);		//Reset heli settings
				break;
			}*/
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
				
				//NOTE: We do not use AddDestination() for setting the flight. We just add points in the spline
				
				SDRC_ChopperCompCore.ResetOriginalValues(owner);	//Reset heli settings
				m_fSpeed = 2;										//Set a speed to start the raise from	
				m_fSpeedMax = m_fSpeedMin * 1.5;
				m_fSpeedMin = 2;
				m_fThrottle = 2.5;
				
				//For raise, we add points to the spline
				ResetFlight();
				
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
			{
				SetState(SDRC_EHeliState.HOVER_UP);
				SetTimeInState(m_vFlyDestinations[0].value);
				
				//NOTE: We do not use AddDestination() for setting the flight. We just add one point in the spline
				
				//Reset heli settings
				SDRC_ChopperCompCore.ResetOriginalValues(owner);
				
				//Stop heli from moving
				m_fSpeedMin = 0.3;
				m_fSpeedMax = 0.6;
				m_fSpeedSlowingMul = 0;
				
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