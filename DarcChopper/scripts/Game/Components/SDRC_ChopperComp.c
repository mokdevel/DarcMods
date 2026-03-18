//SDRC_ChopperComp.c

//Changes done in prefabs:
// - SCR_AIVehicleUsageComponent : Set true to Can Be Piloted

#ifdef WORKBENCH
//	#define CHOPPER_TESTING
#endif

//------------------------------------------------------------------------------------------------
//class SDRC_ChopperCompClass : ScriptGameComponentClass { }
class SDRC_ChopperCompClass : ScriptComponentClass { }

//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class SDRC_FlyPathPoint
{
	[Attribute(defvalue: "1", uiwidget: UIWidgets.SearchComboBox, desc: "SDRC_EFlyWayPointType", enums: ParamEnumArray.FromEnum( SDRC_EFlyWayPointType ) )]
	SDRC_EFlyWayPointType type;
	
	[Attribute(defvalue: "0 0 0", desc: "Position, waiting time, hover height, etc..")]
	vector pt;

	[Attribute(defvalue: "0 0 0", desc: "Position, waiting time, hover height, etc..")]
	float value
		
	void Set(SDRC_EFlyWayPointType type_ = SDRC_EFlyWayPointType.WP_FLY, vector pt_ = vector.Zero, float value_ = 0)
	{
		type = type_;
		pt = pt_;
		value = value_;		
	}	
}

//------------------------------------------------------------------------------------------------
//class SDRC_ChopperComp : ScriptGameComponent
class SDRC_ChopperComp : ScriptComponent
{
	private SDRC_ChopperComp s_Instance;	
	ref array<vector> m_vSplinePoints = new array<vector>();
	private VehicleHelicopterSimulation m_Helicopter_s;
		
	//Parameters accessible helicopter parameters
	[Attribute(category: "Chopper", defvalue: "1", desc: "Autostart chopper")]	
	bool m_bAutoStart;
	[Attribute(category: "Chopper", defvalue: "1.2", desc: "Throttle aka acceleration", params: "0.1 3.0 0.1")]	
	float m_fThrottle;
	float m_fThrottleOrig;
	[Attribute(category: "Chopper", defvalue: "2.0", desc: "Main rotor force", params: "0.1 5.0 0.1")]	
	float m_fRotorForce0;
	float m_fRotorForce0Orig;
	[Attribute(category: "Chopper", defvalue: "1.2", desc: "Rear rotor force", params: "0.1 4.0 0.1")]	
	float m_fRotorForce1;
	float m_fRotorForce1Orig;
	
	[Attribute(category: "Chopper", defvalue: "10.0", desc: "Minimum speed", params: "1.0 100.0 0.1")]	
	float m_fSpeedMin;				//Minimum speed
	float m_fSpeedMinOrig;
	[Attribute(category: "Chopper", defvalue: "30.0", desc: "Maximum speed", params: "1.0 100.0 0.1")]	
	float m_fSpeedMax;				//Maximum speed
	float m_fSpeedMaxOrig;
	[Attribute(category: "Chopper", defvalue: "40.0", desc: "Minimum fly height (from ground level)", params: "5 100.0 1")]	
	float m_fFlyHeightLow;			//Flight height low
	[Attribute(category: "Chopper", defvalue: "80.0", desc: "Maximum fly height (from ground level)", params: "5 600.0 1")]	
	float m_fFlyHeightHigh;			//Flight height high
	[Attribute(category: "Chopper", defvalue: "300", desc: "Minimum distance for waypoint", params: "0.1 1000.0 0.1")]	
	float m_fDistanceLow;			//Distance for waypoint min
	[Attribute(category: "Chopper", defvalue: "500", desc: "Maximum distance for waypoint", params: "0.1 1000.0 0.1")]	
	float m_fDistanceHigh;			//..max

	//Category: AI settings
	[Attribute(category: "AI settings", defvalue: "", desc: "The faction to use")]	
	string m_sFaction;
	[Attribute(category: "AI settings", defvalue: typename.EnumToString(SDRC_EHeliCargoSeatFill, SDRC_EHeliCargoSeatFill.LOW), uiwidget: UIWidgets.ComboBox, desc: "How to fill cargo seats.", enumType: SDRC_EHeliCargoSeatFill)]		
	SDRC_EHeliCargoSeatFill m_CargoSeatFill;
	[Attribute(category: "AI settings", desc: "Characters to spawn in the chopper", params: "et")]
	ref array<ref SCR_DefaultOccupantData> m_aCrew;
	//Crew settings
//	[Attribute(defvalue: EAISkill.REGULAR.ToString(), UIWidgets.ComboBox, desc: "AI skill in combat", "", ParamEnumArray.FromEnum(EAISkill), category: "Common")]
	[Attribute(category: "AI settings", defvalue: typename.EnumToString(EAISkill, EAISkill.REGULAR), uiwidget: UIWidgets.ComboBox, desc: "AI skill", enumType: EAISkill)]	
	EAISkill m_AISkill;	
	[Attribute(category: "AI settings", defvalue: "1.0", desc: "AI perception", params: "0.1 3.0 0.1")]	
	float m_AIPerception;
	[Attribute(category: "AI settings", defvalue: typename.EnumToString(SDRC_EHeliEnemySearchType, SDRC_EHeliEnemySearchType.ANY_CHAR), uiwidget: UIWidgets.ComboBox, desc: "Type of enemy to search", enumType: SDRC_EHeliEnemySearchType)]		
	SDRC_EHeliEnemySearchType m_EnemySearchType;
	ref array<AIGroup> m_aGroups = {};
	
	//Category: Weapons
	[Attribute(category: "Weapons", defvalue: "10.0", desc: "The sector where rockets may be shot", params: "1.0 45.0 1.0")]	
	float m_RocketSector;
	[Attribute(category: "Weapons", defvalue: "0.5", desc: "Delay between rockets", params: "0.1 30.0 0.1")]	
	float m_RocketDelay;
	float m_fTimeRocketDelay = 0;
	[Attribute(category: "Weapons", defvalue: "10 1 0", desc: "Rocket spawn position")]	
	vector m_RocketPosition;
//	[Attribute(category: "Weapons", params: "et", defvalue: "{EE65544BA845C458}Prefabs/Weapons/Ammo/Ammo_Rocket_S5_HEDP_S5KO.et", desc: "Rocket to use")]	
	[Attribute(category: "Weapons", params: "et", defvalue: "", desc: "Rocket to use")]	
	ref array<ref ResourceName> m_RocketPrefabs;	 
	ResourceName m_RocketPrefab = "";
	[Attribute(category: "Weapons", defvalue: "30", desc: "The amount of rockets available", params: "-1 100 1")]	
	int m_RocketCount;
	[Attribute(category: "Weapons", defvalue: "400", desc: "Rockets can be shot within this distance.", params: "100 1000 50")]	
	int m_RocketRange;
		
