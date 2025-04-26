class SCR_DC_RplHintCompClass : ScriptComponentClass { }
SCR_DC_RplHintCompClass g_RplHintCompClass;
 
//------------------------------------------------------------------------------------------------
class SCR_DC_RplHintComp : ScriptComponent
{
	private static SCR_DC_RplHintComp s_Instance;	
	 
    override void OnPostInit(IEntity owner)
    {
        auto hintEnt = SCR_DC_RplHintEntity.Cast(owner);
        if (!hintEnt)
        {
            SCR_DC_Log.Add("[SCR_DC_RplHintComp] Entity not found.", LogLevel.ERROR);
            return;
        }
				
		BaseRplComponent rplComponent = BaseRplComponent.Cast(hintEnt.FindComponent(BaseRplComponent));
        if (!rplComponent)
        {
            SCR_DC_Log.Add("[SCR_DC_RplHintComp] RplComponent not found.", LogLevel.ERROR);
            return;
        }
 
		s_Instance = this;				
    }
 
	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to component
	*/
	static SCR_DC_RplHintComp GetInstance()
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
		SCR_DC_Log.Add("[SCR_DC_RplHintComp:RpcDo_ShowHint] Hint: " + msg, LogLevel.NORMAL);
		
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
			SCR_DC_Log.Add("[SCR_DC_RplHintComp:RpcDo_ShowHint] SCR_HintManagerComponent not found", LogLevel.ERROR);
		}		
    }	
}