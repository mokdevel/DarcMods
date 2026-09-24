//Modded/SDRC_ChopperExample_0SCR_BaseGameMode.c

modded class SCR_BaseGameMode 
{		
	ref SDRC_ChopperExample_01 m_ChopperExample_01;
	ref SDRC_ChopperExample_02 m_ChopperExample_02;
	ref SDRC_ChopperExample_03 m_ChopperExample_03;
	ref SDRC_ChopperExample_04 m_ChopperExample_04;
	ref SDRC_ChopperExample_05 m_ChopperExample_05;
	ref SDRC_ChopperExample_06 m_ChopperExample_06;
	//ref SDRC_ChopperExample_07 - NOTE: These are prefabs in the example world and do not contain any code. 
	//ref SDRC_ChopperExample_08 - NOTE: These are prefabs in the example world and do not contain any code.  
	ref SDRC_ChopperExample_09 m_ChopperExample_09;
	ref SDRC_ChopperExample_10 m_ChopperExample_10;
	
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
				m_ChopperExample_01 = new SDRC_ChopperExample_01();
				m_ChopperExample_02 = new SDRC_ChopperExample_02();				
				m_ChopperExample_03 = new SDRC_ChopperExample_03();
				m_ChopperExample_04 = new SDRC_ChopperExample_04();				
				m_ChopperExample_05 = new SDRC_ChopperExample_05();
				m_ChopperExample_06 = new SDRC_ChopperExample_06();
				m_ChopperExample_09 = new SDRC_ChopperExample_09();
				m_ChopperExample_10 = new SDRC_ChopperExample_10();
		}
		else
		{
			GetGame().GetCallqueue().CallLater(StartChopperExample, 2000, false);	
			SDRC_Log.Add("[SDRC_ChopperExample_0SCR_BaseGameMode:StartChopperExample] Core not running. Waiting...", LogLevel.DEBUG);
		}
	}	
}