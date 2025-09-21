//SDRC_DeathMarker_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	ref SDRC_DeathMarkerJsonApi m_DC_DeathMarkerConfig = new SDRC_DeathMarkerJsonApi();
	ref SDRC_DeathMarkerConfig m_Config;

	//------------------------------------------------------------------------------------------------
    override void OnGameStart()
    {
        super.OnGameStart();
		
		if (SDRC_Conf.SDRC_ENABLE_DARCDEATHMARKER)
		{
			SDRC_Log.Add("[SDRC_DeathMarker] Starting..", LogLevel.NORMAL);		
			SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode:OnGameStart]", LogLevel.DEBUG);
	
			if (!SDRC_Conf.RELEASE)
			{
				SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode] SDRC_RELEASE not defined. This is a DEVELOPMENT build.", LogLevel.WARNING);
			}
			
			//Load configuration from file
			m_DC_DeathMarkerConfig.Load();
			m_Config = m_DC_DeathMarkerConfig.conf;
		}
		else
		{
			SDRC_Log.Add("[SDRC_DeathMarker] Not started. Development build?", LogLevel.ERROR);
		}
		
    }
			
	//------------------------------------------------------------------------------------------------
	override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		string playername = SDRC_PlayerHelper.GetPlayerName(playerId);
		
		if (playername != "")
		{
			Faction faction = null;				//By default, marker is visible for everyone

			if (m_Config.visibleOnlyToFaction)
			{
				faction = SDRC_PlayerHelper.GetPlayerFaction(playerEntity);
			}
			
			SDRC_MapMarkerHelper.DeleteMarker(playername, true);
			SDRC_MapMarkerHelper.CreateMapMarker(playerEntity.GetOrigin(), DC_EMissionIcon.ICON_DEATHMARKER_SMALL_MAP, playername, playername, m_Config.markerLifeTime, faction: faction);
		}
		
		SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode:OnPlayerKilled] Player died: " + playername, LogLevel.DEBUG);        
		
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);		
	}
};
