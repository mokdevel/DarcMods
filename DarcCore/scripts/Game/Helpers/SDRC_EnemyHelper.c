//Helpers SDRC_EnemyHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various enemy related things
*/

//------------------------------------------------------------------------------------------------
sealed class SDRC_EnemyHelper
{
	private const string DC_CONFIG_FILE_ENEMYLIST = "dc_enemyList.json";
	private const int DC_CONFIG_FILE_ENEMYLIST_JSONVER = 2;
	
	private static ref SDRC_JsonApi2 m_JsonApi = null;
	private static ref SDRC_EnemyListConfig m_Config = new SDRC_EnemyListConfig();			
	
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
		
		//By default, all factions can be used. This may later be changed by a mod like DarcMissions
		SDRC_FactionHelper.GetFactionList(m_sEnemyFactions);
		SDRC_Log.Add("[SDRC_EnemyHelper:Setup] Default enemyFactions: " + m_sEnemyFactions, LogLevel.NORMAL);
				
		m_sDefaultEnemyFactionKey = defaultEnemyFaction;
		m_DefaultEnemyFaction = GetFactionWithName(m_sDefaultEnemyFactionKey);
		if (!m_DefaultEnemyFaction)
		{
			SDRC_Log.Add("[SDRC_EnemyHelper:Setup] Error in setting fallback enemy faction: " + defaultEnemyFaction, LogLevel.ERROR);
		}
		
		//Load configuration from file
		m_JsonApi = new SDRC_JsonApi2(DC_CONFIG_FILE_ENEMYLIST);	
		m_JsonApi.Load(m_Config, SDRC_Config.Cast(m_Config), DC_CONFIG_FILE_ENEMYLIST_JSONVER, safeUpdate: true);		
		m_Config.Populate();	
		
		//Create a C_RANDOMIZED list
		array<string> randomizedLists = {"C_RIFLEMAN", "C_HEAVY", "C_RECON"};
		
		ref SDRC_List randomizedList = new SDRC_List();
		randomizedList.Set("C_RANDOMIZED", {}, {}, {}, {});
		
