//Helpers SCR_DC_HintHelper.c

//------------------------------------------------------------------------------------------------
class SCR_DC_HintHelper
{	
	static void ShowHint(string title, string details, int timeOut)
	{	
		SCR_DC_RplHintComp hintManagerComponent = SCR_DC_RplHintComp.GetInstance();
		if (hintManagerComponent)
		{
			hintManagerComponent.ShowGlobalHint(title, details, timeOut);
		}
		else
		{
			SCR_DC_Log.Add("[SCR_DC_HintHelper:ShowHint] SCR_DC_RplHintComp not found", LogLevel.ERROR);
		}		
		
	}
}
