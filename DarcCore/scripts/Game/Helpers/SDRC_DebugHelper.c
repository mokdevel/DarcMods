//Helpers SDRC_DebugHelper.c

//------------------------------------------------------------------------------------------------
/*!
The entity that takes care of showing debug markers and other things. Currently works only in WB.
*/

//------------------------------------------------------------------------------------------------
modded enum SCR_DebugMenuID {
    MODMENU,
    MODMENU_WAYPOINTS,
    MODMENU_MARKS,
	MODMENU_SPHERES
}

//------------------------------------------------------------------------------------------------
class SDRC_DebugHelperPos : Managed
{
	vector pos;
	int color;
	float radius;
	float height;
	string id;
}

//------------------------------------------------------------------------------------------------
sealed class SDRC_DebugHelper
{
	static bool m_DebugSlots = true;	//TBD: Slots functionality is very untested. Should be under MODMENU
	
	static ref array<ref SDRC_DebugHelperPos> m_Pos = {};
	static ref array<ref SDRC_DebugHelperPos> m_Sphere = {};
	static ref array<ref SDRC_DebugHelperPos> m_MapCircle = {};
	static ref array<IEntity> m_Slots = {};

	//------------------------------------------------------------------------------------------------
	static void ~SDRC_DebugHelper()
	{
		m_Pos.Clear();
		m_Sphere.Clear();
		m_Slots.Clear();
		m_MapCircle.Clear();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Setup the SDRC_DebugHelper. Can be used to run some default code in start.
	*/
	static void Setup()
	{
		// Some init script		
		const string categoryName = "DarcDebug";
		DiagMenu.RegisterMenu(SCR_DebugMenuID.MODMENU , categoryName, categoryName);
		{
		    DiagMenu.RegisterBool(SCR_DebugMenuID.MODMENU_WAYPOINTS, "", "Show waypoints", categoryName);
		    DiagMenu.RegisterBool(SCR_DebugMenuID.MODMENU_MARKS, "", "Show markers", categoryName);
		    DiagMenu.RegisterBool(SCR_DebugMenuID.MODMENU_SPHERES, "", "Show spheres", categoryName);
		}
	}		

	//------------------------------------------------------------------------------------------------
	/*!
	Configure SDRC_DebugHelper.
	*/
	static void Configure(bool waypoint, bool marks, bool spheres)
	{
		DiagMenu.SetValue(SCR_DebugMenuID.MODMENU_WAYPOINTS, waypoint);
		DiagMenu.SetValue(SCR_DebugMenuID.MODMENU_MARKS, marks);
		DiagMenu.SetValue(SCR_DebugMenuID.MODMENU_SPHERES, marks);
	}		
		
	//------------------------------------------------------------------------------------------------
	/*!
	Call this on EOnFrame
	*/
	static void OnFrame(IEntity owner)
	{	
		if (DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_WAYPOINTS))
		{
			SDRC_DebugHelper.DrawWaypointShapes();
			SDRC_DebugHelper.DrawWaypointLines();
		}
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_MARKS))
		{		
			SDRC_DebugHelper.DrawMarks();
		}
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_SPHERES))
		{		
			SDRC_DebugHelper.DrawSpheres();
		}
		
		if (m_DebugSlots)
		{			
			SDRC_DebugHelper.DrawSlots();	
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Draws lines between the waypoints the AI group has.
	\param 
	*/
	static void DrawWaypointLines()
	{
		const int pLimit = 50;
		array<AIGroup> groups = {};
		array<AIWaypoint> waypoints = {};
		array<AIWaypoint> waypointsCycle = {};	
		int shapeFlags = ShapeFlags.ONCE;
		int color = Color.RED;
		bool isCycle = false;
		
		vector p[pLimit];

		SDRC_AIHelper.GroupFindAll(groups);
		
		foreach (AIGroup group: groups)
		{
			if (group != null)
			{
				int index = 0;
				
				group.GetWaypoints(waypoints);
				
				//If group has no waypoints, check the next group
				if (waypoints.Count() == 0)
				{
					continue;
				}

				//Find if it is a cycle				
				foreach (AIWaypoint wp : waypoints)
				{
					if (AIWaypointCycle.Cast(wp) != null)
					{
						isCycle = true;
						break;
					}
				}
				
				//Put the current group position as the first one
				p[index] = RaiseWaypointPos(group.GetOrigin());
				index++;				
				
				//If it is a cycle, make a line to the next target waypoint
				if (isCycle)
				{
					p[index] = RaiseWaypointPos(waypoints[0]);
					index++;
					Shape.CreateLines(color, shapeFlags, p, index);
					//Reset the index to draw the cycle properly
					index = 0;										
				}
					
				foreach (AIWaypoint wp : waypoints)
				{				
					if (AIWaypointCycle.Cast(wp) == null && !isCycle)	//Filter cycle waypoints away
					{
						vector pos = RaiseWaypointPos(wp);
						p[index] = pos;
						index++;
					}
					
					if (AIWaypointCycle.Cast(wp) != null && isCycle)
					{
						AIWaypointCycle.Cast(wp).GetWaypoints(waypointsCycle);
						foreach (AIWaypoint wpc : waypointsCycle)
						{											
							vector pos = RaiseWaypointPos(wpc);
							p[index] = pos;
							index++;
						}						
					}
					
					if (index >= (pLimit - 1))	//-1 to have a slot for cycle wp
					{
						SDRC_Log.Add("[SDRC_DebugHelper:DrawWaypointLines] There are more than " + pLimit + " waypoints. Ignoring the rest.", LogLevel.WARNING);						
						break;
					}
				}
				
				if (isCycle)
				{
					p[index] = p[0];		//Close the cycle
					index++;	
					color = Color.BLUE;		//Cycle is blue
				}
				
				if (index > 0)
				{
					Shape.CreateLines(color, shapeFlags, p, index);
				}
			}
		}				
	}	
				
	//------------------------------------------------------------------------------------------------
	static void DrawWaypointShapes()
	{
		array<AIGroup> groups = {};		
		array<AIWaypoint> waypoints = {};
		int shapeFlags = ShapeFlags.ONCE;

		SDRC_AIHelper.GroupFindAll(groups);	
		
		if (groups.Count() == 0)
			return;

		foreach (AIGroup group : groups)
		{
			if (group != null)
			{
				group.GetWaypoints(waypoints);
				foreach (AIWaypoint wp : waypoints)
				{
					vector pos = RaiseWaypointPos(wp);
					Shape.CreateSphere(Color.DARK_RED, shapeFlags, pos, 0.3);
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	static void DrawSpheres()
	{
		array<vector> sphere = {};
		int shapeFlags = ShapeFlags.ONCE;

		foreach (SDRC_DebugHelperPos dpos: m_Sphere)
		{
			if (dpos != null)
			{
				Shape.CreateSphere(dpos.color, shapeFlags, dpos.pos, dpos.radius);
			}
		}		
	}	
	
	//------------------------------------------------------------------------------------------------
	static void DrawMarks()
	{
		int shapeFlags = ShapeFlags.TRANSP|ShapeFlags.ONCE|ShapeFlags.ADDITIVE;

		if (m_Pos.Count() == 0)
			return;
		
		foreach (SDRC_DebugHelperPos dpos: m_Pos)
		{
			if (dpos != null)
			{
				Shape.CreateCylinder(dpos.color, shapeFlags, dpos.pos, dpos.radius, dpos.height);
			}
		}			
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Adds cylinder marker on the map for easier finding. Can be either MapItem, IEntity or direct position.
	\param MapItem, IEntity, pos Location of the marker
	\param color Color of the marker
	*/	
	
	static void AddDebugPos(MapItem mapItem, int color = Color.RED, float radius = 1.0, string id = "NONE", int height = 300)
	{
		AddDebugPos(mapItem.GetPos(), color, radius, id, height);
	}

	static void AddDebugPos(IEntity entity, int color = Color.RED, float radius = 1.0, string id = "NONE", int height = 300)
	{
		AddDebugPos(entity.GetOrigin(), color, radius, id, height);
	}
	
	static void AddDebugPos(vector pos, int color = Color.RED, float radius = 1.0, string id = "NONE", int height = 300)
	{
		#ifndef SDRC_RELEASE		
			if (DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_MARKS))
			{
				SDRC_DebugHelperPos dpos = new SDRC_DebugHelperPos;
				pos[1] = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]) + (height/2);
				dpos.pos = pos;
				dpos.color = color;
				dpos.radius = radius;
				dpos.id = id;
				dpos.height = height;
				m_Pos.Insert(dpos);
			}
		#endif
	}				
	
	//------------------------------------------------------------------------------------------------
	/*!
	Adds sphere on the map for debugging. Can be either MapItem, IEntity or direct position.
	\param MapItem, IEntity, pos Location of the marker
	\param color Color of the marker
	*/	
	
	static void AddDebugSphere(MapItem mapItem, int color = Color.BLUE, float radius = 1.0, string id = "NONE")
	{
		AddDebugSphere(mapItem.GetPos(), color, radius, id);
	}

	static void AddDebugSphere(IEntity entity, int color = Color.BLUE, float radius = 1.0, string id = "NONE")
	{
		AddDebugSphere(entity.GetOrigin(), color, radius, id);
	}
	
	static void AddDebugSphere(vector pos, int color = Color.BLUE, float radius = 1.0, string id = "NONE")
	{
		#ifndef SDRC_RELEASE
			if (DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_SPHERES))
			{
				SDRC_DebugHelperPos dpos = new SDRC_DebugHelperPos;
				dpos.pos = pos;
				dpos.color = color;
				dpos.radius = radius;
				dpos.id = id;
				dpos.height = 0;
				m_Sphere.Insert(dpos);
			}
		#endif
	}					
	
	//------------------------------------------------------------------------------------------------
	static void AddMapCircle(vector pos, float radius = 100, int color = Color.BLUE, string id = "NONE")
	{
//		if (DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_SPHERES))
		{
			SDRC_DebugHelperPos dpos = new SDRC_DebugHelperPos;
			dpos.pos = pos;
			dpos.color = color;
			dpos.radius = radius;
			dpos.id = id;
			dpos.height = 0;
			m_MapCircle.Insert(dpos);
		}
	}					

	//------------------------------------------------------------------------------------------------
	/*!
	Deletes a debug cylinder with a certain id. The id works as a wild card.
	*/
	static void DeleteDebugPos(string id)
	{
		for (int i = 0; i < m_Pos.Count(); i++)		
		{
			if (m_Pos[i].id.Contains(id))
			{
				m_Pos.Remove(i);
				i--;
			}			
		}		
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Move a debug cylinder with a certain id to a new position
	*/
	static void MoveDebugPos(string id, vector pos)
	{
		for (int i = 0; i < m_Pos.Count(); i++)		
		{
			if (m_Pos[i].id == id)
			{
				m_Pos[i].pos = pos;
				break;
			}			
		}		
	}
			
	//------------------------------------------------------------------------------------------------
	static void DrawSlots()
	{
		int shapeFlags = ShapeFlags.ONCE;

		if (m_Slots.Count() == 0)
			return;

		foreach (IEntity loc: m_Slots)
		{
			if (loc != null)
			{
				vector pos = RaiseWaypointPos(loc.GetOrigin());
				int color = Color.BLUE;
				Shape.CreateCylinder(color, shapeFlags, pos, 1, 100);
			}
		}
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Takes the waypoint/position and raises it with 12 meters.
	\param waypoint Waypoint or position to raise
	*/
	static vector RaiseWaypointPos(AIWaypoint waypoint)
	{
		vector pos = waypoint.GetOrigin();
		pos[1] = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]) + 12;
		
		return pos;
	}	

	static vector RaiseWaypointPos(vector pos)
	{
		pos[1] = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]) + 12;
		
		return pos;
	}	
}