//Modded/SDRC_ChopperExample_0SCR_BaseGameMode.c

modded class SCR_BaseGameMode 
{		
	ref SDRC_ChopperExample_01 m_ChopperExample_1;
	ref SDRC_ChopperExample_02 m_ChopperExample_2;
	ref SDRC_ChopperExample_03 m_ChopperExample_3;
	ref SDRC_ChopperExample_04 m_ChopperExample_4;
	ref SDRC_ChopperExample_05 m_ChopperExample_5;
	ref SDRC_ChopperExample_06 m_ChopperExample_6;
	//ref SDRC_ChopperExample_07 - NOTE: These are prefabs in the example world and do not contain any code. 
	//ref SDRC_ChopperExample_08 - NOTE: These are prefabs in the example world and do not contain any code.  
	ref SDRC_ChopperExample_09 m_ChopperExample_9;
	
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
//				m_ChopperExample_1 = new SDRC_ChopperExample_01();
//				m_ChopperExample_2 = new SDRC_ChopperExample_02();				
//				m_ChopperExample_3 = new SDRC_ChopperExample_03();
//				m_ChopperExample_4 = new SDRC_ChopperExample_04();				
//				m_ChopperExample_5 = new SDRC_ChopperExample_05();
//				m_ChopperExample_6 = new SDRC_ChopperExample_06();
				m_ChopperExample_9 = new SDRC_ChopperExample_09();
		}
		else
		{
			GetGame().GetCallqueue().CallLater(StartChopperExample, 2000, false);	
			SDRC_Log.Add("[SDRC_ChopperExample_0SCR_BaseGameMode:StartChopperExample] Core not running. Waiting...", LogLevel.DEBUG);
		}
	}	
}