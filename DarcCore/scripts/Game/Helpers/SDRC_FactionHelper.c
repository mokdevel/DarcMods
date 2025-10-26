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
	static int GetFactionList(out array<string> factionList, bool printList = false)
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
}