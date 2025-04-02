//Modded/SCR_DC_Core_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	//private 
	private RplDCHint m_HintEntity;

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
			
			Resource resource = Resource.Load("{924FD1167E7B26EA}Prefabs/Helpers/RPLHintHelper.et");
			if (!resource.IsValid())
				return;
	
			m_HintEntity = RplDCHint.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), null));
		}
		else 
		{
			Print("[SCR_DC_Hint_BaseGameMode:NonMaster] Core not needed for client.", LogLevel.DEBUG);        
		}
    }
};
