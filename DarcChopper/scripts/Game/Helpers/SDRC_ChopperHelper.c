//SDRC_ChopperHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_ChopperHelper
{
	//------------------------------------------------------------------------------------------------	
	/*!	
	Calculate a position in front of current heli position, along it's axis. 
	*/
	static vector GetDestinationForward(IEntity owner, float distance)
	{
		vector direction = owner.GetTransformAxis(2);
		direction.Normalize();
		direction[1] = 0;
		vector origin = owner.GetOrigin();
		vector position = origin + direction * distance;
		//Keep the flying flat
		position[1] = origin[1];
		return position;
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Creates a random destination point
	*/	
	static vector GetRandomPosition(vector pos, float distanceLow, float distanceHigh)
	{
		float distance = SDRC_Misc.RandomFloat(distanceLow, distanceHigh);
		vector newpos;
		
		//If distance is under 1.0, use world percentage
		if (distance < 1.0)
		{
			newpos = SDRC_Misc.GetRandomWorldPosPercentage(distance);
		}
		else
		{
			newpos = SDRC_Misc.GetCoordinatesOnCircle(pos, distance, SDRC_Misc.RandomInt(0, 360));
		}
				
		return newpos;
	}	
	
	//------------------------------------------------------------------------------------------------
	static bool IsStillWorking(IEntity owner, bool inInit)
	{
		//If still in init, don't care if no pilots etc yet set.
		if (inInit)
		{
			return true;
		}

		//If working and at least one pilot, all good
		if ( (SDRC_VehicleHelper.IsWorking(owner)) && (SDRC_VehicleHelper.PilotCountAlive(owner) > 0) )
		{
			return true;
		}
		
		//Set damage so it should be destroyed on crash		
		SetHealth(owner, SDRC_Misc.RandomFloat(0, 0.05));
/*		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
		if (damageManager)
		{
			float damage = SDRC_Misc.RandomFloat(0, 0.05);
			damageManager.SetHealthScaled(damage);		
		}*/
		
		//Make the chopper fly unsteadily
		VehicleHelicopterSimulation helicopter_s = VehicleHelicopterSimulation.Cast(owner.GetRootParent().FindComponent(VehicleHelicopterSimulation));
		if (helicopter_s)
		{
			float force = SDRC_Misc.RandomFloat(0, 0.1);
	        helicopter_s.RotorSetForceScaleState(0, force);
			force = SDRC_Misc.RandomFloat(0.1, 2.5);
	        helicopter_s.RotorSetForceScaleState(1, force);
			force = SDRC_Misc.RandomFloat(0.0, 0.1);
			helicopter_s.SetThrottle(force);
		}

		//Remove the canvas and lines if chopper is destroyed
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (chopperComp)
		{
			chopperComp.m_aDrawCommands.Clear();
			chopperComp.m_wCanvas.SetDrawCommands(chopperComp.m_aDrawCommands);
			delete chopperComp.m_wCanvas;
		}		
						
		return false;
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Get helicopter altitude from ground. 

	Could use also float altitude = m_Helicopter_s.GetAltitudeAGL();
	*/	
	static float GetAltitude(IEntity owner)
	{
		float surfaceY = SDRC_Misc.GetSurfaceYWithWater(owner.GetOrigin());
		vector origin = owner.GetOrigin();
		float altitude = origin[1] - surfaceY;
		
		return altitude;		
	}
		
	//------------------------------------------------------------------------------------------------	
	// Damage settings
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	static float GetHealth(IEntity owner)
	{
		float health = -1;
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
		if (damageManager)
		{
			health = damageManager.GetHealth();
		}
		
		return health;
	}
	
	//------------------------------------------------------------------------------------------------
	static void SetHealth(IEntity owner, float health)
	{
		DamageManagerComponent damageManager = DamageManagerComponent.Cast(owner.FindComponent(DamageManagerComponent));
		if (damageManager)
		{
			damageManager.SetHealthScaled(health);
		}
		
		SDRC_Log.Add("[SDRC_ChopperHelper:SetHealth] Setting health: " + health, LogLevel.DEBUG);
	}
	
	//------------------------------------------------------------------------------------------------	
	// Crew functions
	//------------------------------------------------------------------------------------------------	
	static int SpawnCrew(IEntity owner, array<ref SCR_DefaultOccupantData> crewmember, string faction, EAISkill skill = EAISkill.REGULAR, float perceptionFactor = 1.0)
	{		
		int pilotCount = SDRC_VehicleHelper.GetCompartmentCountOfType(owner, ECompartmentType.PILOT);
		int crewCount = 0;
		
		array<ResourceName> crewPrefabs = {}; 
		
		//If no faction defined, find the default vehichle faction and use that
		if (faction == "")
		{			
			Vehicle veh = Vehicle.Cast(owner);
			
			if (veh)
			{
				Faction veh_faction = veh.GetDefaultFaction();
				if (veh_faction)
				{
					faction = veh_faction.GetFactionKey();
					SDRC_Log.Add("[SDRC_ChopperHelper:SpawnCrew] Vehicle faction: " + faction, LogLevel.DEBUG);
				}
			}
		}
		
		//Select crew	
		if (crewmember.IsEmpty())
		{
			//Add pilots			
			for (int i = 0; i < pilotCount; i++)
			{
				ResourceName member = SDRC_EnemyHelper.SelectEnemy("C_CREW", faction);
				if (member == "")
				{
					member = "{472F2B06FF9BF37D}Prefabs/Characters/Factions/CIV/Dockworker/Character_CIV_Dockworker_4.et";
				}
				crewPrefabs.Insert(member);
			}
			
			//Add 1-4 random additional riflemen
			for (int i = 0; i < SDRC_Misc.RandomInt(1, 4); i++)
			{
				ResourceName member = SDRC_EnemyHelper.SelectEnemy("C_RIFLEMAN", faction);
				if (member == "")
				{
					member = "{472F2B06FF9BF37D}Prefabs/Characters/Factions/CIV/Dockworker/Character_CIV_Dockworker_4.et";
				}
				crewPrefabs.Insert(member);
			}
		}
		else
		{
			//Use the provided crew
			foreach (int i, SCR_DefaultOccupantData member : crewmember)
			{
				crewPrefabs.Insert(member.GetDefaultOccupantPrefab());
			}
		}
						
		//Add the crew
		if (crewPrefabs.Count() > 0)
		{			
			SCR_AIGroup	gPilot;
			SCR_AIGroup	gCrew;
			
			foreach (int i, ResourceName prefab : crewPrefabs)
			{
				//Skip empty ones
				if (prefab == "")
				{
					continue;
				}
				
				//Spawn pilots if such is available 
				vector pos = owner.GetOrigin();
				pos = pos + "30 0 30";
				
				if (i < pilotCount)
				{
					if (!gPilot)
					{
						gPilot = SDRC_AIHelper.GroupCreate(faction, pos);						
					}
					SDRC_VehicleHelper.SpawnGroupInVehicle(prefab, owner, gPilot);
				}
				else
				{
					if (!gCrew)
					{
						gCrew = SDRC_AIHelper.GroupCreate(faction, pos);						
					}
					SDRC_VehicleHelper.SpawnGroupInVehicle(prefab, owner, gCrew);
				}
				
				crewCount++;
			}
			
			//Set AI skill
			if (gPilot)
			{
				SDRC_AIHelper.SetAIGroupSettings(gPilot, skill, perceptionFactor);
			}
			if (gCrew)
			{
				SDRC_AIHelper.SetAIGroupSettings(gPilot, skill, perceptionFactor);
			}
			
		}
		else
		{
			SDRC_Log.Add("[SDRC_ChopperHelper:SpawnCrew] No crew defined. Without pilots, we will crash.", LogLevel.WARNING);
		}
		
		return crewCount;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Draws lines to show where chopper is going
	*/
	static void HandleWaypoints(IEntity owner)
	{
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}

		array<AIWaypoint> waypoints = {};
				
		foreach (AIGroup group : chopperComp.m_aGroups)
		{
			if (group != null)
			{
				group.GetWaypoints(waypoints);
				
				//If group has no waypoints, check the next group
				if (waypoints.Count() == 0)
				{
					continue;
				}

				//Find if it is a cycle				
				foreach (AIWaypoint wp : waypoints)
				{
					//Skip cycle waypoints
					if (AIWaypointCycle.Cast(wp) != null)
					{
						continue;
					}
					
					if (AIWaypoint.Cast(wp) != null)
					{
						vector pos = wp.GetOrigin();
						
						EntityPrefabData prefabData = wp.GetPrefabData();
						ResourceName resourceName = prefabData.GetPrefabName();
						resourceName = SDRC_Misc.GetSimpleEntityName(resourceName);
						SDRC_Log.Add("[SDRC_ChopperHelper:HandleWaypoints] Waypoint " + resourceName + " found at: " + pos, LogLevel.DEBUG);						
						
						switch (resourceName)
						{
							case "E_AIWaypoint_Move":
							{
								chopperComp.AddDestination(SDRC_EFlyWayPointType.FLY, pos);
								break;
							}						
							case "E_AIWaypoint_ForcedMove":
							{
								chopperComp.AddDestination(SDRC_EFlyWayPointType.FLY_IMMEDIATELY, pos);
								break;
							}						
							case "E_AIWaypoint_Patrol":
							{
								chopperComp.AddDestination(SDRC_EFlyWayPointType.PATROL, pos);
								break;
							}						
						}
					}
				}
				
				//Clear all waypoints
				SDRC_WPHelper.RemoveWaypoints(group);	
			}
		}
	}
}