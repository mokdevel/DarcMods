modded class SCR_EditorManagerEntity
{
	//Line drawing related
//	static BaseWorld m_World;
//	static WorkspaceWidget m_Workspace;
	private CanvasWidget m_wCanvas;									//Canvas to draw the lines to
	private TextWidget m_FPSWidget;
	
	//Synchronized linedata	
	ref array<vector> m_lineData = {};
	ref array<ref CanvasWidgetCommand> drawCommands = {};	//Line drawing commands			
	
	const int REFRESH_TIME = 1;	//seconds
	float timer;
	int ticktimeOld;
	
	//------------------------------------------------------------------------------------------------
	override bool IsOpened()
	{
		bool isOpened = super.IsOpened();
		
		if (!isOpened)
		{
			if (m_wCanvas)
			{
				//If not in GM mode, canvas is not needed.
				delete m_wCanvas;
			}
			return isOpened;			
		}
		
		if (isOpened)
		{
			int ticktime = SDRC_Misc.GetCurrentTickTime();			
			timer += ticktime - ticktimeOld;
			ticktimeOld = ticktime;
			
			if (timer < REFRESH_TIME)
			{
				return isOpened;
			}
			
			AskForInfo();
			SDRC_Log.Add("[SDRC_EditorManagerEntity:IsOpened] Editor opened.", LogLevel.SPAM);
			timer = 0;
		}
		
		if (m_wCanvas == null)
		{
			//Create a canvas to draw the lines. Keep it small as a big canvas will also capture the mouse which we don't want
			m_wCanvas = CanvasWidget.Cast(g_Game.GetWorkspace().CreateWidgetInWorkspace(WidgetType.CanvasWidgetTypeID, 0, 0, 10, 10, WidgetFlags.VISIBLE | WidgetFlags.IGNORE_CURSOR, new Color(0.0, 0.0, 0.0, 1.0), 1024));			
		}
				
		return isOpened;
	}	
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice) //--- Active only when the entity is local (see InitOwner())
	{
		if (m_wCanvas)
		{
			SDRC_LineDrawHelper.CreateDrawCommandsFromData(m_lineData, drawCommands);			
			if (!drawCommands.IsEmpty())
			{
				m_wCanvas.SetDrawCommands(drawCommands);
			}
		}
		
		super.EOnFrame(owner, timeSlice);
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
		SDRC_Log.Add("[SDRC_SCR_EditorManagerEntity:RpcAsk_GiveMeInfo] Asked by: " + playerID, LogLevel.SPAM);	

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
		SyncLineData(playerID);
    }
	
	//------------------------------------------------------------------------------------------------
    //! Server sends only to the owner of this component's entity
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_ReceivePrivateMessage(string msg)
    {
        Print(string.Format("SDRC_SCR_EditorManagerEntity:Private client message: %1", msg));
    }	
	
	//------------------------------------------------------------------------------------------------
	void SendMessageToPlayer(int playerID, string msg)
	{
	    PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(playerID);
	    if (!pc)
	        return;
				
	    Rpc(RpcDo_ReceivePrivateMessage, msg);
	}	

	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Clear symbols 
	*/
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    protected void RpcDo_ClearLines()
    {
		SDRC_Log.Add("[SDRC_SCR_EditorManagerEntity:RpcDo_ClearLines] Clearing.. ", LogLevel.SPAM);
		ClearLines();
    }	
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Clear symbols
	*/
	void ClearLines()
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
		SDRC_Log.Add("[SDRC_SCR_EditorManagerEntity:RpcDo_SyncLineData] Adding: " + pt, LogLevel.NORMAL);	
		m_lineData.Insert(pt);
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	[Server] Collect the helicopter data and sync to players.
	*/
 	void SyncLineData(int playerID)
	{
		SDRC_Log.Add("[SDRC_SCR_EditorManagerEntity:SyncLineData] Starting..", LogLevel.NORMAL);	
		
/*		//Clear lines on server
		ClearSymbols();*/
		//Clear lines on client
		Rpc(RpcDo_ClearLines);
		
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!m_BaseGameMode)
		{
			return;
		}
 		if (!m_BaseGameMode.chopperFrame)
		{
			return;
		}		
		
		array<vector> positions = {};
		IEntity chopper = m_BaseGameMode.chopperFrame.GetChopperEntity(0);
		if (!chopper)
		{
			return;
		}
		
		SDRC_ChopperDebug.CollectDestinationLines(chopper, positions);
		
		foreach (vector pos : positions)
		{
	        Rpc(RpcDo_SyncLineData, pos);
		}		
	}
		
}