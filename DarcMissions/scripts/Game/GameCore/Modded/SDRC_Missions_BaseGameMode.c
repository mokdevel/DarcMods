//SDRC_Missions_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	ref SDRC_MissionFrame missionFrame;
	
	//------------------------------------------------------------------------------------------------
    override void OnGameStart()
    {
        super.OnGameStart();
		
		if (SDRC_Conf.SDRC_ENABLE_DARCMISSIONS)
		{	
			SDRC_Log.Add("[SDRC_Missions] Starting..", LogLevel.NORMAL);					
			SDRC_Log.Add("[SDRC_Missions_BaseGameMode:OnGameStart]", LogLevel.DEBUG);
	
			if (!SDRC_Conf.RELEASE)
			{
				SDRC_Log.Add("[SDRC_Missions_BaseGameMode] SDRC_RELEASE not defined. This is a DEVELOPMENT build.", LogLevel.WARNING);
			}
					
			if (IsMaster())
			{
				SDRC_Log.Add("[SDRC_Missions_BaseGameMode:IsMaster] OnGameStart", LogLevel.DEBUG);        
				GetGame().GetCallqueue().CallLater(StartMissionFrame, 5000, false);	
			}
			else 
			{
				SDRC_Log.Add("[SDRC_Missions_BaseGameMode:NonMaster] Mission frame not needed for client.", LogLevel.DEBUG);        
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_Missions] Not started. Development build?", LogLevel.ERROR);
		}		
    }

	//------------------------------------------------------------------------------------------------
	private void StartMissionFrame()
	{
		if (SDRC_Conf.coreHasStarted)	//Wait for core to be available
		{		
			missionFrame = new SDRC_MissionFrame();
		}
		else
		{
			GetGame().GetCallqueue().CallLater(StartMissionFrame, 2000, false);	
			SDRC_Log.Add("[SDRC_MissionFrame_BaseGameMode:StartMissionFrame] Core not running. Waiting...", LogLevel.DEBUG);
		}
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
				
		SDRC_Log.Add("[SDRC_Missions_BaseGameMode: OnPlayerSpawned] Player spawned - id: " + playerId, LogLevel.DEBUG);
	}
};
