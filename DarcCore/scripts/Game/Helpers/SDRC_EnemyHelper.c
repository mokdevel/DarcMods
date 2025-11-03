//Helpers SDRC_EnemyHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various enemy related things
*/

//------------------------------------------------------------------------------------------------
sealed class SDRC_EnemyHelper
{
	private const string DC_MISSIONCONFIG_FILE = "dc_enemyList.json";
		
	private static ref SDRC_EnemyListJsonApi m_EnemyListJsonApi;
	private static ref SDRC_ListConfig m_Config;
	private static string m_sDefaultEnemyFactionKey;
	private static Faction m_DefaultEnemyFaction = null;
	private static ref array<string> m_sEnemyFactions = {};
	private static ref array<string> m_sFactionList = {};
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Setup enemyHelper.
	This will prepare the enemyLists, factionList and set default enemyFaction
	*/	
	static void Setup(string defaultEnemyFaction)
	{
		SDRC_Log.Add("[SDRC_EnemyHelper:Setup] Preparing..", LogLevel.NORMAL);

		SDRC_FactionHelper.GetFactionList(m_sFactionList);
				
		m_sDefaultEnemyFactionKey = defaultEnemyFaction;
		m_DefaultEnemyFaction = GetFactionWithName(m_sDefaultEnemyFactionKey);
		if (!m_DefaultEnemyFaction)
		{
			SDRC_Log.Add("[SDRC_EnemyHelper:Setup] Error in setting fallback enemy faction: " + defaultEnemyFaction, LogLevel.ERROR);
		}
		
		//Load enemy config
		m_EnemyListJsonApi = new SDRC_EnemyListJsonApi(DC_MISSIONCONFIG_FILE);
		m_EnemyListJsonApi.Load();
		m_Config = m_EnemyListJsonApi.conf;
		m_Config.Populate();		
	}
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Select the proper enemy resourcename for spawning. 
	\param listName The enemyList to check. If a prefab "{xxx}.." is provided, that is returned.
	*/	
	static void SetEnemyFactions(array<string>enemyFactions)
	{
		SDRC_Log.Add("[SDRC_EnemyHelper:SetEnemyFactions] Setting enemy factions: " + enemyFactions, LogLevel.DEBUG);
		m_sEnemyFactions = enemyFactions;		
		SDRC_EnemyHelper.SanityCheck(m_sEnemyFactions);		
	}	
		
	//------------------------------------------------------------------------------------------------
	static string GetDefaultEnemyFaction()
	{
		return m_sDefaultEnemyFactionKey;
	}
	
/*	static void SetDefaultEnemyFaction(string faction)
	{
		if (faction != "")
		{
			m_sDefaultEnemyFactionKey = faction;
		}
		
		SDRC_Log.Add("[SDRC_EnemyHelper:SetDefaultEnemyFaction] Default enemy faction: " + faction, LogLevel.NORMAL);
	}*/
			
