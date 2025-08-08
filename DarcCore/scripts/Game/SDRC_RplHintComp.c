class SDRC_RplHintCompClass : ScriptComponentClass { }
SDRC_RplHintCompClass g_RplHintCompClass;
 
//------------------------------------------------------------------------------------------------
class SDRC_RplHintComp : ScriptComponent
{
	private static SDRC_RplHintComp s_Instance;	
	 
	//------------------------------------------------------------------------------------------------
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
	/*!
	Show a global hint for all players. Uses replication
	\param title The main title
	\param msg The message to show
	\param icon Icon to show. By default, icon is not shown
	*/
 	void ShowGlobalHint(string title, string msg, int dur, DC_EMissionIcon icon)
    {
        Rpc(RpcDo_ShowHint, title, msg, dur, icon); // broadcast to clients
        RpcDo_ShowHint(title, msg, dur, icon); // try to show on authority
    }
    
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_ShowHint(string title, string msg, int dur, DC_EMissionIcon icon)
    {
		SDRC_Log.Add("[SDRC_RplHintComp:RpcDo_ShowHint] Hint: " + msg, LogLevel.NORMAL);
	
		SDRC_SCR_PopUpNotification.SetIcon(icon);
		SDRC_SCR_PopUpNotification.GetInstance().PopupMsg(title, 20, text2: msg);		
		return;
    }	
}