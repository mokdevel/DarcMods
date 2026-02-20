//SDRC_ChopperHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_ChopperHelper
{
	//Line drawing related
	static CanvasWidget m_wCanvas;
	static BaseWorld m_World;
	static WorkspaceWidget m_Workspace;
	static ref array<ref CanvasWidgetCommand> m_aDrawCommands;
	
	//------------------------------------------------------------------------------------------------	
	/*!	
	Add a destination forward. 
	Create a destination from current heli position, along it's axis
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
	static void DrawLine(vector p0, vector p1, int color = Color.RED)
	{
		int shapeFlags = ShapeFlags.ONCE;
		vector p[2];
		p[0] = p0;
		p[1] = p1;		
		Shape.CreateLines(color, shapeFlags, p, 2);		
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
	// Misc functions
	//------------------------------------------------------------------------------------------------	

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
	
	//------------------------------------------------------------------------------------------------
	/*!
	Draws lines to show where chopper is going
	
	This only works in WB. 
	NOTE: Line drawing functionality exists in game for AIs. See: SCR_WaypointLinesEditorUIComponent
	*/
	static void DrawDestinationLines(IEntity owner)
	{
		array<float> m_Vertices = {};

		//The rest of the stuff is only GM mode.								
		if (!SDRC_PlayerHelper.IsInGMmode())
		{
			if (m_wCanvas)
			{
				//If not in GM mode, canvas is not needed.
				delete m_wCanvas;
			}
			return;
		}
		
		//Is interface visible
		if (!SDRC_PlayerHelper.IsGMInterfaceVisible())
		{
			return;
		}
		
		if (m_wCanvas == null)
		{
			m_wCanvas = CanvasWidget.Cast(g_Game.GetWorkspace().CreateWidgetInWorkspace(WidgetType.CanvasWidgetTypeID, 0, 0, 10, 10, WidgetFlags.VISIBLE | WidgetFlags.NOFOCUS, new Color(0.0, 0.0, 0.0, 1.0), 1024));
			m_World = GetGame().GetWorld();
			m_Workspace = GetGame().GetWorkspace();
		}
				
		if ( (m_wCanvas) && (m_Workspace) && (m_World) )
		{
			//All good
		}
		else
		{
			return;
		}
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}
		
/*		if (!chopperComp.m_bShowDebug)
		{
			return;
		}*/
		
		const int pLimit = 4;
		int shapeFlags = ShapeFlags.ONCE;
	
		vector p[pLimit] = {};
		
		//Add every fifth spline point
		int closestIndex = ((int)(chopperComp.m_iClosestIndex / 5)) * 5 + 1;
		int splinePointCount = chopperComp.m_vSplinePoints.Count() - 1;
		if (closestIndex > splinePointCount)
		{
			closestIndex = splinePointCount;
		}
		
		m_aDrawCommands = {};
		
/*		vector pos1 = "3300 60 2600";
		vector pos3 = "3000 80 2400";
		
		//++ Calculate screen position of points
		vector x0 = m_Workspace.ProjWorldToScreenNative(pos1, m_World);
		vector x1 = m_Workspace.ProjWorldToScreenNative(pos3, m_World);
//		vector x0 = workspace.ProjWorldToScreenNative(pos1, world);
//		vector x1 = workspace.ProjWorldToScreenNative(pos3, world);
		
		//++ Create draw command
		LineDrawCommand line;
		line = new LineDrawCommand();	
		line.m_iColor = Color.RED;
		line.m_fOutlineWidth = 0;
		line.m_fWidth = 4;
		line.m_Vertices = { x0[0], x0[1], x1[0], x1[1] };
		
		//++ Insert into pool of draw commands
		m_aDrawCommands.Insert(line);		*/
		
		//Starting point
		AddVertice(chopperComp.m_vSplinePoints[closestIndex], m_Vertices);
		
		for (int i = closestIndex; i < splinePointCount; i = i + 5)
		{
			AddVertice(chopperComp.m_vSplinePoints[i], m_Vertices);
		} 
		
		AddVertice(chopperComp.m_vSplinePoints[chopperComp.m_vSplinePoints.Count() - 1], m_Vertices);
		//Insert into pool of draw commands
		m_aDrawCommands.Insert(AddLines(m_Vertices, Color.DARK_GREEN));
		
		//Add destinations if any
		if (!chopperComp.m_vFlyDestinations.IsEmpty())
		{	
			//Add last point to be the first for blue lines
			AddVertice(chopperComp.m_vSplinePoints[chopperComp.m_vSplinePoints.Count() - 1], m_Vertices);
			
			foreach (SDRC_FlyPathPoint destination : chopperComp.m_vFlyDestinations)
			{			
				vector pos = destination.pt;
				if (pos[1] == 0)
				{
					pos[1] = SDRC_Misc.GetSurfaceYWithWater(pos) + 40;
				}
				AddVertice(pos, m_Vertices);
			}		
					
			//Insert into pool of draw commands
			m_aDrawCommands.Insert(AddLines(m_Vertices, Color.DARK_BLUE));
		}
		
		if (!m_aDrawCommands.IsEmpty())
		{
			m_wCanvas.SetDrawCommands(m_aDrawCommands);
		}
	}		

	//------------------------------------------------------------------------------------------------
	/*!
	Calculate the vertice screen coords and add to vertices list
	*/
	static void AddVertice(vector pos, out array<float> vertices)
	{
		//Calculate screen position of point
		vector x0 = m_Workspace.ProjWorldToScreenNative(pos, m_World);
		vertices.Insert(x0[0]);
		vertices.Insert(x0[1]);		
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Add vertices to LineDrawCommand and return it
	*/
	static LineDrawCommand AddLines(out array<float> vertices, int color = Color.DARK_GREEN)
	{		
		//Create draw command
		ref LineDrawCommand line = new LineDrawCommand();	
		line.m_Vertices = {};
		line.m_iColor = color;
		line.m_fOutlineWidth = 0;
		line.m_fWidth = 2;
		line.m_Vertices.Copy(vertices);
		vertices.Clear();
		return line;
	}
}