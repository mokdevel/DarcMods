class SCR_DC_RplHintCompClass : ScriptComponentClass { }
SCR_DC_RplHintCompClass g_RplHintCompClass;
 
class SCR_DC_RplHintComp : ScriptComponent
{
	private static SCR_DC_RplHintComp s_Instance;	
	 
//    [RplProp(onRplName: "OnShowHint")]
//	string m_SomeString;
	
	string m_SomeString2;	
	
	private ref array<string> m_SomeArray = {"one", "two", "three", "four"};
	
    override void OnPostInit(IEntity owner)
    {
        // We must belong to some RplComponent in order for replication to work.
        // We search for it and warn user when we can't find it.

		BaseRplComponent m_rplComponent = null;
				
        auto hintEnt = SCR_DC_RplHintEntity.Cast(owner);
        if (!hintEnt)
        {
            SCR_DC_Log.Add("[SCR_DC_RplHintComp] This example requires that the entity is of type `SCR_DC_RplHint`.", LogLevel.WARNING);
            return;
        }
				
        //auto 
		m_rplComponent = BaseRplComponent.Cast(hintEnt.FindComponent(BaseRplComponent));
        if (!m_rplComponent)
        {
            SCR_DC_Log.Add("[SCR_DC_RplHintComp] This example requires that the entity has an RplComponent.", LogLevel.WARNING);
            return;
        }
 
		s_Instance = this;
		
        // We only perform simulation on the authority instance, while all proxy
        // instances just show result of the simulation. Therefore, we only have to
        // subscribe to "frame" events on authority, leaving proxy instances as
        // passive components that do something only when necessary.
        if (m_rplComponent.Role() == RplRole.Authority)
        {
			GetGame().GetCallqueue().CallLater(HintTime, 3000, false);
        }
    }
 
	//------------------------------------------------------------------------------------------------
	//! Get instance of the component.
	
	static SCR_DC_RplHintComp GetInstance()
	{
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
    private void HintTime()
    {
		SCR_DC_Log.Add("[SCR_DC_RplHintComp] Hinttime.", LogLevel.NORMAL);
			
//		m_SomeString = m_SomeArray.GetRandomElement() + " - " + System.GetTickCount();		
//	    Replication.BumpMe();
			
		m_SomeString2 = m_SomeArray.GetRandomElement() + " - " + System.GetTickCount();		
		
		ShowHint();
			
		GetGame().GetCallqueue().CallLater(HintTime, 5000, false);						
	}
 
/*    // Presentation of replicated state on proxy.
    private void OnShowHint()
    {
		ShowHint();
    }*/
	
	private void ShowHint()
	{
//		SCR_HintManagerComponent hintComponent = SCR_HintManagerComponent.GetInstance();
//		hintComponent.ShowCustomHint("Hello", m_SomeString, 2);
		ShowGlobalHint("Oh dear", m_SomeString2, 6);
	}
	
 	void ShowGlobalHint(string hl, string msg, int dur)
    {
        Rpc(RpcDo_ShowPopup, hl, msg, dur); // broadcast to clients
        RpcDo_ShowPopup(hl, msg, dur); // try to show on authority
    }
    
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_ShowPopup(string hl, string msg, int dur)
    {
		SCR_DC_Log.Add("[SCR_DC_RplHintComp] RpcDo_ShowHint:" + msg, LogLevel.NORMAL);
		
//        SCR_PopUpNotification.GetInstance().PopupMsg(hl, dur, msg);
		
		string hintTitle = "Victory!";
		string hintDescription = msg;
		float duration = dur;
		EHint type = EHint.UNDEFINED;
		bool isTimerVisible = true;
		EFieldManualEntryId fieldManualEntry = EFieldManualEntryId.NONE;
		
		// Create the hint info
		SCR_HintUIInfo hintInfo = SCR_HintUIInfo.CreateInfo(hintDescription, hintTitle, duration, type, fieldManualEntry, isTimerVisible);		
		
		SCR_HintManagerComponent hintComponent = SCR_HintManagerComponent.GetInstance();
		if(hintComponent)
		{
//			hintComponent.ShowCustomHint("Hello", msg, dur);
			hintComponent.ShowHint(hintInfo);
		}
		else
		{
			SCR_DC_Log.Add("[SCR_DC_RplHintComp] HintComponent not found", LogLevel.NORMAL);
		}
		
    }	
}