	//Category: Unsorted
	//Flight path
	ref array<ref SDRC_FlyPathPoint> m_vFlightPoints = {};
	[Attribute("", UIWidgets.Object, "Destinations")]	
	ref array<ref SDRC_FlyPathPoint> m_vFlyDestinations;	//Requested destinations
	//Debug stuff	
	[Attribute(defvalue: "0", desc: "Vehicle does not need pilots")]	
	bool m_bUnpiloted;
	//Debug stuff	
	[Attribute(defvalue: "0", desc: "Show debugging information")]	
	bool m_bShowDebug;
	
	//Timing stuff
	private const float TIME_DELAY_READY = 1;			//(seconds) Time before we spawn AIs and init flight path. This will give time for the chopper to properly initialize
	private const float TIME_IN_INIT = 2;				//(seconds) Time to be in init state (after READY). During this time, we don't check for damage or similar things.
	
	//Original destination	
	private vector m_vOriginalDestination;				//Used to know where to patrol
	private vector m_vFirstDestination;					//Where to fly first
		
	//Speed management
	private const float SPEED_INTERVAL = 1.0;			//(seconds) Interval to modify speed of the helicopter
	private const float SPEED_GAIN = 1.0;
	private const int SPEED_TURN_DIV = 92;				//The divider that affects how much speed is decreased on sharp turns. The higher the value, the less brake.	Was: 42
	private float m_fTimeSpeed = 0;

	private float m_fTimeBetweenPts = 1;
	private float m_fTimeBetweenPtsAvg = 1;

	private float m_fTimeBetweenFixes = 30;
	
	float m_fTimeInState = -1;							//The timer to stay in a certain state. This is only in effect when positive value.
	private bool  m_bTimeInStateEnabled = false;
			
	//Turn
	private const int TIME_TURN_INTERVAL_BASE = 40;		//Time to divide with speed to define the final turn time. Smaller value makes heli turn faster.
	
	//Pitch
	private const float PITCH_ANGLE_RAD = 11 * Math.DEG2RAD;		//The pitch angle to use when calculating for speed effect. The faster the heli goes, the steeper the nose should be down.
	private const float PITCH_ANGLE_FLAT_RAD = -45 * Math.DEG2RAD;	//The pitch angle when chopper is flying flat.
	
	//Roll 
	private const float ROLL_ANGLE_MUL = 2.4;			//Multiplier for roll angle along the spline
	
	//Flight path
	private const int POINTS_TO_NEW_DISTANCE = 3;		//How many spline points in to the future flight path is checked before adding new flight points.
	private const int POINTS_TO_SPLINE_START = 5;		//Points to go back from m_iClosestIndex when creating a new flight path 
	private const int DESTINATION_POINT_DIV = 12;		//How many points ahead to look for the destination. This is the divider for speed.

	private const int FLIGHT_FIX_TIME = 8;				//(seconds) Time to wait between flight fixes when chopper is pointing to the sky.
	private const int FLIGHT_FIX_ANGLE = 1.4;			//Angle that enforces 
	
	//Rotor force multipliers
	private const float ROTOR_FORCE_UP_MUL = 1.3;		//Rotor force multiplier in velocity counting. Bigger value makes the heli react faster to up/down movement but also starts stutter.
	
	//Waypoint values
	private const float WP_ANGLE = 60;					//Waypoint angle that is considered steep. This is the angle between current direction and new direction.
														//If chopper destination makes a too steep turn, we will add a few additional points.
	
	//Helistate
	SDRC_EHeliState m_eHeliState;
	private bool m_bInInit;
	
	//Runtime parameters
	private int m_iDestinationPointAdd;
	private float m_fTimeTurnInterval;
	
	const int HEALTH_LIMIT = 500;						//Limit to define the chopper to be heavily damaged. 
	
	//Flight path runtime variables	
	private vector m_vOrigin;							//Current position
	float m_fAltitude;									//Current altitude from ground
	float m_fSpeed;										//Current speed
	float m_fSpeedStart;								//Speed lerp start
	float m_fSpeedTarget;								//Speed lerp target aka end
	float m_fSpeedMul;									//Speed multiplier that depends on the turn
	float m_fSpeedLandingMul;							//Landing speed modifier
	float m_fRotorForceMultiplier;						//Rotor force multiplier that simulates up/down throttle
	
	//Angular velocities
	private vector m_vAngularVel;
	vector m_vRollTarget;
	private vector m_vRadRollVel;
	private vector m_vRadRollBack;
	vector m_vRadRollPitch;

	//Heli directions
	private vector m_vHeliForward;
	private vector m_vHeliDirection;
	private vector m_vHeliDirectionFuture;
	
	//Enemy positions
	vector m_vEnemyPosition = vector.Zero;		//Position of last found enemy
	int m_iEnemyFoundTime;						//Time to wait to before allowing enemy position 
	int m_iEnemyFoundTimeout = 10;				//Time between enemy position updates
	int m_iEnemyForgetTimeout = 30;				//Time to forget the enemy position
		
	//Debug stuff
	private float m_fDbgAngle;
	float m_fAnglePitch;
	private float m_fAngleRoll;
	private float m_fAngleRollBack;				//Remove from final
	
	//Runtime parameters
	int m_iClosestIndex;						//Closest point on spline to heli
	private int m_iOldClosestIndex;
	private int m_iNextIndex;					//Next index to our m_iClosestIndex - depends on speed
	private int m_iFutureIndex;					//Where we are heading in the long run
	vector m_vDestination;						//Lerped m_vDestination that keeps on moving along the spline
	vector m_vDestinationFuture;				//Destination where we eventually plan to fly
	vector m_vSplinePointBelow;					//The point below heli that is close to the spline
	
	//Debug items
	string m_sDid;								//Id for debug items
	//ref array<ref CanvasWidgetCommand> m_aDrawCommands = {};		//Line drawing commands
	//ref CanvasWidget m_wCanvas;					//Canvas to draw the lines to

	//Landing related
	private bool m_bIsLanding;					//If true, landing sequence has started
	private bool m_bFinalLanding;				//If true, we're in the final landing stages really close to the target
	float m_fLandingDistance;					//Distance to start the landing.
	private float m_fLandingSpeed;				//The speed to descend the chopper
	private float m_fSpeedLandingOrig;			//Speed from where we start to descend
	private vector m_fPositionLandingOrig;		//Position from where we start to descend
	
	//Attack related
	float m_fTimerAttack = 0;					//Timer to do attacks
	vector m_vAttackPosition;					//Position to attack

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		//SDRC_Log.Add("[SDRC_ChopperComp:EOnInit] Here!", LogLevel.DEBUG);
				
		SDRC_SpawnHelper.SetPersistence(owner, false);

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
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		#ifdef CHOPPER_TESTING
			m_fDistanceLow = SDRC_Misc.RandomInt(250, 600);
		#endif
		
		if (!GetGame().GetWorld())
		{
			return;
		}
		
		if (!SDRC_Misc.IsMaster())
		{		
			return;
		}
		
		SDRC_Log.Add("[SDRC_ChopperComp] Starting SDRC_ChopperComp", LogLevel.NORMAL);
				
		SetEventMask(owner, EntityEvent.INIT);
		s_Instance = this;
		m_sDid = SDRC_Misc.GetCurrentTickTime().ToString() + Math.RandomInt(0, 10000);
		
