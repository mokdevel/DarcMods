//SDRC_ChopperComp.c

// NOTE: The classes SDRC_ChopperCompClass and SDRC_ChopperComp are defined in DarcCore so that VehicleListHelper works 
//       also with other mods. This is why they are modded here.

//Changes done in prefabs:
// - SCR_AIVehicleUsageComponent : Set true to Can Be Piloted

#ifdef WORKBENCH
	//#define CHOPPER_TESTING
#endif

//------------------------------------------------------------------------------------------------
[ComponentEditorProps(category: "GameScripted/DarcChopper", description: "DarcChopper Main Component")]
modded class SDRC_ChopperCompClass : ScriptComponentClass { }

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
modded class SDRC_ChopperComp : ScriptComponent
{
	private SDRC_ChopperComp s_Instance;	
	ref array<vector> m_vSplinePoints = new array<vector>();
	private VehicleHelicopterSimulation m_Helicopter_s;
//	ref SDRC_ChopperParams params = new SDRC_ChopperParams();
	ref SDRC_ChopperParams params = null;
			
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
	
	[Attribute(category: "Chopper", defvalue: "15.0", desc: "Minimum speed", params: "1.0 100.0 0.1")]	
	float m_fSpeedMin;				//Minimum speed
	float m_fSpeedMinOrig = 2;
	[Attribute(category: "Chopper", defvalue: "40.0", desc: "Maximum speed", params: "1.0 100.0 0.1")]	
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
	[Attribute(category: "AI settings", defvalue: typename.EnumToString(EAISkill, EAISkill.VETERAN), uiwidget: UIWidgets.ComboBox, desc: "AI skill", enumType: EAISkill)]	
	EAISkill m_AISkill;	
	[Attribute(category: "AI settings", defvalue: "1.7", desc: "AI perception", params: "0.1 5.0 0.1")]	
	float m_AIPerception;
	[Attribute(category: "AI settings", defvalue: typename.EnumToString(SDRC_EHeliEnemySearchType, SDRC_EHeliEnemySearchType.ANY_CHAR), uiwidget: UIWidgets.ComboBox, desc: "Type of enemy to search", enumType: SDRC_EHeliEnemySearchType)]		
	SDRC_EHeliEnemySearchType m_EnemySearchType;
	ref array<AIGroup> m_aGroups = {};
	
	//Category: Weapons
	[Attribute(category: "Weapons", defvalue: "12.0", desc: "The sector where rockets may be shot", params: "1.0 45.0 1.0")]	
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
	ref array<ref SDRC_FlyPathPoint> m_vFlyDestinations;		//Requested destinations
	//Autonomous flying stuff
	[Attribute(defvalue: typename.EnumToString(SDRC_EChopperType, SDRC_EChopperType.HELICOPTER), uiwidget: UIWidgets.ComboBox, desc: "The type of the entity.", enumType: SDRC_EChopperType)]		
	SDRC_EChopperType m_EntityType;
	[Attribute(defvalue: "0", desc: "Entity does not need pilots")]	
	bool m_bUnpiloted;
	//Debug stuff	
	[Attribute(defvalue: "0", desc: "Show debugging information")]	
	bool m_bShowDebug;
	
	//Timing stuff
	private const float TIME_DELAY_READY = 1;			//(seconds) Time before we spawn AIs and init flight path. This will give time for the chopper to properly initialize
	private const float TIME_IN_INIT = 1;				//(seconds) Time to be in init state (after READY). During this time, we don't check for damage or similar things.
	
	//Original destination	
	private vector m_vOriginalDestination;				//Used to know where to patrol
		
	//Speed management
	private const float SPEED_INTERVAL = 1.0;			//(seconds) Interval to modify speed of the helicopter
	private const float SPEED_GAIN = 1.0;
	private float m_fTimeSpeed = 0;

	private float m_fTimeBetweenPts = 1;
	private float m_fTimeBetweenPtsAvg = 1;

	private float m_fTimeBetweenFixes = 0;
	
	float m_fTimeInState = -1;							//The timer to stay in a certain state. This is only in effect when positive value.
	float m_fTimeInStateOrig = -1;						//The timer value that was set to m_fTimeInState. This can be used to calculate % gone from the time is was set.
	private bool m_bTimeInStateEnabled = false;	
	
	//Flight path
	private const int POINTS_TO_NEW_DISTANCE = 3;		//How many spline points in to the future flight path is checked before adding new flight points.
	private const int POINTS_TO_SPLINE_START = 5;		//Points to go back from m_iClosestIndex when creating a new flight path 
	private const int DESTINATION_POINT_DIV = 12;		//How many points ahead to look for the destination. This is the divider for speed.

	private const int FLIGHT_FIX_TIME = 2;				//(seconds) Time to wait between flight fixes when chopper is pointing to the sky.
	private const int FLIGHT_FIX_ANGLE = 1.4;			//Angle that enforces 
	
	//Helistate
	private SDRC_EHeliState m_eHeliState;
	private bool m_bInInit;
	private bool m_bSetupDone = false;					//Setup may be called from mod or via EOnInit. Run it only once.
	
	//HeliBehaviour
	private const int TIME_IN_BEHAVIOUR = 600;			//(seconds) Default time to stay in behaviour 
	private SDRC_EHeliBehaviour m_eHeliBehaviour;
	float m_fTimerBehaviour = 0;						//Timer to stay in behaviour before changing to NORMAL
	float m_fTimerBehaviourCycle = 0;					//Timer between actions while in non-NORMAL behaviour
		
	//Health
	private float m_fHealthOrig = 0;
	SDRC_EHeliDamageLevel m_eDamageLevel = SDRC_EHeliDamageLevel.UNDAMAGED;
	const int SAFE_LANDING_SIZE = 50;					//Radius of the are to consider safe for landing
	
	//Runtime parameters
	private int m_iDestinationPointAdd;
	private float m_fTimeTurnInterval;
	
	//Flight path runtime variables	
	private vector m_vOrigin;							//Current position
	float m_fAltitude;									//Current altitude from ground
	float m_fSpeed;										//Current speed
	float m_fSpeedStart;								//Speed lerp start
	float m_fSpeedTarget;								//Speed lerp target aka end
	float m_fSpeedMul;									//Speed multiplier that depends on the turn
	float m_fSpeedSlowingMul;							//[0..1] Landing/braking speed modifier that affects speed and Yaw-Pitch-Roll.
	float m_fRotorForceMultiplier;						//Rotor force multiplier that simulates up/down throttle
	vector m_vVelocityVector;							//Velocity vector stored in case it's needed by external mods
	
	//Angular velocities
	private vector m_vAngularVel;
	vector m_vRollTarget;
	private vector m_vRadRollVel;
	private vector m_vRadRollBack;
	vector m_vRadRollPitch;

	//Heli directions
	private vector m_vHeliForward;
	private vector m_vHeliDirection;
	vector m_vHeliDirectionFuture;
	
	//Debug stuff
	string m_sDid;								//Id for debug items
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
	
	//Landing related
	private bool m_bIsLanding;					//If true, landing sequence has started
	private bool m_bFinalLanding;				//If true, we're in the final landing stages really close to the target
	float m_fLandingDistance;					//Distance to start the landing.
	private float m_fLandingSpeed;				//The speed to descend the chopper
	private float m_fSpeedLandingOrig;			//Speed from where we start to descend
	private vector m_fPositionLandingOrig;		//Position from where we start to descend
	
	//Braking related
	private bool m_bIsBraking;					//If true, braking sequence has started
	float m_fBrakingDistance;					//Distance for braking
	private float m_fBrakingSpeed;				//The speed to brake the chopper
	private float m_fSpeedBrakingOrig;			//Speed from where we start to brake
	private vector m_fPositionBrakingOrig;		//Position from where we start to brake
	
	//Enemy positions
	vector m_vEnemyPosition = vector.Zero;		//Position of last found enemy
	int m_iEnemyFoundTime;						//Time to wait to before allowing enemy position 
	int m_iEnemyFoundTimeout = 2;				//Time between enemy position updates
	int m_iEnemyForgetTimeout = 10;				//Time to forget the enemy position
		
	//Attack related
	private const int TIME_ATTACK_RUN = 40;		//Default time for each attack run
	private float m_fTimerAttack = 0;			//Timer to do attacks
	private vector m_vAttackPosition;			//Position to attack

	
	//The order of things:
	//- Spawn chopper via GM or mod
	//- OnPostInit()
	//  - Calls TypeSetupParams()
	//- EOnInit()
	//- External mod code if any after this. Here we can set AutoStart to false to control the spawn details ourselves.
	//
	//If AutoStart is enabled, we continue automatically with steps below.
	//- Setup() is run after a delay
	//  - Calls TypeSetup()
	//- Ready() is automatically called if AutoStart is enabled. If not, remember to call it in your mod!
	//- ReadyDelayed() .. to do delayed spawn of AI crew
	//- ReadyDelayed_2() .. to do delayed counting of pilots
	//- InitDone() 
	//  - Calls TypeInitDone()
	//- EOnFrame()
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		SDRC_Log.Add("[SDRC_ChopperComp:EOnInit] Here!", LogLevel.DEBUG);
		SDRC_SpawnHelper.SetPersistence(owner, false);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		if (!GetGame().GetWorld())
		{
			return;
		}
		
		if (!SDRC_Misc.IsMaster())
		{		
			return;
		}
		
		SDRC_Log.Add("[SDRC_ChopperComp] Starting SDRC_ChopperComp", LogLevel.NORMAL);
		
		//Deactivate(owner);
		//Set chopper type specific params
		TypeSetupParams(owner);
		SetHeli(m_fSpeedMin, m_fSpeedMax, m_fFlyHeightLow, m_fFlyHeightHigh, m_fDistanceLow, m_fDistanceHigh);						

		//Disable persistence for choppers. We don't need them to be stored.
		SDRC_SpawnHelper.SetPersistence(owner, false);
				
		//SetEventMask(owner, EntityEvent.INIT);
		s_Instance = this;
		m_sDid = SDRC_Misc.GetCurrentTickTime().ToString() + Math.RandomInt(0, 10000);
		m_fHealthOrig = SDRC_VehicleHelper.GetHealth(owner);
		
		m_bInInit = true;
		SetState(SDRC_EHeliState.FLY);
		SetBehaviour(SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR, -1);
		
		//Clear any existing path points
		ResetFlight();
		SetTimeInState(0);
		
		//Initialize enemyFoundTime
		m_iEnemyFoundTime = SDRC_Misc.GetCurrentTickTime() + m_iEnemyFoundTimeout;
		
		//Set wheel brake on
		HelicopterControllerComponent hcc = HelicopterControllerComponent.Cast(owner.FindComponent(HelicopterControllerComponent));
		if (hcc)
		{
			hcc.SetPersistentWheelBrake(true);
		}
		
		m_Helicopter_s = VehicleHelicopterSimulation.Cast(GetOwner().GetRootParent().FindComponent(VehicleHelicopterSimulation));
		if (!m_Helicopter_s)
		{
			SDRC_Log.Add("[SDRC_ChopperComp] VehicleHelicopterSimulation not found.", LogLevel.WARNING);
		}
		
		if (owner.GetPhysics())
		{		
			owner.GetPhysics().SetVelocity("0 0 0");
		}
		
		GetGame().GetCallqueue().CallLater(Setup, TIME_DELAY_READY * 1000, false, owner);		
		
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
	Set the helicopter to normal state
	*/	
	void InitDone(IEntity owner)
	{
		m_bInInit = false;

		//Run chopper type specific InitDone
		TypeInitDone(owner);
					
		//Collect groups in the helicopter 
		SDRC_VehicleHelper.GroupFindAll(owner, m_aGroups);
		
		//Check if pilots were possible to set
		if (m_bUnpiloted)
		{
			SDRC_Log.Add("[SDRC_ChopperComp:InitDone] Unpiloted entity.", LogLevel.DEBUG);			
		}
		else if (SDRC_VehicleHelper.PilotCountAlive(owner) == 0)
		{
			SDRC_Log.Add("[SDRC_ChopperComp:InitDone] Unable to set pilots.", LogLevel.WARNING);
		}
		
		SDRC_Log.Add("[SDRC_ChopperComp:InitDone] DONE!", LogLevel.DEBUG);
		GetGame().GetCallqueue().CallLater(AddChopperToList, 1000, false, owner);	
	}	

	//------------------------------------------------------------------------------------------------	
	// Flight model functionality
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (m_eHeliState == SDRC_EHeliState.DESPAWN)
		{
			DeSpawn(owner);
			return;			
		}
		
		if (m_bInInit)
		{
			//While in init, do not move
			owner.GetPhysics().SetVelocity(vector.Zero);
			
			//If the chopper is damaged, init is considered done.
			if (m_eDamageLevel != SDRC_EHeliDamageLevel.UNDAMAGED)
			{
				InitDone(owner);
			}
			return;
		}
		
		//See if there are type specific EOnFrame things to do
		TypeEOnFrame(owner, timeSlice);
		
		//If chopper is destroyed, let Reforger handle crash etc. Just stop everything we used to do on EOnFrame.
		if (m_eHeliState == SDRC_EHeliState.DESTROYED)
		{
			return;
		}		
		
		m_vOrigin = owner.GetOrigin();
		m_fAltitude = GetAltitude();
	
		m_fTimeSpeed += timeSlice;
		m_fTimeBetweenPts += timeSlice;
		m_fTimeBetweenFixes -= timeSlice;
		m_fTimeInState -= timeSlice;		
		m_fTimeRocketDelay += timeSlice;		
		m_fTimerAttack -= timeSlice;
		m_fTimerBehaviour -= timeSlice;
		m_fTimerBehaviourCycle -= timeSlice;

		//---
		//Check if we're still functional	
		if (!IsStillWorking(owner, m_bInInit))
		{
			//Nope, we're done
			SetState(SDRC_EHeliState.DESTROYED);
			SDRC_DebugHelper.DeleteDebugItems(m_sDid);
			return;
		}		
		//---
		//Normal flying part
		if ( (m_fTimeBetweenPts > 20) && (m_eHeliState == SDRC_EHeliState.FLY) )
		{
			#ifdef WORKBENCH			
				SDRC_Log.Add("[SDRC_ChopperComp] Time in point is very long.", LogLevel.DEBUG);
			#endif
			if (m_iClosestIndex < m_vSplinePoints.Count() - 1)
			{
				m_iClosestIndex++;
			}						
		}
	
		//Adjust time depending on the speed.
		m_fTimeTurnInterval = params.turnTimeIntervalBase / m_fSpeed;
		m_fTimeTurnInterval = Math.Clamp(m_fTimeTurnInterval, 0.6, 3);
				
		//Sometimes the heli direction and path align so that the closest index does not update.
		//In these case the helicopter up vector and world up vector is big.
		float heliUpAngleToWorld = SDRC_Math.GetAngleBetweenVectors(owner.GetTransformAxis(1), vector.Up);	
		if (heliUpAngleToWorld > FLIGHT_FIX_ANGLE)
		{
			if (m_fTimeBetweenFixes < 0)
			{
				#ifdef WORKBENCH
					SDRC_Log.Add("[SDRC_ChopperComp] Fixing flight.", LogLevel.DEBUG);
				#endif

				//Cut the spline
				SDRC_ChopperHelper.CutSplineHead(m_vSplinePoints, m_iClosestIndex);
				
				//Add another point behind the heli. Heli should fix it's flight
				vector direction = vector.Direction(m_vSplinePoints[1], m_vSplinePoints[0]);
				direction.Normalize();
				vector newPos = m_vSplinePoints[0] + direction * 200;
				newPos[1] = m_vOrigin[1];
				m_vSplinePoints.InsertAt(newPos, 0);
				
				//Reset runtime values
				m_iClosestIndex = 0;
				m_fTimeBetweenFixes = FLIGHT_FIX_TIME * 5;	//Let's give some time to do the actual fix
				
				SDRC_ChopperDebug.DrawDebugPaths(owner);
				SDRC_DebugHelper.AddDebugPos(newPos, ARGB(255, 255, 0, 255), 2.0, m_sDid);
			}
		}
		
		//No need to do anything unless we are at the end of spline.
		if (m_iClosestIndex + m_iDestinationPointAdd + POINTS_TO_NEW_DISTANCE >= m_vSplinePoints.Count() - 1)
		{
			m_fTimeBetweenFixes = FLIGHT_FIX_TIME;	//Time between tries to fix the flight
			
			if (   (m_eHeliState == SDRC_EHeliState.FLY) 
				|| (m_eHeliState == SDRC_EHeliState.FLY_AWAY) 
				|| (m_eHeliState == SDRC_EHeliState.RAISE)
			   )
			{
				//Define a new destination and create a new path
				CreateNewFlight(owner);
			}
		}
		
		//Count destintation addition along the spline which is dependent on the speed.
		m_iDestinationPointAdd = m_fSpeed / DESTINATION_POINT_DIV;
		m_iDestinationPointAdd = Math.ClampInt(m_iDestinationPointAdd, 1, 3);
		
		//Find where we're going
		m_iClosestIndex = SDRC_ChopperHelper.FindNextSplinePointIndex(m_vOrigin, m_iClosestIndex, m_vSplinePoints);
		
		if (m_iClosestIndex > m_iOldClosestIndex)
		{
			m_fTimeBetweenPtsAvg = m_fTimeBetweenPts;		//TBD: m_fTimeBetweenPtsAvg is a static value of previous time instead of average 
			m_fTimeBetweenPts = 0;
			m_iOldClosestIndex = m_iClosestIndex;
		}
		
		//Destination point definition
		m_iFutureIndex = m_iClosestIndex + (m_iDestinationPointAdd * 2);
		m_iNextIndex = m_iClosestIndex + m_iDestinationPointAdd;

		//Check that values are within limits
		if (m_iNextIndex > m_vSplinePoints.Count() - 1)
		{
			m_iNextIndex = m_vSplinePoints.Count() - 1;
		}
		
		if (m_iFutureIndex > m_vSplinePoints.Count() - 1)
		{
			m_iFutureIndex = m_vSplinePoints.Count() - 1;
		}

		if (m_fTimeBetweenPtsAvg == 0)
		{
			m_fTimeBetweenPtsAvg = 0.01
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
			m_fSpeed = Math.Clamp(m_fSpeed, m_fSpeedMin, m_fSpeedMax);
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
				
		//Calculate rotor force
		HandleRotorForce(owner);
		
		//Set turn
		SetTurn(owner, m_fTimeTurnInterval);
		
		//Handle states, attacks, ...
		HandleState(owner, timeSlice);
		TypeHandleAttack(owner);
		HandleBehaviour(owner);

		//Set velocity
		SetVelocity(owner, timeSlice);

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
		m_fSpeedMul = Math.Clamp((angle * Math.RAD2DEG), params.turnSpeedDegreeMin, params.turnSpeedDegreeMax);
		m_fSpeedMul = m_fThrottle * (SPEED_GAIN - (m_fSpeedMul / params.turnSpeedDivider));

		//In case we're landing, we need to modify the speed
		m_fSpeedMul = m_fSpeedMul * m_fSpeedSlowingMul;
		
		//Set 
		m_fSpeedStart = m_fSpeed;
		m_fSpeedTarget = m_fSpeed * m_fSpeedMul;

		//If we're too close to ground, slow down the speed to allow time for climb
//		float altitude = SDRC_ChopperHelper.GetAltitude(owner);
		const int ALTITUDE_ADD = 5;
		if ((m_fAltitude + ALTITUDE_ADD) < m_fFlyHeightLow)
		{		
			float mul = (m_fAltitude + ALTITUDE_ADD) / m_fFlyHeightLow;
			mul = Math.Clamp(mul, 0, 1);
			m_fSpeedTarget = m_fSpeedTarget * mul;
		}

		m_fSpeedTarget = Math.Clamp(m_fSpeedTarget, m_fSpeedMin, m_fSpeedMax);
		m_fTimeSpeed = 0;	//Start to change speed
								
		//Handle yaw, pitch, roll		
				
		//ROLL PITCH: Change pitch according to speed
		float endDiv = Math.Clamp((m_fSpeedTarget - m_fSpeedStart), 0.001, 1000);
		float speedMul = (m_fSpeed - m_fSpeedStart) / endDiv;
		m_fAnglePitch = params.pitchAngleRadFlat + params.pitchAngleRad * speedMul;		
		m_fAnglePitch = Math.Clamp(m_fAnglePitch, params.pitchNoseAngleDown, params.pitchNoseAngleUp);	//Nose down, nose up
		
		if (m_eHeliState == SDRC_EHeliState.RAISE)
		{
			//Turn nose down
			m_fAnglePitch = -25 * Math.DEG2RAD;
		}
		
		m_vRadRollPitch = SDRC_Math.RotateAroundAxis(m_vHeliForward, heliPitch, m_fAnglePitch);
		m_vRadRollPitch = SDRC_Math.ComputeAngularVelocity(m_vHeliForward, m_vRadRollPitch, deltaTime * 0.5);
		
		//ROLL ON DIRECTION: See how steep we're turning. Roll the helicopter accordingly for more natural flight. We only care about ZX plane.
		m_fAngleRoll = SDRC_Math.GetAngleBetweenVectorsXZ(m_vHeliForward, m_vHeliDirectionFuture);
		m_fAngleRoll = Math.Clamp(m_fAngleRoll, -0.5, 0.5) * params.rollAngleMul;
		m_vRadRollVel = SDRC_Math.RotateAroundAxis(m_vHeliForward, heliUp, m_fAngleRoll);
		m_vRadRollVel = SDRC_Math.ComputeAngularVelocity(heliUp, m_vRadRollVel, deltaTime);
		
		//Count the angular velocity
		m_vAngularVel = SDRC_Math.ComputeAngularVelocity(m_vHeliForward, m_vHeliDirection, deltaTime);

		//Handle angular velocities when slowing down
		if (m_fSpeedSlowingMul < 0.3)
		{
			//Flatten the chopper when landing/braking/hovering
			m_vAngularVel = vector.Zero;
			m_vRadRollVel = vector.Zero;
			m_vRadRollPitch = vector.Zero;
		}
		else if (m_fSpeedSlowingMul < 1.0)
		{
			//Do only minor adjustments
			m_vAngularVel = m_vAngularVel * m_fSpeedSlowingMul * 0.2;
			m_vRadRollVel = m_vRadRollVel * m_fSpeedSlowingMul;
//			m_vRadRollPitch = m_vRadRollPitch * m_fSpeedSlowingMul * 0.01;
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
			float forceRotorUp = m_fRotorForce0 * params.rotorForceMulUp;
			
			if (m_bFinalLanding)
			{
				velVector[0] = 0;
				velVector[2] = 0;
				velVector[1] = Math.Clamp(velVector[1], 0.01, 0.1);
			}
			
			float vectorUp = velVector[1] * forceRotorUp * m_fRotorForceMultiplier;
			vectorUp = Math.Clamp(vectorUp, -30, 30);
			
			if (vectorUp == 0)
			{
				velVector = "0 -0.5 0";
			}
			else
			{
				velVector = {velVector[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * forceMultiplier, vectorUp, velVector[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * forceMultiplier};
			}
		}
				
		/*if (m_bInInit)
		{
			velVector = vector.Zero;
		}*/
		
		m_vVelocityVector = velVector;
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

		//The normal way to slowly go towards the spline
		int bigMul = params.rotorForceNormal;

		float belowFlyHeightLowMul = 1;
		float distanceFromSplineMul = 1;
		float rayLenMul = 1;

		float splineHeightFromGround = m_vSplinePointBelow[1];		
		float heliHeightFromGround = m_vOrigin[1] - 10;				//Move the origin slightly below the spline
		if (heliHeightFromGround <= 0)
		{
			heliHeightFromGround = 1.0;
		}
		distanceFromSplineMul = (splineHeightFromGround - heliHeightFromGround) / heliHeightFromGround;			
		
		//Modify the values depending on state
		switch (m_eHeliState)
		{
			case SDRC_EHeliState.HOVER_UP:
			{
				if (m_fTimeInState > 0)
				{
					//In HOVER_UP state, do movemements slow
					bigMul = bigMul * (1 - (m_fTimeInState/m_fTimeInStateOrig));
					distanceFromSplineMul = distanceFromSplineMul * (1 - (m_fTimeInState/m_fTimeInStateOrig));
				}
				break;
			}		
			case SDRC_EHeliState.RAISE:
			{
				//In RAISE state, do slow climb
				bigMul = params.rotorForceRaise;
				break;
			}
			case SDRC_EHeliState.BRAKE:
			{
				//In BRAKE state, do movemements faster
				bigMul = bigMul * 3.0;
				break;
			}		
			case SDRC_EHeliState.HOVER:
			{				
				//Stay in one place
				bigMul = params.rotorForceHover;
				break;
			}
			case SDRC_EHeliState.FLY:
			{
				//Modify if we're too close to the ground, do additional raise
				if (m_fAltitude < m_fFlyHeightLow)
				{				
					belowFlyHeightLowMul = 1 + (m_fFlyHeightLow - m_fAltitude) / m_fAltitude;
				}
	
				//If we're close to an object infront of us, raise			
				vector rayEnd = SDRC_ChopperHelper.GetDestinationForward(owner, params.rayLenFront);
				rayEnd[1] = rayEnd[1] - params.rayDown;
				float rayLen = SDRC_Misc.RayCastXZ(owner.GetOrigin(), rayEnd, owner);			
				if (rayLen < 1)
				{
					rayLenMul = 10 * (1.3 - rayLen);
				}
			}
		}
		
		m_fRotorForceMultiplier = bigMul * belowFlyHeightLowMul * distanceFromSplineMul * rayLenMul;
	}
			
	//------------------------------------------------------------------------------------------------	
	// Flight path things
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Create the initial flight path 
	\param owner The chopper entity
	\param destination The first destination to fly to.if
	
	The first destination priority is:
	- Parameter destination if assigned
	- The first fly destination if assigned
	- Random point in front of heli, if nothing is pre defined
	*/
	void InitFlight(IEntity owner, vector destination = vector.Zero)
	{
/*		if (!GetGame().GetWorld())
		{
			return;
		}*/

		//Store the origin. This value is updated in EOnFrame, but needed already in calculations.
		m_vOrigin = owner.GetOrigin();	
		float y = SDRC_Misc.GetSurfaceYWithWater(m_vOrigin, true, owner);
				
		//If we're on low altitude, wait for a moment and then hover to start flight
		if ( (m_vFlyDestinations.IsEmpty()) && (m_vOrigin[1] < (y + 3) ) )
		{	
			m_vOrigin[1] = y + 0.1;
			owner.SetOrigin(m_vOrigin);
			
			//Check if there is a destination assigned. Use this for the future destination
			if ( (!m_vFlyDestinations.IsEmpty()) && (destination == vector.Zero) )
			{
				destination = m_vFlyDestinations[0].pt;
			}
			
			//The low start destinations are added in the beginning of the list.
			//These are added in reverse order to index 0
			vector hoverPos = vector.Zero;
			hoverPos[1] = (m_fFlyHeightLow + m_fFlyHeightHigh) / 2;
			AddDestination(SDRC_EFlyWayPointType.WP_RAISE, hoverPos, index: 0);
			hoverPos[1] = m_fFlyHeightLow;
			AddDestination(SDRC_EFlyWayPointType.WP_HOVER_UP, hoverPos, 5, 0);
			#ifdef WORKBENCH			
				AddDestination(SDRC_EFlyWayPointType.WP_HOVER, "0 0 0", 8, 0);
			#else
				AddDestination(SDRC_EFlyWayPointType.WP_HOVER, "0 0 0", 30, 0);
			#endif
		}
		else //In the air. Normal case to check if a destination was assigned
		{
			//If a fly destination has been assigned, use it
			if ( (!m_vFlyDestinations.IsEmpty()) && (destination == vector.Zero) )
			{
				destination = m_vFlyDestinations[0].pt;
				//Add the destination to the list
				AddDestination(SDRC_EFlyWayPointType.WP_FLY, destination);
			}
		}
		
		//If no destination has been assigned, create a random one to use for rotating the chopper
		if (destination == vector.Zero)
		{
			destination = SDRC_ChopperHelper.GetDestinationForward(owner, params.destinationForwardInitial);
			//Make sure we're on proper flight height.
			destination[1] = SDRC_ChopperHelper.SetPointHeight(destination, m_fFlyHeightLow, m_fFlyHeightHigh); 
		}

		//Turn chopper to face the first destination
		SDRC_Math.TurnEntityTowardsXZ(owner, destination);				
		
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

		SDRC_Log.Add("[SDRC_ChopperComp:InitFlight] Chopper initial position: " + owner.GetOrigin(), LogLevel.DEBUG);
				
		//NOTE: We draw the debug paths once the component is ready
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Create the runtime flight path with waypoint definition
	*/
	void CreateNewFlight(IEntity owner)
	{		
//		vector oldHeight = m_vSplinePoints[m_vSplinePoints.Count() - 1];
		vector oldHeight = m_vSplinePoints[m_iClosestIndex];
		
		//Clear any existing path points
		ResetFlight();

		//Create points for spline		
		CreateFlightPoints(owner);
		
		SDRC_ChopperHelper.SetFlightPointHeight(owner);
		//Set the first points to same height as old ending spline. This smooths the flight
		m_vFlightPoints[0].pt[1] = oldHeight[1];
		m_vFlightPoints[1].pt[1] = oldHeight[1];
				
		array<vector> flyPathPoints = {};
		SDRC_ChopperDebug.GivePoints(flyPathPoints, m_vFlightPoints);
		SDRC_Spline3D.GenerateSplinePoints(flyPathPoints, m_vSplinePoints, -1);
		
		//Search the closest index from the spline start
		m_iClosestIndex = 0;
		m_iOldClosestIndex = m_iClosestIndex;
				
		//Check that points are above ground. Skip some of the points at start.
		//SDRC_ChopperHelper.SetSplinePointsAboveGround(owner, 6);
		SDRC_ChopperHelper.SetSplinePointsAboveGround(owner, 0);
		
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
	/*!	
	Clear the destination as a preparation for a completely new path
	*/
	private void ResetDestinations()
	{
		m_vFlyDestinations.Clear();
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
	private void CreateFlightPoints(IEntity owner, bool fixHeight = false)
	{
		//Add a few points in front to smooth the flight pattern
		float forwardDistance = params.destinationForward;
		vector origin = owner.GetOrigin();
		
		//Height is set to same as heli, but this is only needed at initial flight. 
		//Later on, CreateNewFlight() will set these heights to old spline height.
		vector pos = SDRC_ChopperHelper.GetDestinationForward(owner, forwardDistance/2);
		pos[1] = origin[1];
		if (fixHeight)
		{
			pos[1] = SDRC_ChopperHelper.SetPointHeight(pos, m_fFlyHeightLow, m_fFlyHeightHigh); 
		}
		AddFlyPathPoint(pos);
		//SDRC_DebugHelper.AddDebugPos(pos, ARGB(255, 0, 0, 255), 2.0, m_sDid);
		
		pos = SDRC_ChopperHelper.GetDestinationForward(owner, forwardDistance);
		pos[1] = origin[1];
		if (fixHeight)
		{
			pos[1] = SDRC_ChopperHelper.SetPointHeight(pos, m_fFlyHeightLow, m_fFlyHeightHigh); 
		}
		AddFlyPathPoint(pos);
		//SDRC_DebugHelper.AddDebugPos(pos, ARGB(255, 0, 255, 0), 2.0, m_sDid);
		
		//Add destinations .. if any
		int lastIdx = 0;
		
		//Generate a random destination point if needed
		if (m_vFlyDestinations.IsEmpty())
		{		
			SDRC_ChopperHelper.GenerateWayPoint(owner, pos);
		}
		
		bool firstDestinationHandled = false;
		bool oneShotHandled = false;
		
		//Handle destinations
		foreach (int idx, SDRC_FlyPathPoint flyDestination : m_vFlyDestinations)
		{		
			//FLY points are handled in a serie. Others one at a time.
			if (flyDestination.type != SDRC_EFlyWayPointType.WP_FLY)
			{			
				oneShotHandled = true;
				
				if (firstDestinationHandled)
				{
					break;
				}
			}
			
			//SDRC_DebugHelper.AddDebugPos(flyDestination.pt, ARGB(32, 255, 128, 64), 1.0, m_sDid, 50);
			
			bool destinationHandled = false;
			int patrolCount = 8;	//Do one round for patrol by default (8*45 degrees)
			
			switch (flyDestination.type)
			{
				case SDRC_EFlyWayPointType.WP_ATTACK:
				{
					//Attack to be on low altitude. This will be set in SDRC_ChopperHelper.SetSplinePointsAboveGround()
					SetState(SDRC_EHeliState.ATTACK);
					//NOTE: m_vAttackPosition has been set in AddDestination
					break;
				}
				case SDRC_EFlyWayPointType.WP_BRAKE:
				{
					//Brake height modification
					if (flyDestination.pt[1] != 0)
					{
						flyDestination.pt[1] = SDRC_Misc.GetSurfaceYWithWater(flyDestination.pt) + flyDestination.pt[1];				
					}
					break;
				}
				case SDRC_EFlyWayPointType.WP_HOVER:
				{
					SetNextState(owner, flyDestination.type, false);					
					destinationHandled = true;
					break;
				}
				case SDRC_EFlyWayPointType.WP_SEARCH_DESTROY:
				{	
					SetBehaviour(SDRC_EHeliBehaviour.SEARCH_AND_DESTROY_BEHAVIOUR, flyDestination.value);
					//NOTE: m_vAttackPosition has been set in AddDestination
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
						float value = flyDestination.value;
						if (value <= 0)
						{
							value = params.patrolRadius;
						}
						float range = Math.RandomFloat(value * 0.7, value * 1.3);					
						//Make waypoints around the position to patrol.					
						vector dir = SDRC_Math.RotateAroundAxis(m_vHeliDirection, vector.Up, sign * i * degree * Math.DEG2RAD);
						dir.Normalize();
						pos = flyDestination.pt + dir * range;						
						AddFlyPathPoint(pos);
						//SDRC_DebugHelper.AddDebugPos(pos, ARGB(255, 0, 0, 255), 2.0, m_sDid, 50 + i * 20);
					}
					destinationHandled = true;
					break;
				}				
			}
					
			if (!destinationHandled)
			{	
				//Distance of last flight point defined and the next destination
				float distance = vector.DistanceXZ(m_vFlightPoints[m_vFlightPoints.Count() - 1].pt, flyDestination.pt);
		
				//Get the angle for the destination
				vector p0 = m_vFlightPoints[m_vFlightPoints.Count() - 2].pt;
				vector p1 = m_vFlightPoints[m_vFlightPoints.Count() - 1].pt;
				vector p2 = flyDestination.pt;
				float heliAngle = SDRC_Math.GetRadiansBetweenThreePointsXZ(p0, p1, p2) * Math.RAD2DEG;
	
				SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Distance: " + distance + " - Angle: " + heliAngle, LogLevel.SPAM);
				
				//Is the angle too steep? Re-route.
//				if ( (Math.AbsFloat(heliAngle) < params.wpSteepAngle) && (distance > 200) )
				if (Math.AbsFloat(heliAngle) < params.wpSteepAngle)
				{				
					SDRC_Log.Add("[SDRC_ChopperComp:GenerateWayPoint] Heli direction angle is steep: " + heliAngle, LogLevel.SPAM);
					
					//Get the last point
					vector point = m_vFlightPoints[m_vFlightPoints.Count() - 1].pt;
					
					//We need to take a detour. Add an additional points outside of the line to make the route rounder				
					const float LERP_RND_DEFAULT = 0.65;
					const float DIV_RND_DEFAULT = 1.5;
					
					float lerpRnd = SDRC_Misc.RandomFloat((LERP_RND_DEFAULT - params.detourLerpPosition), LERP_RND_DEFAULT);
					float divRnd = SDRC_Misc.RandomFloat(DIV_RND_DEFAULT, (DIV_RND_DEFAULT + params.detourDivider));
										
					//Depending on the angle decide if we re-route left ot right				
					bool isOnLeft = SDRC_Math.IsPointOnLeft(p0, p1, p2);
									
					//Find a point along the fly path and move it away from the line along tangent
					vector vec = SDRC_Math.CreateOffsetMidPoint(point, flyDestination.pt, (distance / divRnd), lerpRnd, isOnLeft);
					AddFlyPathPoint(vec);				
					//SDRC_DebugHelper.AddDebugPos(vec, ARGB(255, 0, 0, 0), 1.0, m_sDid, 500);
				}
				
				AddFlyPathPoint(flyDestination.pt, flyDestination.type);
				
				SetNextState(owner, flyDestination.type, false);
			}
						
			lastIdx = idx;
			firstDestinationHandled = true;
						
			if (oneShotHandled)
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

		if (type == SDRC_EFlyWayPointType.WP_BRAKE)
		{
			SetState(SDRC_EHeliState.BRAKE);
			
			m_bIsBraking = false;
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
	/*!
	Get altitude from helicopter down to first object below
	*/
	float GetAltitude()
	{
		//Start to look for a position below heli
		float y = m_vOrigin[1] - SDRC_Misc.GetSurfaceYWithWater(m_vOrigin, true, GetOwner(), -0.1);
		if (y < 0)
		{
			y = 0.001;	//Do not set to zero as this is used in some division calculations
		}
		return y;
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
	SDRC_EHeliBehaviour GetBehaviour()
	{
		return m_eHeliBehaviour;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Sets the behaviour.
	\param time (seconds) How long to stay in behaviour. -1 = infinite
	*/	
	void SetBehaviour(SDRC_EHeliBehaviour behaviour, int time)
	{
		m_eHeliBehaviour = behaviour;

		//Reset timer for NORMAL
		if (behaviour == SDRC_EHeliBehaviour.NORMAL_BEHAVIOUR)
		{			
			time = 0;
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
	void SetTimeInState(int seconds)	
	{
		if (seconds == -1)
		{
			seconds = 0;
		}
		
		m_fTimeInState = seconds;
		m_fTimeInStateOrig = seconds;
		
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
	// Type specific functions. These should be over ridden by the specific type
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!
	This sets up the flight model params for a specific SDRC_EChopperType. Override this function in other types.
	This is called immediately when component is initialized.
	*/	
	void TypeSetupParams(IEntity owner) {}
	
	//------------------------------------------------------------------------------------------------
	/*!
	This sets up the flight model params for a specific SDRC_EChopperType. Override this function for other types.
	This is a delayed setup make sure the entity is properly initialized. 
	*/
	void TypeSetup(IEntity owner) {}

	//------------------------------------------------------------------------------------------------
	/*!
	Type specific init finalization
	*/	
	void TypeInitDone(IEntity owner) {}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Type specific faction setup.
	*/	
	void TypeSetFaction(IEntity owner, string faction = "") {}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Type specific things within EOnFrame. Override this function in other types.
	*/	
	void TypeEOnFrame(IEntity owner, float timeSlice) {}

	//------------------------------------------------------------------------------------------------
	/*!
	Type specific to get scaled health
	*/	
	void TypeGetHealthScaled(IEntity owner, out float health) {}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Type specific handling of the final parts after damage that breaks flying
	*/	
	void TypeHandleDamageFinal(IEntity owner) {}

	//------------------------------------------------------------------------------------------------
	/*!
	Type specific handling of attacks. Search for the enemy and then react on the finding.
	*/	
	void TypeHandleAttack(IEntity owner) {}
	
	//------------------------------------------------------------------------------------------------	
	// Helicopter setup - defined in modded class
	//------------------------------------------------------------------------------------------------	
	void Setup(IEntity owner) {}
	void DeSpawn(IEntity owner) {}
	//------------------------------------------------------------------------------------------------	
	// State Handling  - defined in modded class
	//------------------------------------------------------------------------------------------------	
	private void HandleState(IEntity owner, float timeSlice) {}
	private void HandleBehaviour(IEntity owner) {}
	private void SetNextState(IEntity owner, SDRC_EFlyWayPointType nextType = SDRC_EFlyWayPointType.WP_UNDEFINED, bool allowRemove = true) {}
	//------------------------------------------------------------------------------------------------	
	// Damage settings - defined in modded class
	//------------------------------------------------------------------------------------------------	
	bool IsStillWorking(IEntity owner, bool inInit) {}
	//------------------------------------------------------------------------------------------------	
	// Destination settings - defined in modded class
	//------------------------------------------------------------------------------------------------	
	void AddDestination(SDRC_EFlyWayPointType type = SDRC_EFlyWayPointType.WP_FLY, vector destination = vector.Zero, float value = -1, int index = -1) {}
	//------------------------------------------------------------------------------------------------	
	// Special handling - defined in modded class
	//------------------------------------------------------------------------------------------------	
	private void HandleLanding(IEntity owner, float timeSlice) {}
	private void HandleBraking(IEntity owner, float timeSlice) {}
	//------------------------------------------------------------------------------------------------	
	// Enemy related - defined in modded class
	//------------------------------------------------------------------------------------------------	
	void SetEnemySearchType(SDRC_EHeliEnemySearchType type) {}
	//------------------------------------------------------------------------------------------------	
	// Helicopter settings - defined in modded class
	//------------------------------------------------------------------------------------------------	
	void SetHeli(float speedMin, float speedMax, float flyHeightLow, float flyHeightHigh, float distanceLow, float distanceHigh) {}
	void SetEngine(bool engine, float throttle, float rotorForce0, float rotorForce1) {}
	//------------------------------------------------------------------------------------------------	
	// Misc - defined in modded class
	//------------------------------------------------------------------------------------------------	
	void AddChopperToList(IEntity owner) {}
}