//Helpers SDRC_MissionHintHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_MissionHintHelper
{	
	//------------------------------------------------------------------------------------------------
	/*!
	Show a mission hint
	\param title The main title
	\param msg The message to show
	\param icon Icon to show. By default, icon is not shown
	*/
	static void Show(string title, string msg, SDRC_EMissionIcon icon = SDRC_EMissionIcon.NONE)
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
			SDRC_HintHelper.ShowHint(title, msg, timeout, icon);
		}
	}
}
