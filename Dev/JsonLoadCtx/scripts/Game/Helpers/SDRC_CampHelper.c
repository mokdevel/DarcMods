//Helpers SDRC_CampHelper

//------------------------------------------------------------------------------------------------
/*!
Helper functions for spawning camp class 
*/

//------------------------------------------------------------------------------------------------
class SDRC_Camp
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();		
	#ifndef NEW_VERSION_WIP	
		ref SDRC_MissionConfigSecondWave secondWave = new SDRC_MissionConfigSecondWave();	
	#endif
	//Optional settings
	#ifdef NEW_VERSION_WIP		
		ref SDRC_MissionConfigSecondWave secondWave = null;
	#endif
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
		return true;			
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn loot items. This is usually called at the end of the mission.
	*/
	static bool AddLoot(SDRC_Camp camp)
	{
		if (!camp.loot)
		{
			SDRC_Log.Add("[SDRC_CampHelper:AddLoot] No loot defined.", LogLevel.DEBUG);
			return false;
		}
		
		if (!camp.loot.box)
		{
			SDRC_Log.Add("[SDRC_CampHelper:AddLoot] No loot box defined.", LogLevel.DEBUG);
			return false;
		}

		SDRC_Log.Add("[SDRC_CampHelper:AddLoot] Adding to: " + camp.loot.box, LogLevel.SPAM);
		SDRC_Log.Add("[SDRC_CampHelper:AddLoot] Items: " + camp.loot.items, LogLevel.SPAM);
				
		//NOTE: itemChance difficulty is handled in TBD
		SDRC_LootHelper.SpawnItemsToStorage(camp.loot.box, camp.loot.items, camp.loot.itemChance);
		SDRC_Log.Add("[SDRC_CampHelper:AddLoot] Loot added.", LogLevel.DEBUG);
		
		return true;
	}
}