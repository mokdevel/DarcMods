//SDRC_ChopperDebug.c

//------------------------------------------------------------------------------------------------
class SDRC_ChopperDebug
{
	//Line drawing related
	static BaseWorld m_World;
	static WorkspaceWidget m_Workspace;
	
	//------------------------------------------------------------------------------------------------
	/*!
	Draws lines to show where chopper is going
	*/
	static void DrawDestinationLines(IEntity owner)
	{
		array<float> m_Vertices = {};
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}		

		//The rest of the stuff is only GM mode and when interface is visible.								
		if ( (!SDRC_PlayerHelper.IsInGMmode()) || (!SDRC_PlayerHelper.IsGMInterfaceVisible()) || chopperComp.GetState() == SDRC_EHeliState.DESTROYED )
		{
			if (chopperComp.m_wCanvas)
			{
				//If not in GM mode, canvas is not needed.
				delete chopperComp.m_wCanvas;
			}
			return;
		}
		
		if (chopperComp.m_wCanvas == null)
		{
			//Create a canvas to draw the lines. Keep it small as a big canvas will also capture the mouse which we don't want
			chopperComp.m_wCanvas = CanvasWidget.Cast(g_Game.GetWorkspace().CreateWidgetInWorkspace(WidgetType.CanvasWidgetTypeID, 0, 0, 10, 10, WidgetFlags.VISIBLE | WidgetFlags.NOFOCUS, new Color(0.0, 0.0, 0.0, 1.0), 100000));
			m_World = GetGame().GetWorld();
			m_Workspace = GetGame().GetWorkspace();
		}
				
		if ( (chopperComp.m_wCanvas) && (m_Workspace) && (m_World) )
		{
			//All good
		}
		else
		{
			return;
		}
		
/*		SCR_CameraEditorComponent cameraManager = SCR_CameraEditorComponent.Cast(SCR_CameraEditorComponent.GetInstance(SCR_CameraEditorComponent));
		if (cameraManager)
		{
			SCR_ManualCamera GMCamera = cameraManager.GetCamera();				
			if (GMCamera)
			{
				Print("camerapos:" + GMCamera.GetAngles());
			}
		}*/
				
/*		vector textMat[4];
		GetGame().GetWorld().GetCurrentCamera(textMat);
		vector position = textMat[2];
		Print("camerapos:" + position);*/
		
		
/*		if (!chopperComp.m_bShowDebug)
		{
			return;
		}*/
		
		//Add every nth spline point
		int nth = 4;
		int closestIndex = ((int)(chopperComp.m_iClosestIndex / nth)) * nth + 1;
		int splinePointCount = chopperComp.m_vSplinePoints.Count() - 1;
		if (closestIndex > splinePointCount)
		{
			closestIndex = splinePointCount;
		}
		
		chopperComp.m_aDrawCommands.Clear();// = {};
		
		//Starting point
		AddVertice(chopperComp.m_vSplinePoints[closestIndex], m_Vertices);
		
		for (int i = closestIndex; i < splinePointCount; i = i + nth)
		{
			AddVertice(chopperComp.m_vSplinePoints[i], m_Vertices);
		} 
		
		AddVertice(chopperComp.m_vSplinePoints[chopperComp.m_vSplinePoints.Count() - 1], m_Vertices);
		//Insert into pool of draw commands
		chopperComp.m_aDrawCommands.Insert(AddLines(m_Vertices, Color.DARK_GREEN));
		
