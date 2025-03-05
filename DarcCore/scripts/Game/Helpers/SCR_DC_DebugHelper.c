//Helpers SCR_DC_DebugHelper.c

modded enum SCR_DebugMenuID {
    MODMENU = 236,		//TBD: This should work without the need to have a value here. 
    MODMENU_WAYPOINTS,
    MODMENU_MARKS
}

//------------------------------------------------------------------------------------------------
/*enum MyModDebugMenu_Enum
{
    MODMENU = SCR_DebugMenuID.DEBUGUI_NEWTUTORIAL_COURSE_MOVE_TO_WP,
    MODMENU_WAYPOINTS,
    MODMENU_MARKS
};*/

//------------------------------------------------------------------------------------------------
class SCR_DC_DebugHelperPos : Managed
{
	vector pos;
	int color;
	float radius;
	float height;
	string id;
}

//------------------------------------------------------------------------------------------------
sealed class SCR_DC_DebugHelper
{
	static bool m_DebugSlots = true;
	
	static ref array<ref SCR_DC_DebugHelperPos> m_Pos = {};
	static ref array<IEntity> m_Slots = {};
			
	//------------------------------------------------------------------------------------------------
	/*!
	Setup the SCR_DC_DebugHelper. Can be used to run some default code in start.
	*/
	static void Setup()
	{
		// Some init script		
		const string categoryName = "DarcDebug";
		DiagMenu.RegisterMenu(SCR_DebugMenuID.MODMENU , categoryName, categoryName);
		{
		    DiagMenu.RegisterBool(SCR_DebugMenuID.MODMENU_WAYPOINTS, "", "Show waypoints", categoryName);
		    DiagMenu.RegisterBool(SCR_DebugMenuID.MODMENU_MARKS, "", "Show markers", categoryName);
		}
	}		

	//------------------------------------------------------------------------------------------------
	/*!
	Configure SCR_DC_DebugHelper.
	*/
	static void Configure(bool waypoint, bool marks)
	{
		DiagMenu.SetValue(SCR_DebugMenuID.MODMENU_WAYPOINTS, waypoint);
		DiagMenu.SetValue(SCR_DebugMenuID.MODMENU_MARKS, marks);
	}		
		
	//------------------------------------------------------------------------------------------------
	/*!
	Call this on EOnFrame
	*/
	static void OnFrame(IEntity owner)
	{	
		if (DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_WAYPOINTS))
		{
			SCR_DC_DebugHelper.DrawWaypointShapes();
			SCR_DC_DebugHelper.DrawWaypointLines();
		}
		
		if (DiagMenu.GetBool(SCR_DebugMenuID.MODMENU_MARKS))
		{		
			SCR_DC_DebugHelper.DrawMarks();
		}
		
		if (m_DebugSlots)
		{			
			SCR_DC_DebugHelper.DrawSlots();	
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Draws lines between the waypoints the AI group has.
	\param 
	*/
	static void DrawWaypointLines()
	{
		array<AIGroup> groups = {};
		array<AIWaypoint> waypoints = {};
		array<AIWaypoint> waypointsCycle = {};	
		int shapeFlags = ShapeFlags.ONCE;
		int color = Color.RED;
		bool isCycle = false;
		
		vector p[50];

		SCR_DC_AIHelper.GroupFindAll(groups);
		
		foreach(AIGroup group: groups)
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
				foreach(AIWaypoint wp : waypoints)
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
					
				foreach(AIWaypoint wp : waypoints)
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
						foreach(AIWaypoint wpc : waypointsCycle)
						{											
							vector pos = RaiseWaypointPos(wpc);
							p[index] = pos;
							index++;
						}						
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

		SCR_DC_AIHelper.GroupFindAll(groups);	
		
		if (groups.Count() == 0)
			return;

		foreach(AIGroup group : groups)
		{
			if (group != null)
			{
				group.GetWaypoints(waypoints);
				foreach(AIWaypoint wp : waypoints)
				{
					vector pos = RaiseWaypointPos(wp);
					Shape.CreateSphere(Color.DARK_RED, shapeFlags, pos, 0.3);
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	static void DrawMarks()
	{
		int shapeFlags = ShapeFlags.TRANSP|ShapeFlags.ONCE|ShapeFlags.ADDITIVE;

		if (m_Pos.Count() == 0)
			return;
		
		foreach(SCR_DC_DebugHelperPos dpos: m_Pos)
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
/*		SCR_DC_DebugHelperPos dpos = new SCR_DC_DebugHelperPos;
		dpos.pos = mapItem.GetPos();
		dpos.color = color;
		dpos.radius = radius;
		dpos.id = id;
		dpos.height = height;
		m_Pos.Insert(dpos);	*/
	}

	static void AddDebugPos(IEntity entity, int color = Color.RED, float radius = 1.0, string id = "NONE", int height = 300)
	{
		AddDebugPos(entity.GetOrigin(), color, radius, id, height);
/*		SCR_DC_DebugHelperPos dpos = new SCR_DC_DebugHelperPos;
		dpos.pos = entity.GetOrigin();
		dpos.color = color;
		dpos.radius = radius;
		dpos.id = id;
		dpos.height = height;
		m_Pos.Insert(dpos);	*/
	}
	
	static void AddDebugPos(vector pos, int color = Color.RED, float radius = 1.0, string id = "NONE", int height = 300)
	{
		SCR_DC_DebugHelperPos dpos = new SCR_DC_DebugHelperPos;
		pos[1] = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]) + (height/2);
		dpos.pos = pos;
		dpos.color = color;
		dpos.radius = radius;
		dpos.id = id;
		dpos.height = height;
		m_Pos.Insert(dpos);	
	}				
	
	//------------------------------------------------------------------------------------------------
	/*!
	Deletes a debug cylinder with a certain id. The id works as a wild card.
	*/
	static void DeleteDebugPos(string id)
	{
		for (int i = 0; i < m_Pos.Count(); i++)		
		{
			if(m_Pos[i].id.Contains(id))
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
			if(m_Pos[i].id == id)
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

		foreach(IEntity loc: m_Slots)
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