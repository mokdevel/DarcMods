//Helpers SDRC_HintHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_HintHelper
{	
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
