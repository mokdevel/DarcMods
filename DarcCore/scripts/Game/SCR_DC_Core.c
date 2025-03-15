//------------------------------------------------------------------------------------------------
/*!
This is the main Core file. 

Add this to your StartGameTrigger or use SCR_DC_GameCoreBase.c

	//------------------------------------------------------------------------------------------------
	// user script
	private bool m_GameHasStarted = false;
	private ref SCR_DC_Core DCCore;

	override void EOnActivate(IEntity owner)
	{
		if (!m_GameHasStarted)
		{
			m_GameHasStarted = true;		
			DCCore = new SCR_DC_Core(true);
			DCCore.Run();
		}
	}
	//------------------------------------------------------------------------------------------------
*/
//------------------------------------------------------------------------------------------------
class SCR_DC_Core
{
	private ref SCR_DC_CoreJsonApi m_DC_CoreConfig = new SCR_DC_CoreJsonApi();	
	private ref SCR_DC_CoreConfig m_Config;	
	
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Core()
	{
		SCR_DC_Log.Add("[SCR_DC_Core] Starting SCR_DC_Core", LogLevel.NORMAL);
		
		//Load configuration from file
		m_DC_CoreConfig.Load();
		m_Config = m_DC_CoreConfig.conf;

		SCR_DC_Log.SetLogLevel(m_Config.logLevel);			
		if (!SCR_DC_Conf.RELEASE)
		{
			SCR_DC_Log.SetLogLevel(DC_LogLevel.DEBUG);
		}			

		//Set debug visibility
		SCR_DC_DebugHelper.Configure(m_Config.debugShowWaypoints, m_Config.debugShowMarks);							
	}

	void ~SCR_DC_Core()
	{
		SCR_DC_Log.Add("[SCR_DC_Core] Stopping SCR_DC_Core", LogLevel.NORMAL);
	}
}