//Modded/SCR_DC_Core_BaseGameMode.c
modded class SCR_BaseGameMode 
{
	//private 
	private SCR_DC_DebugEntity m_DebugEntity;
	
    override void OnGameStart()
    {
        super.OnGameStart();
		
		SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode:OnGameStart]", LogLevel.NORMAL);
		
		if(IsMaster())
		{
			SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode:IsMaster] OnGameStart", LogLevel.DEBUG);        
			
			//Initialize the SCR_DC_DebugEntity
			SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode] Creating SCR_DC_DebugEntity", LogLevel.NORMAL);        
			m_DebugEntity = SCR_DC_DebugEntity.Cast(GetGame().SpawnEntity(SCR_DC_DebugEntity, GetGame().GetWorld(), null));
		}
		else 
		{
			SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode:NonMaster] Core not needed for client.", LogLevel.DEBUG);        
		}
    }
};
