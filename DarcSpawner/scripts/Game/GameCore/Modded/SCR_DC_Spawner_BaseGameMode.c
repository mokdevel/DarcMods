//Modded/SCR_DC_Spawner_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	//private 
	ref SCR_DC_Spawner dcSpawner;

    override void OnGameStart()
    {
        super.OnGameStart();
		
		SCR_DC_Log.Add("[SCR_DC_Spawner_BaseGameMode:OnGameStart]", LogLevel.NORMAL);
		
		if (!SCR_DC_Core.RELEASE)
		{
			SCR_DC_Log.Add("[SCR_DC_Spawner_BaseGameMode] SCR_DC_Core.RELEASE not true. This is a DEVELOPMENT build.", LogLevel.WARNING);
		}
		
		if(IsMaster())
		{
			SCR_DC_Log.Add("[SCR_DC_Spawner_BaseGameMode:IsMaster] OnGameStart", LogLevel.NORMAL);        
			GetGame().GetCallqueue().CallLater(StartSpawner, 10000, false);	
		}
		else 
		{
			SCR_DC_Log.Add("[SCR_DC_Spawner_BaseGameMode:NonMaster] Spawner not needed for client.", LogLevel.NORMAL);        
		}
    }
	
	override void OnPlayerSpawned(int playerId, IEntity controlledEntity)	
	{
		super.OnPlayerSpawned(playerId, controlledEntity);
		
		SCR_MapMarkerManagerComponent mapMarkerMgr = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		if (mapMarkerMgr)
			mapMarkerMgr.SetStreamRulesForPlayer(playerId);
		
		SCR_DC_Log.Add("[SCR_DC_Spawner_BaseGameMode] Player spawned", LogLevel.DEBUG);
	}
	
	private void StartSpawner()
	{
		dcSpawner = new SCR_DC_Spawner();
		dcSpawner.Run();
	}
};
