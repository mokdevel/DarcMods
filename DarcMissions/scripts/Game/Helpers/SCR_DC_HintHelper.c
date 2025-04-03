//Helpers SCR_DC_HintHelper.c

//------------------------------------------------------------------------------------------------
class SCR_DC_HintHelper
{	
	static void ShowHint(string title, string details, int timeOut)
	{	
		SCR_DC_RplHintComp hintManagerComponent = SCR_DC_RplHintComp.GetInstance();
		hintManagerComponent.ShowGlobalHint(title, details, timeOut);
	}
}
