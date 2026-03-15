modded class SCR_EditorManagerEntity
{
	const int REFRESH_TIME = 5;	//seconds
	float timer;
	int ticktimeOld;
	
	override bool IsOpened()
	{
		bool isOpened = super.IsOpened();
		
		if (isOpened)
		{
			int ticktime = SDRC_Misc.GetCurrentTickTime();			
			timer += ticktime - ticktimeOld;
			ticktimeOld = ticktime;
			
			if (timer > REFRESH_TIME)
			{
				AskForInfo();
				SDRC_Log.Add("[SDRC_EditorManagerEntity:IsOpened] Editor opened.", LogLevel.SPAM);
				timer = 0;				
			}			
		}
		
		return isOpened;
	}	
	
	//------------------------------------------------------------------------------------------------
 	void AskForInfo()
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();		
		Rpc(RpcAsk_GiveMeInfo, playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Client requests for information
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_GiveMeInfo(int playerID)
    {
		SDRC_Log.Add("[SDRC:SCR_EditorManagerEntity:RpcAsk_GiveMeInfo] Asked by: " + playerID, LogLevel.SPAM);	

		string msg = "Count: ";
		
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!m_BaseGameMode)
		{
			return;
		}
 		if (!m_BaseGameMode.chopperFrame)
		{
			return;
		}
		
		int count = m_BaseGameMode.chopperFrame.GetChopperCount();		
		msg = msg + count;
		
		SendMessageToPlayer(playerID, msg);
    }
	
	//------------------------------------------------------------------------------------------------
    //! Server sends only to the owner of this component's entity
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_ReceivePrivateMessage(string msg)
    {
        Print(string.Format("SDRC:Private client message: %1", msg));
    }	
	
	//------------------------------------------------------------------------------------------------
	void SendMessageToPlayer(int playerID, string msg)
	{
	    PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(playerID);
	    if (!pc)
	        return;
				
	    Rpc(RpcDo_ReceivePrivateMessage, msg);
	}	
	
	
}