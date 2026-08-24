//Mission SDRC_ChopperExample_6.c

//------------------------------------------------------------------------------------------------
/*!
Summary: spawn - initial destination - fly zig-zag

This is a very simple example of spawning a chopper and set an initial destination and additional 
points to create a zig-azg flight.
*/

//------------------------------------------------------------------------------------------------
class SDRC_ChopperExample_6
{
	private IEntity m_Vehicle = null;
	private SDRC_ChopperComp m_Vehicle_c;
	vector m_vPosOrigin = "2443 0 1488";
	vector m_vFlyHereFirst = "2443 0 1638";
	float m_fMinFlyHeight = 30;
	//------------------------------------------------------------------------------------------------
	void SDRC_ChopperExample_6()
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
			Print("[SDRC_ChopperExample_1] Could not spawn chopper", LogLevel.ERROR);
			return;			
		}

		//Turn vehicle towards first flight position
		SDRC_Math.TurnEntityTowardsXZ(m_Vehicle, m_vFlyHereFirst);
		//Add our flight path
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, m_vFlyHereFirst);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, "2573 0 1703");
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, "2415 0 1798");
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, "2607 0 1900");
	}	
}