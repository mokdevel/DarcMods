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
	private static string m_sDefaultEnemyFactionKey = "USSR";
	private static ref array<string>m_sEnemyFactions = {};
	
	//------------------------------------------------------------------------------------------------
	static void Setup()
	{
		//Load loot config
		m_EnemyListJsonApi.Load();
		m_Config = m_EnemyListJsonApi.conf;
		m_Config.Populate();		
	}

	//------------------------------------------------------------------------------------------------
	static void SanityCheck(array<string>enemyFactions)
	{
		//Sanity check
		SDRC_Log.Add("[SDRC_EnemyHelper:SanityCheck] Checking that all factions (" + enemyFactions + ") have enemies.", LogLevel.NORMAL);
		
		array<string>factionsFound = {};		
		
		foreach (SDRC_List list : m_Config.lists)
		{
			int count = 0;
			factionsFound.Clear();
			
			foreach(string faction : enemyFactions)
			{
				foreach(ResourceName enemy : list.items)
				{
					if (enemy.Contains("_" + faction + "_"))
					{
						count++;
						factionsFound.Insert(faction);
						break;
					}
				}
			}
			
			if (count == enemyFactions.Count())
			{
				SDRC_Log.Add("[SDRC_EnemyHelper:SanityCheck] " + list.id + " has " + count + " enemy factions", LogLevel.DEBUG);
			}
			else
			{
				SDRC_Log.Add("[SDRC_EnemyHelper:SanityCheck] " + list.id + " is missing enemies. " + count + "/" + enemyFactions.Count() + " factions found: " + factionsFound , LogLevel.WARNING);
			}
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	static void SetEnemyFactions(array<string>enemyFactions)
	{
		SDRC_Log.Add("[SDRC_EnemyHelper:SetEnemyFactions] Setting enemy factions: " + enemyFactions, LogLevel.DEBUG);
		m_sEnemyFactions = enemyFactions;		
		SDRC_EnemyHelper.SanityCheck(m_sEnemyFactions);		
	}
	
	//------------------------------------------------------------------------------------------------
	static void SetDefaultEnemyFaction(string faction)
	{
		if (faction != "")
		{
			m_sDefaultEnemyFactionKey = faction;
		}
		
		SDRC_Log.Add("[SDRC_EnemyHelper:SetDefaultEnemyFaction] Default enemy faction: " + faction, LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------
	static ResourceName SelectEnemyFaction(string faction = "RANDOM")
	{
		if (faction == "RANDOM")
		{
//			SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
//			faction = baseGameMode.missionFrame.m_Config.enemyFactions.GetRandomElement();				//TBD: This is pointing to missionFrame even if it's in core. Should move the enemyFactions as a parameter.
			faction = m_sEnemyFactions.GetRandomElement();
			SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemyFaction] RANDOM: " + faction, LogLevel.DEBUG);
			return faction;
		}
		
		if (faction == "DEFAULT")
		{
			faction = m_sDefaultEnemyFactionKey;
			SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemyFaction] DEFAULT: " + faction, LogLevel.DEBUG);
			return faction;
		}
				
		SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemyFaction] Selected: " + faction + " (no change)", LogLevel.DEBUG);
		return faction;
	}
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Select the proper enemy resourcename for spawning. 
	\param listName The enemyList to check. If a prefab "{xxx}.." is provided, that is returned.
	*/	
	static ResourceName SelectEnemy(string listName, string faction = "DEFAULT")
	{
		int index = -1;		
		array<string>enemyList = {};
		
		if (listName[0] == "{")		//If it is already a resource name, return
		{		
			return listName;
		}
		
		//Select the enemy faction from a list
		faction = SelectEnemyFaction(faction);
		
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
			SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] No enemyList with id: " + listName + ". Typo?", LogLevel.WARNING);
			return "";				
		}

		//Filter with faction		
		foreach(string enemy : m_Config.lists[index].items)
		{
			if (enemy.Contains("_" + faction + "_"))
			{
				enemyList.Insert(enemy);
			}
		}
		
		//ResourceName resourceName = m_Config.lists[index].items.GetRandomElement();
		if (enemyList.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] No enemies in " + listName + " for faction: " + faction, LogLevel.WARNING);
			return "";				
		}
		
		ResourceName resourceName = enemyList.GetRandomElement();
		SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] Selected: (" + listName + ") " + resourceName, LogLevel.DEBUG);
		return resourceName;
	}
}
