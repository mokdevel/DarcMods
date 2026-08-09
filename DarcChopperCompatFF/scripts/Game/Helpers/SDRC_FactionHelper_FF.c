//Helpers SDRC_FactionHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various faction related things
*/
modded class SDRC_FactionHelper
{			
	//------------------------------------------------------------------------------------------------
	/*!
	Get factionKey list for playable factions.
	
	This modded function is needed as using the super will return all factions. We're only interested in the
	player faction.
	*/	
	override static int GetFactionKeyListPlayable(out array<string> factionList, bool printList = false)
	{
		string faction = JWK.GetFactions().GetPlayerFactionKey();
		
		if (faction == "")
		{
			SDRC_Log.Add("[SDRC_FactionHelper_FF:GetFactionKeyListPlayable] No player faction found.", LogLevel.ERROR);
		}		
		else
		{
			factionList.Insert(faction);
		}
				
		return factionList.Count();
	}
}