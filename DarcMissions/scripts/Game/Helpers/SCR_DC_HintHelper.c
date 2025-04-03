//Helpers SCR_DC_HintHelper.c

//------------------------------------------------------------------------------------------------
class SCR_DC_HintHelper
{	
	static void ShowHint(string title, string details, int timeOut)
	{	
		
		SCR_DC_RplHintEntity rplHintEntity = SCR_DC_RplHintEntity.GetInstance();
		
		SCR_DC_RplHintComp hintManagerComponent = SCR_DC_RplHintComp.GetInstance();
		hintManagerComponent.ShowGlobalHint(details, title, timeOut);
		//(details, title, 10, false, EFieldManualEntryId.NONE, true);

/*		
		
		SCR_HintUIInfo info = SCR_HintUIInfo();
        info.CreateInfo("HintUIInfo Description", "HintUIInfo name", 10.0, EHint.CONFLICT_OVERVIEW, EFieldManualEntryId.CONFLICT_OVERVIEW, false);
        
        SCR_HintManagerComponent hintManagerComponent = SCR_HintManagerComponent.GetInstance();
		if (hintManagerComponent)
		{
			hintManagerComponent.ShowCustomHint(details, title, 10, false, EFieldManualEntryId.NONE, true);
		}
        //SCR_HintManagerComponent.GetInstance().ShowHint(info);
		return;
*/		
/*		array<int> players = {};
		GetGame().GetPlayerManager().GetPlayers(players);
		
		World world = GetGame().GetWorld();
		SCR_ScenarioFrameworkSystem m_HintHelper;
		
		if (world)
		{
			m_HintHelper = SCR_ScenarioFrameworkSystem.Cast(world.FindSystem(SCR_ScenarioFrameworkSystem));
		}
		
		if (m_HintHelper)
		{
			//The hint needs to be sent to each player
			foreach(int player : players)
			{
				FactionKey factionKey = SCR_DC_PlayerHelper.GetPlayerFactionKey(player);
				
				SCR_DC_Log.Add("[SCR_DC_HintHelper:ShowHint] Player FactionKey: " + factionKey, LogLevel.DEBUG);
				factionKey = "";	//TBD: Leaving this like this as it seems to work for now. 
				m_HintHelper.ShowHint(details, title, timeOut, factionKey, player);
			}
		}
		else
		{
			SCR_DC_Log.Add("[SCR_DC_HintHelper:ShowHint] Could not send hints.", LogLevel.ERROR);
		}*/
	}
	
	/*
 	static void ShowGlobalPopup(string hl, string msg, int dur, array<string> fkeys)
    {
        Rpc(RpcDo_ShowPopup, hl, msg, dur, fkeys); // broadcast to clients
        RpcDo_ShowPopup(hl, msg, dur, fkeys); // try to show on authority
    }
    
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_ShowPopup(string hl, string msg, int dur, array<string> fkeys)
    {
        if (fkeys && !fkeys.IsEmpty()) {
            SCR_FactionManager fm = SCR_FactionManager.Cast(GetGame().GetFactionManager());
            if (!fm) return;
            Faction f = fm.GetLocalPlayerFaction();
            if (!f) return;
            if (fkeys.Contains(f.GetFactionKey())) return;
        }
        SCR_PopUpNotification.GetInstance().PopupMsg(hl, dur, msg);
    }
	*/	
}
