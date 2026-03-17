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
	ref array<ref CanvasWidgetCommand> drawCommands = {};	//Line drawing commands			
	
	const int REFRESH_TIME = 1;	//seconds
	
	float timer;
	int ticktimeOld;
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)	
	{
		//Maybe find m_BaseGameMode and m_SDRC_RplLineDrawEntity here for cleaner code
		super.EOnInit(owner);
	}
	
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

//			SDRC_RplPlayerComp pcc = SDRC_RplPlayerComp.FindLocalInstance();
			SDRC_RplLineDrawComp rplLineDrawComp = SDRC_RplLineDrawComp.FindLocalInstance();
			if (rplLineDrawComp)
			{
				rplLineDrawComp.AskForInfo();
			}
			
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
			if (!SDRC_PlayerHelper.IsGMInterfaceVisible())
			{
				drawCommands.Clear();
			}
			else
			{
				
				SDRC_RplLineDrawComp rplLineDrawComp = SDRC_RplLineDrawComp.GetInstance();
				if (rplLineDrawComp)
				{
					SDRC_LineDrawHelper.CreateDrawCommandsFromData(rplLineDrawComp.m_lineData, drawCommands);
				}
			}
			
			m_wCanvas.SetDrawCommands(drawCommands);
		}
		
		super.EOnFrame(owner, timeSlice);
	}	
}