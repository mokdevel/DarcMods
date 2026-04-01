modded class SAL_DroneSignalComponent
{
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		if (System.IsConsoleApp())
			return;
		
		if (m_Id == -1)
			m_Id = RplComponent.Cast(owner.FindComponent(RplComponent)).Id();
		
		if (m_DroneController.m_iOwner != SCR_PlayerController.GetLocalPlayerId()  || !m_DroneManager.m_aActiveDrones.Contains(m_Id))
			return;
		
		int antennaMultiplier = 1;
		if (m_DroneManager.IsPlayerAntennaOwner(SCR_PlayerController.GetLocalPlayerId()))
		{
			antennaMultiplier = SAL_DroneAntennaComponent.Cast(m_DroneManager.GetPlayersAntenna(SCR_PlayerController.GetLocalPlayerId()).FindComponent(SAL_DroneAntennaComponent)).m_iAntennaRangeMultiplier;
		}
		
		if (m_fTimer < 0.01)
		{
			m_fTimer += timeSlice;
			return;
		}
		else
			m_fTimer = 0;

		/* Start: Changes by darc */		
		vector droneOrigin = owner.GetOrigin();
		vector playerOrigin = droneOrigin;
		if (SCR_PlayerController.GetLocalControlledEntity())
		{
			playerOrigin = SCR_PlayerController.GetLocalControlledEntity().GetOrigin();
		}
		/* End: Changes by darc */		
		
		int distanceFrom = vector.Distance(playerOrigin, droneOrigin);
		float distancePercentage = distanceFrom/ (m_fDroneRange * antennaMultiplier);
		float signalLoss = Math.Pow(distancePercentage, 2.2);  // tuned exponent
		
		vector droneSignal = droneOrigin;
		droneSignal[1] = droneSignal[1] + 0.5;
		droneSignal[2] = droneSignal[2] - 0.5;
		
		vector playerSignal = playerOrigin;
		playerSignal[1] = playerSignal[1] + 2;
		
		ref array<int> playerIDs = {};
		GetGame().GetPlayerManager().GetAllPlayers(playerIDs);
		
		ref array<IEntity> allEntities = {};
		foreach(int playerID: playerIDs)
		{
			allEntities.Insert(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerID));
		}
		ref array<AIAgent> allAgents = {};
		if(GetGame().GetAIWorld())
		{
			GetGame().GetAIWorld().GetAIAgents(allAgents);
			foreach(AIAgent agent: allAgents)
			{
				allEntities.Insert(agent.GetControlledEntity());
			}
		}
		allEntities.Insert(owner);
		float losFraction = RayCastLine(droneSignal, playerSignal, EPhysicsLayerDefs.Terrain, allEntities);
		float obstructionBase = 1.0 - losFraction;
		float distWeight = Math.Pow(distancePercentage, 1.50);
		float proximityWeight = Math.Lerp(0.50, 1.60, losFraction);          
		float targetObstructionPenalty = Math.Clamp(Math.Pow(obstructionBase, 0.50) * distWeight * proximityWeight, 0.0, 1.0);
		
		m_fObstructionPenaltySmoothed = Math.Lerp(m_fObstructionPenaltySmoothed, targetObstructionPenalty, timeSlice * 2.5);
		
		float finalSignal = Math.Clamp(signalLoss + m_fObstructionPenaltySmoothed, 0.0, 1.0);
		
		
		ref array<IEntity> jammers = {};
		//Find Jammers In Range
		foreach (RplId jammerId: m_DroneManager.m_aJammers)
		{	
			if (!Replication.FindItem(jammerId))
				continue;
			IEntity jammer = RplComponent.Cast(Replication.FindItem(jammerId)).GetEntity();
			if (!jammer)
				continue;
			SAL_DroneJammerComponent jammerComp = SAL_DroneJammerComponent.Cast(jammer.FindComponent(SAL_DroneJammerComponent));
			float maxRange = jammerComp.m_fJammingRange;
			
			if (vector.Distance(droneOrigin, jammer.GetOrigin()) > maxRange)
				continue;
			
			jammers.Insert(jammer);
		}
		
		IEntity closestJammer;
		//Find closest Jammer
		if (jammers.GetSizeOf() > 0)
		{
			foreach (IEntity jammer: jammers)
			{
				if (!closestJammer)
				{
					closestJammer = jammer;
					continue;
				}
					
				
				if (vector.Distance(droneOrigin, jammer.GetOrigin()) < vector.Distance(droneOrigin, closestJammer.GetOrigin()))
					closestJammer = jammer;
			}
		
			if (closestJammer)
			{
				SAL_DroneJammerComponent jammerComp = SAL_DroneJammerComponent.Cast(closestJammer.FindComponent(SAL_DroneJammerComponent));
				float jammerDist = vector.Distance(droneOrigin, closestJammer.GetOrigin());
				float jammerNorm = Math.Clamp(jammerDist / jammerComp.m_fJammingRange, 0.0, 1.0);
				float jammerLoss = Math.Pow(1 - jammerNorm, 3.0);
		
				// Combine using probability union: L_eff = 1 - (1 - Lp)(1 - Lj)
				finalSignal = finalSignal + jammerLoss - (finalSignal * jammerLoss); // L_eff
			}
		}
		m_fSignalStrength = Math.Round(finalSignal * 255);
		m_fRSSI = Math.Clamp(signalLoss + m_fObstructionPenaltySmoothed, 0.0, 1.0);
		m_fLQ = finalSignal;
	}
}