	//------------------------------------------------------------------------------------------------
	static void SanityCheck(array<string>enemyFactions)
	{
		//Sanity check
		SDRC_Log.Add("[SDRC_EnemyHelper:SanityCheck] Checking that all factions (" + enemyFactions + ") have enemies.", LogLevel.NORMAL);
		
		array<string>factionsFound = {};		
		array<string>factionsMissing = {};
		
		foreach (SDRC_List list : m_Config.lists)
		{
//			int count = 0;
			factionsFound.Clear();
			factionsMissing.Clear();
			
			foreach(string faction : enemyFactions)
			{
				string factionToTest = faction;
				
				//Check if the requested faction has an aka
				foreach(SDRC_Aka aka : m_Config.akas)
				{
					if (aka.names[0] == faction)
					{
						factionToTest = aka.names[1];
						SDRC_Log.Add("[SDRC_EnemyHelper:SanityCheck] Testing " + faction + " as " + factionToTest, LogLevel.DEBUG);
						break;
					}
				}
				
				//Collect factions found
				foreach(ResourceName enemy : list.items)
				{
					if (enemy.Contains("_" + factionToTest + "_"))
					{
//						count++;
						factionsFound.Insert(faction);
						break;
					}
				}

				//Collect factions missing
				foreach(ResourceName enemy : list.items)
				{
					if (!enemy.Contains("_" + factionToTest + "_"))
					{
						factionsMissing.Insert(faction);
						break;
					}
				}
			}
			
			if (factionsFound.Count() == enemyFactions.Count())
			{
				SDRC_Log.Add("[SDRC_EnemyHelper:SanityCheck] " + list.id + " OK. Has " + factionsFound.Count() + " enemy factions", LogLevel.DEBUG);
			}
			else
			{
				SDRC_Log.Add("[SDRC_EnemyHelper:SanityCheck] " + list.id + " is missing enemies in faction: " + factionsMissing, LogLevel.WARNING);
			}
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Select the proper enemy resourcename for spawning. 
	\param listName The enemyList to check. If a prefab "{xxx}.." is provided, that is returned.
	*/	
	static ResourceName SelectEnemy(string listName, string faction)
	{
		int index = -1;		
		array<string> enemyList = {};
		
		if (listName[0] == "{")		//If it is already a resource name, return
		{		
			return listName;
		}
		
		//Select the enemy faction from a list
		faction = SelectEnemyFaction(faction);

		//Check if the requested faction has an aka
		foreach(SDRC_Aka aka : m_Config.akas)
		{
			if (aka.names[0] == faction)
			{
				SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] Using " + aka.names[1] + " for " + faction, LogLevel.DEBUG);
				faction = aka.names[1];
				break;
			}
		}
				
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
		
		ResourceName resourceName = "";
		
		if (enemyList.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] Enemy name does not have faction in it. Using the full list.", LogLevel.WARNING);
			if (!m_Config.lists[index].items.IsEmpty())
			{
				resourceName = m_Config.lists[index].items.GetRandomElement();
			}
		}
		else		
		{
			resourceName = enemyList.GetRandomElement();
		}
		
		if (resourceName == "")
		{
			SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] No enemy selected. List  (" + listName + ") has " + enemyList.Count() + " enemies.", LogLevel.ERROR);
		}
		else
		{
			SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] Selected: (" + listName + ") " + resourceName, LogLevel.DEBUG);
		}
		return resourceName;
	}
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Select faction for the enemy.
	\param faction The faction requested
	*/	
	static string SelectEnemyFaction(string faction = "")
	{
		if (m_sEnemyFactions.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemyFaction] No enemy factions defined.", LogLevel.WARNING);
			return "";
		}
		
		if (faction == "")	//RANDOM
		{
			faction = m_sEnemyFactions.GetRandomElement();
			SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemyFaction] Selected: " + faction, LogLevel.SPAM);
			return faction;
		}
		
		if (faction != "")
		{			
			if (m_sFactionList.Contains(faction))
			{
				//faction = m_sDefaultEnemyFactionKey;
				SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemyFaction] Mission specific: " + faction, LogLevel.SPAM);
				return faction;
			}
			else
			{
				SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemyFaction] Incorrect faction requested: " + faction + " . Using default: " + m_sDefaultEnemyFactionKey, LogLevel.WARNING);
				faction = m_sDefaultEnemyFactionKey;
				return faction;
			}
		}
				
		SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemyFaction] Selected: " + faction + " (no change)", LogLevel.SPAM);
		return faction;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get Faction with string name
	*/	
	static Faction GetFactionWithName(string name)
	{
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager)
		{			
			SDRC_Log.Add("[SDRC_EnemyHelper:GetFactionWithName] No faction manager found.", LogLevel.ERROR);
			return m_DefaultEnemyFaction;
		}
		
		Faction faction = factionManager.GetFactionByKey(name);
		if (!faction)
		{
			SDRC_Log.Add("[SDRC_EnemyHelper:GetFactionWithName] Using default faction: " + m_sDefaultEnemyFactionKey, LogLevel.WARNING);
			return m_DefaultEnemyFaction;
		}
		
		return faction;
	}	
}
