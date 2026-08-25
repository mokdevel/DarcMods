//SDRC_Missions_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	ref SDRC_MissionFrame missionFrame = null;
	private SDRC_RplGMEntity m_SDRC_RplGMEntity;
	
	//------------------------------------------------------------------------------------------------
	//Values needed for runtime SDRC_MissionSubIdxEditorAttributeBaseValues modifying. Ugly hack!
	//AR does not provide proper runtime editing of the editable values so trick the game. We use 
	//a global value that can be set and read in SDRC_MissionSubIdxEditorAttributeBaseValues
	//The table is filled in SDRC_MissionEnumHelper:CreateAllConfigs
	
	ref array<int> missionBigIndex = {0};	//The largest index found in the missionListMission. First item is prefilled to reflect SDRC_EMissionType.NONE
	int missionBigIndexForEdit = 0;			//The index to use for the specific 
	//------------------------------------------------------------------------------------------------
		
	//------------------------------------------------------------------------------------------------
    override void OnGameModeStart()
    {
        super.OnGameModeStart();
		
		if (SDRC_Conf.SDRC_ENABLE_DARCMISSIONS)
		{	
			SDRC_Log.Add("[SDRC_Missions] Starting..", LogLevel.NORMAL);					
			SDRC_Log.Add("[SDRC_Missions_BaseGameMode:OnGameModeStart]", LogLevel.DEBUG);
	
			if (!SDRC_Conf.RELEASE)
			{
				SDRC_Log.Add("[SDRC_Missions_BaseGameMode] SDRC_RELEASE not defined. This is a DEVELOPMENT build.", LogLevel.WARNING);
			}
					
			if (IsMaster())
			{
				//Initialize the SDRC_RplGMEntity
				Resource resource;				
				SDRC_Log.Add("[SDRC_Missions_BaseGameMode] Creating SDRC_RplGMEntity", LogLevel.NORMAL);
				resource = Resource.Load("{034174406440412B}Prefabs/Helpers/RPLGMHelper.et");
				if (!resource.IsValid())
				{
					SDRC_Log.Add("[SDRC_Missions_BaseGameMode] Failed to create SDRC_RplGMEntity.", LogLevel.ERROR);
					return;
				}
				m_SDRC_RplGMEntity = SDRC_RplGMEntity.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld()));
				SDRC_SpawnHelper.SetPersistence(m_SDRC_RplGMEntity, false);
				
				SDRC_Log.Add("[SDRC_Missions_BaseGameMode:IsMaster] OnGameModeStart", LogLevel.DEBUG);        
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
			SDRC_Log.Add("[SDRC_Missions_BaseGameMode:StartMissionFrame] Core not running. Waiting...", LogLevel.DEBUG);
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
