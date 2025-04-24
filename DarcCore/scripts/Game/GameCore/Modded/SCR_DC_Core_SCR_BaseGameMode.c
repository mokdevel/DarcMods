//Modded/SCR_DC_Core_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	//private 
	private ref SCR_DC_Core m_SCR_DC_Core;
	private SCR_DC_DebugEntity m_SCR_DC_DebugEntity;
	private SCR_DC_RplHintEntity m_SCR_DC_RplHintEntity;
	
    override void OnGameStart()
    {
        super.OnGameStart();
		
		SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode:OnGameStart]", LogLevel.DEBUG);
		
		if(IsMaster())
		{
			SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode:IsMaster] OnGameStart", LogLevel.SPAM);        

			//Initialize core
			m_SCR_DC_Core = new SCR_DC_Core();
					
			//Initialize the SCR_DC_DebugEntity
			SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode] Creating SCR_DC_DebugEntity", LogLevel.NORMAL);        
			m_SCR_DC_DebugEntity = SCR_DC_DebugEntity.Cast(GetGame().SpawnEntity(SCR_DC_DebugEntity, GetGame().GetWorld(), null));

			//Initialize the SCR_DC_HintEntity
			SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode] Creating SCR_DC_HintEntity", LogLevel.NORMAL);
			Resource resource = Resource.Load("{CF2D9888CE3C5AD7}Prefabs/Helpers/RPLHintHelper.et");
			if (!resource.IsValid())
			{
				SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode] Failed to create SCR_DC_RplHintEntity.", LogLevel.ERROR);
				return;
			}
	
			m_SCR_DC_RplHintEntity = SCR_DC_RplHintEntity.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld()));
		}
		else 
		{
			SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode:NonMaster] Core not needed for client.", LogLevel.DEBUG);        
		}		
    }	
}