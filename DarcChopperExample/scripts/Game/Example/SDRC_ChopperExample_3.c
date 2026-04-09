//Mission SDRC_ChopperExample_3.c

//------------------------------------------------------------------------------------------------
/*!
An example of spawning a chopper and set an initial fly path and eventually landing at the airfield. 
Once crew is out, fly back to original position and fly away.
*/

//------------------------------------------------------------------------------------------------
class SDRC_ChopperExample_3
{
	private IEntity m_Vehicle = null;
	private SDRC_ChopperComp m_Vehicle_c;
	//private VehicleHelicopterSimulation m_Vehicle_s;
	vector m_vPosOrigin = "1300 0 2100";
	vector m_vFlyHereFirst = "1600 0 2300";
	float m_fMinFlyHeight = 30;
	//------------------------------------------------------------------------------------------------
	void SDRC_ChopperExample_3()
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
		
		//m_Vehicle_c.SetAutostart(false);
		//Modify some values to our liking
		m_Vehicle_c.SetHeli(10, 30, 		//Min/max speed
		                    30, 60, 		//Min/max height
		                    200, 800);		//Min/max distance for new destination
		//Only react to players as enemies
		m_Vehicle_c.SetEnemySearchType(SDRC_EHeliEnemySearchType.PLAYER);
		
		//Do setup
		m_Vehicle_c.Setup(m_Vehicle);

		//Add our flight path
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, m_vFlyHereFirst);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, "1800 0 2300");
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, "1800 0 2600");
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_M_LAND_TROOPS, "1740 0 2930");
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY, m_vPosOrigin);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY_AWAY);
		m_Vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_DESPAWN);
	}	

	//------------------------------------------------------------------------------------------------
/*	private void MissionSpawnCrew()
	{
		//Select pilots
		ResourceName pilot = SDRC_EnemyHelper.SelectEnemy("C_CREW", GetFaction());

		if (pilot == "")
		{
			//If pilots not available, let's spawn a regular rifleman as a pilot
			pilot = SDRC_EnemyHelper.SelectEnemy("C_RIFLEMAN", GetFaction());
		}		

		//Spawn pilots if such is available 
		if (pilot != "")
		{
			SCR_AIGroup group = SDRC_AIHelper.GroupCreate(GetFaction(), GetPos());
			
			for (int i = 0; i < 2; i++)
			{		
				SDRC_VehicleHelper.SpawnGroupInVehicle(pilot, m_Vehicle, group, GetFaction());
				
				if (group)
				{			
					SDRC_AIHelper.SetAIGroupSettings(group, m_DC_Chopper.ai.GetSkill(GetDifficulty()), m_DC_Chopper.ai.GetPerception(GetDifficulty()));
					m_Groups.Insert(group);					
				}
			}
		}
		
		//Spawn mission AI
		int aiCount = m_DC_Chopper.ai.GetCount(GetDifficulty());
		
		for (int i = 0; i < aiCount; i++)
		{		
			string groupToSpawn = m_DC_Chopper.ai.types.GetRandomElement();
			ResourceName aiType = SDRC_EnemyHelper.SelectEnemy(groupToSpawn, GetFaction());
			
			SCR_AIGroup group = SDRC_AIHelper.GroupCreate(GetFaction(), GetPos());
			SDRC_VehicleHelper.SpawnGroupInVehicle(aiType, m_Vehicle, group, GetFaction());
			
			if (group)
			{			
				SDRC_AIHelper.SetAIGroupSettings(group, m_DC_Chopper.ai.GetSkill(GetDifficulty()), m_DC_Chopper.ai.GetPerception(GetDifficulty()));
				m_Groups.Insert(group);					
			}
		}		
		
		//All done, activate
		m_Vehicle_c.Ready(m_Vehicle);
	}	*/
}