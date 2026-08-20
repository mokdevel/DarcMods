//Modded/SDRC_ChopperExample_SCR_BaseGameMode.c

modded class SCR_BaseGameMode 
{		
	ref SDRC_ChopperExample_1 m_ChopperExample_1;
	ref SDRC_ChopperExample_2 m_ChopperExample_2;
	ref SDRC_ChopperExample_3 m_ChopperExample_3;
	ref SDRC_ChopperExample_4 m_ChopperExample_4;
	
	//------------------------------------------------------------------------------------------------
    override void OnGameModeStart()
    {
        super.OnGameModeStart();
		
		if (SDRC_Conf.SDRC_ENABLE_DARCCHOPPER)
		{	
			SDRC_Log.Add("[SDRC_ChopperExample] Starting..", LogLevel.NORMAL);					
	
			if (!SDRC_Conf.RELEASE)
			{
				SDRC_Log.Add("[SDRC_Chopper_BaseGameMode] SDRC_RELEASE not defined. This is a DEVELOPMENT build.", LogLevel.WARNING);
			}
					
			if (IsMaster())
			{				
				GetGame().GetCallqueue().CallLater(StartChopperExample, 1000, false);					
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
	
	//------------------------------------------------------------------------------------------------
	private void StartChopperExample()
	{
		if (SDRC_Conf.coreHasStarted)	//Wait for core to be available
		{		
//				m_ChopperExample_1 = new SDRC_ChopperExample_1();				
//				m_ChopperExample_2 = new SDRC_ChopperExample_2();				
//				m_ChopperExample_3 = new SDRC_ChopperExample_3();				
				m_ChopperExample_4 = new SDRC_ChopperExample_4();				
		}
		else
		{
			GetGame().GetCallqueue().CallLater(StartChopperExample, 5000, false);	
			SDRC_Log.Add("[SDRC_ChopperExample_SCR_BaseGameMode:StartChopperExample] Core not running. Waiting...", LogLevel.DEBUG);
		}
	}	
}