		//Add destinations if any
		if (!chopperComp.m_vFlyDestinations.IsEmpty())
		{	
			//Add last point to be the first for blue lines
//			AddVertice(chopperComp.m_vSplinePoints[chopperComp.m_vSplinePoints.Count() - 1], m_Vertices);
			vector prevPos = chopperComp.m_vSplinePoints[chopperComp.m_vSplinePoints.Count() - 1];
			
			foreach (SDRC_FlyPathPoint destination : chopperComp.m_vFlyDestinations)
			{
				int color = -1;

				vector pos = destination.pt;
				if (pos[1] == 0)
				{
					pos[1] = SDRC_Misc.GetSurfaceYWithWater(pos) + 40;
				}
				
				switch (destination.type)
				{
					case SDRC_EFlyWayPointType.WP_FLY:					
					{
						color = Color.DARK_BLUE;
						break;
					}
					case SDRC_EFlyWayPointType.WP_LAND:
					{
						color = Color.DARK_CYAN;
						break;
					}				
					case SDRC_EFlyWayPointType.WP_PATROL:
					{
						color = Color.GRAY;
						break;
					}				
					case SDRC_EFlyWayPointType.WP_ATTACK:
					{
						color = Color.RED;
						pos[1] = SDRC_Misc.GetSurfaceYWithWater(pos) + 10;
						break;
					}				
				}
				
				if (color == -1)
				{
					continue;
				}
				
				AddVertice(prevPos, m_Vertices);
				AddVertice(pos, m_Vertices);
				prevPos = pos;
				//Insert into pool of draw commands
				chopperComp.m_aDrawCommands.Insert(AddLines(m_Vertices, color));
			}		
					
//			//Insert into pool of draw commands
//			chopperComp.m_aDrawCommands.Insert(AddLines(m_Vertices, Color.DARK_BLUE));
		}
		
		if (!chopperComp.m_aDrawCommands.IsEmpty())
		{
			chopperComp.m_wCanvas.SetDrawCommands(chopperComp.m_aDrawCommands);			
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
		if (x0[2] > 0)
		{
			vertices.Insert(x0[0]);
			vertices.Insert(x0[1]);
		}
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
		line.m_fWidth = 1.5;
		line.m_Vertices.Copy(vertices);
		vertices.Clear();
		return line;
	}
	
	//------------------------------------------------------------------------------------------------	
	// Debug shapes
	// These *ONLY* works in WB. 
	//------------------------------------------------------------------------------------------------	

	//------------------------------------------------------------------------------------------------
	/*!	
	Convert flyPathPoints to simple vector points.
	TBD: This is stupid code
	*/
	static void GivePoints(out array<vector> points, array<ref SDRC_FlyPathPoint> flyPathPoints)
	{
		points.Clear();
		
		foreach (SDRC_FlyPathPoint flyPathPoint : flyPathPoints)
		{
			points.Insert(flyPathPoint.pt);
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	static void DrawDebugPaths(IEntity owner)
	{
//		if (!m_bShowDebug)
/*		if (!SDRC_Conf.SHOW_DEBUG)
		{
			return;
		}*/
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}
		
		array<vector> flyPathPoints = {};
		GivePoints(flyPathPoints, chopperComp.m_vFlightPoints);
		
		SDRC_DebugHelper.DeleteDebugItems(chopperComp.m_sDid, marks: false);
		SDRC_DebugHelper.DrawPointList(chopperComp.m_vSplinePoints, chopperComp.m_sDid);		
		SDRC_DebugHelper.DrawPointList(flyPathPoints, chopperComp.m_sDid, ARGB(10, 64, 64, 192));
	}
		
	//------------------------------------------------------------------------------------------------	
	/*!
	Draw debugging details of helicopter
	*/	
	static void DrawHelicopterVectors(IEntity owner)
	{
//		if (!m_bShowDebug)
		if (!SDRC_Conf.SHOW_DEBUG)		
		{
			return;
		}
		
		if (!SDRC_PlayerHelper.IsGMInterfaceVisible())
		{
			return;
		}
		
		vector origin = owner.GetOrigin();
		SCR_VehicleDamageManagerComponent damageManager = SCR_VehicleDamageManagerComponent.Cast(owner.FindComponent(SCR_VehicleDamageManagerComponent));
		float health = damageManager.GetHealth();

		VehicleHelicopterSimulation helicopter_s = VehicleHelicopterSimulation.Cast(owner.GetRootParent().FindComponent(VehicleHelicopterSimulation));
		if (!helicopter_s)
		{
			return;
		}

		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}
		
		vector heliUp = owner.GetTransformAxis(1);
		float angUp = SDRC_Math.GetAngleBetweenVectors(heliUp, vector.Up);
				
		if (DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_INFO))
		{		
			string debugText = 	//"Speedangle:" + angle * Math.RAD2DEG + "\n" +
								SCR_Enum.GetEnumName(SDRC_EHeliState, chopperComp.m_eHeliState) + " (" + 
								SDRC_Misc.FloatWithDecimals(chopperComp.m_fTimeInState) + ")";
								if (chopperComp.m_vEnemyPosition != vector.Zero)
								{
									debugText = debugText + " (enemy)";
								}
			debugText = debugText + "\n";
			
			debugText = debugText +
							   	"Speed:" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fSpeed) + " " +
							   	"(" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fSpeedStart) + "/" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fSpeedTarget) + ") " +
	//						   	"Avg time:" + m_fTimeBetweenPtsAvg + "\n" +
							   	"mul:" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fSpeedMul, 2) + " " + 
							   	"min:" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fSpeedMin) + "\n" + 
								"";		
			debugText = debugText + 
							   	"Alt:" + 
								SDRC_Misc.FloatWithDecimals(chopperComp.m_fAltitude) + " " + 
							   	SDRC_Misc.FloatWithDecimals(helicopter_s.GetAltitudeAGL()) + " " +
								"\n" + 
							   	"RotorForceMul:" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fRotorForceMultiplier, 2) + "\n" +
	//						   	"SplinePoints:" + m_vSplinePoints.Count() + "\n" +
	//						   	"TurnInternal:" + m_fTimeTurnInterval + "\n" +
	//							"Angle: " + m_fDbgAngle * Math.RAD2DEG + "\n" +
