class SDRC_RplHintCompClass : ScriptComponentClass { }
SDRC_RplHintCompClass g_RplHintCompClass;
 
//------------------------------------------------------------------------------------------------
class SDRC_RplHintComp : ScriptComponent
{
	private static SDRC_RplHintComp s_Instance;	
	 
    override void OnPostInit(IEntity owner)
    {
        auto hintEnt = SDRC_RplHintEntity.Cast(owner);
        if (!hintEnt)
        {
            SDRC_Log.Add("[SDRC_RplHintComp] Entity not found.", LogLevel.ERROR);
            return;
        }
				
		BaseRplComponent rplComponent = BaseRplComponent.Cast(hintEnt.FindComponent(BaseRplComponent));
        if (!rplComponent)
        {
            SDRC_Log.Add("[SDRC_RplHintComp] RplComponent not found.", LogLevel.ERROR);
            return;
        }
 
		s_Instance = this;				
    }
 
	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to component
	*/
	static SDRC_RplHintComp GetInstance()
	{
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
 	void ShowGlobalHint(string hl, string msg, int dur)
    {
        Rpc(RpcDo_ShowHint, hl, msg, dur); // broadcast to clients
        RpcDo_ShowHint(hl, msg, dur); // try to show on authority
    }
    
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_ShowHint(string title, string msg, int dur)
    {
		SDRC_Log.Add("[SDRC_RplHintComp:RpcDo_ShowHint] Hint: " + msg, LogLevel.NORMAL);
	
		SCR_PopUpNotification.GetInstance().PopupMsg(title, 20, text2: msg);		
		return;
			
		string hintTitle = title;
		string hintDescription = msg;
		float duration = dur;
		EHint type = EHint.UNDEFINED;
		bool isTimerVisible = true;
		EFieldManualEntryId fieldManualEntry = EFieldManualEntryId.NONE;
		
		// Create the hint info
		SCR_HintUIInfo hintInfo = SCR_HintUIInfo.CreateInfo(hintDescription, hintTitle, duration, type, fieldManualEntry, isTimerVisible);		
		
		SCR_HintManagerComponent hintComponent = SCR_HintManagerComponent.GetInstance();
		if (hintComponent)
		{
			hintComponent.ShowHint(hintInfo, ignoreShown: true);
		}
		else
		{
			SDRC_Log.Add("[SDRC_RplHintComp:RpcDo_ShowHint] SCR_HintManagerComponent not found", LogLevel.ERROR);
		}		
    }	
}