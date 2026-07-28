modded class SAL_DroneControllerComponent
{
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		//super.EOnFrame(owner, timeSlice);
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
/*		if (m_iOwner != SCR_PlayerController.GetLocalPlayerId() || !m_DroneManager.m_aActiveDrones.Contains(m_DroneId))
			return;
		
		if (!SCR_PlayerController.GetLocalControlledEntity())
				return;*/
			
/*		BaseWeaponManagerComponent weaponMan = BaseWeaponManagerComponent.Cast(ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity()).FindComponent(BaseWeaponManagerComponent));
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
		}*/
		
		
		if (m_InputManager.GetActionValue("ArmDrone") > 0)
				ArmDrone();

		//If the drone controller arms it this is what starts listening for inputs
		if (m_bIsArmed)
		{
/*			float lq = 1 - m_SignalComponent.m_fLQ;
			
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
			}*/
			
			float rawInput = Math.Clamp(m_InputManager.GetActionValue("DroneUp"), -1.0, 1.0);			
			rawInput = 1.0;	//darc
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

	override void SendPacket(IEntity owner, float timeSlice)
	{
		//darc: Force some RPMs. TBD: Could be taken from ChopperComp
		m_aRotorRPM[0] = 0.5;
		m_aRotorRPM[1] = 0.5;
		m_aRotorRPM[2] = 0.5;
		m_aRotorRPM[3] = 0.5;
		//darc		
		
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
	
	override void SendPacketServer(IEntity owner, float timeSlice)
	{
		//darc: Force some RPMs. TBD: Could be taken from ChopperComp
		m_aRotorRPM[0] = 0.5;
		m_aRotorRPM[1] = 0.5;
		m_aRotorRPM[2] = 0.5;
		m_aRotorRPM[3] = 0.5;
		//darc		
		
		Physics rigidBody = GetOwner().GetPhysics();
		vector transform[4];
		owner.GetTransform(transform);
		
		m_vCurrentVelocity = "0.5 0.5 0.5";
		
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
		
	override void ArmDrone()
	{
		m_bIsArmed = !m_bIsArmed;
		
		if (m_bIsArmed)
		{
			SDRC_Log.Add("[SDRC_SAL_DroneControllerComponent:ArmDrone] Armed.", LogLevel.DEBUG);
			SAL_DroneNetworkPacket packet = new SAL_DroneNetworkPacket;
			packet.SetDrone(m_DroneId);
			packet.SetIsArmed(m_bIsArmed);	
			//SCR_PlayerController.Cast(GetGame().GetPlayerController()).DisarmDrone(packet);
		}
		else
		{
			SDRC_Log.Add("[SDRC_SAL_DroneControllerComponent:ArmDrone] Disarmed.", LogLevel.DEBUG);
			SAL_DroneNetworkPacket packet = new SAL_DroneNetworkPacket;
			packet.SetDrone(m_DroneId);
			packet.SetIsArmed(m_bIsArmed);	
			//SCR_PlayerController.Cast(GetGame().GetPlayerController()).DisarmDrone(packet);
		}
			
	}		
	
	override bool HasController()
	{
		//The drone has always an AI with a controller.
		//TBD: We could assign an AI that works as the driver for the drone. Handled in e.g. ChopperComp
		return true;
	}
	
	override void ClientFrameChecks(IEntity owner, float timeSlice)
	{
		Physics rigidBody = GetOwner().GetPhysics();
		if (owner.GetParent() == null && rigidBody.GetSimulationState() == 0)
		{
			rigidBody.ChangeSimulationState(SimulationState.SIMULATION);
			rigidBody.SetActive(true);
		}
		
		// Makes sure everyone is tracking the drones new position if no one is controlling it, if you dont do this picking up the drone breaks
/*		if (!m_bIsConnected)
			rigidBody.EnableGravity(1);
		else if (m_bIsConnected && m_iOwner != SCR_PlayerController.GetLocalPlayerId())
			rigidBody.EnableGravity(0);*/
		
		if (m_SoundComponent)
		{
			if (!m_SoundComponent.IsEngineOn() && m_bIsArmed)
				m_SoundComponent.StartEngine();
			else if (m_SoundComponent.IsEngineOn() && !m_bIsArmed)
				m_SoundComponent.ShutOffEngine();
		}
//		vector transform[4];
//		owner.GetTransform(transform);
			
		//Enables the gravity for the person controlling the drone
/*		if (m_iOwner == SCR_PlayerController.GetLocalPlayerId())
			rigidBody.EnableGravity(1);*/
		
		//darc: Force set gravity for everyone.
		rigidBody.EnableGravity(1);
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
		if (!m_DroneManager)	//darc: We send the information always
		//if (!m_DroneManager || m_iOwner != SCR_PlayerController.GetLocalPlayerId())
			return;
		
/*		// Needed for when the drone is not armed and in the air so everyone can still track where its at
		if (!m_bIsArmed)
		{
			if (!IsOnGround(owner))
				SendPacket(owner, timeSlice);
			return;
		}*/
		
		//Sends data to other players every 30ms
		SendPacket(owner, timeSlice);
	}
	
	override void EOnSimulate(IEntity owner, float timeSlice)
	{
		//super.EOnSimulate(owner, timeSlice);
		Physics rigidBody = GetOwner().GetPhysics();
		if (!rigidBody) return;
		
		if (!m_DroneManager) 
			m_DroneManager = SAL_DroneConnectionManager.GetInstance();
		
		//Same as above just checks to see if the person running this is the drones controller
		if (!m_DroneManager)	//darc: We run this always
		//if (!m_DroneManager || m_iOwner != SCR_PlayerController.GetLocalPlayerId())
			return;
		
		// Needed for when the drone is not armed and in the air so everyone can still track where its at
		if (!m_bIsArmed)
			return;

		//If the drone is stabalized duh
		if (m_bStabilized)
			CalculateStablizedInputs(owner, timeSlice, rigidBody);
		else
			CalculateAcroInputs(owner, timeSlice, rigidBody);
		
		//darc: Add some torque
//		m_vInputTorque = "0.5 0.5 0.5";	
//		m_vThrustForce = "0.5 0.5 0.5";
		//darc
		
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
		//rigidBody.ApplyImpulse(m_vThrustForce);
		//rigidBody.ApplyTorque(worldTorque * timeSlice);
	}	
}
