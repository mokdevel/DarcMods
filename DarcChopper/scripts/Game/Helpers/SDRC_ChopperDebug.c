//SDRC_ChopperDebug.c

//------------------------------------------------------------------------------------------------
class SDRC_ChopperDebug
{
	//Line drawing related
	static BaseWorld m_World;
	static WorkspaceWidget m_Workspace;
	
	//------------------------------------------------------------------------------------------------
	/*!
	Collect desination lines to show where chopper is going
	*/
	static void CollectDestinationLines(IEntity owner, out array<vector> vertices)
	{
		if (!owner)
		{
			return;
		}
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}		
		
		if (chopperComp.m_vSplinePoints.IsEmpty())
		{
			return;
		}

		if (chopperComp.GetState() == SDRC_EHeliState.DESTROYED)
		{
			return;
		}
				
		//Add every nth spline point
		int nth = (chopperComp.m_vSplinePoints.Count() - chopperComp.m_iClosestIndex) / 8;
		nth = Math.ClampInt(nth, 4, 15);
		
		int closestIndex = ((int)(chopperComp.m_iClosestIndex / nth)) * nth + 1;
		int splinePointCount = chopperComp.m_vSplinePoints.Count() - 1;
		if (closestIndex > splinePointCount)
		{
			closestIndex = splinePointCount;
		}

		// Add spline points
		
		//Select color
		vertices.Insert(SetColor(Color.DARK_GREEN));
		
		//Starting point
		//vertices.Insert(chopperComp.m_vSplinePoints[closestIndex]);
		
		for (int i = closestIndex; i < splinePointCount; i = i + nth)
		{
			vertices.Insert(chopperComp.m_vSplinePoints[i]);
		} 
		
		//Last point
		vertices.Insert(chopperComp.m_vSplinePoints[chopperComp.m_vSplinePoints.Count() - 1]);

		const int NONE_COLOR = -2;	//NOTE: -1 is Color.WHITE so we another value
		
		//Add destinations if any
		if (!chopperComp.m_vFlyDestinations.IsEmpty())
		{	
			int currentColor = NONE_COLOR;
			
			foreach (SDRC_FlyPathPoint destination : chopperComp.m_vFlyDestinations)
			{
				int color = NONE_COLOR;	

				vector pos = destination.pt;
				pos[1] = SDRC_Misc.GetSurfaceYWithWater(pos) + 20;
				
				switch (destination.type)
				{
					case SDRC_EFlyWayPointType.WP_FLY:					
					{
						color = Color.DARK_BLUE;
						break;
					}
					case SDRC_EFlyWayPointType.WP_LAND:
					case SDRC_EFlyWayPointType.WP_LAND_VERTICAL:
					{
						color = Color.DARK_CYAN;
						break;
					}				
					case SDRC_EFlyWayPointType.WP_PATROL:
					{
						color = Color.GRAY;
						break;
					}
					case SDRC_EFlyWayPointType.WP_CRASH:
					case SDRC_EFlyWayPointType.WP_ATTACK:
					{
						color = Color.RED;
						break;
					}
					case SDRC_EFlyWayPointType.WP_SEARCH_DESTROY:
					{
						color = Color.WHITE;
						break;
					}
					case SDRC_EFlyWayPointType.WP_FLY_AWAY:
					{
						color = Color.PINK;
						break;
					}
				}
				
				//If no color change, skip. 
				if (color == -2)
				{
					//This should never happen
					continue;
				}
				
				//If needed, change the color
				if (color != currentColor)
				{
					currentColor = color;
					vertices.Insert(SetColor(currentColor));					
				}
				
				vertices.Insert(pos);
			}		
		}		
		
		//Add an end point
		vector pos = vector.Zero;
		pos[0] = SDRC_ELineDrawCommand.END;
		vertices.Insert(pos);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Create a vector that represents a color change. Format "-1 color 0". -1 is the color change code.
	*/
	static vector SetColor(int color)
	{
		vector vcol = "-1 0 0";
		vcol[1] = color;
		return vcol;
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
		if (!owner)
		{
			return;
		}
//		if (!m_bShowDebug)
		if (!SDRC_Conf.SHOW_DEBUG)
		{
			return;
		}
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}
		
		array<vector> flyPathPoints = {};
		GivePoints(flyPathPoints, chopperComp.m_vFlightPoints);
		
