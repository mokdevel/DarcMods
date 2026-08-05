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
	Get enemy factions
	
	We check all factions against existing player's factions.
	*/	
	static array<string> GetEnemyFactionKeys()
	{
		array<string> enemyFactions = {};
		
		array<int> players = {};
		GetGame().GetPlayerManager().GetPlayers(players);

		array<Faction> factionList = {};
		int factionCount = SDRC_FactionHelper.GetFactionList(factionList);
		
		foreach (Faction faction : factionList)
		{
			foreach (int player : players)
			{
				PlayerController playerController = GetGame().GetPlayerManager().GetPlayerController(player);
				int playerID = playerController.GetPlayerId();
				
				Faction playerFaction = SDRC_PlayerHelper.GetPlayerFaction(playerID);
//				Faction playerFaction = SDRC_PlayerHelper.GetPlayerFaction(player);
				
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

		return enemyFactions;
	}	
}