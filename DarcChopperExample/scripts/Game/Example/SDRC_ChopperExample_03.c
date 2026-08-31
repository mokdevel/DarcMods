//Mission SDRC_ChopperExample_03.c

//------------------------------------------------------------------------------------------------
/*!
Summary: spawn - initial destination - land troops - stop engine

An example of spawning a chopper and set an initial fly path and eventually landing at the airfield. 
Once crew is out, fly back to original position and fly away.
*/

//------------------------------------------------------------------------------------------------
class SDRC_ChopperExample_03
{
	private IEntity m_Vehicle = null;
	private SDRC_ChopperComp m_Vehicle_c;
	vector m_vPosOrigin = "1300 0 3000";
	float m_fMinFlyHeight = 30;
	//------------------------------------------------------------------------------------------------
	void SDRC_ChopperExample_03()
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
			SDRC_Log.Add("[SDRC_ChopperExample_03] Could not spawn chopper", LogLevel.ERROR);
			return;			
		}
		
		//Modify some values to our liking
		m_Vehicle_c.SetHeli(10, 30, 		//Min/max speed
		                    m_fMinFlyHeight, 60, 		//Min/max height
		                    200, 800);		//Min/max distance for new destination
		//Only react to players as enemies
		m_Vehicle_c.SetEnemySearchType(SDRC_EHeliEnemySearchType.PLAYER);

		//Do setup
		vector flyHereFirst = "1100 0 2900";		
		
		//Turn vehicle towards first flight position
		SDRC_Math.TurnEntityTowardsXZ(m_Vehicle, flyHereFirst);
		//Add our flight path
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, flyHereFirst);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_M_LAND, "908 0 2820");		
/*			
			//Optionally you can set the braking and vertical landing that is done by WP_M_LAND
			m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_BRAKE, "908 3 2820");
			m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_LAND_VERTICAL);
*/		
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_GET_OUT);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_WAIT_GETOUT);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_WAIT, value: 5);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_STOP_ENGINE);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_END);
	}	
}