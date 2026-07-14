class SDRC_SAL_DroneControllerComponentClass: ScriptComponentClass 
{
}

modded class SAL_DroneControllerComponent: SDRC_SAL_DroneControllerComponent
{}
modded class SAL_DroneControllerComponentClass: ScriptComponentClass 
{
}

class SDRC_SAL_DroneControllerComponent: ScriptComponent
{
	SAL_DroneConnectionManager m_DroneManager;
	InputManager m_InputManager;
	SAL_DroneSignalComponent m_SignalComponent;
	SAL_DroneBatteryComponent m_BatteryComponent;
	SAL_DroneSoundComponent m_SoundComponent;
	CameraManager m_CameraManager;
	SAL_CameraZoomComponent m_CameraZoom;
	SCR_GarbageSystem m_GarbageSystem;
	RplComponent m_RplComp;
	
	bool m_bIsConnected = false;
	int m_iOwner = -1;
	
	[Attribute("35000")] int m_iMaxThrustRPM;
	[Attribute("1")] bool m_bNightSightsEnabled;
	[Attribute("0")] bool m_bStabilized;
	[Attribute("1")] float m_fPitchSensitivity;
	[Attribute("1")] float m_fYawSensitivity;
	[Attribute("1")] float m_fRollSensitivity;
	[Attribute("1.9")] float m_fHoverMargin;
	[Attribute("")] ref SAL_DroneStablizerProfile m_DroneStablizerProfile;
	
	RplId m_DroneId;

	float m_aRotorRPM[4] = { 0.0, 0.0, 0.0, 0.0 };
	IEntity m_aRotors[4];
	RplId m_aRotorsRplId[4];
	IEntity m_Drone;

	float m_iThrottle = 0.0;
	float m_iPitch = 0.0;
	float m_iRoll = 0.0;
	float m_iYaw = 0.0;
	bool m_bIsArmed = false;
	bool m_bIsActive = false;
	bool m_bNightSightActive = false;
	
	float m_fWeight = 0;

	float m_fHoverForce = 0.0;
	float m_fMaxAdditionalThrust = 0.0;
	float m_fSyncTimer = 0.0;
	float m_fCurrentRollDeg;
	float m_fCurrentPitchDeg
	
	bool m_bIsTriggered = false;
	bool m_bIsDestroyed = false;
	
	int rotorSpinDir[4] = { 1, -1, -1, 1 };
	
	vector m_vLocalAngVel;
	vector m_vInputTorque;
	vector m_vThrustForce;
	
	vector m_vCurrentVelocity = "0 0 0";

	void SpinRotors(float timeSlice)
	{
//		if (m_aRotors[].GetSizeOf() < 4)
//			InitializeRotors();

		for (int i = 0; i < 4; i++)
		{
			IEntity rotor = m_aRotors[i];
			if (!rotor) continue;

			float degPerSecond = m_aRotorRPM[i] * 6.0;
			float rotationAmount = rotorSpinDir[i] * degPerSecond * timeSlice;
			vector oldAngles = rotor.GetLocalAngles();
			oldAngles[1] = oldAngles[1] + rotationAmount;
			rotor.SetAngles(oldAngles);
		}
	}
	
	float m_fRPMSmoothFactor = 5.0; // Smoothing speed, tweak this
	
	void UpdateSimulatedRPMs(float timeSlice)
	{
		float baseRPM = m_iThrottle * m_iMaxThrustRPM;
	
		float targetRPMs[4];
		targetRPMs[0] = baseRPM + (-m_iPitch + m_iRoll + m_iYaw) * 1000;
		targetRPMs[1] = baseRPM + (-m_iPitch - m_iRoll - m_iYaw) * 1000;
		targetRPMs[2] = baseRPM + ( m_iPitch + m_iRoll - m_iYaw) * 1000;
		targetRPMs[3] = baseRPM + ( m_iPitch - m_iRoll + m_iYaw) * 1000;
	
		for (int i = 0; i < 4; i++)
		{
			targetRPMs[i] = Math.Clamp(targetRPMs[i], 0.0, m_iMaxThrustRPM);
			m_aRotorRPM[i] = Math.Lerp(m_aRotorRPM[i], targetRPMs[i], timeSlice * m_fRPMSmoothFactor);
		}
	}
	
