//Helpers SDRC_CampHelper

//------------------------------------------------------------------------------------------------
/*!
Helper functions for spawning camp class 
*/

//------------------------------------------------------------------------------------------------
class SDRC_Camp : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();		
	
	//Optional settings
	ref SDRC_Loot loot = null;
	ref array<ref SDRC_Structure> campItems = {};	
}

sealed class SDRC_CampHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn camp items, AI and loot defined in a SDRC_camp structure
	\param mission Mission that is calling this
	\param idx Index of the item to spawn. This needs to be increased each run.
	\param camp The SDRC_camp structure to use
	\param rotation The rotation of the items around Y axis
	\param disableArsenal Defines if vehicle arsenals are to be disabled
	\return true/false if the mission spawning is ready and mission can continue to next stage 
	*/
	static bool Spawn(SDRC_Mission mission, int idx, SDRC_Camp camp, float rotation = 0, bool disableArsenal = true)
	{
		IEntity entity;
		
		//Spawn entities one by one
		if ( (idx < camp.campItems.Count()) && (camp.campItems.Count() > 0) )
		{			
			entity = SDRC_SpawnHelper.SpawnStructures(camp.campItems, mission.GetPos(), rotation, idx);
			
			if (entity != NULL)
			{ 
				mission.AddToEntityList(entity);
				//Disable arsenal
				string resourceName = camp.campItems[idx].GetResource();
				SDRC_VehicleHelper.DisableVehicleArsenal(entity, resourceName, disableArsenal);				
			}
			else
			{
				SDRC_Log.Add("[SDRC_campHelper:Spawn] Could not load: " + camp.campItems[idx], LogLevel.ERROR);
			}
			
			return false;
		}
		else
		{
			//Spawn mission AI 
			int aiCount = camp.ai.GetCount(camp.general.difficulty);
			
			for (int i = 0; i < aiCount; i++)
			{
				//SCR_AIGroup group = SDRC_MissionHelper.SpawnMissionAIGroup(camp.ai.types.GetRandomElement(), mission.GetPos(), mission.GetFaction());
				SCR_AIGroup group = SDRC_MissionHelper.SpawnMissionAIGrouRandom(camp.ai.types, mission.GetPos(), mission.GetFaction());
				if (group)
				{
					SDRC_AIHelper.SetAIGroupSkill(group, camp.ai.GetSkill(camp.general.difficulty), camp.ai.GetPerception(camp.general.difficulty));					
					mission.AddToGroupsList(group);
					
					int minRange = camp.ai.waypointRange[0];
					int maxRange = camp.ai.waypointRange[1];
					
					//If there are more than one group and loot, spawn one to protect the loot. 
					//For the first group, waypointRange is ignored.
					if ((camp.loot) && i == 0)
					{
						minRange = 5;
						maxRange = 30;					
					}
					
					SDRC_WPHelper.CreateMissionAIWaypoints(group, camp.ai.waypointGenType, mission.GetPos(), "0 0 0", camp.ai.waypointMoveType, minRange, maxRange);
//					SDRC_WPHelper.CreateMissionAIWaypoints(group, SDRC_EWaypointGenerationType.LOITER, GetPos(), "0 0 0", SDRC_EWaypointMoveType.LOITER, camp.waypointRange[0], camp.waypointRange[1]);
				}
				SDRC_Log.Add("[SDRC_Mission_Patrol:MissionSpawn] AI groups spawned: " + mission.GetGroupsCount() + " (tried: " + aiCount + ")", LogLevel.DEBUG);
			}
			
			//Put the loot box in right place
			if ( (camp.loot) && (camp.loot.box == null) )
			{
				camp.loot.box = mission.GetFromEntityList(0);
			}
			
/*			//Put loot
			if (camp.loot)			
			{
				camp.loot.box = mission.GetFromEntityList(0);
				SDRC_LootHelper.SpawnItemsToStorage(camp.loot.box, camp.loot.items, camp.loot.itemChance);
				SDRC_Log.Add("[SDRC_campHelper:Spawn] Loot added.", LogLevel.DEBUG);								
			}*/

			return true;			
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn loot items. This is usually called at the end of the mission.
	*/
	static bool AddLoot(SDRC_Camp camp)
	{
		//NOTE: Error checking for missing loot or box is done in SpawnItemsToStorage()
		SDRC_LootHelper.SpawnItemsToStorage(camp.loot.box, camp.loot.items, camp.loot.itemChance);
		SDRC_Log.Add("[SDRC_campHelper:AddLoot] Loot added.", LogLevel.DEBUG);								
		
		return true;
	}
}