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
				SDRC_Log.Add("[SDRC_EditorManagerEntity:IsOpened] Editor opened.", LogLevel.NORMAL);
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
		SDRC_Log.Add("[SDRC:SCR_EditorManagerEntity:RpcAsk_GiveMeInfo] Asked by: " + playerID, LogLevel.DEBUG);	

		SendMessageToPlayer(playerID, "Here is your message!");
    }
	
	//------------------------------------------------------------------------------------------------
    //! Server sends only to the owner of this component's entity
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_ReceivePrivateMessage(string msg)
    {
        Print(string.Format("Private client message: %1", msg));
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