		foreach (string rlist : randomizedLists)
		{		
			int index = SDRC_ListHelper.FindRightList(m_Config.lists, rlist);
			if (index != -1)
			{
				foreach (string item : m_Config.lists[index].items)
				{
					randomizedList.items.Insert(item);
				}
			}			
		}		
		m_Config.lists.Insert(randomizedList);
		if (SDRC_Log.GetLogLevel() > DC_LogLevel.NORMAL)
		{
			SDRC_Log.Add("[SDRC_EnemyHelper:Setup] List: " + randomizedList.id + " (" + randomizedList.items.Count() + ")", LogLevel.DEBUG);				
			randomizedList.items.Debug();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Select the proper enemy resourcename for spawning. 
	\param listName The enemyList to check. If a prefab "{xxx}.." is provided, that is returned.
	*/	
	static void SetEnemyFactions(array<string> enemyFactions)
	{
		SDRC_Log.Add("[SDRC_EnemyHelper:SetEnemyFactions] Setting enemy factions: " + enemyFactions, LogLevel.SPAM);
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
	static void SanityCheck(array<string> enemyFactions)
	{
		//Sanity check
		SDRC_Log.Add("[SDRC_EnemyHelper:SanityCheck] Checking that all factions (" + enemyFactions + ") have enemies.", LogLevel.NORMAL);
		
		array<string> factionsFound = {};		
		array<string> factionsMissing = {};
		
		foreach (SDRC_List list : m_Config.lists)
		{
			factionsFound.Clear();
			factionsMissing.Clear();
			
			foreach (string faction : enemyFactions)
			{
				array<string> factionsToTest = {};
				
				//Check if the requested faction has an aka
				foreach (SDRC_Aka aka : m_Config.akas)
				{
					if (aka.names[0] == faction)
					{
						for (int i = 1; i < aka.names.Count(); i++)
						{
							factionsToTest.Insert(aka.names[i]);					
						}						
					}
				}
				
				//If no Akas, add the default
				if (factionsToTest.IsEmpty())
				{
					factionsToTest.Insert(faction);
				}

				foreach (string factionToTest : factionsToTest)
				{
					SDRC_Log.Add("[SDRC_EnemyHelper:SanityCheck] Testing " + faction + " as " + factionToTest, LogLevel.DEBUG);
					
					//Collect factions found
					foreach (ResourceName enemy : list.items)
					{
//						if (enemy.Contains("_" + factionToTest + "_"))
						if ( (enemy.Contains("_" + factionToTest + "_")) || (enemy.Contains("_" + factionToTest + ".")) )
						{
							if (!factionsFound.Contains(faction))
							{
								factionsFound.Insert(faction);
							}
							break;
						}
					}
	
					//Collect factions missing
/*					foreach (ResourceName enemy : list.items)
					{
						if (!enemy.Contains("_" + factionToTest + "_"))
						{
							if (!factionsMissing.Contains(faction))
							{
								factionsMissing.Insert(faction);
							}
							break;
						}
					}*/
				}
				
				//Collect factions missing
				if (!factionsFound.Contains(faction))
				{
					factionsMissing.Insert(faction);
				}
			}
			
			if (factionsFound.Count() == enemyFactions.Count())
			{
				SDRC_Log.Add("[SDRC_EnemyHelper:SanityCheck] " + list.id + " OK. Has " + factionsFound.Count() + " enemy factions", LogLevel.DEBUG);
			}
			else
			{
				SDRC_Log.Add("[SDRC_EnemyHelper:SanityCheck] " + list.id + " is missing enemies in faction: " + factionsMissing, LogLevel.WARNING);
				if (list.id.Contains("C_"))
				{
					//TBD: The problem with some of the characters is that they don't have any faction IDs. For example: "justASoldier.et"
					SDRC_Log.Add("[SDRC_EnemyHelper:SanityCheck] For C_xxxx lists, the sanity check may report incorrect info. Check logs for details.", LogLevel.WARNING);
				}
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
		array<string> enemyList = {};
		array<string> factions = {};
		
		if (listName[0] == "{")		//If it is already a resource name, return
		{		
			return listName;
		}
		
		//Select the enemy faction from a list
		faction = SelectEnemyFaction(faction);
		//Put the factions as default in the list.
		factions.Insert(faction);

		//Check if the requested faction has an aka
		foreach (SDRC_Aka aka : m_Config.akas)
		{			
			if (aka.names[0] == faction)
			{			
				factions.Clear();
				for (int i = 1; i < aka.names.Count(); i++)
				{
					factions.Insert(aka.names[i]);					
				}
			}
			
/*			if (aka.names[0] == faction)
			{
				SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] Using " + aka.names[1] + " for " + faction, LogLevel.DEBUG);
				faction = aka.names[1];
				break;
			}*/
		}

		//Find the right list index		
		int index = SDRC_ListHelper.FindRightList(m_Config.lists, listName);
		
		//Did we find it?
		if (index == -1)
		{
			SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] No enemyList with id: " + listName + ". Typo?", LogLevel.WARNING);
			return "";				
		}

		//Filter with faction		
		foreach (string enemy : m_Config.lists[index].items)
		{
			foreach (string fac : factions)
			{
//				if (enemy.Contains("_" + fac + "_"))	//This checks for "_US_"
				if (enemy.Contains(fac + "_"))			//This checks for "US_".
				{
					enemyList.Insert(enemy);
					continue;
				}
				if (enemy.Contains("_" + fac + "."))	//This checks for "_US.". Typically this is not the case, but there are some mods like Ballien Creatures where the naming is different.
				{
					enemyList.Insert(enemy);
					continue;
				}
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
				SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemyFaction] Mission specific: " + faction, LogLevel.DEBUG);
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
