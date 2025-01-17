//SCR_DC_GameCoreBase.c

IEntity m_MarkerEntity;	//TBD: Should be inside SCR_BaseGameMode

modded class SCR_BaseGameMode 
{
	//private 
	ref SCR_DC_MissionFrame missionFrame;

    override void OnGameStart()
    {
        super.OnGameStart();
		
		SCR_DC_Log.Add("[SCR_DC_GameCoreBase:OnGameStart]", LogLevel.NORMAL);
//		SCR_DC_Log.Add("[SCR_DC_GameCoreBase:OnGameStart] Worldname :" + SCR_DC_Misc.GetWorldName(), LogLevel.DEBUG);

		if (!SCR_DC_Core.RELEASE)
		{
			SCR_DC_Log.Add("[SCR_DC_GameCoreBase] SCR_DC_Core.RELEASE not true. This is a DEVELOPMENT build.", LogLevel.WARNING);
		}
				
		//Initialize the SCR_DC_MapMarkerEntity
		SCR_DC_Log.Add("[SCR_DC_GameCoreBase:OnGameStart] Creating SCR_DC_MapMarkerEntity", LogLevel.NORMAL);        
		string m_PrefabToSpawn = "{923122A12D5F2CF9}Prefabs/SCR_DC_MapMarker.et";
		Resource m_Resource = Resource.Load(m_PrefabToSpawn);
		m_MarkerEntity = GetGame().SpawnEntityPrefab(m_Resource, GetGame().GetWorld());		
		
		if(IsMaster())
		{
			SCR_DC_Log.Add("[SCR_DC_GameCoreBase:IsMaster] OnGameStart", LogLevel.NORMAL);        
	
			missionFrame = new SCR_DC_MissionFrame();
			missionFrame.MissionFrameStart();
						
//			GetGame().GetCallqueue().CallLater(TurnOnX, 5000, true);			
		}
		else 
		{
			SCR_DC_Log.Add("[SCR_DC_GameCoreBase:NonMaster] Mission frame not needed for client.", LogLevel.NORMAL);        
		}
    }
};
