modded class SAL_DroneControllerComponent
{
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
}
