modded class SCR_PlayerController
{
	// === CLIENT → SERVER ===
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	override void RpcDo_SendTransformToServer(SAL_DroneNetworkPacket packet)
	{
		return;
		
		if (!Replication.FindItem(packet.GetDrone()))
			return;
		
		RplComponent droneComp = RplComponent.Cast(Replication.FindItem(packet.GetDrone()));
		if (!droneComp)
			return;
		
		IEntity drone = droneComp.GetEntity();
		if (!drone)
			return;
		
		vector prevTransform[4];
		drone.GetTransform(prevTransform);
		vector transform[4];
		packet.GetTransform(transform);
		
/*		if (vector.Distance(drone.GetOrigin(), transform[3]) > 10)
		{
			SAL_DroneConnectionManager.GetInstance().TeleportDroneBack(packet.GetDrone(), transform);
			return;
		}*/
		
		GenericEntity droneEntity = GenericEntity.Cast(drone);
		droneEntity.SetTransform(transform);
		droneEntity.Update();
		droneEntity.OnTransformReset();
		//droneComp.ForceNodeMovement(prevTransform[3]);
		
		SAL_DroneControllerComponent droneController = SAL_DroneControllerComponent.Cast(drone.FindComponent(SAL_DroneControllerComponent));
		
		SAL_DroneBatteryComponent.Cast(droneEntity.FindComponent(SAL_DroneBatteryComponent)).m_fCurrentBattery = packet.GetBatteryLevel();
		droneController.m_vCurrentVelocity = packet.GetCurrentVelocity();
		
		if (SAL_DroneConnectionManager.GetInstance())
			SAL_DroneConnectionManager.GetInstance().ReplicateTransform(packet);
	}
	
	override void ReplicateTransform(SAL_DroneNetworkPacket packet)
	{
		Rpc(RpcDo_ReplicateTransform, packet);
	}
	
	[RplRpc(RplChannel.Unreliable, RplRcver.Owner)]
	override void RpcDo_ReplicateTransform(SAL_DroneNetworkPacket packet)
	{
		vector transform[4];
		packet.GetTransform(transform);

/*		float val = transform[3][0];
		transform[3][0] = val + Math.RandomFloat(0, 100);
		val = transform[3][2];
		transform[3][2] = val + Math.RandomFloat(0, 100);*/
		
		if (!Replication.FindItem(packet.GetDrone()))
			return;
		
		IEntity drone = RplComponent.Cast(Replication.FindItem(packet.GetDrone())).GetEntity();
		if (!drone)
			return;
		
		if (!SAL_DroneConnectionManager.GetInstance())
			return;
		
		SAL_DroneControllerComponent droneController = SAL_DroneControllerComponent.Cast(drone.FindComponent(SAL_DroneControllerComponent));

		//Send the transform to clients from server.		
		if (droneController.m_iOwner != SCR_PlayerController.GetLocalPlayerId())  // Only apply if not the controller
		{
			GenericEntity droneEntity = GenericEntity.Cast(drone);
			droneEntity.SetTransform(transform);
			droneEntity.Update();
			droneEntity.OnTransformReset();
			
			RplId rotors[4];
			packet.GetRotors(rotors);
			
			float rotorRPMs[4];
			packet.GetRotorRPMs(rotorRPMs);
			float averageRPM = (rotorRPMs[0] + rotorRPMs[1] + rotorRPMs[2] + rotorRPMs[3]) / 4;
			
			SAL_DroneSoundComponent soundComp = SAL_DroneSoundComponent.Cast(drone.FindComponent(SAL_DroneSoundComponent));
			if (soundComp)
				soundComp.m_fAverageRotorRPM = averageRPM;
			
			if (droneController.m_bIsArmed)
			{
				for (int i = 0; i < 4; i++)
				{
					if (!Replication.FindItem(rotors[i]))
						 continue;
					
					IEntity rotor = RplComponent.Cast(Replication.FindItem(rotors[i])).GetEntity();
					if (!rotor) continue;
					
					float degPerSecond = rotorRPMs[i] * 6.0;
					float rotationAmount = m_aRotorSpinDir[i] * degPerSecond * packet.GetTimeSlice();
					vector oldAngles = rotor.GetLocalAngles();
					oldAngles[1] = oldAngles[1] + rotationAmount;
					rotor.SetAngles(oldAngles);
				}
			}
			
			SAL_DroneBatteryComponent.Cast(droneEntity.FindComponent(SAL_DroneBatteryComponent)).m_fCurrentBattery = packet.GetBatteryLevel();
		}
	}
}