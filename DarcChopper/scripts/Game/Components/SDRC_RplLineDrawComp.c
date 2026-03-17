//SDRC_RplLineDrawComp.c

//------------------------------------------------------------------------------------------------
/*!

*/
class SDRC_RplLineDrawCompClass : ScriptComponentClass { }
SDRC_RplLineDrawCompClass g_RplLineDrawCompClass;
 
//------------------------------------------------------------------------------------------------
class SDRC_RplLineDrawComp : ScriptComponent
{
	private static SDRC_RplLineDrawComp s_Instance;	

	ref array<vector> m_lineData = {};

	//------------------------------------------------------------------------------------------------
    override void OnPostInit(IEntity owner)
    {
        auto ent = SDRC_RplLineDrawEntity.Cast(owner);
        if (!ent)
        {
            SDRC_Log.Add("[SDRC_RplLineDrawComp] Entity not found.", LogLevel.ERROR);
            return;
        }
				
		BaseRplComponent rplComponent = BaseRplComponent.Cast(ent.FindComponent(BaseRplComponent));
        if (!rplComponent)
        {
            SDRC_Log.Add("[SDRC_RplLineDrawComp] RplComponent not found.", LogLevel.ERROR);
            return;
        }

		s_Instance = this;				
    }
 
	//------------------------------------------------------------------------------------------------
	//! Get instance owned by local player
	static SDRC_RplLineDrawComp FindLocalInstance()
	{
		PlayerController playerController = GetGame().GetPlayerController();
		
		if (!playerController)
		{
			return null;
		}
		
		return SDRC_RplLineDrawComp.Cast(playerController.FindComponent(SDRC_RplLineDrawComp));
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to component
	*/
	static SDRC_RplLineDrawComp GetInstance()
	{
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
 	void AskForInfo()
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();		
		SDRC_Log.Add("[SDRC_RplLineDrawComp:AskForInfo] Asking: " + playerId, LogLevel.NORMAL);	
		Rpc(RpcAsk_GiveMeInfo, playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Client requests for information
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_GiveMeInfo(int playerId)
    {
		SDRC_Log.Add("[SDRC_RplLineDrawComp:RpcAsk_GiveMeInfo] Asked by: " + playerId, LogLevel.NORMAL);	
		SyncLineData(playerId);
    }
	
	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Clear line data
	*/
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    protected void RpcDo_ClearLineData()
    {
		SDRC_Log.Add("[SDRC_RplLineDrawComp:RpcDo_ClearLineData] Clearing.. ", LogLevel.SPAM);
		ClearLineData();
    }	
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Clear line data
	*/
	void ClearLineData()
	{
		m_lineData.Clear();
	}	
		
	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Syncronize line data to client
	*/
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    protected void RpcDo_SyncLineData(vector pt)
    {
		SDRC_Log.Add("[SDRC_RplLineDrawComp:RpcDo_SyncLineData] Adding: " + pt, LogLevel.NORMAL);	
		m_lineData.Insert(pt);
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	[Server] Collect the helicopter data and sync to player.
	*/
 	void SyncLineData(int playerId)
	{		
		//Clear lines on client
		Rpc(RpcDo_ClearLineData);
		
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!m_BaseGameMode)
		{
			return;
		}
 		if (!m_BaseGameMode.chopperFrame)
		{
			return;
		}		
		
		array<IEntity> choppers = {};
		//IEntity chopper = m_BaseGameMode.chopperFrame.GetChopperEntity(0);
		m_BaseGameMode.chopperFrame.GetAllChopperEntity(choppers);
		//if (!chopper)
		if (choppers.IsEmpty())
		{
			return;
		}

		SDRC_Log.Add("[SDRC_RplLineDrawComp:SyncLineData] Starting..", LogLevel.NORMAL);	

		array<vector> positions = {};
		
		foreach (IEntity chopper : choppers)
		{
			SDRC_ChopperDebug.CollectDestinationLines(chopper, positions);
		}
		
		foreach (vector pos : positions)
		{
	        Rpc(RpcDo_SyncLineData, pos);
		}
		
//        Rpc(RpcDo_SyncLineData, pos);
	}
}