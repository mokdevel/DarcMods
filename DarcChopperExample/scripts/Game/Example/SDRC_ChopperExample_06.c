//Mission SDRC_ChopperExample_06.c

//------------------------------------------------------------------------------------------------
/*!
Summary: spawn - initial destination - fly zig-zag

This is a very simple example of spawning a chopper and set an initial destination and additional 
points to create a zig-azg flight.
*/

//------------------------------------------------------------------------------------------------
class SDRC_ChopperExample_06
{
	private IEntity m_Vehicle = null;
	private SDRC_ChopperComp m_Vehicle_c;
	vector m_vPosOrigin = "2400 0 600";
	float m_fMinFlyHeight = 30;
	//------------------------------------------------------------------------------------------------
	void SDRC_ChopperExample_06()
	{
		//Set to initial position height
		if (m_vPosOrigin[1] == 0)
		{
			m_vPosOrigin[1] = m_fMinFlyHeight + SDRC_Misc.GetSurfaceYWithWater(m_vPosOrigin, true);
		}
		
		//Select chopper and spawn
		string resourceName	= "{5678893357C6FC10}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HE_Patrol.et";
		m_Vehicle = SDRC_SpawnHelper.SpawnItem(m_vPosOrigin, resourceName, 0, -1, false);
		
		//Find the chopper component
		if (m_Vehicle)
		{
			m_Vehicle_c = SDRC_ChopperComp.Cast(m_Vehicle.FindComponent(SDRC_ChopperComp));
		}
		
		//Check that all is good
		if ( (!m_Vehicle) || (!m_Vehicle_c) )
		{
			SDRC_Log.Add("[SDRC_ChopperExample_06] Could not spawn chopper", LogLevel.ERROR);
			return;			
		}

		//Do setup
		vector flyHereFirst = m_vPosOrigin + "100 0 0";
		
		//Turn vehicle towards first flight position
		SDRC_Math.TurnEntityTowardsXZ(m_Vehicle, flyHereFirst);
		//Add our flight path
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, flyHereFirst + "200 0 200");
		SDRC_DebugHelper.AddDebugPos(flyHereFirst + "200 0 200", ARGB(255, 255, 0, 192), 2.0);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, flyHereFirst + "-200 0 400");
		SDRC_DebugHelper.AddDebugPos(flyHereFirst + "-200 0 400", ARGB(255, 255, 0, 128), 2.0);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, flyHereFirst + "200 0 600");
		SDRC_DebugHelper.AddDebugPos(flyHereFirst + "200 0 600", ARGB(255, 255, 0, 64), 2.0);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, flyHereFirst + "-200 0 800");
		SDRC_DebugHelper.AddDebugPos(flyHereFirst + "-200 0 800", ARGB(255, 255, 0, 64), 2.0);
	}	
}