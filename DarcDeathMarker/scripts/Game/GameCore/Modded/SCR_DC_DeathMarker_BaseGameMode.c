//SCR_DC_DeathMarker_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	#ifdef SCR_DC_ENABLE_DARCDEATHMARKER
	//------------------------------------------------------------------------------------------------
    override void OnGameStart()
    {
        super.OnGameStart();
		
		SCR_DC_Log.Add("[SCR_DC_DeathMarker_BaseGameMode:OnGameStart]", LogLevel.DEBUG);

		if (!SCR_DC_Conf.RELEASE)
		{
			SCR_DC_Log.Add("[SCR_DC_DeathMarker_BaseGameMode] SCR_DC_RELEASE not defined. This is a DEVELOPMENT build.", LogLevel.WARNING);
		}
				
		if (IsMaster())
		{
			SCR_DC_Log.Add("[SCR_DC_DeathMarker_BaseGameMode:IsMaster] OnGameStart", LogLevel.DEBUG);        
//			GetGame().GetCallqueue().CallLater(StartMissionFrame, 5000, false);	
		}
		else 
		{
			SCR_DC_Log.Add("[SCR_DC_DeathMarker_BaseGameMode:NonMaster] Mission frame not needed for client.", LogLevel.DEBUG);        
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
				
		SCR_DC_Log.Add("[SCR_DC_DeathMarker_BaseGameMode: OnPlayerSpawned] Player spawned - id: " + playerId, LogLevel.DEBUG);
	}
*/	
	override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);		
		
		string playername = SCR_DC_PlayerHelper.GetPlayerName(playerId);
		
		if (playername != "")
		{
			SCR_DC_MapMarkerHelper.DeleteMarker(playername);
			SCR_DC_MapMarkerHelper.CreateMapMarker(playerEntity.GetOrigin(), DC_EMissionIcon.BLACK_X_SMALL, playername, playername);
		}
		
		SCR_DC_Log.Add("[SCR_DC_DeathMarker_BaseGameMode:OnPlayerKilled] Player died: " + playername, LogLevel.DEBUG);        
	}
	
	override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		super.OnPlayerDisconnected(playerId, cause, timeout);
		
		string playername = SCR_DC_PlayerHelper.GetPlayerName(playerId);
		
		if (playername != "")
		{
			SCR_DC_MapMarkerHelper.DeleteMarker(playername);
		}
			
		SCR_DC_Log.Add("[SCR_DC_DeathMarker_BaseGameMode:OnPlayerDisconnected] Player disconnected: " + playername, LogLevel.DEBUG);        
	}
			
	#endif
};
