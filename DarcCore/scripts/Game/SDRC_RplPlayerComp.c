class SDRC_RplPlayerCompClass : ScriptComponentClass { }
SDRC_RplPlayerCompClass g_RplPlayerCompClass;

//------------------------------------------------------------------------------------------------
class SDRC_RplPlayerComp : ScriptComponent
{
	private static SDRC_RplPlayerComp s_Instance;	
	 
    override void OnPostInit(IEntity owner)
    {
        auto hintEnt = SDRC_RplHintEntity.Cast(owner);
        if (!hintEnt)
        {
            SDRC_Log.Add("[SDRC_RplPlayerComp] Entity not found.", LogLevel.ERROR);
            return;
        }
				
		BaseRplComponent rplComponent = BaseRplComponent.Cast(hintEnt.FindComponent(BaseRplComponent));
        if (!rplComponent)
        {
            SDRC_Log.Add("[SDRC_RplPlayerComp] RplComponent not found.", LogLevel.ERROR);
            return;
        }
 
		s_Instance = this;				
    }
 
	//------------------------------------------------------------------------------------------------
	//! Get instance owned by local player
	static SDRC_RplPlayerComp FindLocalInstance()
	{
		PlayerController playerController = GetGame().GetPlayerController();
		
		if (!playerController)
		{
			return null;
		}
		
		return SDRC_RplPlayerComp.Cast(playerController.FindComponent(SDRC_RplPlayerComp));
	}	
	
	//------------------------------------------------------------------------------------------------
	//!	Return instance to component
	static SDRC_RplPlayerComp GetInstance()
	{
		return s_Instance;
	}

	//------------------------------------------------------------------------------------------------
 	void AskForInfo()
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();		
		Rpc(RpcAsk_GiveMeInfo, playerId);
	}
	
	//------------------------------------------------------------------------------------------------
 	void AskForMissionDeletion(string missionId)
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();		
		Rpc(RpcAsk_DeleteMission, playerId, missionId);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Client requests for information
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_GiveMeInfo(int playerID)
    {
		SDRC_Log.Add("[SDRC_RplPlayerComp:RpcAsk_GiveMeInfo] Asked by: " + playerID, LogLevel.SPAM);	

		SDRC_RplGMComp gmComp = SDRC_RplGMComp.FindInstance();
		if (gmComp)
		{
			gmComp.SyncMapSymbols(playerID);
		}
    }
	
	//------------------------------------------------------------------------------------------------
	//! Client requests for mission deletion
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_DeleteMission(int playerID, string missionId)
    {
		SDRC_Log.Add("[SDRC_RplPlayerComp:RpcAsk_DeleteMission] Asked by: " + playerID, LogLevel.SPAM);	

		SDRC_RplGMComp gmComp = SDRC_RplGMComp.FindInstance();
		if (gmComp)
		{
			gmComp.DoDeleteMission(playerID, missionId);
		}
    }	
}