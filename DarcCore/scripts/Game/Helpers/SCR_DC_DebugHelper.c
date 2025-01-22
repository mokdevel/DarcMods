//Helpers SCR_DC_DebugHelper.c

//------------------------------------------------------------------------------------------------
enum MyModDebugMenu_Enum
{
    ModMenu = SCR_DebugMenuID.DEBUGUI_NEWTUTORIAL_COURSE_MOVE_TO_WP,
    ModMenu_WAYPOINTS,
    ModMenu_MARKS
};

//------------------------------------------------------------------------------------------------
class SCR_DC_DebugHelperPos : Managed
{
	vector pos;
	int color;
	float radius;
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
		DiagMenu.RegisterMenu(MyModDebugMenu_Enum.ModMenu , categoryName, categoryName);
		{
		    DiagMenu.RegisterBool(MyModDebugMenu_Enum.ModMenu_WAYPOINTS, "", "Show waypoints", categoryName);
		    DiagMenu.RegisterBool(MyModDebugMenu_Enum.ModMenu_MARKS, "", "Show markers", categoryName);
		}
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!
	Call this on EOnFrame
	*/
	static void OnFrame(IEntity owner)
	{	
		if (DiagMenu.GetBool(MyModDebugMenu_Enum.ModMenu_WAYPOINTS))
		{
			SCR_DC_DebugHelper.DrawWaypointShapes();
			SCR_DC_DebugHelper.DrawWaypointLines();
		}
		
		if (DiagMenu.GetBool(MyModDebugMenu_Enum.ModMenu_MARKS))
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
//					if (AIWaypointCycle.Cast(wp) == null && wpType == DC_WaypointLineType.NORMAL)	//Filter cycle waypoints away
					if (AIWaypointCycle.Cast(wp) == null && !isCycle)	//Filter cycle waypoints away
					{
						vector pos = RaiseWaypointPos(wp);
						p[index] = pos;
						index++;
					}
					
//					if (AIWaypointCycle.Cast(wp) != null && wpType == DC_WaypointLineType.CYCLE)
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
				vector pos = RaiseWaypointPos(dpos.pos);
				Shape.CreateCylinder(dpos.color, shapeFlags, pos, dpos.radius, 300);
			}
		}			
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Adds cylinder marker on the map for easier finding. Can be either MapItem, IEntity or direct position.
	\param MapItem, IEntity, pos Location of the marker
	\param color Color of the marker
	*/	
	
	static void AddDebugPos(MapItem mapItem, int color = Color.RED, float radius = 1.0)
	{
		SCR_DC_DebugHelperPos dpos = new SCR_DC_DebugHelperPos;
		dpos.pos = mapItem.GetPos();
		dpos.color = color;
		dpos.radius = radius;
		m_Pos.Insert(dpos);	
	}

	static void AddDebugPos(IEntity entity, int color = Color.RED, float radius = 1.0)
	{
		SCR_DC_DebugHelperPos dpos = new SCR_DC_DebugHelperPos;
		dpos.pos = entity.GetOrigin();
		dpos.color = color;
		dpos.radius = radius;
		m_Pos.Insert(dpos);	
	}
	
	static void AddDebugPos(vector pos, int color = Color.RED, float radius = 1.0)
	{
		SCR_DC_DebugHelperPos dpos = new SCR_DC_DebugHelperPos;
		dpos.pos = pos;
		dpos.color = color;
		dpos.radius = radius;
		m_Pos.Insert(dpos);	
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
	Takes the waypoint/position and raises it with 2 meters.
	\param waypoint Waypoint or position to raise
	*/
	static vector RaiseWaypointPos(AIWaypoint waypoint)
	{
		vector pos = waypoint.GetOrigin();
		pos[1] = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]) + 2;
		
		return pos;
	}	

	static vector RaiseWaypointPos(vector pos)
	{
		pos[1] = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]) + 2;
		
		return pos;
	}
	
/*	
ComponentType type = CompnentType.Cast(object.FindCoponent(ComponentType))	
array<AIAgent> agents = new array<AIAgent>;
            group.GetAgents(agents);
	
GetGame().GetWorld().QueryEntitiesBySphere(w.FindEntityByName("Trigger_" + loc).GetOrigin(), 170, processAI, filterAI);
        }
    }
    
    bool filterAI(IEntity ent)
    {
        SCR_AIGroup aig = SCR_AIGroup.Cast(ent);
        // Runs 13000 times, so it does find entities
        if (!aig) return false;
        // Runs 0 times, despite there being AI groups present that FindEntityByName() can manually find and successfully cast to SCR_AIGroup
        return true;	
	
*/	
	
}