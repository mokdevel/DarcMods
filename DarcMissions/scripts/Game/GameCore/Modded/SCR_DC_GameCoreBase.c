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
				
		if(IsMaster())
		{
			SCR_DC_Log.Add("[SCR_DC_GameCoreBase:IsMaster] OnGameStart", LogLevel.NORMAL);        

			//Initialize the SCR_DC_MapMarkerEntity
			SCR_DC_Log.Add("[SCR_DC_GameCoreBase:OnGameStart] Creating SCR_DC_MapMarkerEntity", LogLevel.NORMAL);        
			string m_PrefabToSpawn = "{F158EB543483DC6A}Prefabs/Markers/SCR_DC_MapMarker.et";
			Resource m_Resource = Resource.Load(m_PrefabToSpawn);
			
			vector pos = "913 39 2624";
			vector transform[4];
			EntitySpawnParams params = EntitySpawnParams();
			SCR_DC_SpawnHelper.GetTransformFromPosAndRot(transform, pos, 0);
			params.Transform = transform;
			
			m_MarkerEntity = GetGame().SpawnEntityPrefab(m_Resource, GetGame().GetWorld(), params);		
				
			missionFrame = new SCR_DC_MissionFrame();
			missionFrame.MissionFrameStart();
		}
		else 
		{
			SCR_DC_Log.Add("[SCR_DC_GameCoreBase:NonMaster] Mission frame not needed for client.", LogLevel.NORMAL);        
		}
    }
};
