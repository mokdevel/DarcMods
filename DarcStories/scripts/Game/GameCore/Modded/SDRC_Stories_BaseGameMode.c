//SDRC_Missions_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	ref SDRC_StoriesFrame storiesFrame = null;
	
	//------------------------------------------------------------------------------------------------
    override void OnGameStart()
    {
        super.OnGameStart();
		
		if (SDRC_Conf.SDRC_ENABLE_DARCSTORIES)
		{	
			SDRC_Log.Add("[SDRC_Stories] Starting..", LogLevel.NORMAL);					
	
			if (!SDRC_Conf.RELEASE)
			{
				SDRC_Log.Add("[SDRC_Stories_BaseGameMode] SDRC_RELEASE not defined. This is a DEVELOPMENT build.", LogLevel.WARNING);
			}
					
			if (IsMaster())
			{
				SDRC_Log.Add("[SDRC_Stories_BaseGameMode:IsMaster] OnGameStart", LogLevel.DEBUG);        
				GetGame().GetCallqueue().CallLater(StartStoriesFrame, 5000, false);	
			}
			else 
			{
				SDRC_Log.Add("[SDRC_Stories_BaseGameMode:NonMaster] Mission frame not needed for client.", LogLevel.DEBUG);        
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_Stories] Not started. Development build?", LogLevel.ERROR);
		}		
    }

	//------------------------------------------------------------------------------------------------
	private void StartStoriesFrame()
	{
		if (SDRC_Conf.coreHasStarted)	//Wait for core to be available
		{		
			storiesFrame = new SDRC_StoriesFrame();
		}
		else
		{
			GetGame().GetCallqueue().CallLater(StartStoriesFrame, 2000, false);	
			SDRC_Log.Add("[SDRC_MissionFrame_BaseGameMode:StartStoriesFrame] Core not running. Waiting...", LogLevel.DEBUG);
		}
	}
};
