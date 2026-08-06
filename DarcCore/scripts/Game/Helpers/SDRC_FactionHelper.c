//Helpers SDRC_FactionHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various faction related things
*/
sealed class SDRC_FactionHelper
{			
	//------------------------------------------------------------------------------------------------
	/*!
	Get factionlist
	*/	
	static int GetFactionList(out array<Faction> factionList, bool printList = false)
	{
		array<Faction> factions = {};
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		
		if (!factionManager)
		{			
			SDRC_Log.Add("[SDRC_FactionHelper:GetFactionList] No faction manager found.", LogLevel.ERROR);
			return 0;
		}
		
		factionManager.GetFactionsList(factions);

		foreach (Faction faction : factions)
		{
			factionList.Insert(faction);
			
			if (printList)
			{
				SDRC_Log.Add("[SDRC_FactionHelper:GetFactionList] Faction found: " + faction, LogLevel.NORMAL);				
			}
		}
				
		return factionList.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get factionKey list
	*/	
	static int GetFactionKeyList(out array<string> factionList, bool printList = false)
	{
		array<Faction> factions = {};
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		
		if (!factionManager)
		{			
			SDRC_Log.Add("[SDRC_FactionHelper:GetFactionList] No faction manager found.", LogLevel.ERROR);
			return 0;
		}
		
		factionManager.GetFactionsList(factions);

		foreach (Faction faction : factions)
		{
			string factionName = faction.GetFactionKey();
			if (factionName != "")
			{
				factionList.Insert(factionName);
			}
			
			if (printList)
			{
				SDRC_Log.Add("[SDRC_FactionHelper:GetFactionList] Faction found: " + factionName, LogLevel.NORMAL);				
			}
		}
				
		return factionList.Count();
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Compare two faction keys and determine if they're enemies to each other.
	*/	
	static bool IsEnemies(string myFactionKey, string otherFactionKey)
	{
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager)
			return false;
		
		Faction myFaction = factionManager.GetFactionByKey(myFactionKey);
		Faction otherFaction = factionManager.GetFactionByKey(otherFactionKey);
		
		return myFaction.IsFactionEnemy(otherFaction);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Check if given faction is in the enemy faction list. An enemy faction is defined by the fact that it's not playable.
	*/	
/*	static bool IsEnemyFaction(string enemyFaction)
	{
		array<string> enemyFactions = {};
		SDRC_FactionHelper.GetEnemyFactionKeys(enemyFactions);
		
		if (enemyFactions.Contains(enemyFaction))
		{
			return true;
		}
		
		return false;
	}	*/
			
	//------------------------------------------------------------------------------------------------
	/*!
	Get enemy factions towards all players. This is collected by checking all factions against existing player's factions.
	If the faction is an enemy, it's collected to enemyFactions.
	
	NOTE: This will return weird values in case you have both US and USSR players. For US, USSR is enemy and vice versa. 
		  The list will have both factions listed as enemies.
	
	TBD: WIP, this is not tested in MP!
	*/	
/*	static int GetEnemyFactionKeysForPlayer(out array<string> enemyFactions)
	{
		array<int> players = {};
		GetGame().GetPlayerManager().GetPlayers(players);

		array<Faction> factionList = {};
		int factionCount = SDRC_FactionHelper.GetFactionList(factionList);
		
		foreach (Faction faction : factionList)
		{
			foreach (int player : players)
			{
				Faction playerFaction = SDRC_PlayerHelper.GetPlayerFaction(player);
				
				if (playerFaction)
				{
					if (playerFaction.IsFactionEnemy(faction))
					{
						enemyFactions.Insert(faction.GetFactionKey());
						break;
					}
				}
			}		
		}		
		
		SDRC_Log.Add("[SDRC_FactionHelper:GetEnemyFactionKeys] Enemy factions: " + enemyFactions, LogLevel.DEBUG);				

		return enemyFactions.Count();
	}	*/
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get enemy factions. This is collected from non-playable factions.
	*/	
	static int GetEnemyFactionKeys(out array<string> enemyFactions)
	{
		array<Faction> factionList = {};
		int factionCount = SDRC_FactionHelper.GetFactionList(factionList);
		
		foreach (Faction faction : factionList)
		{
			SCR_Faction scrFaction = SCR_Faction.Cast(faction);

			if (!scrFaction.IsPlayable())
			{
				enemyFactions.Insert(faction.GetFactionKey());
			}
		}		
		
		SDRC_Log.Add("[SDRC_FactionHelper:GetEnemyFactionKeys] Enemy factions: " + enemyFactions, LogLevel.DEBUG);				

		return enemyFactions.Count();
	}
}