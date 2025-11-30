//SDRC_ChopperComp.c

//Changed done in prefabs:
// - SCR_AIVehicleUsageComponent : Set true to Can Be Piloted

//------------------------------------------------------------------------------------------------
class SDRC_ChopperCompClass : ScriptGameComponentClass { }
//SDRC_RplGMCompClass g_RplGMCompClass;

//------------------------------------------------------------------------------------------------
class SDRC_ChopperComp : ScriptGameComponent
{
	private static SDRC_ChopperComp s_Instance;	
	private ref array<vector> m_vSplinePoints = new array<vector>();
	private ref array<vector> m_vTangentPoints = new array<vector>();

	//Speed management
	const float SPEED_INTERVAL = 2;		
	const float SPEED_GAIN = 1.4;		
	private float m_fTimeSpeed = 0;
	
	private float m_fTimeBetweenPts = 1;
	private float m_fTimeBetweenPtsAvg = 1;
		
	//Turn
	const int TIME_TURN_INTERVAL_BASE = 40;			//Time to divide with speed to define the final turn time. Smaller value makes heli turn faster.
//	private float m_fTimeTurn = 0;
	
	//Pitch
	const float PITCH_ANGLE = 40;					//The pitch angle to use when calculating for speed effect. The faster the heli goes, the steeper the nose should be down.
	
	//Flight path
	const int SPLINE_POINT_DISTANCE = 10;//14;//25;		//Distance between spline points
	const int POINTS_TO_NEW_DISTANCE = 3;			//How many spline points in to the future flight path is checked before adding new flight points.
	const int POINTS_TO_SPLINE_START = 6;			//Points to go back from m_iClosestIndex when creating a new flight path 
	const int DESTINATION_POINT_DIV = 12;			//How many points ahead to look for the destination. This is the divider for speed.
	const float TIME_IN_INIT = 25;					//Seconds to be in init state
	
	//Helicopter parameters
	const float ROTOR_FORCE_UP = 18.0;	
	
	//Setup parameters
	private float m_fGroundLow = 5;
	private float m_fGroundHigh = 40;
	private float m_fLen = 0;
	
	//Runtime parameters
	private bool m_bInInit = true;			//While in init, consider the chopper to be flying.
	private bool m_bDestroyed = false;
	private int m_iSegments;
	private int m_iSegmentPoints;
	private int m_iDestinationPointAdd;
	private float m_fTimeTurnInterval;
	
	//Flight path runtime variables	
	private float m_fSpeedMin = 10;			//Minimum speed
	private float m_fSpeedMax = 30;			//Maximum speed
	private float m_fSpeedGain = 1.1;		//Speed gain aka acceleration
	private float m_fSpeed = 30;			//Current speed
	private float m_fSpeedStart;			//Speed lerp start
	private float m_fSpeedTarget;			//Speed lerp target aka end
	private float m_fSpeedMul = 1;			//Speed multiplier that depends on the turn
	
	//Angular velocities
	private vector m_vAngularVel;
	private vector m_vRollTarget;
	private vector m_vRadRollVel;
	private vector m_vRadRollBack;
	private vector m_vRadRollPitch;

	//Debug stuff
	private float m_fDbgAngle;
	private float m_fDbgAnglePitch;
		
	int m_iClosestIndex;					//Closest point on spline to heli
	int m_iNewClosestIndex;
	int m_iNextIndex;						//Next index to our m_iClosestIndex - depends on speed
	int m_iFutureIndex;						//Where we are heading in the long run
	vector m_vDestination;
	vector m_vDestinationFuture;

	//Flight path
	//Arland
	ref array<vector> m_vPathPoints = {
		"1500 020 1800",
		"1500 030 2000",
		"1300 010 2200",
	};

