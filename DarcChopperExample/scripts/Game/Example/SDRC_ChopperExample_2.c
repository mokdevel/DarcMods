//Mission SDRC_ChopperExample_2.c

//------------------------------------------------------------------------------------------------
/*!
Summary: spawn - initial destination - fly a while - land troops - fly - fly away

An example of spawning a chopper and set an initial fly path and eventually landing at the airfield. 
Once crew is out, fly back to original position and fly away.
*/

//------------------------------------------------------------------------------------------------
class SDRC_ChopperExample_2
{
	private IEntity m_Vehicle = null;
	private SDRC_ChopperComp m_Vehicle_c;
	vector m_vPosOrigin = "1300 0 2100";
	vector m_vFlyHereFirst = "1600 0 2300";
	float m_fMinFlyHeight = 30;
	//------------------------------------------------------------------------------------------------
	void SDRC_ChopperExample_2()
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
		
		//Disable autostart to set things our selves. Call Ready() after the setup.
//		m_Vehicle_c.SetAutostart(false);
		//Modify some values to our liking
		m_Vehicle_c.SetHeli(10, 30, 				//Min/max speed
		                    m_fMinFlyHeight, 60, 	//Min/max height
		                    200, 800);				//Min/max distance for new destination
		//Only react to players as enemies
		m_Vehicle_c.SetEnemySearchType(SDRC_EHeliEnemySearchType.PLAYER);
		
		//Do setup
//		m_Vehicle_c.Setup(m_Vehicle);
//		m_Vehicle_c.Ready(m_Vehicle);
		
		//Turn vehicle towards first flight position
		SDRC_Math.TurnEntityTowardsXZ(m_Vehicle, m_vFlyHereFirst);
		//Add our flight path
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, m_vFlyHereFirst);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, "1800 0 2300");
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, "1800 0 2600");
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_M_LAND_TROOPS, "1740 0 2930");
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, m_vPosOrigin);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY_AWAY);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_DESPAWN);
	}	
}