		m_bInInit = true;
		SetState(SDRC_EHeliState.FLY);
		
		//Clear any existing path points
		ResetFlight();
		SetTimeInState(0);
		//Save the original values
		SDRC_ChopperCompCore.StoreOriginalValues(owner);
		
		SDRC_ChopperEnemyHelper.GetWeapons(owner);
		
		m_iEnemyFoundTime = SDRC_Misc.GetCurrentTickTime() + m_iEnemyFoundTimeout;
		
		m_Helicopter_s = VehicleHelicopterSimulation.Cast(GetOwner().GetRootParent().FindComponent(VehicleHelicopterSimulation));
		if (m_Helicopter_s)
		{
	        m_Helicopter_s.EngineStart();
	        m_Helicopter_s.SetThrottle(m_fThrottle);
	        m_Helicopter_s.RotorSetForceScaleState(0, m_fRotorForce0);
	        m_Helicopter_s.RotorSetForceScaleState(1, m_fRotorForce1);
			SetHeli(m_fSpeedMin, m_fSpeedMax, m_fFlyHeightLow, m_fFlyHeightHigh, m_fDistanceLow, m_fDistanceHigh);						
			
			if (m_bAutoStart)
			{
				//NOTE: This section is to be done in the mod
				SetEnemySearchType(SDRC_EHeliEnemySearchType.PLAYER);
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_ChopperComp] VehicleHelicopterSimulation not found.", LogLevel.ERROR);
		}		
		
