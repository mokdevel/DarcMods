//SCR_DC_GameCoreBase.c

modded class SCR_BaseGameMode 
{
	ref SCR_DC_MissionFrame missionFrame;

    override void OnGameStart()
    {
        super.OnGameStart();
		
		SCR_DC_Log.Add("[SCR_DC_GameCoreBase:OnGameStart]", LogLevel.NORMAL);

		if (!SCR_DC_Core.RELEASE)
		{
			SCR_DC_Log.Add("[SCR_DC_GameCoreBase] SCR_DC_Core.RELEASE not true. This is a DEVELOPMENT build.", LogLevel.WARNING);
		}
				
		if(IsMaster())
		{
			SCR_DC_Log.Add("[SCR_DC_GameCoreBase:IsMaster] OnGameStart", LogLevel.NORMAL);        
				
			missionFrame = new SCR_DC_MissionFrame();
//			missionFrame.MissionFrameStart();
		}
		else 
		{
			SCR_DC_Log.Add("[SCR_DC_GameCoreBase:NonMaster] Mission frame not needed for client.", LogLevel.DEBUG);        
		}
    }
};