//								"AnglePitch: " + m_fAnglePitch * Math.RAD2DEG + "\n" +
								"AnglePitch: " + SDRC_Misc.FloatWithDecimals(chopperComp.m_fAnglePitch) + "\n" +
	//							"AngleRoll: " + m_fAngleRoll * Math.RAD2DEG + "\n" +
	//							"AngleRollBack: " + m_fAngleRollBack * Math.RAD2DEG + "\n" +
	//							"DestinationPointAdd: " + m_iDestinationPointAdd + "\n" 
								"";
			debugText = debugText + 
//								"Init:" + m_bInInit + ", " +
//								"Pilots::" + SDRC_VehicleHelper.PilotCountAlive(owner) + "\n" +
//								"Working:" + SDRC_VehicleHelper.IsWorking(owner) + " - " + 
//								"Health: " + health + "\n" + 
	//							"Is piloted:" + SDRC_VehicleHelper.IsPiloted(owner) + "\n" +
								"";

			if (angUp > 1.3)
			{
				debugText = debugText + "AngleUp: ******** " + angUp + " ********";
			}
					
			DebugTextWorldSpace.Create(GetGame().GetWorld(), debugText, DebugTextFlags.ONCE, origin[0], origin[1], origin[2], 20);
		}
			
		if (!DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_LINES))
		{		
			return;
		}
		
		int idx = chopperComp.m_iClosestIndex;
		if (idx > chopperComp.m_vSplinePoints.Count() - 1)
		{
			idx = chopperComp.m_vSplinePoints.Count() - 1;			
			//SDRC_Log.Add("[SDRC_ChopperComp:DrawHelicopterVectors] Index fixed.", LogLevel.WARNING);
		}
						
		//Planned destination
		SDRC_ChopperDebug.DrawLine(origin, chopperComp.m_vSplinePoints[idx], Color.GRAY);		
		
		//Chopper destination direction vector
		vector vFwd = vector.Direction(origin, chopperComp.m_vDestination);