		if (m_RocketPrefabs.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_ChopperComp] No rockets available.", LogLevel.NORMAL);
		}
		else
		{
			m_RocketPrefab = m_RocketPrefabs.GetRandomElement();
			SDRC_Log.Add("[SDRC_ChopperComp] Using rockets: " + SDRC_Misc.GetSimpleEntityName(m_RocketPrefab), LogLevel.NORMAL);
		}
				
		super.OnPostInit(owner);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~SDRC_ChopperComp()
	{
		Clear();		
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Do final clear up of things when heli is to be despawned.
	*/	
	void Clear()
	{
		SDRC_DebugHelper.DeleteDebugItems(m_sDid);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to component
	*/
	SDRC_ChopperComp GetInstance()
	{
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Once all init things are done, activate the component after a small delay
	*/	
	void Ready(IEntity owner)
	{
		//Set ready in a few seconds
		GetGame().GetCallqueue().CallLater(ReadyDelayed, TIME_DELAY_READY * 1000, false, owner);
	}
	
	//------------------------------------------------------------------------------------------------
	//Delayed spawn of AI crew
	
	void ReadyDelayed(IEntity owner)
	{
		// Some things needs to be done delayed
		if (m_bAutoStart)
		{
			//Spawn crew 
			int crewCount = SDRC_ChopperCrewHelper.SpawnCrew(owner, m_CargoSeatFill, m_aCrew, m_sFaction, m_AISkill, m_AIPerception);
			SDRC_Log.Add("[SDRC_ChopperComp] Crew count: " + crewCount, LogLevel.DEBUG);
		}		
		
		GetGame().GetCallqueue().CallLater(ReadyDelayed_2, TIME_DELAY_READY * 1000, false, owner);		
	}

	//------------------------------------------------------------------------------------------------
	//Delayed flight init and activation
	
	void ReadyDelayed_2(IEntity owner)
	{		
		//Collect groups in the helicopter 
		SDRC_VehicleHelper.GroupFindAll(owner, m_aGroups);
						
		// Some things needs to be done delayed
		if (m_bAutoStart)
		{
			//Init flight path
			InitFlight(owner, owner.GetOrigin());
		}

		SDRC_ChopperDebug.DrawDebugPaths(owner);
				
		SetEventMask(owner, EntityEvent.FRAME);
		Activate(owner);
		
		GetGame().GetCallqueue().CallLater(InitDone, TIME_IN_INIT * 1000, false, owner);
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Set the helicopter to normal state
	*/	
	void InitDone(IEntity owner)
	{
		m_bInInit = false;
			
		//Check if pilots were possible to set
		if (SDRC_VehicleHelper.PilotCountAlive(owner) == 0)
		{
			SDRC_Log.Add("[SDRC_ChopperComp] Unable to set pilots.", LogLevel.WARNING);			
		}
	}	
	
	//------------------------------------------------------------------------------------------------	
	// Flight model functionality
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (m_bInInit)
		{
			//Flatten the helicopter while being in init
			//SDRC_ChopperHelper.SetHorizontal(owner, timeSlice);
			return;
		}
		
		m_vOrigin = owner.GetOrigin();
		m_fAltitude = m_vOrigin[1] - SDRC_Misc.GetSurfaceYWithWater(m_vOrigin);
	
		m_fTimeSpeed += timeSlice;
		m_fTimeBetweenPts += timeSlice;
		m_fTimeBetweenFixes -= timeSlice;
		m_fTimeInState -= timeSlice;		
		m_fTimeRocketDelay += timeSlice;		
		m_fTimerAttack -= timeSlice;
		
//		SDRC_ChopperDebug.DrawDestinationLines(owner);
		
		//Check if we're still working. 
		//---
		//TBD: This section is not needed every frame. Could be done every x seconds - not that critical
		if (m_eHeliState == SDRC_EHeliState.DESTROYED)
		{
			//If chopper is destroyed, let Reforger handle crash etc. Just stop everything we used to do on EOnFrame.
			return;
		}
		
		//Check if we're still functional
		if (!SDRC_ChopperHelper.IsStillWorking(owner, m_bInInit))
		{
			//Nope, we're done
			SetState(SDRC_EHeliState.DESTROYED);
			SDRC_DebugHelper.DeleteDebugItems(m_sDid);
		}
		
		//If the chopper is damaged, init is considered done.
		if (SDRC_ChopperHelper.GetHealth(owner) < HEALTH_LIMIT)
		{
			InitDone(owner);
		}				
		//---
		
		//Normal flying part
		
		//Adjust time depending on the speed.
		m_fTimeTurnInterval = TIME_TURN_INTERVAL_BASE / m_fSpeed;
		m_fTimeTurnInterval = Math.Clamp(m_fTimeTurnInterval, 0.6, 3);
				
		//If we've been stuck on a point, force new flight path. 
		//Sometimes the heli direction and path align so that the closest index does not update.
		//In these case the helicopter up vector and world up vector is big.
		bool bCreateNewPath = false;
		float heliUpAngleToWorld = SDRC_Math.GetAngleBetweenVectors(owner.GetTransformAxis(1), vector.Up);	
		if (heliUpAngleToWorld > FLIGHT_FIX_ANGLE)
		{
			if (m_fTimeBetweenFixes < 0)
			{
				#ifdef WORKBENCH
					SDRC_Log.Add("[SDRC_ChopperComp] Fixing flight.", LogLevel.DEBUG);
				#endif
				//TBD: Change to align the chopper towards target
				bCreateNewPath = true;
			}
		}
		else
		{
			//All good, reset fix time
			m_fTimeBetweenFixes = FLIGHT_FIX_TIME;
		}
		
		//No need to do anything unless we are at the end of spline.
		if ((m_iClosestIndex + m_iDestinationPointAdd + POINTS_TO_NEW_DISTANCE >= m_vSplinePoints.Count() - 1) || bCreateNewPath)
		{
			m_fTimeBetweenFixes = FLIGHT_FIX_TIME;	//Time between tries to fix the flight
			
			if (   (m_eHeliState == SDRC_EHeliState.FLY) 
				|| (m_eHeliState == SDRC_EHeliState.FLY_AWAY) 
			   )
			{
				//Define a new destination and create a new path
				CreateNewFlight(owner);
				//m_fTimeBetweenFixes = FLIGHT_FIX_TIME;	//Time between tries to fix the flight
			}
		}
		
		//Count destintation addition along the spline which is dependent on the speed.
		m_iDestinationPointAdd = m_fSpeed / DESTINATION_POINT_DIV;
		m_iDestinationPointAdd = Math.ClampInt(m_iDestinationPointAdd, 1, 3);
		
		//Find where we're going
//		float distance = SDRC_Spline3D.GetDistanceFromSpline(m_vSplinePoints, m_vOrigin, m_iClosestIndex, false);	//NOTE: This will set m_iClosestIndex
		m_iClosestIndex = SDRC_ChopperHelper.FindNextSplinePointIndex(m_vOrigin, m_iClosestIndex, m_vSplinePoints);
		
		if (m_iClosestIndex > m_iOldClosestIndex)
		{
			m_fTimeBetweenPtsAvg = m_fTimeBetweenPts;		//TBD: This is a static value of previous time instead of average 
			m_fTimeBetweenPts = 0;
			m_iOldClosestIndex = m_iClosestIndex;
		}
		
		//Destination point definition
		m_iFutureIndex = m_iClosestIndex + (m_iDestinationPointAdd * 2);
		m_iNextIndex = m_iClosestIndex + m_iDestinationPointAdd;

		if (m_iNextIndex > m_vSplinePoints.Count() - 1)
		{
			m_iNextIndex = m_vSplinePoints.Count() - 1;
		}
		
		if (m_iFutureIndex > m_vSplinePoints.Count() - 1)
		{
			m_iFutureIndex = m_vSplinePoints.Count() - 1;
		}
		
		//Lerped m_vDestination that keeps on moving along the spline
		float td = m_fTimeBetweenPts / m_fTimeBetweenPtsAvg;
		td = Math.Clamp(td, 0, 1);
		m_vDestination = vector.Lerp(m_vSplinePoints[m_iNextIndex], m_vDestinationFuture, td);
		
		//Destination where we eventually plan to fly
		m_vDestinationFuture = m_vSplinePoints[m_iFutureIndex];
		
		//Lerp speed		
		if (m_fTimeSpeed < SPEED_INTERVAL)
		{
			float ts = m_fTimeSpeed / SPEED_INTERVAL;
			m_fSpeed = Math.Lerp(m_fSpeedStart, m_fSpeedTarget, ts);
			m_fSpeed = Math.Clamp(m_fSpeed, m_fSpeedMin, m_fSpeedMax)
		}

		//Get chopper direction
		m_vHeliForward = owner.GetTransformAxis(2);
		m_vHeliDirection = vector.Direction(m_vOrigin, m_vDestination);		
		m_vHeliDirectionFuture = vector.Direction(m_vOrigin, m_vDestinationFuture);

		//Find the point on spline below the helicopter. This is not exact.
		if (m_vSplinePoints.Count() == 1)
		{
			m_iClosestIndex = 0;
		}
		int prevIndex = m_iClosestIndex - 1;	
		if (prevIndex < 0)
		{
			prevIndex = m_iClosestIndex;
		}
		m_vSplinePointBelow = vector.Lerp(m_vSplinePoints[prevIndex], m_vSplinePoints[m_iClosestIndex], td);
				
		//Set velocity 
		HandleRotorForce(owner);
		
		//Set turn
		SetTurn(owner, m_fTimeTurnInterval);
		
		HandleState(owner, timeSlice);

		SetVelocity(owner, timeSlice);
	
		//Handle attacks
		if (m_fTimerAttack < 0)	
		{
			//Search for enemies
			SDRC_ChopperEnemyHelper.SearchForEnemy(owner);
			
			if (m_fTimeRocketDelay > m_RocketDelay)
			{
				SDRC_ChopperEnemyHelper.SearchEnemyForRocket(owner);
				m_fTimeRocketDelay = 0;
			}
		}
		else
		{
			if (m_fTimeRocketDelay > m_RocketDelay)
			{
				SDRC_ChopperEnemyHelper.EnemyFoundForRocket(owner, m_vAttackPosition);
				m_fTimeRocketDelay = 0;
			}
		}

		SDRC_ChopperHelper.HandleWaypoints(owner);				
		SDRC_ChopperDebug.DrawHelicopterVectors(owner);
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Set turn for heli
	
	Multiple angular velocities are calculated.
	- Speed multiplier : The faster we're going, certain values needs to grow.
	- Roll pitch : The faster we go, the steeper the heli nose shall point down.
	- Roll on direction: The steeper the turn we're about to make, roll the heli left or right .
	- Roll back : Set the heli to return to normal flight position.
	*/
	private void SetTurn(IEntity owner, float deltaTime)
	{
		if (deltaTime == 0)
		{
			return;
		}

		//Get the heli vectors
		//vector heliVelocity = owner.GetPhysics().GetVelocity();
		vector heliPitch = owner.GetTransformAxis(0);
		vector heliUp = owner.GetTransformAxis(1);
		//vector heliRoll = owner.GetTransformAxis(2);
		
		//SPEED: Set speed according to previous turns
		//Ignore height component
		float angle = Math.AbsFloat(SDRC_Math.GetAngleBetweenVectorsXZ(m_vHeliForward, m_vHeliDirectionFuture));
		m_fDbgAngle = angle;
		
		//Count the angle of the turn. The steeper the turn, the slower heli should be moving.
		m_fSpeedMul = Math.Clamp((angle * Math.RAD2DEG), 1, 90);											//Was 1,90
		m_fSpeedMul = m_fThrottle * (SPEED_GAIN - (m_fSpeedMul / SPEED_TURN_DIV));

		//In case we're landing, we need to modify the speed
		m_fSpeedMul = m_fSpeedMul * m_fSpeedLandingMul;
		
		//Set 
		m_fSpeedStart = m_fSpeed;
		m_fSpeedTarget = m_fSpeed * m_fSpeedMul;

		//If we're too close to ground, slow down the speed to allow time for climb
		float altitude = SDRC_ChopperHelper.GetAltitude(owner);
		const int ALTITUDE_ADD = 5;
		if ((altitude + ALTITUDE_ADD) < m_fFlyHeightLow)
		{		
//				float surfaceY = SDRC_Misc.GetSurfaceYWithWater(m_vOrigin);
//				float mul = (m_vOrigin[1] - surfaceY) / (m_fFlyHeightLow - surfaceY);
			float mul = (altitude + ALTITUDE_ADD) / m_fFlyHeightLow;
			mul = Math.Clamp(mul, 0, 1);
			m_fSpeedTarget = m_fSpeedTarget * mul;
		}

		m_fSpeedTarget = Math.Clamp(m_fSpeedTarget, m_fSpeedMin, m_fSpeedMax);
		m_fTimeSpeed = 0;	//Start to change speed
								
		//Handle yaw, pitch roll		
		
		//ROLL PITCH: Change pitch according to speed		
		m_fAnglePitch = PITCH_ANGLE_FLAT_RAD + PITCH_ANGLE_RAD * m_fSpeedMul;
		
		if (m_eHeliState == SDRC_EHeliState.RAISE)
		{
			//Turn nose down
			m_fAnglePitch = -15 * Math.DEG2RAD;
		}
		
		m_vRadRollPitch = SDRC_Math.RotateAroundAxis(m_vHeliForward, heliPitch, m_fAnglePitch);
		m_vRadRollPitch = SDRC_Math.ComputeAngularVelocity(m_vHeliForward, m_vRadRollPitch, deltaTime * 0.5);
		
		//ROLL ON DIRECTION: See how steep we're turning. Roll the helicopter accordingly for more natural flight. We only care about ZX plane.
		m_fAngleRoll = SDRC_Math.GetAngleBetweenVectorsXZ(m_vHeliForward, m_vHeliDirectionFuture);
		m_fAngleRoll = Math.Clamp(m_fAngleRoll, -0.5, 0.5) * ROLL_ANGLE_MUL;
		m_vRadRollVel = SDRC_Math.RotateAroundAxis(m_vHeliForward, heliUp, m_fAngleRoll);
		m_vRadRollVel = SDRC_Math.ComputeAngularVelocity(heliUp, m_vRadRollVel, deltaTime);
		
		//Count the angular velocity
		m_vAngularVel = SDRC_Math.ComputeAngularVelocity(m_vHeliForward, m_vHeliDirection, deltaTime);

		//Handle landing time angular velocities
		if (m_fSpeedLandingMul < 0.3)
		{
			//Flatten the chopper when landing
			m_vAngularVel = vector.Zero;
			m_vRadRollVel = vector.Zero;
			m_vRadRollPitch = vector.Zero;
		}
		else if (m_fSpeedLandingMul < 1.0)
		{
			//Do only minor adjustments
			m_vAngularVel = m_vAngularVel * m_fSpeedLandingMul * 0.2;
			m_vRadRollVel = m_vRadRollVel * m_fSpeedLandingMul;
//			m_vRadRollPitch = m_vRadRollPitch * m_fSpeedLandingMul * 0.01;
		}
					
		//ROLL UP (YAW): Count the angle from heli up vs world up. The heli should slowly move back to horizontal flight.
		m_fAngleRollBack = SDRC_Math.GetAngleBetweenVectors(heliUp, vector.Up);
		m_vRadRollBack = SDRC_Math.ComputeAngularVelocity(heliUp, vector.Up, deltaTime * 0.6);
		
		owner.GetPhysics().SetAngularVelocity(m_vAngularVel + m_vRadRollVel + m_vRadRollBack + m_vRadRollPitch);
	}

	//------------------------------------------------------------------------------------------------	
	/*!	
	Set velocity vector
	*/
	private void SetVelocity(IEntity owner, float timeSlice)
	{
		//Set velocity
		vector velVector = vector.Zero;
		
		if (    (m_eHeliState != SDRC_EHeliState.ON_GROUND)
		     && (m_eHeliState != SDRC_EHeliState.WAIT)
		   )
		{
			vector rotVector = owner.GetAngles();
			velVector = m_vDestination;
			velVector.Normalize();
			float forceMultiplier = m_fSpeed;
			float forceRotorUp = m_fRotorForce0 * ROTOR_FORCE_UP_MUL * 10;
			
			if (m_bFinalLanding)
			{
				velVector[0] = 0;
				velVector[2] = 0;
			}
			
			velVector = {velVector[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * forceMultiplier, velVector[1] * forceRotorUp * m_fRotorForceMultiplier, velVector[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * forceMultiplier};
		}
				
		owner.GetPhysics().SetVelocity(velVector);
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Simulate the use of throttle.
	
	This will modify the rotor force up multiplier trying to keep the heli flying close to the spline points. 
	In case of too low altitude, panic mode is enabled. 
	*/
	private void HandleRotorForce(IEntity owner)
	{
		if (m_vDestination == vector.Zero)
		{
			return;		
		}

		float splineHeightFromGround = m_vSplinePointBelow[1];		
		float heliHeightFromGround = m_vOrigin[1] - 3;				//Move the origin slightly below the spline
				
		//The normal way to slowly go towards the spline
		int bigMul = 60;	//was 25
		
		switch (m_eHeliState)
		{
			case SDRC_EHeliState.HOVER:
			{
				//In HOVER state, do movemements slow
				bigMul = 14;
				break;
			}		
			case SDRC_EHeliState.RAISE:
			{
				//In RAISE state, do somewhat rapid climb slow
				bigMul = 150;
				break;
			}
		}
				
		//In fly state, react to low flying
		if (m_eHeliState == SDRC_EHeliState.FLY)
		{
			//We're below the spline, let's raise bit more agressively
			if (heliHeightFromGround < splineHeightFromGround)
			{
				bigMul = 200;
			}
					
			//Modify if we're too close to the ground, raise very aggressively
			float altitude = SDRC_ChopperHelper.GetAltitude(owner);
			
			if (altitude < m_fFlyHeightLow)
			{
				bigMul = 300;
			}				
		}
		
		if (splineHeightFromGround <= 0)
		{
			splineHeightFromGround = 0.01;
		}
		m_fRotorForceMultiplier = 0 - (bigMul * ( (heliHeightFromGround - splineHeightFromGround) / splineHeightFromGround ));	
	}
			
	//------------------------------------------------------------------------------------------------	
	// Flight path things
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Create the initial flight path 
	*/
	void InitFlight(IEntity owner, vector origin)	
	{
/*		if (!GetGame().GetWorld())
		{
			return;
		}*/
		
		vector initialPos = owner.GetOrigin();
		initialPos[1] = Math.Clamp(initialPos[1], m_fFlyHeightLow, m_fFlyHeightHigh);
		owner.SetOrigin(initialPos);
		owner.Update();
		
		//Store the original firstDestination
		m_vOriginalDestination = origin;
		
		//Create initial flypath. 		
		//We add two points to m_vFlightPoints that work as guide for the heli direction. 		
		AddFlyPathPoint(origin);	//point 1
		//AddDebugMarker(origin, ARGB(255, 0, 255, 00), 2.0, m_sDid, 10);
		
		//If no destination were defined, let's create a random one
		if (m_vFlyDestinations.IsEmpty())
		{
			if (m_vFirstDestination == vector.Zero)
			{
				vector transform[4];
				owner.GetTransform(transform);
				vector angle = transform[2];
				angle.Normalized();
				m_vFirstDestination = owner.GetOrigin() + angle * m_fDistanceLow;
			}
			
			#ifndef CHOPPER_TESTING
				AddDestination(SDRC_EFlyWayPointType.WP_FLY, m_vFirstDestination);
			#else
				//m_vFirstDestination = "3071 33 2544";
				AddDestination(SDRC_EFlyWayPointType.WP_M_LAND_TROOPS, m_vFirstDestination);
			#endif
			//SDRC_DebugHelper.AddDebugPos(m_vFirstDestination, ARGB(255, 255, 00, 00), 5.0, m_sDid, 200);			
		}

		//Add a point towards the first destination
		vector helipos = owner.GetOrigin();
		vector direction = vector.Direction(helipos, m_vFlyDestinations[0].pt);
		vector pos = owner.GetOrigin() + direction.Normalized() * 10;
		pos[1] = helipos[1];
		AddFlyPathPoint(pos);		//point 2 .. some meters towards the destination
		//AddDebugMarker(pos, ARGB(255, 0, 255, 00), 2.0, m_sDid, 10);
		
		//Set chopper to initial position in case an specific origin was provided
		if (m_bAutoStart)
		{		
			owner.SetOrigin(m_vFlightPoints[0].pt);
		}

		//Store the origin. This value is updated in EOnFrame, but needed already in calculations.
		m_vOrigin = owner.GetOrigin();				
		SDRC_Log.Add("[SDRC_ChopperComp:InitFlight] Chopper initial position: " + owner.GetOrigin(), LogLevel.DEBUG);
		
		//Turn chopper to face the first destination
		SDRC_Math.TurnEntityTowardsXZ(owner, m_vFlyDestinations[0].pt);
		
		if (!m_bAutoStart)	//With autostart, use the origin of the chopper spawn
		{
			SDRC_ChopperHelper.SetFlightPointHeight(owner);
		}
		
		//Create points for spline
		CreateFlightPoints(owner);
		
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
		
		//NOTE: We draw the debug paths once the component is ready
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Create the runtime flight path with waypoint definition
	
	\param firstDestination 
	*/
	void CreateNewFlight(IEntity owner, vector firstDestination = vector.Zero)
	{
		//Clear any existing path points
		ResetFlight();
		
		//Add an initial flight position. 
		if (firstDestination == vector.Zero)
		{		
			AddFlyPathPoint(m_vDestinationFuture);
		}
		else
		{
			AddFlyPathPoint(firstDestination);
		}
		
		//Create points for spline		
		CreateFlightPoints(owner);
		
		SDRC_ChopperHelper.SetFlightPointHeight(owner);
		//Set the first points to same height as heli. 
		m_vFlightPoints[0].pt[1] = m_vOrigin[1];
		m_vFlightPoints[1].pt[1] = m_vOrigin[1];
		
		array<vector> flyPathPoints = {};
		SDRC_ChopperDebug.GivePoints(flyPathPoints, m_vFlightPoints);
		SDRC_Spline3D.GenerateSplinePoints(flyPathPoints, m_vSplinePoints, -1);
		
		//Search the closest indes from the spline start
		m_iClosestIndex = 0;
		float distance = SDRC_Spline3D.GetDistanceFromSpline(m_vSplinePoints, owner.GetOrigin(), m_iClosestIndex, false);	//NOTE: This will set m_iClosestIndex
		m_iOldClosestIndex = m_iClosestIndex;
		
		//Check that points are above ground
		SDRC_ChopperHelper.SetSplinePointsAboveGround(owner, 6);
		
		if (m_vSplinePoints.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_ChopperComp:CreateFlightPath] No points!", LogLevel.ERROR);
		}
		
		SDRC_ChopperDebug.DrawDebugPaths(owner);
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Clear the fly path as a preparation for a completely new path
	*/
	void ResetFlight()
	{
		m_vFlightPoints.Clear();
		m_vSplinePoints.Clear();
	}

	//------------------------------------------------------------------------------------------------	
	// States 
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	SDRC_EHeliState GetState()
	{
		return m_eHeliState;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetState(SDRC_EHeliState state)
	{
		m_eHeliState = state;
		
		//If in normal flight mode, disable the TimeInState counter
		if (state == SDRC_EHeliState.FLY)
		{			
			SetTimeInState(0)
		}
		
		SDRC_Log.Add("[SDRC_ChopperComp:SetState] State: " + SCR_Enum.GetEnumName(SDRC_EHeliState, m_eHeliState), LogLevel.SPAM);
	}
	
	//------------------------------------------------------------------------------------------------	
	void SetTimeInState(int seconds)	
	{
		if (seconds == -1)
		{
			seconds = 0;
		}
		
		m_fTimeInState = seconds;
		
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
	Handle state (machine)
	*/
	private void HandleState(IEntity owner, float timeSlice)
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
	Sets the next destination for an action. 
	- FLY will start to fly
	- Others will have some action bound to them.
	*/
	void SetNextState(IEntity owner, SDRC_EFlyWayPointType nextType = SDRC_EFlyWayPointType.WP_UNDEFINED, bool allowRemove = true)
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
				AddDestination(SDRC_EFlyWayPointType.WP_END, m_vFlightPoints[m_vFlightPoints.Count() - 1].pt); 
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
				m_Helicopter_s.EngineStop();
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
	// Fly point handling
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Create fly points
	Takes the points from m_vFlyDestinations and generates points to be used for spline creation
	*/	
	private void CreateFlightPoints(IEntity owner)
	{
		float forwardDistance = 20;
			
		//If we have a destination and it's far away, increase the 	forwardDistance
		if (!m_vFlyDestinations.IsEmpty())
		{
			float distance = vector.DistanceXZ(owner.GetOrigin(), m_vFlyDestinations[0].pt);
			if (distance > 200)
			{
				forwardDistance = SDRC_Misc.RandomInt( (distance/4), (distance/1.3) );
			}			
		}		
		
		//Get vector from heli position to the first point to fly to.
		vector origin = owner.GetOrigin();
		vector direction = vector.Direction(origin, m_vFlightPoints[m_vFlightPoints.Count() - 1].pt);
		//Create one additional point as the first heli path point
		vector pos = owner.GetOrigin() + direction.Normalized() * forwardDistance;
		pos[1] = origin[1];
		AddFlyPathPoint(pos);
		//SDRC_DebugHelper.AddDebugPos(pos, ARGB(255, 0, 255, 00), 1.0, m_sDid, 30); */
		
		//Add destinations .. if any
		int lastIdx = -1;
		
		//Generate a random destination point if needed
		if (m_vFlyDestinations.IsEmpty())
		{		
			SDRC_ChopperHelper.GenerateWayPoint(owner);
		}
		
		//Handle destinations
		foreach (int idx, SDRC_FlyPathPoint flyDestination : m_vFlyDestinations)
		{		
			//SDRC_DebugHelper.AddDebugPos(flyDestination.pt, ARGB(32, 255, 128, 64), 1.0, m_sDid, 50);
			
			//Distance of last flight point defined and the next destination
			float distance = vector.DistanceXZ(m_vFlightPoints[m_vFlightPoints.Count() - 1].pt, flyDestination.pt);
	
			//Get the angle for the destination
			vector dir0;	//Previous flight direction
			vector dir1;	//New flight direction
			vector p0 = m_vFlightPoints[m_vFlightPoints.Count() - 2].pt;
			vector p1 = m_vFlightPoints[m_vFlightPoints.Count() - 1].pt;
			vector p2 = flyDestination.pt;
			float heliAngle = SDRC_Math.GetRadiansBetweenThreePointsXZ(p0, p1, p2) * Math.RAD2DEG;

			SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Distance: " + distance + " - Angle: " + heliAngle, LogLevel.SPAM);
			
			//Is the angle too steep? Re-route.
			if ( (Math.AbsFloat(heliAngle) < WP_ANGLE) && (distance > 200) )
			{				
				SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Heli direction angle is steep: " + heliAngle, LogLevel.SPAM);
				
				//Get the last point
				vector point = m_vFlightPoints[m_vFlightPoints.Count() - 1].pt;
				
				//We need to take a detour. Add an additional points outside of the line to make the route rounder				
				float lerpRnd = SDRC_Misc.RandomFloat(0.25, 0.65);
				float divRnd = SDRC_Misc.RandomFloat(1.5, 4);
				
				//Depending on the angle decide if we re-route left ot right				
				bool isOnLeft = SDRC_Math.IsPointOnLeft(p0, p1, p2);
								
				//Find a point along the fly path and move it away from the line along tangent
				vector vec = SDRC_Math.CreateOffsetMidPoint(point, flyDestination.pt, (distance / divRnd), lerpRnd, isOnLeft);
				AddFlyPathPoint(vec);				
				//SDRC_DebugHelper.AddDebugPos(vec, ARGB(255, 0, 0, 0), 1.0, m_sDid, 500);
			}
			
			AddFlyPathPoint(flyDestination.pt, flyDestination.type);
			lastIdx = idx;
			
			//If distance is really short, add a mid point
/*			if (distance < 200)
			{
				p0 = m_vFlightPoints[m_vFlightPoints.Count() - 1].pt;
				p1 = m_vFlightPoints[m_vFlightPoints.Count() - 2].pt;
				vector mid = vector.Lerp(p0, p1, 0.5);
				AddFlyPathPoint(mid, index: m_vFlightPoints.Count() - 2);
			}*/
			
			switch (flyDestination.type)
			{
				case SDRC_EFlyWayPointType.WP_PATROL:
				{
					//If request to patrol, create additional points around position
			
					int count = 20;
					int degree = 45; 		// Degrees per count
					int sign = 1;			//SDRC_Misc.RandomSign(); <- does not work very well
					
					for (int i = 0; i < count; i++)
					{
						float value = flyDestination.value;
						if (value <= 0)
						{
							value = 400;
						}
						float range = Math.RandomFloat(value * 0.7, value * 1.3);					
	//					float range = Math.RandomFloat(m_fDistanceLow, m_fDistanceHigh);
						//Make waypoints around the position to patrol.					
						vector dir = SDRC_Math.RotateAroundAxis(m_vHeliDirection, vector.Up, sign * i * degree * Math.DEG2RAD);
						dir.Normalize();
	//					pos = m_vOriginalDestination + dir * range;
						pos = flyDestination.pt + dir * range;						
						AddFlyPathPoint(pos);
						//SDRC_DebugHelper.AddDebugPos(pos, ARGB(255, 0, 0, 255), 2.0, m_sDid, 50 + i * 20);
					}					
					break;
				}				
/*				case SDRC_EFlyWayPointType.WP_RAISE:
				{				
					//Create a first destination that is a short way to where we're planning to go. Smoothens the flight.
					vector firstDestination = SDRC_ChopperHelper.GetDestinationForward(owner, m_vFlyDestinations[0].pt[0] / 5);
					//Make it low, to get the helicopter nose down.
					firstDestination[1] = (firstDestination[1] / 2);
					AddFlyPathPoint(firstDestination);
					//Then fly forward
					pos = SDRC_ChopperHelper.GetDestinationForward(owner, m_vFlyDestinations[0].pt[0]);				
					pos[1] = m_fFlyHeightLow + 5;	//Fly to a point slightly above low fly point				
//					AddDestination(SDRC_EFlyWayPointType.WP_RAISE, pos);
					AddFlyPathPoint(pos);
					SetState(SDRC_EHeliState.FLY);
					break;
				}*/
				
/*				case SDRC_EFlyWayPointType.WP_FLY_AWAY_IMMEDIATELY:	//If request to fly away, set the right state
				case SDRC_EFlyWayPointType.WP_FLY_AWAY:				//If request to fly away, set the right state
				case SDRC_EFlyWayPointType.WP_END:					//If request to end, stop simulation
				{
					SetNextState(owner, flyDestination.type);
					break;
				}*/
				default:
				{
					SetNextState(owner, flyDestination.type, false);
					break;
				}
			}
									
			//FLY points are handled in a serie. Others one at a time.
			if (flyDestination.type != SDRC_EFlyWayPointType.WP_FLY)
			{				
				break;
			}
		}

		//If only two points, add a mid point
		if (m_vFlightPoints.Count() <= 2)
		{
			vector p0 = m_vFlightPoints[0].pt;
			vector p1 = m_vFlightPoints[1].pt;
			vector mid = vector.Lerp(p0, p1, 0.5);
			AddFlyPathPoint(mid, index: 1);
		}		
		
		SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Created " + m_vFlightPoints.Count() + " points.", LogLevel.SPAM);
		
		//Clear the destinations 
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
	private void AddFlyPathPoint(vector destination, SDRC_EFlyWayPointType type = SDRC_EFlyWayPointType.WP_FLY, int index = -1)
	{
		if (type == SDRC_EFlyWayPointType.WP_LAND)
		{
			destination = SDRC_Misc.SetPosToSurface(destination);
			SetState(SDRC_EHeliState.LAND);
			
			m_bIsLanding = false;
		}
		
		SDRC_FlyPathPoint fpp = new SDRC_FlyPathPoint();
		
		if (index == -1)
		{
			fpp.Set(type, destination);
			m_vFlightPoints.Insert(fpp);
		}
		else
		{
			fpp.Set(type, destination);
			m_vFlightPoints.InsertAt(fpp, index);
		}
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
	void AddDestination(SDRC_EFlyWayPointType type = SDRC_EFlyWayPointType.WP_FLY, vector destination = vector.Zero, float value = -1, int index = -1)
	{
		//In normal case, we just add a destination for future handling. 
		//Below are a few special cases where we need either react immediately of change some other params.

		bool addDestinationPoint = true;
		
		switch (type)
		{
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
				//NOTE: Will drop through FLY_AWAY
			}		
			case SDRC_EFlyWayPointType.WP_FLY_AWAY:
			{
				//Fly away after all destinations have been handled
				SetState(SDRC_EHeliState.FLY_AWAY);
				break;
			}
			case SDRC_EFlyWayPointType.WP_ATTACK:
			{
				m_vAttackPosition = destination;			//Where to attack
				m_fTimerAttack = value;						//For how long to continue attacks
				break;
			}
			case SDRC_EFlyWayPointType.WP_LAND:
			{
				//This just falls through
				break;
			}
			
			//Macro actions
			case SDRC_EFlyWayPointType.WP_M_LAND_TROOPS:
			{
				AddDestination(SDRC_EFlyWayPointType.WP_LAND, destination);
				AddDestination(SDRC_EFlyWayPointType.WP_GET_OUT);
				
				int crewCount = SDRC_ChopperCrewHelper.CountCrew(GetOwner());
				crewCount = 5 + crewCount * 3;	//Give N seconds per AI plus additional time
				
				AddDestination(SDRC_EFlyWayPointType.WP_WAIT, value : crewCount);
				vector hoverPos = vector.Zero;
				hoverPos[1] = m_fFlyHeightLow;
				AddDestination(SDRC_EFlyWayPointType.WP_HOVER_UP, hoverPos, 8);
				AddDestination(SDRC_EFlyWayPointType.WP_RAISE, "150 0 0");
				
				//All things are already added
				addDestinationPoint = false;
				break;
			}
			case SDRC_EFlyWayPointType.WP_M_LAND_TO_FREE_SPOT:
			{
				int emptySize = 40;
				
				bool foundLandingSpot = false;

				for (int i = 0; i < 3; i++)
				{
					if (SDRC_SpawnHelper.FindEmptyPos(destination, 100 + i * 100, emptySize))
					{
						foundLandingSpot = true;
						break;
					}
				}
								
				if (foundLandingSpot)
				{
					//Safe landing position found
					SDRC_DebugHelper.AddDebugPos(destination, ARGB(255, 64, 255, 64), emptySize, m_sDid, 10.0);				
					AddDestination(SDRC_EFlyWayPointType.WP_LAND, destination);
					AddDestination(SDRC_EFlyWayPointType.WP_GET_OUT);
					AddDestination(SDRC_EFlyWayPointType.WP_WAIT, value : value);
				}
				else
				{
					//No safe landing position found
					AddDestination(SDRC_EFlyWayPointType.WP_FLY, destination);
				}
				
/*				vector hoverPos = vector.Zero;
				hoverPos[1] = m_fFlyHeightLow;
				AddDestination(SDRC_EFlyWayPointType.WP_WAIT, value : value);
				AddDestination(SDRC_EFlyWayPointType.WP_HOVER_UP, hoverPos, 12);
				AddDestination(SDRC_EFlyWayPointType.WP_RAISE, "200 0 0");*/
				//All things are already added
				addDestinationPoint = false;
				break;
			}
			case SDRC_EFlyWayPointType.WP_M_ATTACK:
			{
				AddDestination(SDRC_EFlyWayPointType.WP_ATTACK, destination);
				//Do random count of bombing runs
				int runCount = SDRC_Misc.RandomInt(0, 4);
				//Do multiple ones if requested
				for (int i = 0; i < runCount; i++)
				{
					float angle = SDRC_Misc.RandomFloat(0, 360);
					float distance = SDRC_Misc.RandomFloat(200, 400);
					vector rndPos = SDRC_Misc.GetCoordinatesOnCircle(destination, distance, angle);
					AddDestinationPoint(SDRC_EFlyWayPointType.WP_FLY, rndPos, value);
					distance = SDRC_Misc.RandomFloat(200, 400);
					rndPos = SDRC_Misc.GetCoordinatesOnCircle(destination, distance, angle + SDRC_Misc.RandomFloat(-120, 120));
					AddDestinationPoint(SDRC_EFlyWayPointType.WP_FLY, rndPos, value);
					AddDestinationPoint(SDRC_EFlyWayPointType.WP_ATTACK, destination, value);

				}
				m_vAttackPosition = destination;			//Where to attack				
				m_fTimerAttack = value * (runCount + 1);	//For how long to continue attacks. +1 to avoid runCount = 0 resulting in zero time
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
			
/*			SDRC_FlyPathPoint fpp = new SDRC_FlyPathPoint();
			fpp.Set(type, destination, value);				
			
			if (index > -1)
			{
				m_vFlyDestinations.InsertAt(fpp, index);
			}
			else
			{
				m_vFlyDestinations.Insert(fpp);
			}*/
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
	/*!	
	Clear the destination as a preparation for a completely new path
	*/
	void ResetDestinations()
	{
		m_vFlyDestinations.Clear();
	}	

	//------------------------------------------------------------------------------------------------	
	// Special handling
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Handle landing
	*/
	private void HandleLanding(IEntity owner, float timeSlice)
	{
		vector origin = owner.GetOrigin();
				
		vector lastPt = m_vSplinePoints[m_vSplinePoints.Count() - 1];
		float distance = vector.Distance(origin, lastPt);

		if (distance < m_fLandingDistance)
		{
			float height = m_Helicopter_s.GetAltitudeAGL();
				
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
				decrMul = Math.Clamp(decrMul, 0.1, 1.0);

				m_fSpeedTarget = m_fSpeedLandingOrig * distMul + 0.01;
								
				float decreasePower = -1.0;
				
				//Check if we're close to landing place, slow down and descent
				vector closePos = m_vSplinePoints[m_vSplinePoints.Count() - 3];
				//If we have passed the point, adjust values
				if (SDRC_Math.HasPassedPointXZ(m_fPositionLandingOrig, closePos, owner.GetOrigin()))
				{
					distance = vector.Distance(origin, m_fPositionLandingOrig);
					float distanceClose = vector.Distance(closePos, m_fPositionLandingOrig);
					float mulc = distanceClose / distance;
					
					m_fSpeedTarget = distance / 4;
					m_fSpeedTarget = Math.Clamp(decrMul, 1.0, 5.0);
					decreasePower = 5 * ((mulc + decrMul) / 2);
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
				SDRC_Log.Add("[SDRC_ChopperComp:HandleLanding] Ground contact!", LogLevel.DEBUG);
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
	
	//------------------------------------------------------------------------------------------------	
	// Enemy related
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------	
	/*!
	Enable/Disable enemy searching
	*/		
	void SetEnemySearchType(SDRC_EHeliEnemySearchType type)
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
	// Helicopter settings
	//------------------------------------------------------------------------------------------------	

	//------------------------------------------------------------------------------------------------	
	void SetHeli(float speedMin, float speedMax, float flyHeightLow, float flyHeightHigh, float distanceLow, float distanceHigh)
	{
		SDRC_Log.Add("[SDRC_ChopperComp:SetHeli] Updating values.", LogLevel.DEBUG);
	
		m_fSpeedMin = speedMin;
		m_fSpeedMax = speedMax;
		m_fFlyHeightLow = flyHeightLow;
		m_fFlyHeightHigh = flyHeightHigh;
		m_fDistanceLow = distanceLow;
		m_fDistanceHigh = distanceHigh;
		
		m_fSpeed = m_fSpeedMin;
	}	
		
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
}