//SDRC_DeathMarker_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	#ifdef SDRC_ENABLE_DARCDEATHMARKER
	//------------------------------------------------------------------------------------------------
    override void OnGameStart()
    {
        super.OnGameStart();
		
		SDRC_Log.Add("[SDRC_MissionFrame] Starting SDRC_DeathMarker", LogLevel.NORMAL);		
		SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode:OnGameStart]", LogLevel.DEBUG);

		if (!SDRC_Conf.RELEASE)
		{
			SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode] SDRC_RELEASE not defined. This is a DEVELOPMENT build.", LogLevel.WARNING);
		}
				
		if (IsMaster())
		{
			SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode:IsMaster] OnGameStart", LogLevel.DEBUG);        
		}
		else 
		{
			SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode:NonMaster] Mission frame not needed for client.", LogLevel.DEBUG);        
		}
    }
			
	//------------------------------------------------------------------------------------------------
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
