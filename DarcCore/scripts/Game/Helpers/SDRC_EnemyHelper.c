//Helpers SDRC_EnemyHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various enemy related things
*/

//------------------------------------------------------------------------------------------------
sealed class SDRC_EnemyHelper
{
	private static ref SDRC_EnemyListJsonApi m_EnemyListJsonApi = new SDRC_EnemyListJsonApi();	
	private static ref SDRC_ListConfig m_Config;
	
	static void Setup()
	{
		//Load loot config
		m_EnemyListJsonApi.Load();
		m_Config = m_EnemyListJsonApi.conf;
		m_Config.Populate();
	}

	//------------------------------------------------------------------------------------------------
	/*! 
	Find the loot item
	*/	
	static ResourceName SelectEnemy(string listName)
	{
		if (listName[0] == "{")		//If it is already a resource name, return
		{		
			return listName;
		}
		
		int index = -1;		
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		
		array<string>factions = baseGameMode.missionFrame.m_Config.enemyFactions;
		array<string>enemyList = {};

		//Find the right list index		
		for (int i = 0; i < m_Config.lists.Count(); i++)		
		{
			if (m_Config.lists[i].id == listName)
			{
				index = i;
				break;
			}
		}
		
		//Did we find it?
		if (index == -1)
		{
			SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] No enemylist with id: " + listName + ". Typo?", LogLevel.WARNING);
			return "";				
		}

		//Filter with factions
		foreach(string enemy : m_Config.lists[index].items)
		{
			foreach (string filter: factions)
			{
				if (enemy.Contains(filter))
				{
					enemyList.Insert(enemy);
				}
			}
		}
		
		//ResourceName resourceName = m_Config.lists[index].items.GetRandomElement();
		if (enemyList.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] No enemies in " + listName + " for faction: " + factions, LogLevel.WARNING);
			return "";				
		}
		
		ResourceName resourceName = enemyList.GetRandomElement();
		SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] Selected: (" + listName + ") " + resourceName, LogLevel.DEBUG);
		return resourceName;
	}
}
