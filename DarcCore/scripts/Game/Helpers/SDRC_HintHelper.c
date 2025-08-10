//Helpers SDRC_HintHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_HintHelper
{	
	//------------------------------------------------------------------------------------------------
	/*!
	Show a mission hint
	\param title The main title
	\param msg The message to show
	\param icon Icon to show. By default, icon is not shown
	*/
	static void ShowHintMission(string title, string msg, DC_EMissionIcon icon = DC_EMissionIcon.NONE)
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
			ShowHint(title, msg, timeout, icon);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Show a mission hint
	\param title The main title
	\param msg The message to show
	\param icon Icon to show. By default, icon is not shown
	*/
	static void ShowHint(string title, string msg, int timeOut, DC_EMissionIcon icon = -1)
	{	
		SDRC_RplHintComp hintManagerComponent = SDRC_RplHintComp.GetInstance();
		if (hintManagerComponent)
		{
			hintManagerComponent.ShowGlobalHint(title, msg, timeOut, icon);
		}
		else
		{
			SDRC_Log.Add("[SDRC_HintHelper:ShowHint] SDRC_RplHintComp not found", LogLevel.ERROR);
		}		
	}
}
