//Modded/SDRC_Spawner_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	//private 
	ref SDRC_Spawner dcSpawner;

    override void OnGameStart()
    {
	    super.OnGameStart();
		
		if (SDRC_Conf.SDRC_ENABLE_DARCSPAWNER)
		{
			
			SDRC_Log.Add("[SDRC_Spawner] Starting..", LogLevel.NORMAL);		
			SDRC_Log.Add("[SDRC_Spawner_BaseGameMode:OnGameStart]", LogLevel.DEBUG);
			
			if (!SDRC_Conf.RELEASE)
			{
				SDRC_Log.Add("[SDRC_Spawner_BaseGameMode] SDRC_Core.RELEASE not true. This is a DEVELOPMENT build.", LogLevel.WARNING);
			}
			
			if (IsMaster())
			{
				SDRC_Log.Add("[SDRC_Spawner_BaseGameMode:IsMaster] OnGameStart", LogLevel.DEBUG);        
				GetGame().GetCallqueue().CallLater(StartSpawner, 15000, false);	
			}
			else 
			{
				SDRC_Log.Add("[SDRC_Spawner_BaseGameMode:NonMaster] Spawner not needed for client.", LogLevel.DEBUG);        
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_Spawner] Not started. Development build?", LogLevel.ERROR);
		}
    }
	
	override void OnPlayerSpawned(int playerId, IEntity controlledEntity)	
	{
		super.OnPlayerSpawned(playerId, controlledEntity);
		
		SCR_MapMarkerManagerComponent mapMarkerMgr = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		if (mapMarkerMgr)
			mapMarkerMgr.SetStreamRulesForPlayer(playerId);
		
		SDRC_Log.Add("[SDRC_Spawner_BaseGameMode] Player spawned", LogLevel.DEBUG);
	}
	
	private void StartSpawner()
	{		
		if (SDRC_Conf.coreHasStarted)
		{		
			dcSpawner = new SDRC_Spawner();
			dcSpawner.Run();
		}
		else
		{
			GetGame().GetCallqueue().CallLater(StartSpawner, 2000, false);	
			SDRC_Log.Add("[SDRC_Spawner_BaseGameMode:StartSpawner] Core not running. Waiting...", LogLevel.DEBUG);
		}
	}
};
