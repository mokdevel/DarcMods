//Helpers SDRC_HintHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_HintHelper
{	
	//------------------------------------------------------------------------------------------------
	static void ShowHintMission(string title, string details)
	{	
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!m_BaseGameMode)
		{
			return;
		}
		
 		if (!m_BaseGameMode.missionFrame)
		{
			return;
		}
		
		int timeout = m_BaseGameMode.missionFrame.m_Config.missionHintTime;
		if (timeout > 0)
		{
			ShowHint(title, details, timeout);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static void ShowHint(string title, string details, int timeOut)
	{	
		SDRC_RplHintComp hintManagerComponent = SDRC_RplHintComp.GetInstance();
		if (hintManagerComponent)
		{
			hintManagerComponent.ShowGlobalHint(title, details, timeOut);
		}
		else
		{
			SDRC_Log.Add("[SDRC_HintHelper:ShowHint] SDRC_RplHintComp not found", LogLevel.ERROR);
		}		
	}
}
