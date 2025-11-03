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
	private float m_fTimeTurn = 0;

	//Speed management
	const float SPEED_INTERVAL = 4;		
	private float m_fTimeSpeed = 0;
		
	//Flight path
	const int SPLINE_POINT_DISTANCE = 25;
	const int TIME_TURN_INTERVAL_BASE = 25;
	const int DESTINATION_POINT_DIV = 15;
	const int PITCH_ANGLE = 6;				//The angle to use when calculating for speed effect
	const float ROTOR_FORCE_UP = 18.0;	
	const float TIME_IN_INIT = 25;			//Seconds to be in init state
	
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
	private float m_fSpeedMin = 10;
	private float m_fSpeedMax = 20;
	private float m_fSpeedGain = 1.33;
	private float m_fSpeed = 30;
	private float m_fSpeedStart;
	private float m_fSpeedTarget;
	private float m_fSpeedMul = 1;
	private bool m_bDoTurn = true;
	private vector m_vRollTarget;
	private vector m_vRadRollVel;
	private vector m_vRadRollBack;
	private vector m_vRadRollPitch;

	//Debug stiff
	private float m_fDbgAngle;
		
	int closestIndex;
	int newClosestIndex;
	vector m_vDestination;
	vector m_vDestinationFuture;
	
	override void OnPostInit(IEntity owner)
	{
		SDRC_Log.Add("[SDRC_ChopperComp] Starting SDRC_ChopperComp", LogLevel.NORMAL);
		s_Instance = this;				
		SetEventMask(owner, EntityEvent.FRAME | EntityEvent.POSTFRAME);
		Activate(owner);
		
		InitFlightPath();
		
		closestIndex = 3;
		newClosestIndex = closestIndex + 1;
		m_vDestination = m_vSplinePoints[closestIndex];

		m_fSpeedTarget = m_fSpeed;
		
		//Set chopper initial position		
		owner.SetOrigin(m_vSplinePoints[0]);
		vector angles = vector.Direction(owner.GetOrigin(), m_vDestination);
		angles.Normalize();
		angles = angles.VectorToAngles();
		owner.SetYawPitchRoll(angles);

		SetVelocity(owner);
		SetTurn(owner, m_fTimeTurnInterval);
		
//		GetGame().GetCallqueue().CallLater(SetDamage, (TIME_IN_INIT + 5) * 1000, false, owner);		
		GetGame().GetCallqueue().CallLater(InitDone, TIME_IN_INIT * 1000);
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
		m_fTimeTurn += timeSlice;

		vector origin = owner.GetOrigin();

		//Adjust time depending on the time.
//		m_fTimeTurnInterval = TIME_TURN_INTERVAL_BASE / m_fSpeed;
		m_fTimeTurnInterval = m_fSpeed / 60;
		m_fTimeTurnInterval = Math.Clamp(m_fTimeTurnInterval, 0.3, 1.5);
				
		//Count destintation addition along the spline which is dependent on the speed.
		m_iDestinationPointAdd = m_fSpeed / DESTINATION_POINT_DIV;
		if (m_iDestinationPointAdd < 1)
		{
			m_iDestinationPointAdd = 1;
		}
		
		//Draw where we are planning to go
		float distance = SDRC_Spline3D.GetDistanceFromSpline(m_vSplinePoints, origin, newClosestIndex);		

		if (newClosestIndex > closestIndex)
		{
			closestIndex = newClosestIndex;
			m_bDoTurn = true;
		}
		
		if (newClosestIndex < closestIndex)
		{
//			closestIndex++;
			m_bDoTurn = true;
		}

		if (newClosestIndex == closestIndex)
		{
			closestIndex++;
			m_bDoTurn = true;
		}
				
		m_vDestination = m_vSplinePoints[closestIndex + m_iDestinationPointAdd];
		m_vDestinationFuture = m_vSplinePoints[closestIndex + (m_iDestinationPointAdd * 2)];
		
		if (m_fTimeSpeed < SPEED_INTERVAL)
		{
			float t = m_fTimeSpeed / SPEED_INTERVAL;
			m_fSpeed = Math.Lerp(m_fSpeedStart, m_fSpeedTarget, t);
			m_fSpeed = Math.Clamp(m_fSpeed, m_fSpeedMin, m_fSpeedMax)
		}

		SetVelocity(owner);

		if ( (m_fTimeTurn > m_fTimeTurnInterval) && (m_bDoTurn) )
		{
			SetTurn(owner, m_fTimeTurnInterval);
			m_fTimeTurn = m_fTimeTurn - m_fTimeTurnInterval * 1.1;
			m_bDoTurn = false;
		}
				
/* From: SCR_HelicopterCinematicFlyComponent
		vector velOrig = GetOwner().GetPhysics().GetVelocity();
		vector rotVector = GetOwner().GetAngles();
		vector vel = {velOrig[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * forceMultiplier, velOrig[1], velOrig[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * forceMultiplier };
		GetOwner().GetPhysics().SetVelocity(vel);				
*/		
	}

	//------------------------------------------------------------------------------------------------	
	void SetVelocity(IEntity owner)
	{
		vector origin = owner.GetOrigin();
		
		//Define speed
//		float speedMul = vector.Distance(origin, m_vDestination);
		vector heliForward = owner.GetTransformAxis(2);
		vector heliDirection = vector.Direction(origin, m_vDestination);				
		vector heliVelocity = owner.GetPhysics().GetVelocity();
		
/*		float t = m_fTimeSpeed / SPEED_INTERVAL;
		vector lerped = vector.Lerp(origin, m_vDestination, t);
		vector velVector = vector.Direction(origin, lerped);*/
		
		//Set velocity
		vector velVector = vector.Direction(origin, m_vDestination);
		vector rotVector = owner.GetAngles();
		velVector.Normalize();
		float forceMultiplier = m_fSpeed;
		float gas = ROTOR_FORCE_UP;
		velVector = {velVector[0] + Math.Sin(rotVector[1] * Math.DEG2RAD) * forceMultiplier, velVector[1] * gas, velVector[2] + Math.Cos(rotVector[1] * Math.DEG2RAD) * forceMultiplier};
		
//		velVector = velVector * m_fSpeed;
//		velVector = velVector * m_fSpeedMax;
		owner.GetPhysics().SetVelocity(velVector);		
	}
	
	//------------------------------------------------------------------------------------------------	
	void SetTurn(IEntity owner, float deltaTime)
	{
		if (deltaTime == 0)
		{
			return;
		}

		//Check if we're still working .Not needed every frame. //TBD: Could be done every x seconds - not that critical
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
		float angle = Math.AbsFloat(SDRC_Math.GetAngleBetweenVectors(heliDirection, heliDirectionFuture));
		m_fDbgAngle = angle * Math.RAD2DEG;
		m_fSpeedMul = Math.Clamp((angle * Math.RAD2DEG), 1, 90);
		m_fSpeedMul = m_fSpeedGain - (m_fSpeedMul / 60);
		m_fSpeedTarget = m_fSpeed * m_fSpeedMul;
		m_fSpeedTarget = Math.Clamp(m_fSpeedTarget, m_fSpeedMin, m_fSpeedMax);
		m_fSpeedStart = m_fSpeed;
		m_fTimeSpeed = 0;	//Start to change speed

		//ROLL PITCH: Change pitch according to speed		
		m_vRadRollPitch = SDRC_Math.RotateAroundAxis(heliForward, heliPitch, m_fSpeedMul * PITCH_ANGLE * Math.DEG2RAD);
		m_vRadRollPitch = SDRC_Math.ComputeAngularVelocity(heliForward, m_vRadRollPitch, deltaTime * 1.5);
				
		//ROLL UP (YAW): Count the angle from heli up vs world up. The heli should slowly move back to horizontal flight.
		m_vRadRollBack = SDRC_Math.ComputeAngularVelocity(heliUp, vector.Up, deltaTime * 1.2);

		//ROLL ALONG SPLINE: Calculate roll along the spline
		int rollIdxStart = closestIndex - 1;//(m_iDestinationPointAdd / 2);
		if (rollIdxStart < 0)
		{
			rollIdxStart = 0;
		}
		int rollIdxEnd = closestIndex + (m_iDestinationPointAdd);
		if (rollIdxEnd <= closestIndex)
		{
			rollIdxEnd = closestIndex + 1;
		}
		float roll = SDRC_Spline3D.ComputeSplineRoll(m_vSplinePoints[rollIdxStart], m_vSplinePoints[closestIndex], m_vSplinePoints[rollIdxEnd], m_vRollTarget);
		
		//ROLL ON DIRECTION: See how steep we're turning. Roll the helicopter accordingly for more natural flight.
		vector heliVelocity = owner.GetPhysics().GetVelocity();
//		float angVelTurn = GetAngleBetweenVectors(heliForward, heliDirection);
//		float angVelTurn = GetAngleBetweenVectors(heliForward, heliDirectionFuture);
//		float angVelTurn = GetAngleBetweenVectors(heliVelocity, heliDirection);
		float angVelTurn = SDRC_Math.GetAngleBetweenVectors(heliVelocity, heliDirectionFuture);
//		float angVelTurn = GetAngleBetweenVectors(heliForward, heliVelocity);
		
		angVelTurn = angVelTurn + roll;
		angVelTurn = Math.Clamp(angVelTurn, -0.7, 0.7);
		m_vRadRollVel = "0 0 0";
		m_vRadRollVel[2] = -angVelTurn;
		
		//Count the angular velocity
		vector angularVel = SDRC_Math.ComputeAngularVelocity(heliVelocity, heliDirection, deltaTime);
//		vector angularVel = ComputeAngularVelocity(heliForward, heliDirection, deltaTime);
		
		owner.GetPhysics().SetAngularVelocity(angularVel + m_vRadRollVel + m_vRadRollBack + m_vRadRollPitch);
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

		//Arland
		array<vector> pathPoints = {
			"1500 020 2000",
			"1400 010 2200",
			"1600 015 2300",
			"1900 030 2900",
			"2300 040 2500",
			"2400 020 2250",	//Timber Ridge
			"3100 030 2800",	//Beauregard
			"2400 030 1600",
			"1900 000 1300",
//			"1500 000 2200",
//			"2200 020 2200",
		};
		
		//Count flight path length - straight lines
		for (int i = 0; i < pathPoints.Count() - 2; i++)
		{
			m_fLen = m_fLen + vector.Distance(pathPoints[i], pathPoints[i + 1]);
		}
		
		m_iSegments = pathPoints.Count() - 1;
		m_iSegmentPoints = (m_fLen/m_iSegments) / SPLINE_POINT_DISTANCE;
		
		SDRC_Log.Add("[SDRC_ChopperComp:InitFlightPath] Flight path length: " + m_fLen + " , segments: " + m_iSegments + " fpSegmentPoints: " + m_iSegmentPoints, LogLevel.DEBUG);
		
		m_fGroundLow = 5;
		m_fGroundHigh = 40;
		
		foreach (int i, vector pt : pathPoints)
		{
			float y = GetGame().GetWorld().GetSurfaceY(pt[0], pt[2]);
			if (SDRC_Misc.IsPosInWater(pt))	//Is it under water?
			{
				y = GetGame().GetWorld().GetOceanHeight(pt[0], pt[2]);;
			}
			y = pt[1] + y + SDRC_Misc.RandomFloat(m_fGroundLow, m_fGroundHigh); 
			vector newPt = pt;
			newPt[1] = y;
			pathPoints[i] = newPt;
		}
					
		SDRC_Spline3D.GenerateSplinePoints(pathPoints, m_vSplinePoints, m_vTangentPoints, m_iSegmentPoints, true);
		m_vDestination = m_vSplinePoints[1];
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
						   	"Speed:" + Math.Round(10*m_fSpeed)/10 + "\n" +
						   	"SpeedStart/Target:" + Math.Round(10*m_fSpeedStart)/10 + "/" + Math.Round(10*m_fSpeedTarget)/10 + "\n" +
						   	"SpeedMul:" + m_fSpeedMul + "\n";
		debugText = debugText + 
						   	"TurnInternal:" + m_fTimeTurnInterval + "\n" +
//							"m_fTimeSpeed: " + m_fTimeSpeed + "\n" +
							"DbgAngle: " + m_fDbgAngle + "\n" +
							"DestinationPointAdd: " + m_iDestinationPointAdd + "\n";
		debugText = debugText + 
							"In Init:" + m_bInInit + "\n" +
							"Is working:" + SDRC_VehicleHelper.IsWorking(owner) + "\n" +
							"Pilot count:" + SDRC_VehicleHelper.PilotCountAlive(owner) + "\n" +
							"Is piloted:" + SDRC_VehicleHelper.IsPiloted(owner) + "\n" +
							"Health: " + health;
							
		DebugTextWorldSpace.Create(GetGame().GetWorld(), debugText, DebugTextFlags.ONCE, origin[0], origin[1], origin[2], 20);
				
		//Planned destination
		DrawLine(origin, m_vSplinePoints[closestIndex], Color.GRAY);		
		
		//Chopper destination direction vector
		vector vFwd = vector.Direction(origin, m_vDestination);
		vFwd.Normalize();
		DrawLine(origin, origin + (vFwd * 10), Color.BLACK);

		//Chopper future destination direction vector
		vFwd = vector.Direction(origin, m_vDestinationFuture);
		vFwd.Normalize();
		DrawLine(origin, origin + (vFwd * 40), Color.BLACK);		

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