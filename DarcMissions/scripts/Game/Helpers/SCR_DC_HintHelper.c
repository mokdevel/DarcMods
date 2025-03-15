//Helpers SCR_DC_HintHelper.c

//------------------------------------------------------------------------------------------------
class SCR_DC_HintHelper
{	
	static void ShowHint(string title, string details, int timeOut)
	{	
		array<int> players = {};
		GetGame().GetPlayerManager().GetPlayers(players);
		
		World world = GetGame().GetWorld();
		SCR_ScenarioFrameworkSystem m_HintHelper;
		
		if (world)
		{
			m_HintHelper = SCR_ScenarioFrameworkSystem.Cast(world.FindSystem(SCR_ScenarioFrameworkSystem));
		}
		
		if (m_HintHelper)
		{
			//The hint needs to be sent to each player
			foreach(int player : players)
			{
				m_HintHelper.ShowHint(details, title, timeOut, "", player);
			}
		}
		else
		{
			SCR_DC_Log.Add("[SCR_DC_HintHelper:ShowHint] Could not send hints.", LogLevel.ERROR);
		}
	}	
}
