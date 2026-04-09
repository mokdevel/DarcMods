//Modded/SDRC_Core_BaseGameMode.c

modded class SCR_BaseGameMode 
{		
	ref SDRC_ChopperExample_1 m_ChopperExample_1;
	ref SDRC_ChopperExample_2 m_ChopperExample_2;
	
	//------------------------------------------------------------------------------------------------
    override void OnGameStart()
    {
        super.OnGameStart();
		
		if (SDRC_Conf.SDRC_ENABLE_DARCCHOPPER)
		{	
			SDRC_Log.Add("[SDRC_ChopperExample] Starting..", LogLevel.NORMAL);					
	
			if (!SDRC_Conf.RELEASE)
			{
				SDRC_Log.Add("[SDRC_Chopper_BaseGameMode] SDRC_RELEASE not defined. This is a DEVELOPMENT build.", LogLevel.WARNING);
			}
					
			if (IsMaster())
			{				
				m_ChopperExample_1 = new SDRC_ChopperExample_1();				
				m_ChopperExample_2 = new SDRC_ChopperExample_2();				
			}
			else 
			{
				SDRC_Log.Add("[SDRC_Chopper_BaseGameMode:NonMaster] Chopper frame not needed for client.", LogLevel.DEBUG);        
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_Chopper] Not started. Development build?", LogLevel.ERROR);
		}		
    }
}