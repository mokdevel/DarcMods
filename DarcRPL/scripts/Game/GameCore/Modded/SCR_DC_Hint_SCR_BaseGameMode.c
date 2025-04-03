//Modded/SCR_DC_Core_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	//private 
	private SCR_DC_RplHint m_HintEntity;

    override void OnGameStart()
    {
        super.OnGameStart();
		
		Print("[SCR_DC_Hint_BaseGameMode:OnGameStart]", LogLevel.DEBUG);
		
		if(IsMaster())
		{
			Print("[SCR_DC_Hint_BaseGameMode:IsMaster] OnGameStart", LogLevel.SPAM);        
		
			//Initialize the SCR_DC_HintEntity
			Print("[SCR_DC_Hint_BaseGameMode] Creating SCR_DC_HintEntity", LogLevel.NORMAL);
			
	        EntitySpawnParams params = EntitySpawnParams();
	        params.TransformMode = ETransformMode.WORLD;			
//	        params.Transform[3] = "6000 0 6000";			
	        params.Transform[3] = "0 0 0";			
			
			Resource resource = Resource.Load("{924FD1167E7B26EB}Prefabs/Helpers/RPLHintHelper.et");
			if (!resource.IsValid())
			{
				Print("[SCR_DC_Hint_BaseGameMode] Failed to create SCR_DC_HintEntity", LogLevel.NORMAL);
				return;
			}
	
			m_HintEntity = SCR_DC_RplHint.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params));
		}
		else 
		{
			Print("[SCR_DC_Hint_BaseGameMode:NonMaster] SCR_DC_HintEntity not needed for client.", LogLevel.DEBUG);        
		}
    }
};
