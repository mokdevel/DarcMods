//SDRC_DeathMarker_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	#ifdef SDRC_ENABLE_DARCDEATHMARKER
	//------------------------------------------------------------------------------------------------
    override void OnGameStart()
    {
        super.OnGameStart();
		
		SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode:OnGameStart]", LogLevel.DEBUG);

		if (!SDRC_Conf.RELEASE)
		{
			SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode] SDRC_RELEASE not defined. This is a DEVELOPMENT build.", LogLevel.WARNING);
		}
				
		if (IsMaster())
		{
			SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode:IsMaster] OnGameStart", LogLevel.DEBUG);        
//			GetGame().GetCallqueue().CallLater(StartMissionFrame, 5000, false);	
		}
		else 
		{
			SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode:NonMaster] Mission frame not needed for client.", LogLevel.DEBUG);        
		}
    }
			
	//------------------------------------------------------------------------------------------------
	//TBD: Should use OnPlayerSpawnFinalize_S
/*	override void OnPlayerSpawned(int playerId, IEntity controlledEntity)	
	{
		super.OnPlayerSpawned(playerId, controlledEntity);
		
		//Set markers to stream to joining players
		SCR_MapMarkerManagerComponent mapMarkerMgr = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		if (mapMarkerMgr)
			mapMarkerMgr.SetStreamRulesForPlayer(playerId);
				
		SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode: OnPlayerSpawned] Player spawned - id: " + playerId, LogLevel.DEBUG);
	}
*/	
	override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		string playername = SDRC_PlayerHelper.GetPlayerName(playerId);
		
		if (playername != "")
		{
			SDRC_MapMarkerHelper.DeleteMarker(playername, true);
			SDRC_MapMarkerHelper.CreateMapMarker(playerEntity.GetOrigin(), DC_EMissionIcon.BLACK_X_SMALL, playername, playername, 10*60);
		}
		
		SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode:OnPlayerKilled] Player died: " + playername, LogLevel.DEBUG);        
		
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);		
	}
	#endif
};
