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
		
		SDRC_Log.Add("[SDRC_Core] Starting..", LogLevel.NORMAL);					
		SDRC_Log.Add("[SDRC_Core_BaseGameMode:OnGameStart]", LogLevel.DEBUG);
		
		if (IsMaster())
		{
			SDRC_Log.Add("[SDRC_Core_BaseGameMode:IsMaster] OnGameStart", LogLevel.SPAM);        

			//Initialize core
			m_SDRC_Core = new SDRC_Core();

			if (!m_SDRC_Core.IsCoreStarted())
			{
				return;
			}
								
			//Initialize the SDRC_DebugEntity
			SDRC_Log.Add("[SDRC_Core_BaseGameMode] Creating SDRC_DebugEntity", LogLevel.NORMAL);        
			m_SDRC_DebugEntity = SDRC_DebugEntity.Cast(GetGame().SpawnEntity(SDRC_DebugEntity, GetGame().GetWorld(), null));
			
			//Initialize the SDRC_HintEntity
			Resource resource;
			SDRC_Log.Add("[SDRC_Core_BaseGameMode] Creating SDRC_HintEntity", LogLevel.NORMAL);
			resource = Resource.Load("{CF2D9888CE3C5AD7}Prefabs/Helpers/RPLHintHelper.et");
			if (!resource.IsValid())
			{
				SDRC_Log.Add("[SDRC_Core_BaseGameMode] Failed to create RPLHintHelper.", LogLevel.ERROR);
				return;
			}
			m_SDRC_RplHintEntity = SDRC_RplHintEntity.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld()));
			SDRC_SpawnHelper.SetPersistence(m_SDRC_RplHintEntity, false);
												
			//For testing
			//GetGame().GetCallqueue().CallLater(DebugStart, 15000, false);	
			
			GetGame().GetCallqueue().CallLater(WaitingForCore, 2000, false);
		}
		else 
		{
			SDRC_Log.Add("[SDRC_Core_BaseGameMode:NonMaster] Core not needed for client.", LogLevel.DEBUG);
		}		
    }
	
	//------------------------------------------------------------------------------------------------
	private void WaitingForCore()
	{
		if (SDRC_Conf.coreInitReady)	//Wait for core to be available
		{		
			//Core initialized properly
			SDRC_Conf.coreHasStarted = true;
			SDRC_Log.Add("[SDRC_Core_BaseGameMode:WaitingForCore] Core has started.", LogLevel.NORMAL);
		}
		else
		{
			GetGame().GetCallqueue().CallLater(WaitingForCore, 2000, false);	
			SDRC_Log.Add("[SDRC_Core_BaseGameMode:WaitingForCore] Waiting for core init to finalize...", LogLevel.DEBUG);
		}
		
		GetGame().GetCallqueue().CallLater(ErrorReporter, 10000, true);		
	}	
	
	//------------------------------------------------------------------------------------------------
	private void ErrorReporter()
	{
		if (!SDRC_Conf.errorList.IsEmpty())
		{
			SDRC_Log.Add("[SDRC DarcMods:ErrorReporter] ------------------", LogLevel.ERROR);
			SDRC_Log.Add("[SDRC DarcMods:ErrorReporter] Errors found:", LogLevel.ERROR);
			foreach (string errorStr : SDRC_Conf.errorList)
			{
				SDRC_Log.Add("[SDRC DarcMods:ErrorReporter] - " + errorStr, LogLevel.ERROR);
			}
			SDRC_Log.Add("[SDRC DarcMods:ErrorReporter] ------------------", LogLevel.ERROR);
		}
	}
	
	/*
	private void DebugStart()
	{		
		SDRC_Conf.coreHasStarted = true;
	}
	*/	
}