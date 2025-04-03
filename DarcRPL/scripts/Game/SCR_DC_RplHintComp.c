class RplHintCompClass : ScriptComponentClass { }
RplHintCompClass g_RplHintCompClass;
 
class RplHintComp : ScriptComponent
{
//	private IEntity m_HintEntity;	
	 
    [RplProp(onRplName: "OnShowHint")]
	string m_SomeString;
	
	string m_SomeString2;
	private ref array<string> m_SomeArray = {"one", "two", "three", "four"};
	
    override void OnPostInit(IEntity owner)
    {
        // We must belong to some RplComponent in order for replication to work.
        // We search for it and warn user when we can't find it.

		BaseRplComponent m_rplComponent = null;
				
        auto hintEnt = SCR_DC_RplHint.Cast(owner);
        if (!hintEnt)
        {
            Print("SCR_DC This example requires that the entity is of type `SCR_DC_RplHint`.", LogLevel.WARNING);
            return;
        }
				
        //auto 
		m_rplComponent = BaseRplComponent.Cast(hintEnt.FindComponent(BaseRplComponent));
        if (!m_rplComponent)
        {
            Print("SCR_DC This example requires that the entity has an RplComponent.", LogLevel.WARNING);
            return;
        }
 
        // We only perform simulation on the authority instance, while all proxy
        // instances just show result of the simulation. Therefore, we only have to
        // subscribe to "frame" events on authority, leaving proxy instances as
        // passive components that do something only when necessary.
        if (m_rplComponent.Role() == RplRole.Authority)
        {
			GetGame().GetCallqueue().CallLater(HintTime, 3000, false);
        }
    }
 
    private void HintTime()
    {
//		if (m_rplComponent.Role() == RplRole.Authority)
		if (true)
		{
			Print("SCR_DC Hinttime.", LogLevel.NORMAL);
			
			//This is for the RplProp
//			m_SomeString = m_SomeArray.GetRandomElement() + " - " + System.GetTickCount();	
//	        Replication.BumpMe();
			
			m_SomeString2 = m_SomeArray.GetRandomElement() + " - 2 - " + System.GetTickCount();
			
			ShowHint();
			
			GetGame().GetCallqueue().CallLater(HintTime, 10000, false);						
		}
		else
		{
			Print("SCR_DC Hinttime for client. Only run once.", LogLevel.NORMAL);
		}
    }
 
    // Presentation of replicated state on proxy. This is used for the RplProp.
    private void OnShowHint()
    {
		ShowHint();
    }
	
	// Show the hint
	private void ShowHint()
	{
		//RplProp section
	//	SCR_HintManagerComponent hintComponent = SCR_HintManagerComponent.GetInstance();
	//	hintComponent.ShowCustomHint("Hello", m_SomeString, 2);
		
		//Rpc section
		ShowGlobalPopup("Oh dear", m_SomeString2, 6);
	}
	
 	void ShowGlobalPopup(string hl, string msg, int dur)
    {
        Rpc(RpcDo_ShowPopup, hl, msg, dur); // broadcast to clients
        RpcDo_ShowPopup(hl, msg, dur); // try to show on authority
    }
    
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_ShowPopup(string hl, string msg, int dur)
    {
		Print("SCR_DC RpcDo_ShowPopup:" + msg, LogLevel.NORMAL);
		
        SCR_PopUpNotification.GetInstance().PopupMsg(hl, dur, msg);
		
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
			Print("SCR_DC hintComponent not found", LogLevel.NORMAL);
		}
		
    }	
}