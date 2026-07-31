//Modded/SDRC_Core_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	//private 
	ref SDRC_Core m_SDRC_Core;
	private SDRC_DebugEntity m_SDRC_DebugEntity;
	private SDRC_RplHintEntity m_SDRC_RplHintEntity;
	
    override void OnGameModeStart()
    {
        super.OnGameModeStart();
		
		SDRC_Log.Add("[SDRC_ChopperCompatFF] Starting..", LogLevel.NORMAL);					
		SDRC_Log.Add("[SDRC_SDRC_ChopperCompatFF_BaseGameMode:OnGameModeStart]", LogLevel.DEBUG);
		
		if (IsMaster())
		{
			SDRC_Log.Add("[SDRC_SDRC_ChopperCompatFF_BaseGameMode:IsMaster] OnGameModeStart", LogLevel.SPAM);        
			GetGame().GetCallqueue().CallLater(StartChopperCompatFF, 1000, false);			
		}
		else 
		{
			SDRC_Log.Add("[SDRC_Core_BaseGameMode:NonMaster] SDRC_ChopperCompatFF not needed for client.", LogLevel.DEBUG);
		}		
    }
	
	//------------------------------------------------------------------------------------------------
	private void StartChopperCompatFF()
	{
		if (SDRC_Conf.coreHasStarted)	//Wait for core to be available
		{		
			//Load the conf once to get it on file system
			SDRC_ChopperCompatFFConfig.LoadConfOnce();
		}
		else
		{
			GetGame().GetCallqueue().CallLater(StartChopperCompatFF, 5000, false);	
			SDRC_Log.Add("[SDRC_Chopper_BaseGameMode:StartChopperCompatFF] Core not running. Waiting...", LogLevel.DEBUG);
		}
	}
}