//SCR_DC_Missions_GameCoreBase.c

modded class SCR_BaseGameMode 
{
	ref SCR_DC_MissionFrame missionFrame;
	
	#ifndef SCR_DC_DISABLE_DARCMISSIONS
	//------------------------------------------------------------------------------------------------
    override void OnGameStart()
    {
        super.OnGameStart();
		
		SCR_DC_Log.Add("[SCR_DC_GameCoreBase:OnGameStart]", LogLevel.DEBUG);

		if (!SCR_DC_Conf.RELEASE)
		{
			SCR_DC_Log.Add("[SCR_DC_GameCoreBase] SCR_DC_RELEASE not defined. This is a DEVELOPMENT build.", LogLevel.WARNING);
		}
				
		if (IsMaster())
		{
			SCR_DC_Log.Add("[SCR_DC_GameCoreBase:IsMaster] OnGameStart", LogLevel.DEBUG);        
			GetGame().GetCallqueue().CallLater(StartMissionFrame, 5000, false);	
		}
		else 
		{
			SCR_DC_Log.Add("[SCR_DC_GameCoreBase:NonMaster] Mission frame not needed for client.", LogLevel.DEBUG);        
		}
    }

	//------------------------------------------------------------------------------------------------
	private void StartMissionFrame()
	{
		missionFrame = new SCR_DC_MissionFrame();
	}
			
	//------------------------------------------------------------------------------------------------
	//TBD: Should use OnPlayerSpawnFinalize_S
	override void OnPlayerSpawned(int playerId, IEntity controlledEntity)	
	{
		super.OnPlayerSpawned(playerId, controlledEntity);
		
		//Set markers to stream to joining players
		SCR_MapMarkerManagerComponent mapMarkerMgr = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		if (mapMarkerMgr)
			mapMarkerMgr.SetStreamRulesForPlayer(playerId);
				
		SCR_DC_Log.Add("[SCR_DC_GameCoreBase: OnPlayerSpawned] Player spawned - id: " + playerId, LogLevel.DEBUG);
	}
	#endif
};