	void DampenCrossInput(float pitchIn, float rollIn, out float pitchOut, out float rollOut, float cutoffRatio = 0.5)
	{
		float pitchAbs = Math.AbsFloat(pitchIn);
		float rollAbs = Math.AbsFloat(rollIn);
	
		if (pitchAbs > rollAbs * (1.0 + cutoffRatio))
		{
			// Pitch is dominant — suppress roll
			rollOut = 0.0;
			pitchOut = pitchIn;
		}
		else if (rollAbs > pitchAbs * (1.0 + cutoffRatio))
		{
			// Roll is dominant — suppress pitch
			pitchOut = 0.0;
			rollOut = rollIn;
		}
		else
		{
			// Allow both, but scale proportionally to reduce unintended influence
			float blendFactor = 0.5;
			pitchOut = pitchIn * blendFactor;
			rollOut = rollIn * blendFactor;
		}
	}

	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.SIMULATE | EntityEvent.FRAME | EntityEvent.FIXEDFRAME);
	}

	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		if (!owner.GetPhysics())
			return;
		m_Drone = owner;
		Physics rigidBody = GetOwner().GetPhysics();
		m_DroneManager = SAL_DroneConnectionManager.GetInstance();
		m_InputManager = GetGame().GetInputManager();
		m_SignalComponent = SAL_DroneSignalComponent.Cast(owner.FindComponent(SAL_DroneSignalComponent));
		m_BatteryComponent = SAL_DroneBatteryComponent.Cast(owner.FindComponent(SAL_DroneBatteryComponent));
		m_SoundComponent = SAL_DroneSoundComponent.Cast(owner.FindComponent(SAL_DroneSoundComponent));
		m_CameraManager = GetGame().GetCameraManager();
		m_CameraZoom = SAL_CameraZoomComponent.Cast(owner.FindComponent(SAL_CameraZoomComponent));
		m_GarbageSystem = SCR_GarbageSystem.GetByEntityWorld(owner);
		m_RplComp = RplComponent.Cast(owner.FindComponent(RplComponent));
		
		rigidBody.SetActive(true);
		float mass = rigidBody.GetMass();
		m_fWeight = mass;
		m_fHoverForce = mass * 9.81;
		m_fMaxAdditionalThrust = m_fHoverForce;
		m_DroneId = RplComponent.Cast(owner.FindComponent(RplComponent)).Id();
		
		if (m_bStabilized)
		{
			float ratio = mass / 0.8;
			float scaledRatio = Math.Clamp((mass - 0.8) / (2 - 0.8), 0.0, 1);
			float dampening = Math.Lerp(0.7, 0.99, scaledRatio);
			
			rigidBody.SetDamping(dampening, dampening);
		}
		else
		{
			float ratio = mass / 0.8;
			float scaledRatio = Math.Clamp((mass - 0.8) / (4 - 0.8), 0.0, 1);
			float dampening = Math.Lerp(0.4, 0.99, scaledRatio);
			
			rigidBody.SetDamping(dampening, dampening);
		}
		GetGame().GetCallqueue().CallLater(InitializeRotors, 200, false);
	}

	void InitializeRotors()
	{
		SlotManagerComponent sm = SlotManagerComponent.Cast(GetOwner().FindComponent(SlotManagerComponent));
		if (!sm) return;

		m_aRotors[0] = sm.GetSlotByName("Rotor1").GetAttachedEntity();
		m_aRotors[1] = sm.GetSlotByName("Rotor2").GetAttachedEntity();
		m_aRotors[2] = sm.GetSlotByName("Rotor3").GetAttachedEntity();
		m_aRotors[3] = sm.GetSlotByName("Rotor4").GetAttachedEntity();
		
		m_aRotorsRplId[0] = SCR_PlayerController.GetRplId(sm.GetSlotByName("Rotor1").GetAttachedEntity());
		m_aRotorsRplId[1] = SCR_PlayerController.GetRplId(sm.GetSlotByName("Rotor2").GetAttachedEntity());
		m_aRotorsRplId[2] = SCR_PlayerController.GetRplId(sm.GetSlotByName("Rotor3").GetAttachedEntity());
		m_aRotorsRplId[3] = SCR_PlayerController.GetRplId(sm.GetSlotByName("Rotor4").GetAttachedEntity());
	}
	
	void ArmDrone()
	{
		if (m_DroneManager.GetPlayersActiveDrone(SCR_PlayerController.GetLocalPlayerId()) != GetOwner())
			return;
		
		m_bIsArmed = !m_bIsArmed;
		
		if (m_bIsArmed)
		{
			SAL_DroneNetworkPacket packet = new SAL_DroneNetworkPacket;
			packet.SetDrone(m_DroneId);
			packet.SetIsArmed(m_bIsArmed);	
			SCR_PlayerController.Cast(GetGame().GetPlayerController()).DisarmDrone(packet);
		}
		else
		{
			SAL_DroneNetworkPacket packet = new SAL_DroneNetworkPacket;
			packet.SetDrone(m_DroneId);
			packet.SetIsArmed(m_bIsArmed);	
			SCR_PlayerController.Cast(GetGame().GetPlayerController()).DisarmDrone(packet);
		}
			
	}
	
	bool HasController()
	{
		if (!SCR_PlayerController.GetLocalControlledEntity())
				return false;
		
		CharacterWeaponManagerComponent weaponMan = CharacterWeaponManagerComponent.Cast(SCR_PlayerController.GetLocalControlledEntity().FindComponent(CharacterWeaponManagerComponent));
		if (!weaponMan)
			return false;
		
		ref array<IEntity> weaponList = {};
		weaponMan.GetWeaponsList(weaponList);
		foreach (IEntity weapon: weaponList)
		{
			if (!weapon)
				continue;
			
			if (weapon.GetPrefabData().GetPrefabName() == "{E2434ED1318D8476}Prefabs/Characters/Items/DroneController.et")
				return true;
		}
		return false;
	}

	override void EOnFrame(IEntity owner, float timeSlice)
	{
		super.EOnFrame(owner, timeSlice);
		if (owner.GetParent() != null)
			return;
		
		if (!m_GarbageSystem)
			m_GarbageSystem = SCR_GarbageSystem.GetByEntityWorld(owner);
		
		if(m_GarbageSystem)
			if (m_GarbageSystem.IsInserted(owner))
				m_GarbageSystem.Withdraw(owner);
		
		if (IsOnGround(owner) && m_iOwner == -1)
		{
			m_bIsActive = false;
			return;
		}
		else
			m_bIsActive = true;
		// Needed for deconstructor
		if (m_DroneId == -1)
			m_DroneId = RplComponent.Cast(owner.FindComponent(RplComponent)).Id();
		//Why havent we found it
		if (!m_DroneManager)
			m_DroneManager = SAL_DroneConnectionManager.GetInstance();
		if (!m_RplComp)
			m_RplComp = RplComponent.Cast(owner.FindComponent(RplComponent));
		//Ohh hell nah
		if (!m_DroneManager)
			return;
		
		//Everything below is all on the client
		if (System.IsConsoleApp())
			return;
			
		
		ClientFrameChecks(owner, timeSlice);
		//Rest of the code non drone controllers don't need to worry about
		if (m_iOwner != SCR_PlayerController.GetLocalPlayerId() || !m_DroneManager.m_aActiveDrones.Contains(m_DroneId))
			return;
		
		if (!SCR_PlayerController.GetLocalControlledEntity())
				return;
			
		BaseWeaponManagerComponent weaponMan = BaseWeaponManagerComponent.Cast(ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity()).FindComponent(BaseWeaponManagerComponent));
		if (!weaponMan)
			return;
		
		if (SCR_PlayerController.GetLocalControlledEntity().GetParent() != null)
		{
			if (!HasController())
				return;
		}
		else
		{
			if (!weaponMan.GetCurrentWeapon())
				return;
		
			if (weaponMan.GetCurrentWeapon().GetOwner().GetPrefabData().GetPrefabName() != "{E2434ED1318D8476}Prefabs/Characters/Items/DroneController.et")
				return;
		}
		
		
		if (m_InputManager.GetActionValue("ArmDrone") > 0)
				ArmDrone();

		//If the drone controller arms it this is what starts listening for inputs
		if (m_bIsArmed)
		{
			float lq = 1 - m_SignalComponent.m_fLQ;
			
			if (lq <= 0.1)
			{
				ArmDrone();
				SCR_PlayerController.Cast(GetGame().GetPlayerController()).DisconnectDrone();
			}
				
			if (lq < 0.2)
				return;
			
			if (lq < 0.4)
			{
				float dropChance = Math.Clamp(1.0 - ((lq - 0.4) / (0.2 - 0.4)), 0.0, 1.0);
				if (Math.RandomFloat01() < dropChance)
					return;
			}
			
			float rawInput = Math.Clamp(m_InputManager.GetActionValue("DroneUp"), -1.0, 1.0);
			m_iThrottle = (rawInput + 1.0) * 0.5;

			if (m_InputManager.GetLastUsedInputDevice() == EInputDeviceType.GAMEPAD || m_bStabilized)
			{	
				m_iPitch = -m_InputManager.GetActionValue("DroneForward") * m_fPitchSensitivity;
				m_iRoll  = -m_InputManager.GetActionValue("DroneLeft") * m_fRollSensitivity;
				float yawEnvelope = 1.0 - Math.AbsFloat((m_iThrottle - 0.5) * 2.0);
				yawEnvelope = Math.Clamp(yawEnvelope, 0.3, 1.0);
				m_iYaw    = m_InputManager.GetActionValue("DroneYaw") * m_fYawSensitivity * yawEnvelope;
			}
			else
			{
				m_iPitch = -m_InputManager.GetActionValue("DroneForward") * m_fPitchSensitivity/4;
				m_iRoll  = -m_InputManager.GetActionValue("DroneLeft") * m_fRollSensitivity/4;
				m_iYaw    = m_InputManager.GetActionValue("DroneYaw") * m_fYawSensitivity/4;
			}
			

			if (Math.AbsFloat(m_iYaw) < 0.1)
				m_iYaw = 0;
			
			float averageRPM = (m_aRotorRPM[0] + m_aRotorRPM[1] + m_aRotorRPM[2] + m_aRotorRPM[3]) / 4;
			
			m_SoundComponent.m_fAverageRotorRPM = averageRPM;
		}
	}
	
	void ClientFrameChecks(IEntity owner, float timeSlice)
	{
		Physics rigidBody = GetOwner().GetPhysics();
		if (owner.GetParent() == null && rigidBody.GetSimulationState() == 0)
		{
			rigidBody.ChangeSimulationState(SimulationState.SIMULATION);
			rigidBody.SetActive(true);
		}
		
		// Makes sure everyone is tracking the drones new position if no one is controlling it, if you dont do this picking up the drone breaks
		if (!m_bIsConnected)
			rigidBody.EnableGravity(1);
		else if (m_bIsConnected && m_iOwner != SCR_PlayerController.GetLocalPlayerId())
			rigidBody.EnableGravity(0);
		
		if (m_SoundComponent)
		{
			if (!m_SoundComponent.IsEngineOn() && m_bIsArmed)
				m_SoundComponent.StartEngine();
			else if (m_SoundComponent.IsEngineOn() && !m_bIsArmed)
				m_SoundComponent.ShutOffEngine();
		}
		vector transform[4];
		owner.GetTransform(transform);
			
		//Enables the gravity for the person controlling the drone
		if (m_iOwner == SCR_PlayerController.GetLocalPlayerId())
			rigidBody.EnableGravity(1);
	}
	
	void ServerFrameChecks(IEntity owner, float timeSlice)
	{
		Physics rigidBody = GetOwner().GetPhysics();
		if (owner.GetParent() == null && rigidBody.GetSimulationState() == 0)
		{
			rigidBody.ChangeSimulationState(SimulationState.SIMULATION);
			rigidBody.SetActive(true);
		}
		
		// Makes sure everyone is tracking the drones new position if no one is controlling it, if you dont do this picking up the drone breaks
		if (!m_bIsConnected)
		{
			rigidBody.EnableGravity(1);
			if (!IsOnGround(owner))
				SendPacketServer(owner, timeSlice);
		}
		else
			rigidBody.EnableGravity(0);
	}
	
	void SendPacket(IEntity owner, float timeSlice)
	{
		Physics rigidBody = GetOwner().GetPhysics();
		vector transform[4];
		owner.GetTransform(transform);
				
		SAL_DroneNetworkPacket packet = new SAL_DroneNetworkPacket;
		packet.SetDrone(m_DroneId);
		packet.SetRotors(m_aRotorsRplId);
		packet.SetRotorRPMs(m_aRotorRPM);
		packet.SetTransform(transform);
		packet.SetTimeSlice(timeSlice);
		packet.SetIsTriggerd(m_bIsTriggered);
		packet.SetBatteryLevel(m_BatteryComponent.m_fCurrentBattery);
		packet.SetVelocity(rigidBody.GetVelocity());
		if (m_bIsTriggered)
		{
			//Set the position of the drone to where it hit on the clients screen
			SAL_DroneExplosionComponent droneExplComp = SAL_DroneExplosionComponent.Cast(owner.FindComponent(SAL_DroneExplosionComponent));
			transform[3] = droneExplComp.m_HitEntity.CoordToParent(droneExplComp.m_vHitVector);
			
			packet.SetExplosion(droneExplComp.m_sExplosionEffect);
			packet.SetTransform(transform);
			SCR_PlayerController.Cast(GetGame().GetPlayerController()).ExplodeDrone(packet);
		}
		else
			SCR_PlayerController.Cast(GetGame().GetPlayerController()).SendTransformToServer(packet);
	}
	
	void SendPacketServer(IEntity owner, float timeSlice)
	{
		Physics rigidBody = GetOwner().GetPhysics();
		vector transform[4];
		owner.GetTransform(transform);
		
		if (m_vCurrentVelocity != "0 0 0")
		{
			rigidBody.SetVelocity(m_vCurrentVelocity);
			m_vCurrentVelocity = "0 0 0";
		}
		
		SAL_DroneNetworkPacket packet = new SAL_DroneNetworkPacket;
		packet.SetDrone(m_DroneId);
		packet.SetRotors(m_aRotorsRplId);
		packet.SetRotorRPMs(m_aRotorRPM);
		packet.SetTransform(transform);
		packet.SetTimeSlice(timeSlice);
		packet.SetIsTriggerd(m_bIsTriggered);
		packet.SetBatteryLevel(m_BatteryComponent.m_fCurrentBattery);
		if (m_bIsTriggered)
		{
			//Set the position of the drone to where it hit on the clients screen
			SAL_DroneExplosionComponent droneExplComp = SAL_DroneExplosionComponent.Cast(owner.FindComponent(SAL_DroneExplosionComponent));
			transform[3] = droneExplComp.m_HitEntity.CoordToParent(droneExplComp.m_vHitVector);
			
			packet.SetExplosion(droneExplComp.m_sExplosionEffect);
			packet.SetTransform(transform);
			packet.SetExplosion(SAL_DroneExplosionComponent.Cast(owner.FindComponent(SAL_DroneExplosionComponent)).m_sExplosionEffect);
			m_DroneManager.ExplodeDroneServer(packet);
		}
		else
			m_DroneManager.ReplicateTransform(packet);
	}
	
	bool IsOnGround(IEntity owner)
	{
		vector origin = owner.GetOrigin();
		vector end = origin + Vector(0, -1, 0) * 1000.0; // cast 1000 meters down
			
		TraceParam trace = new TraceParam();
		trace.Start = origin;
		trace.End = end;
		trace.Exclude = owner; // prevent hitting self
		trace.Flags = TraceFlags.WORLD;
			
		float height = GetGame().GetWorld().TraceMove(trace, null) * 10000;
		return height <= 2;
	}
	
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		//Everything below is all on the client
		if (System.IsConsoleApp())
		{
			ServerFrameChecks(owner, timeSlice);
			return;
		}
		Physics rigidBody = GetOwner().GetPhysics();
		if (!rigidBody) return;
		
		if (!m_DroneManager) 
			m_DroneManager = SAL_DroneConnectionManager.GetInstance();
		
		//Same as above just checks to see if the person running this is the drones controller
		if (!m_DroneManager || m_iOwner != SCR_PlayerController.GetLocalPlayerId())
			return;
		
		// Needed for when the drone is not armed and in the air so everyone can still track where its at
		if (!m_bIsArmed)
		{
			if (!IsOnGround(owner))
				SendPacket(owner, timeSlice);
			return;
		}
		
		//Sends data to other players every 30ms
		SendPacket(owner, timeSlice);
	}

	override void EOnSimulate(IEntity owner, float timeSlice)
	{
		super.EOnSimulate(owner, timeSlice);
		Physics rigidBody = GetOwner().GetPhysics();
		if (!rigidBody) return;
		
		if (!m_DroneManager) 
			m_DroneManager = SAL_DroneConnectionManager.GetInstance();
		
		//Same as above just checks to see if the person running this is the drones controller
		if (!m_DroneManager || m_iOwner != SCR_PlayerController.GetLocalPlayerId())
			return;
		
		// Needed for when the drone is not armed and in the air so everyone can still track where its at
		if (!m_bIsArmed)
			return;

		//If the drone is stabalized duh
		if (m_bStabilized)
			CalculateStablizedInputs(owner, timeSlice, rigidBody);
		else
			CalculateAcroInputs(owner, timeSlice, rigidBody);
		
		//Helps stabalize the drones tilt
		vector dampingTorque = -m_vLocalAngVel * 30;
		vector controlTorque = ((m_vInputTorque * 20) + dampingTorque);

		//Summarize that torque
		vector worldTorque =
			(owner.GetTransformAxis(0) * controlTorque[0]) +
			(owner.GetTransformAxis(1) * controlTorque[1]) +
			(owner.GetTransformAxis(2) * controlTorque[2]);

		//Get the rotors spinning and the drone in the sky
		UpdateSimulatedRPMs(timeSlice);
		SpinRotors(timeSlice);
		rigidBody.ApplyImpulse(m_vThrustForce);
		rigidBody.ApplyTorque(worldTorque * timeSlice);
	}
	
	void CalculateAcroInputs(IEntity owner, float timeSlice, Physics physics)
	{
		Physics rigidBody = GetOwner().GetPhysics();
		vector thrustForce;
		vector inputTorque;
		vector localAngVel;
		
		//Get upward direction of drone body
		vector thrustDir = owner.GetTransformAxis(1);
			
		//How far throttle is from neutral (0.5)
		float thrustOffset = (m_iThrottle - 0.5) * 2.0;
			
		//Calculate thrust with margin
		float totalThrust = (m_fHoverForce * m_fHoverMargin) + (thrustOffset * m_fMaxAdditionalThrust * 4);
		
		//Cap total thrust to a safety ceiling
		float maxThrust = m_fHoverForce + m_fMaxAdditionalThrust * 1.5;
		totalThrust = Math.Clamp(totalThrust, 0, maxThrust);
			
		//Apply thrust in the drone's up direction
		thrustForce = thrustDir * totalThrust * timeSlice;
				
		vector worldAngVel = rigidBody.GetAngularVelocity();
		localAngVel[0] = vector.Dot(worldAngVel, owner.GetTransformAxis(0));
		localAngVel[1] = vector.Dot(worldAngVel, owner.GetTransformAxis(1));
		localAngVel[2] = vector.Dot(worldAngVel, owner.GetTransformAxis(2));
			
		inputTorque[0] = -m_iPitch;
		inputTorque[1] = m_iYaw;
		inputTorque[2] = m_iRoll;
		
		m_vInputTorque = inputTorque;
		m_vLocalAngVel = localAngVel;
		m_vThrustForce = thrustForce;
	}
	
	
	void CalculateStablizedInputs(IEntity owner, float timeSlice, Physics physics)
	{
		Physics rigidBody = GetOwner().GetPhysics();
		vector thrustForce;
		vector inputTorque;
		vector localAngVel;
		
		//Get them angles
		vector up = owner.GetTransformAxis(1);
		vector right = owner.GetTransformAxis(0);
		vector forward = owner.GetTransformAxis(2);
		vector worldUp = vector.Up;
			
		DampenCrossInput(m_iPitch, m_iRoll, m_iPitch, m_iRoll, 0.4); // 0.2 = tighter stick discipline
		
		// --- Calculate vertical lift force ---
		float verticalLiftEfficiency = Math.Max(vector.Dot(up, worldUp), 0.1);
		float hoverThrust = (m_fHoverForce * 1.02) / verticalLiftEfficiency;
		float thrustOffset = (m_iThrottle - 0.5) * 2.0;
		float userThrust   = thrustOffset * m_fMaxAdditionalThrust;
			
		float baseThrust = hoverThrust + userThrust;
			
		// --- Boost only the sideways part based on tilt ---
		float maxTiltAngleDeg = m_DroneStablizerProfile.m_fMaxTiltDegrees;
		float targetPitchDeg  = -m_iPitch * maxTiltAngleDeg;
		float targetRollDeg   = -m_iRoll  * maxTiltAngleDeg;
			
		float tiltMag = Math.Sqrt(targetPitchDeg * targetPitchDeg + targetRollDeg * targetRollDeg) / maxTiltAngleDeg;
		float tiltBoost = tiltMag * m_DroneStablizerProfile.m_fTiltThrustGain;  // 0–1
			
		// Decompose thrust: keep vertical lift fixed, boost only lateral
		float verticalComponent = vector.Dot(up, worldUp);
		vector verticalThrust = worldUp * baseThrust * verticalComponent;
		vector lateralThrust  = (up - worldUp * verticalComponent).Normalized() * baseThrust * tiltBoost * 2;
			
		thrustForce = (verticalThrust + lateralThrust);
		
		//Only apply vertical velocity damping when throttle is near hover (user not actively climbing/descending)
//		if (Math.AbsFloat(m_iThrottle) < 0.05)
//		{
//			float verticalVelocity = vector.Dot(rigidBody.GetVelocity(), worldUp);
//			thrustForce -= worldUp * verticalVelocity * 2.0 * timeSlice;
//		}
		
		//Angular velocity
		vector worldAngVel = rigidBody.GetAngularVelocity();
		localAngVel[0] = vector.Dot(worldAngVel, right);
		localAngVel[1] = vector.Dot(worldAngVel, up);
		localAngVel[2] = vector.Dot(worldAngVel, forward);
		
		bool isRollNeutral = Math.AbsFloat(m_iRoll) < 0.1;
		bool isPitchNeutral = Math.AbsFloat(m_iPitch) < 0.1;
			
		//Measure current drone tilt from level (90 = level)
		float pitchDot = vector.Dot(forward, worldUp);
		float rollDot  = vector.Dot(right, worldUp);
			
		m_fCurrentPitchDeg = Math.RAD2DEG * Math.Acos(Math.Clamp(pitchDot, -1.0, 1.0)) - 90.0;
		m_fCurrentRollDeg  = Math.RAD2DEG * Math.Acos(Math.Clamp(rollDot,  -1.0, 1.0)) - 90.0;
			
		//Compute angle error (desired - actual)
		float pitchError = targetPitchDeg - m_fCurrentPitchDeg;
		float rollError  = targetRollDeg  - m_fCurrentRollDeg;
			
		//Proportional gain
		float angleCorrectionStrength = m_DroneStablizerProfile.m_fAngleCorrectionRate; // tune: smaller = slower, bigger = twitchy
		
		float fovDifference
		if (m_CameraManager.CurrentCamera())
			fovDifference = Math.Clamp((m_CameraManager.CurrentCamera().GetVerticalFOV() - m_CameraZoom.m_iMaxZoom) / (m_CameraZoom.m_iMinZoom - m_CameraZoom.m_iMaxZoom), 0.01, 1.0);
		else
			fovDifference = 1;
		
		inputTorque[0] = Math.Clamp(pitchError * angleCorrectionStrength, -0.5, 0.5); // X = pitch
		inputTorque[2] = Math.Clamp(rollError  * -angleCorrectionStrength, -0.5, 0.5); // Z = roll
		inputTorque[1] = m_iYaw * fovDifference;
		
		//Counter drift
		vector velocityWorld = rigidBody.GetVelocity();
		vector localVel;
		localVel[0] = vector.Dot(velocityWorld, forward);
		localVel[1] = vector.Dot(velocityWorld, up);
		localVel[2] = -vector.Dot(velocityWorld, right);
		
		float velocityDampStrength = m_DroneStablizerProfile.m_fVelocityDampeningStrength;
		if (isPitchNeutral)
			inputTorque[0] = inputTorque[0] - Math.Clamp(localVel[0] * velocityDampStrength, -0.5, 0.5);
		if (isRollNeutral)
			inputTorque[2] = inputTorque[2] - Math.Clamp(localVel[2] * velocityDampStrength, -0.5, 0.5);
			
		// --- horizontal (XZ) speed limiter ---------------------------------
		vector horizVel = velocityWorld;
		horizVel[1] = 0;                                   // ignore vertical
			
		float horizSpeed = horizVel.Length();
		if (horizSpeed > m_DroneStablizerProfile.m_fMaxHorizontalSpeed)
		{
			// brake force opposite to travel direction
			vector brakeDir = -horizVel.Normalized();
			float excess    = horizSpeed - m_DroneStablizerProfile.m_fMaxHorizontalSpeed;
		 	vector brakeForce = brakeDir * (excess * m_DroneStablizerProfile.m_fBrakeStrength);
		
		   	// add to thrustForce so it is applied every frame
		   	thrustForce += brakeForce;
		}
		
		m_vInputTorque = inputTorque;
		m_vLocalAngVel = localAngVel;
		m_vThrustForce = thrustForce  * timeSlice;
	}
	
	void ~SDRC_SAL_DroneControllerComponent()
	{
		if (!GetGame().GetWorld())
			return;
		
		if (!GetGame().GetPlayerController())
			return;
		
		if (!m_DroneManager.IsDronePlayersRplId(m_DroneId))
			return;
		
		SCR_PlayerController.Cast(GetGame().GetPlayerController()).DisconnectSpecificDrone(m_DroneId);
	}
	
		
	override bool RplSave(ScriptBitWriter writer)
	{
		
		writer.WriteBool(m_bIsArmed);
		
		writer.WriteBool(m_bIsConnected);
		writer.WriteInt(m_iOwner);
		return true;
	}
	
	override bool RplLoad(ScriptBitReader reader)
	{
		
		reader.ReadBool(m_bIsArmed);
		
		reader.ReadBool(m_bIsConnected);
		reader.ReadInt(m_iOwner);
		return true;
	}
}
