//Helpers SDRC_HintHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_HintHelper
{	
	//------------------------------------------------------------------------------------------------
	/*!
	Show a hint
	\param title The main title
	\param msg The message to show
	\param icon Icon to show. By default, icon is not shown
	*/
	static void ShowHint(string title, string msg, int timeOut, SDRC_EMissionIcon icon = -1)
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