//		vFwd.Normalize();
//		SDRC_ChopperDebug.DrawLine(origin, origin + (vFwd * 20), Color.WHITE);
//		SDRC_ChopperDebug.DrawLine(origin, origin + vFwd, Color.WHITE);
		SDRC_ChopperDebug.DrawLine(origin, chopperComp.m_vDestination, Color.WHITE);

		//Chopper future destination direction vector
		vFwd = vector.Direction(origin, chopperComp.m_vDestinationFuture);
//		vFwd.Normalize();
//		DrawLine(origin, origin + (vFwd * 50), Color.BLACK);		
		SDRC_ChopperDebug.DrawLine(origin, origin + vFwd, Color.BLACK);		

		//Point on spline below the helicopter
		SDRC_ChopperDebug.DrawLine(origin, chopperComp.m_vSplinePointBelow, Color.RED);		
		
		//Draw vectors
		vector vDir2 = owner.GetTransformAxis(2);	//Forward
//		SDRC_ChopperDebug.DrawLine(origin, origin + (vDir2 * 30), Color.CYAN);		

		vector vDir0 = owner.GetTransformAxis(0);	//Side
//		SDRC_ChopperDebug.DrawLine(origin, origin + (vDir0 * 10), Color.DARK_CYAN);

		vector vDir1 = owner.GetTransformAxis(1);	//Up
		SDRC_ChopperDebug.DrawLine(origin, origin + (vDir1 * 15), Color.MAGENTA);		
		
		vector vUp = vector.Up;						//World Up
		SDRC_ChopperDebug.DrawLine(origin, origin + (vUp * 10), Color.MAGENTA);		
		
		//Roll vector
		vector vRoll = chopperComp.m_vRollTarget * Math.DEG2RAD;
		vRoll.Normalize();
//		SDRC_ChopperDebug.DrawLine(origin, origin + (vRoll * 15), Color.BLUE);		

		//RollPitch vector
		vRoll = chopperComp.m_vRadRollPitch * Math.DEG2RAD;
		vRoll.Normalize();
//		SDRC_ChopperDebug.DrawLine(origin, origin + (vRoll * 15), Color.BLUE);		
		
/*		vector vVec = m_vRadRollVel;
		vVec[1] = -vVec[2];
		vVec[2] = vVec[0];
		vVec[0] = 0;
		vVec.Normalize();
		SDRC_ChopperDebug.DrawLine(origin, origin + (vVec * 45), Color.WHITE);

		vVec = m_vRadRollBack;
		vVec.Normalize();
		SDRC_ChopperDebug.DrawLine(origin, origin + (vVec * 35), Color.WHITE);*/
								
		//Draw velocity vector
		vector vVel = owner.GetPhysics().GetVelocity();
		vVel.Normalize();
//		float currentSpeed = vVel.Length();
		SDRC_ChopperDebug.DrawLine(origin, origin + (vVel * chopperComp.m_fSpeed), Color.GRAY_75);			
		
		//Enemy stuff
		
		//Draw current enemy sighting		
		if (chopperComp.m_vEnemyPosition != "0 0 0")
		{
			SDRC_ChopperDebug.DrawLine(origin, chopperComp.m_vEnemyPosition, Color.PINK);
		}
		
		//Draw eyesight		
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(owner.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		array<IEntity> occupants = {};
		scr_compartmentManager.GetOccupants(occupants);

		foreach (IEntity occupant : occupants)
		{
			SCR_AICombatComponent aicc = SCR_AICombatComponent.Cast(occupant.FindComponent(SCR_AICombatComponent));
			if (aicc)
			{
				BaseTarget bt = aicc.GetCurrentTarget();
				if (bt)
				{
					IEntity target = bt.GetTargetEntity();
//					if (EntityUtils.IsPlayer(target))
//					{
						SDRC_ChopperDebug.DrawLine(occupant.GetOrigin(), target.GetOrigin(), Color.RED);
//					}
				}
			}
		}
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
}