modded class SAL_DroneControllerComponent
{
	vector m_TargetTransform[4] = {};
	
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{		
		//Do these things only on server side
		if (!SDRC_Misc.IsMaster())
		{
			return;
		}
		
		//super.EOnFrame(owner, timeSlice);
		if (owner.GetParent() != null)
			return;
		
		if (!m_GarbageSystem)
			m_GarbageSystem = SCR_GarbageSystem.GetByEntityWorld(owner);
		
		if(m_GarbageSystem)
			if (m_GarbageSystem.IsInserted(owner))
				m_GarbageSystem.Withdraw(owner);
		
/*		if (IsOnGround(owner) && m_iOwner == -1)
		{
			m_bIsActive = false;
			return;
		}
		else
			m_bIsActive = true;*/
		
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
//		if (System.IsConsoleApp())
//			return;

		if (SDRC_Misc.IsMaster())
		{
			ServerFrameChecks(owner, timeSlice);
			return;
		}
		
		return;
		
		//----------- We stop here on the server side ----------- 
		//Below code just left for reference 
		
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

	override void ServerFrameChecks(IEntity owner, float timeSlice)
	{
		Physics rigidBody = GetOwner().GetPhysics();
		if (owner.GetParent() == null && rigidBody.GetSimulationState() == 0)
		{
			rigidBody.ChangeSimulationState(SimulationState.SIMULATION);
			rigidBody.SetActive(true);
		}
				
		SendPacketServer(owner, timeSlice);
				
/*		TBD: Some component is disabling connection... need to investigate where that happens.
		if (!m_bIsConnected)
		{
			rigidBody.EnableGravity(1);
			SendPacketServer(owner, timeSlice);
			
//			if (!IsOnGround(owner))
//				SendPacketServer(owner, timeSlice);
		}
		else
			rigidBody.EnableGravity(0);*/
	}	
	
	override void SendPacket(IEntity owner, float timeSlice)
	{
		return;
		
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
		m_aRotorRPM[0] = 500;
		m_aRotorRPM[1] = 500;
		m_aRotorRPM[2] = 500;
		m_aRotorRPM[3] = 500;
		//darc		
		
		Physics rigidBody = GetOwner().GetPhysics();
		vector transform[4];
		owner.GetTransform(transform);
		
		SDRC_ChopperComp cc = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (cc)
		{
			m_vCurrentVelocity = cc.m_vVelocityVector;
		}
		
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
		//packet.SetVelocity(cc.m_vVelocityVector);		//darc
		
		/*if (m_bIsTriggered)
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
			m_DroneManager.ReplicateTransform(packet);*/
		
		ReplicateTransformS(packet);

		//darc: This section added here. We want to server to sync the drone to other players constantly. This was in fixed frame.
//		SendPacket(owner, timeSlice);
	}	

	void ReplicateTransformS(SAL_DroneNetworkPacket packet)
	{
		//Print(string.Format("RpcDo_ReplicateTransform called. IsServer=%1 IsClient=%2", Replication.IsServer(), Replication.IsClient() ));
		Rpc(RpcDo_ReplicateTransformS, packet);
	}
	
//	[RplRpc(RplChannel.Unreliable, RplRcver.Owner)]
//	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	[RplRpc(RplChannel.Unreliable, RplRcver.Broadcast)]
	void RpcDo_ReplicateTransformS(SAL_DroneNetworkPacket packet)
	{
		vector transform[4];
		packet.GetTransform(transform);
		
		if (!Replication.FindItem(packet.GetDrone()))
			return;
		
		IEntity drone = RplComponent.Cast(Replication.FindItem(packet.GetDrone())).GetEntity();
		if (!drone)
			return;
		
		if (!SAL_DroneConnectionManager.GetInstance())
			return;

		//Store the target transform to be used in EOnFrame
		m_TargetTransform = transform;
		
		SAL_DroneControllerComponent droneController = SAL_DroneControllerComponent.Cast(drone.FindComponent(SAL_DroneControllerComponent));

		//Send the transform to clients from server.		
		GenericEntity droneEntity = GenericEntity.Cast(drone);
		Physics rigidBody = droneEntity.GetPhysics();
		if (rigidBody)
		{
			rigidBody.ChangeSimulationState(SimulationState.SIMULATION);
			rigidBody.SetActive(true);
			rigidBody.EnableGravity(0);
		}
/*		
		droneEntity.SetTransform(transform);
		droneEntity.Update();
		droneEntity.OnTransformReset();*/
			
		RplId rotors[4];
		packet.GetRotors(rotors);
		
		float rotorRPMs[4];
		packet.GetRotorRPMs(rotorRPMs);
		float averageRPM = (rotorRPMs[0] + rotorRPMs[1] + rotorRPMs[2] + rotorRPMs[3]) / 4;

		SAL_DroneSoundComponent soundComp = SAL_DroneSoundComponent.Cast(drone.FindComponent(SAL_DroneSoundComponent));
		if (soundComp)
			soundComp.m_fAverageRotorRPM = averageRPM;

		//darc: Maybe not the right place...
		if (!soundComp.IsEngineOn() && m_bIsArmed)
			soundComp.StartEngine();
		else if (soundComp.IsEngineOn() && !m_bIsArmed)
			soundComp.ShutOffEngine();
		//darc
		
		if (droneController.m_bIsArmed)
		{
			for (int i = 0; i < 4; i++)
			{
				if (!Replication.FindItem(rotors[i]))
					 continue;
				
				IEntity rotor = RplComponent.Cast(Replication.FindItem(rotors[i])).GetEntity();
				if (!rotor) continue;
				
				float degPerSecond = rotorRPMs[i] * 6.0;
				int m_aRotorSpinDir[4] = { 1, -1, -1, 1 };					
				float rotationAmount = m_aRotorSpinDir[i] * degPerSecond * packet.GetTimeSlice();
				vector oldAngles = rotor.GetLocalAngles();
				oldAngles[1] = oldAngles[1] + rotationAmount;
				rotor.SetAngles(oldAngles);
			}
		}
		
		SAL_DroneBatteryComponent.Cast(droneEntity.FindComponent(SAL_DroneBatteryComponent)).m_fCurrentBattery = packet.GetBatteryLevel();
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
		//No client checks used as everything is handled in EOnFixedFrame and EOnFrame
		return;
	}	
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		vanilla.EOnFrame(owner, timeSlice);
		
		//Do these things only on client side
		if (SDRC_Misc.IsMaster())
		{
			return;
		}
		
		//Count a smoothMove value which is used in the lerp to move towards target
		float smoothSpeed = 8.0; // Higher = faster response
		float smoothMove = Math.Clamp(timeSlice * smoothSpeed, 0.0, 1.0);
/*		float smoothMove = 1.0 - Math.Exp(-smoothSpeed * timeSlice);*/
		
		//Static value
//		float smoothMove = 0.15;
		
		//Get current transform and lerp it towards target transform (destination between EOnFixedFrame)
		vector current[4];
		owner.GetTransform(current);
		
		// Position
		current[3] = vector.Lerp(current[3], m_TargetTransform[3], smoothMove);
		
		// Rotation
		// Interpolate the basis vectors
		current[0] = vector.Lerp(current[0], m_TargetTransform[0], smoothMove);
		current[1] = vector.Lerp(current[1], m_TargetTransform[1], smoothMove);
		current[2] = vector.Lerp(current[2], m_TargetTransform[2], smoothMove);
		
		// Set transform
		owner.SetTransform(current);		
		owner.Update();
		//owner.OnTransformReset();
	}
	
	override void EOnSimulate(IEntity owner, float timeSlice)
	{
		//No simulation used
		return;
	}	
}
