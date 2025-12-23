//Modded/SCR_DC_Marker_BaseGameMode.c

IEntity m_MarkerEntity;	//TBD: Should be inside SCR_BaseGameMode

modded class SCR_BaseGameMode 
{
	//private 
	private SCR_DC_DebugEntity m_DebugEntity;	
	private ref SCR_DC_Marker m_DC_Marker;
	
    override void OnGameStart()
    {
        super.OnGameStart();
		
		SCR_DC_Log.Add("[SCR_DC_Marker_BaseGameMode:OnGameStart]", LogLevel.NORMAL);
		
		if(IsMaster())
		{
			SCR_DC_Log.Add("[SCR_DC_Marker_BaseGameMode:IsMaster] OnGameStart", LogLevel.DEBUG);        

			//Initialize the SCR_DC_MapMarkerEntity
			SCR_DC_Log.Add("[SCR_DC_Marker_BaseGameMode] Creating SCR_DC_MapMarkerEntity", LogLevel.NORMAL);        
			string m_PrefabToSpawn = "{F158EB543483DC6A}Prefabs/Markers/SCR_DC_MapMarker.et";
			Resource m_Resource = Resource.Load(m_PrefabToSpawn);
			m_MarkerEntity = GetGame().SpawnEntityPrefab(m_Resource, GetGame().GetWorld());						
			
			//Initialize the Marker code
			m_DC_Marker = new SCR_DC_Marker();
		}
		else 
		{
			SCR_DC_Log.Add("[SCR_DC_Marker_BaseGameMode:NonMaster] Core not needed for client.", LogLevel.DEBUG);        
		}
    }
};
