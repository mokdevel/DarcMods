//Modded/SDRC_Core_BaseGameMode.c

modded class SCR_BaseGameMode 
{	
	ref SDRC_ChopperFrame chopperFrame;
	SDRC_RplLineDrawEntity m_SDRC_RplLineDrawEntity;
	SDRC_RplLineDrawComp m_SDRC_RplLineDrawComp;
	
	//------------------------------------------------------------------------------------------------
    override void OnGameStart()
    {
        super.OnGameStart();
		
		if (SDRC_Conf.SDRC_ENABLE_DARCCHOPPER)
		{	
			SDRC_Log.Add("[SDRC_Chopper] Starting..", LogLevel.NORMAL);					
			SDRC_Log.Add("[SDRC_Chopper_BaseGameMode:OnGameStart]", LogLevel.DEBUG);
	
			if (!SDRC_Conf.RELEASE)
			{
				SDRC_Log.Add("[SDRC_Chopper_BaseGameMode] SDRC_RELEASE not defined. This is a DEVELOPMENT build.", LogLevel.WARNING);
			}
					
			if (IsMaster())
			{				
				//Initialize the SDRC_RplLineDrawEntity
/*				SDRC_Log.Add("[SDRC_Chopper_BaseGameMode] Creating SDRC_RplLineDrawEntity", LogLevel.NORMAL);        
				m_SDRC_RplLineDrawEntity = SDRC_RplLineDrawEntity.Cast(GetGame().SpawnEntity(SDRC_RplLineDrawEntity, GetGame().GetWorld(), null));				
				SDRC_SpawnHelper.SetPersistence(m_SDRC_RplLineDrawEntity, false);
				
				SDRC_RplLineDrawComp m_SDRC_RplLineDrawComp = SDRC_RplLineDrawComp.Cast(m_SDRC_RplLineDrawEntity.FindComponent(SDRC_RplLineDrawComp));
				if (!m_SDRC_RplLineDrawComp)
				{
					SDRC_Log.Add("[SDRC_Chopper_BaseGameMode] SDRC_RplLineDrawComp not found.", LogLevel.ERROR);
				}*/

				//Initialize the SDRC_RplLineDrawEntity
				Resource resource;				
				SDRC_Log.Add("[SDRC_Missions_BaseGameMode] Creating SDRC_RplLineDrawEntity", LogLevel.NORMAL);
				resource = Resource.Load("{CEDB121F1881E0E3}Prefabs/Helpers/RPLLineDrawHelper.et");
				if (!resource.IsValid())
				{
					SDRC_Log.Add("[SDRC_Missions_BaseGameMode] Failed to create SDRC_RplLineDrawEntity.", LogLevel.ERROR);
					return;
				}
				m_SDRC_RplLineDrawEntity = SDRC_RplLineDrawEntity.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld()));
				SDRC_SpawnHelper.SetPersistence(m_SDRC_RplLineDrawEntity, false);				
								
				//Initialize the SDRC_EditorManager
/*				Resource resource;
				SDRC_Log.Add("[SDRC_Chopper_BaseGameMode] Creating SDRC_EditorManager", LogLevel.NORMAL);
				resource = Resource.Load("{CEDB121F1881E0E3}Prefabs/Helpers/RPLLineDrawHelper.et");
				if (!resource.IsValid())
				{
					SDRC_Log.Add("[SDRC_Chopper_BaseGameMode] Failed to create EditorManager.", LogLevel.ERROR);
					return;
				}				
				m_SCR_EditorManagerEntity = SCR_EditorManagerEntity.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld()));
				if (!m_SCR_EditorManagerEntity)
				{
					SDRC_Log.Add("[SDRC_Chopper_BaseGameMode] Failed to create EditorManager.", LogLevel.ERROR);
					return;
				}
				SDRC_SpawnHelper.SetPersistence(m_SCR_EditorManagerEntity, false);*/
				
				SDRC_Log.Add("[SDRC_Chopper_BaseGameMode:IsMaster] OnGameStart", LogLevel.DEBUG);        
				GetGame().GetCallqueue().CallLater(StartChopperFrame, 5000, false);	
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
	private void StartChopperFrame()
	{
		if (SDRC_Conf.coreHasStarted)	//Wait for core to be available
		{		
			chopperFrame = new SDRC_ChopperFrame();
		}
		else
		{
			GetGame().GetCallqueue().CallLater(StartChopperFrame, 2000, false);	
			SDRC_Log.Add("[SDRC_Chopper_BaseGameMode:StartChopperFrame] Core not running. Waiting...", LogLevel.DEBUG);
		}
	}
}