		SDRC_DebugHelper.DeleteDebugItems(chopperComp.m_sDid, marks: false);
		SDRC_DebugHelper.DrawPointList(chopperComp.m_vSplinePoints, chopperComp.m_sDid);		
		SDRC_DebugHelper.DrawPointList(flyPathPoints, chopperComp.m_sDid, ARGB(10, 64, 64, 192), 1.0);
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
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}
		
		if (chopperComp.GetState() == SDRC_EHeliState.DESTROYED)
		{
			return;
		}		
		
		vector origin = owner.GetOrigin();

		vector heliUp = owner.GetTransformAxis(1);
		float angUp = SDRC_Math.GetAngleBetweenVectors(heliUp, vector.Up);
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_INFO))
		{		
			string debugText = "";
			//Show time in state
			debugText = debugText +	SCR_Enum.GetEnumName(SDRC_EHeliState, chopperComp.GetState());
			if (chopperComp.m_fTimeInStateLeft > 0)
			{
				debugText = debugText +	" (" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fTimeInStateLeft) + ")";
			}

			if (chopperComp.m_fTimerBehaviour > 0)
			{
				debugText = debugText +	" " + SCR_Enum.GetEnumName(SDRC_EHeliBehaviour, chopperComp.GetBehaviour()) + " (" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fTimerBehaviour) + ")";
			}
			
			//Show if enemy is known			
			if (chopperComp.m_vEnemyPosition != vector.Zero)
			{
				debugText = debugText + " (enemy)";
			}
			
			float health;
			chopperComp.TypeGetHealthScaled(owner, health);
			
			string textAltAgl = "";
			
			VehicleHelicopterSimulation helicopter_s = VehicleHelicopterSimulation.Cast(owner.GetRootParent().FindComponent(VehicleHelicopterSimulation));
			if (helicopter_s)
			{
				textAltAgl = " AGL:" + SDRC_Misc.FloatWithDecimals(helicopter_s.GetAltitudeAGL());
			}
			
			debugText = debugText + " Hlth: " + SCR_Enum.GetEnumName(SDRC_EHeliDamageLevel, chopperComp.m_eDamageLevel) + " (" + SDRC_Misc.FloatWithDecimals(health, 2) + ")";
			debugText = debugText + "\n";
			
			debugText = debugText +
							   	"Spd:" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fSpeed) + " " +
							   	"(" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fSpeedMin) + "-" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fSpeedMax) + ")" + 
							   	"/start:" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fSpeedStart) + "/target:" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fSpeedTarget) +
							   	"/mul:" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fSpeedMul, 2) + 
	//						   	"Avg time:" + m_fTimeBetweenPtsAvg + "\n" +
								" \n";
			debugText = debugText + 
//							   	"Alt:" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fAltitude) + " " + textAltAgl + "\n" + 
							   	"RotorForceMul:" + SDRC_Misc.FloatWithDecimals(chopperComp.m_fRotorForceMultiplier, 2) + "\n" +
//						   		"SplinePoints:" + m_vSplinePoints.Count() + "\n" +
//						   		"TurnInternal:" + m_fTimeTurnInterval + "\n" +
//								"Angle: " + m_fDbgAngle * Math.RAD2DEG + "\n" +
								"AnglePitch: " + SDRC_Misc.FloatWithDecimals(chopperComp.m_fAnglePitch) + "\n" +
//								"AngleRoll: " + m_fAngleRoll * Math.RAD2DEG + "\n" +
//								"AngleRollBack: " + m_fAngleRollBack * Math.RAD2DEG + "\n" +
//								"DestinationPointAdd: " + m_iDestinationPointAdd + "\n" 
								"";
			debugText = debugText + 
//								"Init:" + chopperComp.m_bInInit + ", " +
//								"Pilots: " + SDRC_VehicleHelper.PilotCountAlive(owner) + "\n" +
								"Groups: " + chopperComp.m_aGroups.Count() + "\n" +
//								"Working: " + SDRC_VehicleHelper.IsWorking(owner) + " - " + 
//								"Is piloted: " + SDRC_VehicleHelper.IsPiloted(owner) + "\n" +
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
		}
			
		//Planned destination
		if (idx > -1)	//If no spline points, don't draw this line
		{
			SDRC_ChopperDebug.DrawLine(origin, chopperComp.m_vSplinePoints[idx], Color.GRAY);		
		}
		
		//Chopper destination direction vector
		SDRC_ChopperDebug.DrawLine(origin, chopperComp.m_vDestination, Color.WHITE);

		//Chopper future destination direction vector
		vector vFwd = vector.Direction(origin, chopperComp.m_vDestinationFuture);
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
		
		//Draw raycast stuff
		float rayLen = chopperComp.params.rayLenFront;		
		vector rayEnd = SDRC_ChopperHelper.GetDestinationForward(owner, chopperComp.params.rayLenFront);
		rayEnd[1] = rayEnd[1] - chopperComp.params.rayDown;
		
		float len = SDRC_Misc.RayCast(origin, rayEnd, owner);
		int color = Color.GREEN;
		if (len < 1)
		{
			color = Color.RED;
		}
		SDRC_ChopperDebug.DrawLine(origin, rayEnd, color);		
		
		//Enemy stuff
		
		//Draw current enemy sighting		
		if (chopperComp.m_vEnemyPosition != "0 0 0")
		{
			SDRC_ChopperDebug.DrawLine(origin, chopperComp.m_vEnemyPosition, Color.PINK);
		}
		
		//Draw eyesight		
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(owner.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		if (scr_compartmentManager)
		{		
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
						if (target)
						{
							SDRC_ChopperDebug.DrawLine(occupant.GetOrigin(), target.GetOrigin(), Color.RED);
						}
					}
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