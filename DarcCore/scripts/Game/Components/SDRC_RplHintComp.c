[ComponentEditorProps(category: "GameScripted/DarcCore", description: "DarcCore Hint Component")]
class SDRC_RplHintCompClass : ScriptComponentClass { }
SDRC_RplHintCompClass g_RplHintCompClass;
 
//------------------------------------------------------------------------------------------------
class SDRC_RplHintComp : ScriptComponent
{
	private static SDRC_RplHintComp s_Instance;	
 	protected static SDRC_EHintPosition m_Position;

	//------------------------------------------------------------------------------------------------
    override void OnPostInit(IEntity owner)
    {
        auto ent = SDRC_RplHintEntity.Cast(owner);
        if (!ent)
        {
            SDRC_Log.Add("[SDRC_RplHintComp] Entity not found.", LogLevel.ERROR);
            return;
        }
				
		BaseRplComponent rplComponent = BaseRplComponent.Cast(ent.FindComponent(BaseRplComponent));
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
 	void ShowGlobalHint(string title, string msg, int dur, SDRC_EMissionIcon icon, SDRC_EHintPosition position = SDRC_EHintPosition.UP_LEFT)
    {
        Rpc(RpcDo_ShowHint, title, msg, dur, icon, position); 	// broadcast to clients
        RpcDo_ShowHint(title, msg, dur, icon, position); 		// try to show on authority
    }
    
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_ShowHint(string title, string msg, int dur, SDRC_EMissionIcon icon, SDRC_EHintPosition position)
    {
		SDRC_Log.Add("[SDRC_RplHintComp:RpcDo_ShowHint] Hint: " + msg, LogLevel.NORMAL);
	
		SDRC_SCR_PopUpNotification.GetInstance().SetPosition(position);
		SDRC_SCR_PopUpNotification.GetInstance().SetIcon(icon);
		SDRC_SCR_PopUpNotification.GetInstance().PopupMsg(title, 20, text2: msg);		
		return;
    }	
}