//Modded/SDRC_Core_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	//private 
	ref SDRC_Core m_SDRC_Core;
	private SDRC_DebugEntity m_SDRC_DebugEntity;
	private SDRC_RplHintEntity m_SDRC_RplHintEntity;
	private SDRC_RplGMEntity m_SDRC_RplGMEntity;
	
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
					
			//Initialize the SDRC_DebugEntity
			SDRC_Log.Add("[SDRC_Core_BaseGameMode] Creating SDRC_DebugEntity", LogLevel.NORMAL);        
			m_SDRC_DebugEntity = SDRC_DebugEntity.Cast(GetGame().SpawnEntity(SDRC_DebugEntity, GetGame().GetWorld(), null));

			Resource resource;
			
			//Initialize the SDRC_HintEntity
			SDRC_Log.Add("[SDRC_Core_BaseGameMode] Creating SDRC_HintEntity", LogLevel.NORMAL);
			resource = Resource.Load("{CF2D9888CE3C5AD7}Prefabs/Helpers/RPLHintHelper.et");
			if (!resource.IsValid())
			{
				SDRC_Log.Add("[SDRC_Core_BaseGameMode] Failed to create RPLHintHelper.", LogLevel.ERROR);
				return;
			}
			m_SDRC_RplHintEntity = SDRC_RplHintEntity.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld()));


			//Core initialized properly
			SDRC_Conf.coreHasStarted = true;

#ifndef SDRC_RELEASE				
/*			//Initialize the SDRC_GMEntity
			SDRC_Log.Add("[SDRC_Core_BaseGameMode] Creating SDRC_GMEntity", LogLevel.NORMAL);
			resource = Resource.Load("{034174406440412B}Prefabs/Helpers/RPLGMHelper.et");
			if (!resource.IsValid())
			{
				SDRC_Log.Add("[SDRC_Core_BaseGameMode] Failed to create RPLGMHelper.", LogLevel.ERROR);
				return;
			}
			m_SDRC_RplGMEntity = SDRC_RplGMEntity.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld()));*/
#endif
									
			//For testing
			//GetGame().GetCallqueue().CallLater(DebugStart, 15000, false);	
		}
		else 
		{
			SDRC_Log.Add("[SDRC_Core_BaseGameMode:NonMaster] Core not needed for client.", LogLevel.DEBUG);
		}		
    }
	
	/*
	private void DebugStart()
	{		
		SDRC_Conf.coreHasStarted = true;
	}
	*/	
}