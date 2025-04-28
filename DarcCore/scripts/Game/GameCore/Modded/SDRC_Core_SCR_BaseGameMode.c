//Modded/SDRC_Core_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	//private 
	private ref SDRC_Core m_SDRC_Core;
	private SDRC_DebugEntity m_SDRC_DebugEntity;
	private SDRC_RplHintEntity m_SDRC_RplHintEntity;
	
    override void OnGameStart()
    {
        super.OnGameStart();
		
		SDRC_Log.Add("[SDRC_Core_BaseGameMode:OnGameStart]", LogLevel.DEBUG);
		
		if (IsMaster())
		{
			SDRC_Log.Add("[SDRC_Core_BaseGameMode:IsMaster] OnGameStart", LogLevel.SPAM);        

			//Initialize core
			m_SDRC_Core = new SDRC_Core();
					
			//Initialize the SDRC_DebugEntity
			SDRC_Log.Add("[SDRC_Core_BaseGameMode] Creating SDRC_DebugEntity", LogLevel.NORMAL);        
			m_SDRC_DebugEntity = SDRC_DebugEntity.Cast(GetGame().SpawnEntity(SDRC_DebugEntity, GetGame().GetWorld(), null));

			//Initialize the SDRC_HintEntity
			SDRC_Log.Add("[SDRC_Core_BaseGameMode] Creating SDRC_HintEntity", LogLevel.NORMAL);
			Resource resource = Resource.Load("{CF2D9888CE3C5AD7}Prefabs/Helpers/RPLHintHelper.et");
			if (!resource.IsValid())
			{
				SDRC_Log.Add("[SDRC_Core_BaseGameMode] Failed to create SDRC_RplHintEntity.", LogLevel.ERROR);
				return;
			}
	
			m_SDRC_RplHintEntity = SDRC_RplHintEntity.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld()));
		}
		else 
		{
			SDRC_Log.Add("[SDRC_Core_BaseGameMode:NonMaster] Core not needed for client.", LogLevel.DEBUG);        
		}		
    }	
}