	int m_iFlyPathIdx = 0;
	ref array<vector> m_vFlyPath = {
		"1700 015 2400",
		"1300 030 2600",
		"1700 040 2800",
		"1300 020 3000",
	};	
		
/*	//Arland
	ref array<vector> m_vPathPoints = {
		"1500 020 1800",
		"1500 030 2000",
		"1400 010 2200",
	};

	int m_iFlyPathIdx = 0;
	ref array<vector> m_vFlyPath = {
		"1600 015 2300",
		"1900 030 2900",
		"2300 040 2500",
		"2400 020 2250",	//Timber Ridge
		"3100 030 2800",	//Beauregard
		"2400 030 1600",
		"1900 000 1300",
		"1500 000 2200",
		"2200 020 2200",
	};*/
		
	override void OnPostInit(IEntity owner)
	{
		SDRC_Log.Add("[SDRC_ChopperComp] Starting SDRC_ChopperComp", LogLevel.NORMAL);
		s_Instance = this;				
		SetEventMask(owner, EntityEvent.FRAME | EntityEvent.POSTFRAME);
		
		m_fGroundLow = 5;
		m_fGroundHigh = 40;
		
		InitFlightPath();
		
		m_iClosestIndex = 3;
		m_iNewClosestIndex = m_iClosestIndex + 1;

		m_fSpeedTarget = m_fSpeed;
		
		//Set chopper initial position		
		owner.SetOrigin(m_vSplinePoints[0]);
		vector angles = vector.Direction(owner.GetOrigin(), m_vSplinePoints[m_iClosestIndex]);
		angles.Normalize();
		angles = angles.VectorToAngles();
		owner.SetYawPitchRoll(angles);

		SetVelocity(owner);
		
//		GetGame().GetCallqueue().CallLater(SetDamage, (TIME_IN_INIT + 5) * 1000, false, owner);		
		GetGame().GetCallqueue().CallLater(InitDone, TIME_IN_INIT * 1000);
		Activate(owner);
	}
 
	void SetDamage(IEntity owner)
	{
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(owner.FindComponent(DamageManagerComponent));
		float health = 0.02;//SDRC_Misc.RandomFloat(0, 0.15);
		if (damageManager)
		{
			damageManager.SetHealthScaled(health);
		}
		
		SDRC_Log.Add("[SDRC_ChopperComp:SetDamage] Setting health: " + health, LogLevel.DEBUG);
	}

	void InitDone()
	{
		m_bInInit = false;
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
	// Flight model functionality
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		DrawHelicopterVectors(owner);
						
		//If chopper is destroyed, let Reforger handle crash etc.
		//Checking is done in Turn function. Not needed in every frame.
		if (m_bDestroyed)
		{
			return;
		}
				
		m_fTimeSpeed += timeSlice;
		m_fTimeBetweenPts += timeSlice;		

		vector origin = owner.GetOrigin();

		//Adjust time depending on the time.
		m_fTimeTurnInterval = TIME_TURN_INTERVAL_BASE / m_fSpeed;
		m_fTimeTurnInterval = Math.Clamp(m_fTimeTurnInterval, 0.6, 3);	//Was 1.8
				
		//Count destintation addition along the spline which is dependent on the speed.
		m_iDestinationPointAdd = m_fSpeed / DESTINATION_POINT_DIV;
		m_iDestinationPointAdd = Math.Clamp(m_iDestinationPointAdd, 1, 3);
		
		//Find where we're going
		float distance = SDRC_Spline3D.GetDistanceFromSpline(m_vSplinePoints, origin, m_iNewClosestIndex);	//NOTE: This will set m_iNewClosestIndex

		if (m_iNewClosestIndex > m_iClosestIndex)
		{
			m_iClosestIndex = m_iNewClosestIndex;
			m_fTimeBetweenPtsAvg = m_fTimeBetweenPts;
			m_fTimeBetweenPts = 0;
		}
		else if (m_iNewClosestIndex == m_iClosestIndex)
		{
			m_iClosestIndex++;
			m_fTimeBetweenPtsAvg = m_fTimeBetweenPts;
			m_fTimeBetweenPts = 0;
		}
				
		//Destination point definition
		m_iFutureIndex = m_iClosestIndex + (m_iDestinationPointAdd * 2);
		m_iNextIndex = m_iClosestIndex + m_iDestinationPointAdd;

		if (m_iNextIndex >= m_vSplinePoints.Count() - 1)
		{
			m_iNextIndex = m_vSplinePoints.Count() - 1;
		}
		
		if (m_iFutureIndex >= m_vSplinePoints.Count() - 1)
		{
			m_iFutureIndex = m_vSplinePoints.Count() - 1;
		}
		
		m_vDestination = m_vSplinePoints[m_iNextIndex];
		m_vDestinationFuture = m_vSplinePoints[m_iFutureIndex];
		
		//Lerped destination
		float td = m_fTimeBetweenPts / m_fTimeBetweenPtsAvg;
		td = Math.Clamp(td, 0, 1);
		
//		m_vDestination = vector.Lerp(origin, m_vSplinePoints[m_iNextIndex], td);
		m_vDestination = vector.Lerp(m_vSplinePoints[m_iNextIndex], m_vDestinationFuture, td);
		
		if (m_fTimeSpeed < SPEED_INTERVAL)
		{
			float ts = m_fTimeSpeed / SPEED_INTERVAL;
			m_fSpeed = Math.Lerp(m_fSpeedStart, m_fSpeedTarget, ts);
			m_fSpeed = Math.Clamp(m_fSpeed, m_fSpeedMin, m_fSpeedMax)
		}

		//Set velocity 
		SetVelocity(owner);
		
		//Set turn
		SetTurn(owner, m_fTimeTurnInterval);
				
		//Check if we need to define a new destination and create a new path
		CreateFlightPath(origin);
	}

