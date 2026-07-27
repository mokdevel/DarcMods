modded class SAL_DroneControllerComponentClass: ScriptComponentClass 
{
}

modded class SAL_DroneControllerComponent: ScriptComponent
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
		if (m_iOwner != SCR_PlayerController.GetLocalPlayerId() || !m_DroneManager.m_aActiveDrones.Contains(m_DroneId))
			return;
		
		if (!SCR_PlayerController.GetLocalControlledEntity())
				return;
			
/*		--- Modded by darc ---
		
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
*/		
		
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
	
	override void ClientFrameChecks(IEntity owner, float timeSlice)
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

/*		//darc
		SDRC_ChopperComp cc = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (cc)
		{
			owner.GetPhysics().SetVelocity(cc.m_vVelocityVector);
		}
		//darc
*/					
		//Enables the gravity for the person controlling the drone
		if (m_iOwner == SCR_PlayerController.GetLocalPlayerId())
			rigidBody.EnableGravity(1);
	}
	
	override void ServerFrameChecks(IEntity owner, float timeSlice)
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
	
	override void SendPacket(IEntity owner, float timeSlice)
	{
		Physics rigidBody = GetOwner().GetPhysics();
		vector transform[4];
		owner.GetTransform(transform);
				
		SAL_DroneNetworkPacket packet = new SAL_DroneNetworkPacket;
		packet.SetOwner(owner);			//darc
		packet.SetDrone(m_DroneId);
		packet.SetRotors(m_aRotorsRplId);
		packet.SetRotorRPMs(m_aRotorRPM);
		packet.SetTransform(transform);
		packet.SetTimeSlice(timeSlice);
		packet.SetIsTriggerd(m_bIsTriggered);
		packet.SetBatteryLevel(m_BatteryComponent.m_fCurrentBattery);
		
		//darc
/*		SDRC_ChopperComp cc = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (cc)
		{
			vector vel = rigidBody.GetVelocity() - cc.m_vVelocityVector;
			packet.SetVelocity(vel);
		}*/
		//darc
		
		packet.SetVelocity(rigidBody.GetVelocity());
//		packet.SetVelocity("2.1 2.1 2.1");
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
}