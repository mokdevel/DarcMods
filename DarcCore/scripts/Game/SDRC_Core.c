//------------------------------------------------------------------------------------------------
/*!
This is the main Core file. 
*/

//------------------------------------------------------------------------------------------------
class SDRC_EmptyPos : Managed
{
	int limit;
	ref array<string> ignoreFilter = {};
	ref array<string> stopFilter = {};
	ref array<string> classFilter = {};
	ref array<string> objectFilter = {};
}

//------------------------------------------------------------------------------------------------
class SDRC_Core
{
	private const string DC_CONFIG_FILE_CORE = SDRC_Conf.CORE_CONFIG_FILE;
	private const int DC_CONFIG_FILE_CORE_VER = 1;
	
	private ref SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_CONFIG_FILE_CORE);	
	ref SDRC_CoreConfig m_Config = new SDRC_CoreConfig();	
	
	private ref array<string> m_sAddonList = {};
	private ref array<string> m_sFactionList = {};
	//------------------------------------------------------------------------------------------------
	void SDRC_Core()
	{
		SDRC_Log.Add("[SDRC_Core] Starting SDRC_Core", LogLevel.NORMAL);
		
		//Load configuration from file
		m_JsonApi.Load(m_Config, SDRC_Config.Cast(m_Config), DC_CONFIG_FILE_CORE_VER);

		SDRC_Log.SetLogLevel(m_Config.logLevel);

		//Print general information
		SDRC_Conf.subDir = m_Config.subDir;
		SDRC_Log.Add("[SDRC_Core] -------- General information --------", LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] Conf destination: $profile:/" + SDRC_Conf.CONF_DIRECTORY + "/" + SDRC_Conf.subDir, LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] World name: " + SDRC_Misc.GetWorldName(true), LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] World size: " + SDRC_Misc.GetWorldSize(), LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] World has ocean: " + GetGame().GetWorld().IsOcean(), LogLevel.NORMAL);
		
//		SCR_AIWorld aiWorld = SCR_AIWorld.Cast(GetGame().GetAIWorld());
		bool bAiWorld = false;
//		if (aiWorld)
		if (GetGame().GetAIWorld())
		{
			bAiWorld = true;
		}
		SDRC_Log.Add("[SDRC_Core] World has AIWorld: " + bAiWorld, LogLevel.NORMAL);
		
		bool bPersistence = false;
		if (PersistenceSystem.GetInstance())
		{
			bPersistence = true;
		}
		SDRC_Log.Add("[SDRC_Core] World has persistence: " + bPersistence, LogLevel.NORMAL);
		
		int factionCount = SDRC_FactionHelper.GetFactionList(m_sFactionList);
		SDRC_Log.Add("[SDRC_Core] Factions available: " + factionCount + " - " + m_sFactionList, LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] Fallback faction: " + m_Config.fallbackEnemyFaction, LogLevel.NORMAL);
		
		SDRC_Misc.GetAddonList(m_sAddonList, true);
		
		SDRC_Log.Add("[SDRC_Core] -------------------------------------", LogLevel.NORMAL);

		GetGame().GetCallqueue().CallLater(FillBuildingCache, 2000, false);			
		
		//Initialize LootHelper
		SDRC_LootHelper.Setup();
		
		//Initialize AmmoHelper
		SDRC_AmmoHelper.Setup();
		
		//Initialize EnemyHelper
		SDRC_EnemyHelper.Setup(m_Config.fallbackEnemyFaction);
		
		//Set debug visibility
		SDRC_DebugHelper.Configure(m_Config.debugShowWaypoints, m_Config.debugShowMarks, m_Config.debugShowSpheres, m_Config.debugShowLines, m_Config.debugShowInfo);
	}

	void ~SDRC_Core()
	{
		SDRC_Log.Add("[~SDRC_Core] Stopping SDRC_Core", LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------		
	void FillBuildingCache()
	{
		//Initialize building cache
		SDRC_BuildingHelper.FillBuildingsCache(m_Config.buildingExcludeFilter);
		GetGame().GetCallqueue().CallLater(FillLocationCache, 2000, false);			
	}
	
	//------------------------------------------------------------------------------------------------		
	void FillLocationCache()
	{
		//Initialize locations cache
		SDRC_Locations.FillLocationsCache(m_Config.locationAkas, m_Config.buildingAkas);

		//Core initialized properly
		SDRC_Conf.coreInitReady = true;
		SDRC_Log.Add("[SDRC_Core] Init ready.", LogLevel.NORMAL);		
	}	
}