	//------------------------------------------------------------------------------------------------	
	void SetVelocity(IEntity owner)
	{
		vector origin = owner.GetOrigin();
		
		//Define speed
		vector heliForward = owner.GetTransformAxis(2);
		vector heliDirection = vector.Direction(origin, m_vDestination);				
		vector heliVelocity = owner.GetPhysics().GetVelocity();
		
		//Set velocity
		vector velVector = vector.Direction(origin, m_vDestination);
		vector rotVector = owner.GetAngles();
		velVector.Normalize();
		float forceMultiplier = m_fSpeed;
		float gas = ROTOR_FORCE_UP;
		velVector = {velVector[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * forceMultiplier, velVector[1] * gas, velVector[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * forceMultiplier};
		
		owner.GetPhysics().SetVelocity(velVector);		
	}
	
	//------------------------------------------------------------------------------------------------	
	void SetTurn(IEntity owner, float deltaTime)
	{
		if (deltaTime == 0)
		{
			return;
		}

		//Check if we're still working. Not needed every frame. //TBD: Could be done every x seconds - not that critical
		IsStillWorking(owner);
		
		//Get heli position
		vector origin = owner.GetOrigin();
		//Get the heli vectors
		vector heliPitch = owner.GetTransformAxis(0);
		vector heliUp = owner.GetTransformAxis(1);
		vector heliForward = owner.GetTransformAxis(2);
		//Get chopper direction
		vector heliDirection = vector.Direction(origin, m_vDestination);		
		vector heliDirectionFuture = vector.Direction(origin, m_vDestinationFuture);
		
		//SPEED: Set speed according to previous turns
		float angle = Math.AbsFloat(SDRC_Math.GetAngleBetweenVectors(heliDirection, m_vDestination));
		m_fDbgAngle = angle * Math.RAD2DEG;
		//Count the angle of the turn. The steeper the turn, the slower heli should be moving.
		m_fSpeedMul = Math.Clamp((angle * Math.RAD2DEG), 1, 90);
		m_fSpeedMul = m_fSpeedGain * (SPEED_GAIN - (m_fSpeedMul / 90));
		m_fSpeedStart = m_fSpeed;
		m_fSpeedTarget = m_fSpeed * m_fSpeedMul;
		m_fSpeedTarget = Math.Clamp(m_fSpeedTarget, m_fSpeedMin, m_fSpeedMax);
		m_fTimeSpeed = 0;	//Start to change speed

		//ROLL PITCH: Change pitch according to speed		
		m_fDbgAnglePitch = (m_fSpeedMul - 1) * PITCH_ANGLE * Math.DEG2RAD;
		m_fDbgAnglePitch = Math.Clamp(m_fDbgAnglePitch, -0.5, 0.5);		
		m_vRadRollPitch = SDRC_Math.RotateAroundAxis(heliForward, heliPitch, m_fDbgAnglePitch);
		m_vRadRollPitch = SDRC_Math.ComputeAngularVelocity(heliForward, m_vRadRollPitch, deltaTime);
						
		//ROLL UP (YAW): Count the angle from heli up vs world up. The heli should slowly move back to horizontal flight.
		m_vRadRollBack = SDRC_Math.ComputeAngularVelocity(heliUp, vector.Up, deltaTime * 0.5);
	
		//ROLL ON DIRECTION: See how steep we're turning. Roll the helicopter accordingly for more natural flight.
		vector heliVelocity = owner.GetPhysics().GetVelocity();
		float angVelTurn = SDRC_Math.GetAngleBetweenVectors(heliVelocity, heliDirectionFuture);
		
		angVelTurn = angVelTurn;// + roll;
		angVelTurn = Math.Clamp(angVelTurn, -0.5, 0.5);
		m_vRadRollVel = "0 0 0";
		m_vRadRollVel[2] = -angVelTurn;
		
		//Dummy
//		m_vRadRollVel = "0 0 0";
//		m_vRadRollBack = "0 0 0";
//		m_vRadRollPitch = "0 0 0";
		
		//Count the angular velocity
		m_vAngularVel = SDRC_Math.ComputeAngularVelocity(heliForward, heliDirection, deltaTime);
		
		owner.GetPhysics().SetAngularVelocity(m_vAngularVel + m_vRadRollVel + m_vRadRollBack + m_vRadRollPitch);
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	
	*/
	void InitFlightPath()	
	{
		if (!GetGame().GetWorld())
		{
			return;
		}
		
		//Structures
/*		array<vector> pathPoints = {
			"0000 010 000",
			"0060 040 100",
			"0030 050 200",
			"0100 030 240",
			"0200 030 160",
			"0220 030 140",
			"0120 020 080",
		};*/

		//Count flight path length - straight lines
		for (int i = 0; i < m_vPathPoints.Count() - 2; i++)
		{
			m_fLen = m_fLen + vector.Distance(m_vPathPoints[i], m_vPathPoints[i + 1]);
		}
		
		m_iSegments = m_vPathPoints.Count() - 1;
		m_iSegmentPoints = (m_fLen/m_iSegments) / SPLINE_POINT_DISTANCE;
		
		SDRC_Log.Add("[SDRC_ChopperComp:InitFlightPath] Flight path length: " + m_fLen + " , segments: " + m_iSegments + " fpSegmentPoints: " + m_iSegmentPoints, LogLevel.DEBUG);
		
		foreach (int i, vector pt : m_vPathPoints)
		{
			m_vPathPoints[i] = RaiseFlyPoint(pt);
		}
					
		SDRC_Spline3D.GenerateSplinePoints(m_vPathPoints, m_vSplinePoints, m_vTangentPoints, m_iSegmentPoints, true);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	
	*/
	void CreateFlightPath(vector origin)	
	{
		if (!GetGame().GetWorld())
		{
			return;
		}

		if (m_iClosestIndex + m_iDestinationPointAdd + POINTS_TO_NEW_DISTANCE < m_vSplinePoints.Count() - 1)
		{
			return;
		}
				
		m_vPathPoints.Clear();
		int splineStartIdx = m_iClosestIndex  - POINTS_TO_SPLINE_START;
		if (splineStartIdx < 0)
		{
			splineStartIdx = 0;
		}
		m_vPathPoints.Insert(m_vSplinePoints[splineStartIdx]);
		m_vPathPoints.Insert(m_vSplinePoints[m_iClosestIndex]);
		m_vPathPoints.Insert(RaiseFlyPoint(m_vFlyPath[m_iFlyPathIdx]));
		m_iFlyPathIdx++;

		SDRC_Spline3D.GenerateSplinePoints(m_vPathPoints, m_vSplinePoints, m_vTangentPoints, m_iSegmentPoints, true);

		float distance = SDRC_Spline3D.GetDistanceFromSpline(m_vSplinePoints, origin, m_iClosestIndex);	//NOTE: This will set m_iNewClosestIndex
		m_iNewClosestIndex = m_iClosestIndex + 1;
	}
			
	//------------------------------------------------------------------------------------------------	
	vector RaiseFlyPoint(vector pt)
	{
		float y = GetGame().GetWorld().GetSurfaceY(pt[0], pt[2]);
		if (SDRC_Misc.IsPosInWater(pt))	//Is it under water?
		{
			y = GetGame().GetWorld().GetOceanHeight(pt[0], pt[2]);;
		}
		y = pt[1] + y + SDRC_Misc.RandomFloat(m_fGroundLow, m_fGroundHigh); 
		vector newPt = pt;
		newPt[1] = y;
		
		return newPt;		
	}

	
	//------------------------------------------------------------------------------------------------
	bool IsStillWorking(IEntity owner)
	{
		if (m_bInInit)
		{
			return true;
		}

		if ( (SDRC_VehicleHelper.IsWorking(owner)) && (SDRC_VehicleHelper.PilotCountAlive(owner) > 0) )
		{
			return true;
		}
		
		m_bDestroyed = true;
		
		//Set damage so it should be destroyed on crash
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(owner.FindComponent(DamageManagerComponent));
		float damage = SDRC_Misc.RandomFloat(0, 0.15);
		if (damageManager)
		{
			damageManager.SetHealthScaled(damage);		
		}
		
		//Make the chopper while unsteadily
		VehicleHelicopterSimulation owner_s = VehicleHelicopterSimulation.Cast(owner.FindComponent(VehicleHelicopterSimulation));
		float force = SDRC_Misc.RandomFloat(0.5, 1.0);
        owner_s.RotorSetForceScaleState(0, force);
		force = SDRC_Misc.RandomFloat(0.7, 2.5);
        owner_s.RotorSetForceScaleState(1, force);
		
		return false;
	}

	//------------------------------------------------------------------------------------------------	
	// Debugging things
	//------------------------------------------------------------------------------------------------	
		
	//------------------------------------------------------------------------------------------------	
	void DrawHelicopterVectors(IEntity owner)
	{
		if (!DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_LINES))
		{		
			return;
		}
			
		vector origin = owner.GetOrigin();
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
		float health = damageManager.GetHealth();

		string debugText = 	//"Speedangle:" + angle * Math.RAD2DEG + "\n" +
						   	"Speed:" + Math.Round(10*m_fSpeed)/10 + " - " +
						   	"Start/Target:" + Math.Round(10*m_fSpeedStart)/10 + "/" + Math.Round(10*m_fSpeedTarget)/10 + "\n" +
						   	"Avg time:" + m_fTimeBetweenPtsAvg + "\n" +
						   	"SpeedMul:" + m_fSpeedMul + "\n";
		debugText = debugText + 
						   	"TurnInternal:" + m_fTimeTurnInterval + "\n" +
							"DbgAngle: " + m_fDbgAngle + "\n" +
							"DbgAnglePitch: " + m_fDbgAnglePitch * Math.RAD2DEG + "\n" +
							"DestinationPointAdd: " + m_iDestinationPointAdd + "\n";
		debugText = debugText + 
							"In Init:" + m_bInInit + ", " +
							"Is working:" + SDRC_VehicleHelper.IsWorking(owner) + "\n" +
							"Pilot count:" + SDRC_VehicleHelper.PilotCountAlive(owner) + "\n" +
							//"Is piloted:" + SDRC_VehicleHelper.IsPiloted(owner) + "\n" +
							"Health: " + health;

		DebugTextWorldSpace.Create(GetGame().GetWorld(), debugText, DebugTextFlags.ONCE, origin[0], origin[1], origin[2], 20);
				
		//Planned destination
		DrawLine(origin, m_vSplinePoints[m_iClosestIndex], Color.GRAY);		
		
		//Chopper destination direction vector
		vector vFwd = vector.Direction(origin, m_vDestination);
//		vFwd.Normalize();
//		DrawLine(origin, origin + (vFwd * 20), Color.WHITE);
		DrawLine(origin, origin + vFwd, Color.WHITE);

		//Chopper future destination direction vector
		vFwd = vector.Direction(origin, m_vDestinationFuture);
//		vFwd.Normalize();
//		DrawLine(origin, origin + (vFwd * 50), Color.BLACK);		
		DrawLine(origin, origin + vFwd, Color.BLACK);		

		//Draw vectors
		vector vDir2 = owner.GetTransformAxis(2);	//Forward
//		DrawLine(origin, origin + (vDir2 * 30), Color.CYAN);		

		vector vDir0 = owner.GetTransformAxis(0);	//Side
//		DrawLine(origin, origin + (vDir0 * 10), Color.DARK_CYAN);

		vector vDir1 = owner.GetTransformAxis(1);	//Up
		DrawLine(origin, origin + (vDir1 * 15), Color.MAGENTA);		
		
		vector vUp = vector.Up;						//World Up
		DrawLine(origin, origin + (vUp * 10), Color.MAGENTA);		
		
		//Roll vector
		vector vRoll = m_vRollTarget * Math.DEG2RAD;
		vRoll.Normalize();
//		DrawLine(origin, origin + (vRoll * 15), Color.BLUE);		

		//RollPitch vector
		vRoll = m_vRadRollPitch * Math.DEG2RAD;
		vRoll.Normalize();
//		DrawLine(origin, origin + (vRoll * 15), Color.BLUE);		
		
/*		vector vVec = m_vRadRollVel;
		vVec[1] = -vVec[2];
		vVec[2] = vVec[0];
		vVec[0] = 0;
		vVec.Normalize();
		DrawLine(origin, origin + (vVec * 45), Color.WHITE);

		vVec = m_vRadRollBack;
		vVec.Normalize();
		DrawLine(origin, origin + (vVec * 35), Color.WHITE);*/
								
		//Draw velocity vector
		vector vVel = owner.GetPhysics().GetVelocity();
		vVel.Normalize();
//		float currentSpeed = vVel.Length();
		DrawLine(origin, origin + (vVel * m_fSpeed), Color.GRAY_75);			
		
		//Enemy stuff		
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(owner.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		array<IEntity> occupants = {};
		scr_compartmentManager.GetOccupants(occupants);

		foreach(IEntity occupant : occupants)
		{
			SCR_AICombatComponent aicc = SCR_AICombatComponent.Cast(occupant.FindComponent(SCR_AICombatComponent));
			if (aicc)
			{
				BaseTarget bt = aicc.GetCurrentTarget();
				if (bt)
				{
					IEntity target = bt.GetTargetEntity();
					if (EntityUtils.IsPlayer(target))
					{
						DrawLine(occupant.GetOrigin(), target.GetOrigin(), Color.RED);
					}
				}
			}
		}
				
	}
	
	//------------------------------------------------------------------------------------------------	
	void DrawLine(vector p0, vector p1, int color = Color.RED)
	{
		int shapeFlags = ShapeFlags.ONCE;
		vector p[2];
		p[0] = p0;
		p[1] = p1;		
		Shape.CreateLines(color, shapeFlags, p, 2);		
	}	
}