//Mission SDRC_ChopperExample_04.c

//------------------------------------------------------------------------------------------------
/*!
Summary: spawn - initial destination - fly - brake - hover up/down - land - stop engine

An example of spawning a chopper and set an initial fly path, do some air tricks and eventually land
and turning engines off. 
*/

//------------------------------------------------------------------------------------------------
class SDRC_ChopperExample_04
{
	private IEntity m_Vehicle = null;
	private SDRC_ChopperComp m_Vehicle_c;
	vector m_vPosOrigin = "1730 0 2530";
	float m_fMinFlyHeight = 30;
	//------------------------------------------------------------------------------------------------
	void SDRC_ChopperExample_04()
	{
		//Set to initial position height
		if (m_vPosOrigin[1] == 0)
		{
			m_vPosOrigin[1] = m_fMinFlyHeight + SDRC_Misc.GetSurfaceYWithWater(m_vPosOrigin, true);
		}
		
		//Select chopper and spawn
		string resourceName	= "{5678893357C6FC10}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HE_Patrol.et";		
//		string resourceName	= "{446634BB04ED3705}Prefabs/Vehicles/Helicopters/UH1H/SP02_GUNSHIP_Patrol.et";		
		m_Vehicle = SDRC_SpawnHelper.SpawnItem(m_vPosOrigin, resourceName, 0, -1, false);
		
		//Find the chopper component
		if (m_Vehicle)
		{
			m_Vehicle_c = SDRC_ChopperComp.Cast(m_Vehicle.FindComponent(SDRC_ChopperComp));
		}
		
		//Check that all is good
		if ( (!m_Vehicle) || (!m_Vehicle_c) )
		{
			SDRC_Log.Add("[SDRC_ChopperExample_04] Could not spawn chopper", LogLevel.ERROR);
			return;			
		}
		
		//Disable autostart to set things our selves. Call Ready() after the setup.
		//Modify some values to our liking
		m_Vehicle_c.SetHeli(10, 30, 				//Min/max speed
		                    m_fMinFlyHeight, 60, 	//Min/max height
		                    200, 800);				//Min/max distance for new destination
		//Only react to players as enemies
		m_Vehicle_c.SetEnemySearchType(SDRC_EHeliEnemySearchType.PLAYER);
		
		//Do setup
		vector flyHereFirst = "1470 0 2360";		//Lands with a long slow descent as the distance to brake position is long
//		vector flyHereFirst = "1400 0 2330";		//Lands with a steeper curve as the distance to brake position is short
		
		//Turn vehicle towards first flight position
		SDRC_Math.TurnEntityTowardsXZ(m_Vehicle, flyHereFirst);
		//Add our flight path		
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, flyHereFirst);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_BRAKE, "1350 3 2287");
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_HOVER, value: 8);				//Hover for 8 seconds
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_HOVER_UP, "0 30 0", 8);			//Raise up 30 meters. Stay in the state or 8 seconds.
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_HOVER_DOWN, "0 -20 0", 8);		//Hover down 20 meters. Stay in the state or 8 seconds.
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_LAND_VERTICAL);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_WAIT, value: 25);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_STOP_ENGINE);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_END);
	}	
}