//SDRC_DeathMarker_BaseGameMode.c

const string DC_CONFIG_FILE_DEATHMARKER = "dc_deathMarkerConfig.json";

modded class SCR_BaseGameMode 
{
	ref SDRC_DeathMarkerJsonApi m_DC_DeathMarkerConfig;// = new SDRC_DeathMarkerJsonApi(DC_CONFIG_FILE_DEATHMARKER);
	ref SDRC_DeathMarkerConfig m_Config = null;

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
			
			if (IsMaster())
			{			
				GetGame().GetCallqueue().CallLater(InitDeathMarker, 5000, false);
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_DeathMarker] Not started. Development build?", LogLevel.ERROR);
		}		
    }
			
	//------------------------------------------------------------------------------------------------
	private void InitDeathMarker()
	{
		if (SDRC_Conf.coreHasStarted)	//Wait for core to be available
		{		
			m_DC_DeathMarkerConfig = new SDRC_DeathMarkerJsonApi(DC_CONFIG_FILE_DEATHMARKER);
			//Load configuration from file
			bool success = m_DC_DeathMarkerConfig.Load();
			
			if (!success)
			{
				SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode:InitDeathMarker] Error loading " + DC_CONFIG_FILE_DEATHMARKER + ". SDRC_DeathMarker not started.", LogLevel.ERROR);
				return;
			}			
			
			m_Config = m_DC_DeathMarkerConfig.conf;
		}
		else
		{
			GetGame().GetCallqueue().CallLater(InitDeathMarker, 2000, false);	
			SDRC_Log.Add("[SDRC_DeathMarker_BaseGameMode:InitDeathMarker] Core not running. Waiting...", LogLevel.DEBUG);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		if (SDRC_Conf.SDRC_ENABLE_DARCDEATHMARKER && m_Config)
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
		}
		
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);		
	}
};
