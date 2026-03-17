//modded class SCR_EditorManagerEntity
/*class SDRC_SCR_EditorManagerEntityClass : SCR_EditorManagerEntityClass
{
}
*/
//class SDRC_SCR_EditorManagerEntity : SCR_EditorManagerEntity
modded class SCR_EditorManagerEntity
{
	//Line drawing related
	private CanvasWidget m_wCanvas;									//Canvas to draw the lines to
	
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
			//int ticktime = SDRC_Misc.GetCurrentTickTime();			
			int ticktime = System.GetTickCount();
			timer += ticktime - ticktimeOld;
			ticktimeOld = ticktime;
			
			if (timer < (REFRESH_TIME * 1000))
			{
				return isOpened;
			}
			
			AskForInfo();
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
			m_wCanvas.SetDrawCommands(drawCommands);
		}
		
		super.EOnFrame(owner, timeSlice);
	}	
	
	//------------------------------------------------------------------------------------------------
 	void AskForInfo()
	{
		if (GetGame().GetPlayerController())
		{
			int playerID = GetGame().GetPlayerController().GetPlayerId();		
			SDRC_Log.Add("[SDRC_SCR_EditorManagerEntity:AskForInfo] Asking: " + playerID, LogLevel.NORMAL);	
			Rpc(RpcAsk_GiveMeInfo, playerID);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Client requests for information
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_GiveMeInfo(int playerID)
    {
		SDRC_Log.Add("[SDRC_SCR_EditorManagerEntity:RpcAsk_GiveMeInfo] Asked by: " + playerID, LogLevel.NORMAL);	
		SyncLineData(playerID);
    }
	
	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Clear line data
	*/
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    protected void RpcDo_ClearLineData()
    {
		SDRC_Log.Add("[SDRC_SCR_EditorManagerEntity:RpcDo_ClearLineData] Clearing.. ", LogLevel.SPAM);
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
		SDRC_Log.Add("[SDRC_SCR_EditorManagerEntity:RpcDo_SyncLineData] Adding: " + pt, LogLevel.NORMAL);	
		m_lineData.Insert(pt);
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	[Server] Collect the helicopter data and sync to player.
	*/
 	void SyncLineData(int playerID)
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
		if (!SDRC_PlayerHelper.IsGMInterfaceVisible())
		{
			drawCommands.Clear();
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

		SDRC_Log.Add("[SDRC_SCR_EditorManagerEntity:SyncLineData] Starting..", LogLevel.NORMAL);	

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