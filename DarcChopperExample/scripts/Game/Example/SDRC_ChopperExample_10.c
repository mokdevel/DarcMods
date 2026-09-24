//Mission SDRC_ChopperExample_10.c

//------------------------------------------------------------------------------------------------
/*!
Summary: spawn - do setup - initial destination - fly - attack

An example of spawning a chopper with autostart disabled. We set the flight paths and do spawning 
of AI before finalizing and preparing to fly. The target is a BTR to destroy.
*/

//------------------------------------------------------------------------------------------------
class SDRC_ChopperExample_10
{
	private IEntity m_Vehicle = null;
	private SDRC_ChopperComp m_Vehicle_c;
	vector m_vPosOrigin = "3100 0 1580";
	float m_fMinFlyHeight = 50;
	//------------------------------------------------------------------------------------------------
	void SDRC_ChopperExample_10()
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
			SDRC_Log.Add("[SDRC_ChopperExample_10] Could not spawn chopper", LogLevel.ERROR);
			return;			
		}
		
		//Disable autostart to set things our selves. Call Ready() after the setup.
		m_Vehicle_c.SetAutostart(false);
		//Modify some values to our liking
		m_Vehicle_c.SetHeli(10, 40, 				//Min/max speed
		                    m_fMinFlyHeight, 80, 	//Min/max height
		                    200, 800);				//Min/max distance for new destination
		m_Vehicle_c.SetEnemySearchType(SDRC_EHeliEnemySearchType.ANY_CHAR);
		m_Vehicle_c.Setup(m_Vehicle);
		
		//Setup the flight path
		vector flyHereFirst = "3000 0 2000";
		//Turn vehicle towards first flight position
		SDRC_Math.TurnEntityTowardsXZ(m_Vehicle, flyHereFirst);
		//Add our flight path
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, flyHereFirst);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_ATTACK, "3170 0 2670");
		//Let the chopper setup everything properly and then do the final parts. Finalization happens after one second.
		GetGame().GetCallqueue().CallLater(FinalizeSetup, 1000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	void FinalizeSetup()
	{
		//Spawn crew
		vector spawnPos = m_vPosOrigin + "3 0 3";
		SCR_AIGroup group = SDRC_AIHelper.GroupCreate("USSR", spawnPos);
		//Spawn a single AI in
		string aiPrefab = "{A62FA97C4EC64F14}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_HeliPilot.et";
		//Spawn a group of AI
		//string aiPrefab = "{657590C1EC9E27D3}Prefabs/Groups/OPFOR/Group_USSR_LightFireTeam.et";
		SDRC_VehicleHelper.SpawnGroupInVehicle(aiPrefab, m_Vehicle, group, "USSR");
		
		//---
		//Here you can add loot or do some other things that is needed
		//---
		
		//Initialize the flight and prepare a flight path
		m_Vehicle_c.InitFlight(m_Vehicle);
		//All done. 
		m_Vehicle_c.Ready(m_Vehicle);
	}
}