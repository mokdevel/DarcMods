//Modded/SDRC_Core_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	//private 
	ref SDRC_Core m_SDRC_Core;
	private SDRC_DebugEntity m_SDRC_DebugEntity;
	private SDRC_RplHintEntity m_SDRC_RplHintEntity;
	
    override void OnGameStart()
    {
        super.OnGameStart();
		
		SDRC_Log.Add("[SDRC_ChopperCompatFF] Starting..", LogLevel.NORMAL);					
		SDRC_Log.Add("[SDRC_SDRC_ChopperCompatFF_BaseGameMode:OnGameStart]", LogLevel.DEBUG);
		
		if (IsMaster())
		{
			//Load the conf once to get it on file system
			SDRC_ChopperCompatFFConfig.LoadConfOnce();
		}
		else 
		{
			SDRC_Log.Add("[SDRC_Core_BaseGameMode:NonMaster] SDRC_ChopperCompatFF not needed for client.", LogLevel.DEBUG);
		}		
    }
}