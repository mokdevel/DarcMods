//Modded/SCR_DC_Core_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	private ref SDRC_LoadCtx m_LoadCtx = null;
	
    override void OnGameStart()
    {
        super.OnGameStart();
		
		Print("[SCR_DC_Ctx_BaseGameMode:OnGameStart]", LogLevel.DEBUG);
		
		if(IsMaster())
		{
			m_LoadCtx = new SDRC_LoadCtx();
			m_LoadCtx.Init();
			Print("[SCR_DC_Ctx_BaseGameMode:IsMaster] OnGameStart", LogLevel.DEBUG);
			
		}
		else 
		{
			Print("[SCR_DC_Ctx_BaseGameMode:NonMaster] SCR_DC_CtxEntity not needed for client.", LogLevel.DEBUG);        
		}
    }
};
