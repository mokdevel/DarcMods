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
	private const int DC_CONFIG_FILE_CORE_JSONVER = 2;
	
	private ref SDRC_JsonApi2 m_DC_CoreJsonApi = new SDRC_JsonApi2(DC_CONFIG_FILE_CORE);	
	ref SDRC_CoreConfig m_Config = new SDRC_CoreConfig();	
	
	private bool m_bCoreStarted = false;
	private bool m_bLocationCacheReady = false;
	private bool m_bBuildingCacheReady = false;
	
	private ref array<string> m_sAddonList = {};
	private ref array<string> m_sFactionList = {};
	//------------------------------------------------------------------------------------------------
	void SDRC_Core()
	{
		SDRC_Log.Add("[SDRC_Core] Starting SDRC_Core", LogLevel.NORMAL);
		
		//Load configuration from file
		bool success = m_DC_CoreJsonApi.Load(m_Config, SDRC_Config.Cast(m_Config), DC_CONFIG_FILE_CORE_JSONVER);
		
		if (!success)
		{
			ShowFailure();
			return;
		}			
		
		SDRC_Log.SetLogLevel(m_Config.logLevel);

		//Print general information
		SDRC_Conf.subDir = m_Config.subDir;

		//If defined, set subDir to the one we set from a CLI
		string subDirCLI;
		if (System.GetCLIParam("darcSubDir", subDirCLI))
		{
			SDRC_Conf.subDir = subDirCLI;
			SDRC_Log.Add("[SDRC_Core] Overriding subDir defined in dc_coreConfig.json with startup parameter " + subDirCLI, LogLevel.NORMAL);
		}

		//Check for road network
		bool hasRoadNetwork = false;
		RoadNetworkManager rnManager = SDRC_RoadHelper.GetRoadNetworkManager();
		if (rnManager)
		{
			hasRoadNetwork = true;
		}
		
		SDRC_Log.Add("[SDRC_Core] -------- General information --------", LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] Platform: " + SDRC_Misc.GetPlatformName(), LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] Conf destination: $profile:/" + SDRC_Conf.CONF_DIRECTORY + "/" + SDRC_Conf.subDir, LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] World name: " + SDRC_Misc.GetWorldName(true), LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] World size: " + SDRC_Misc.GetWorldSize(), LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] World has ocean: " + GetGame().GetWorld().IsOcean(), LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] World has road network: " + hasRoadNetwork, LogLevel.NORMAL);
		
		bool bAiWorld = false;
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

		GetGame().GetCallqueue().CallLater(FillBuildingCache, 2000, false);	//NOTE: If cache is to be filled, is checked in the function below

		//Limit where Loot and Ammo helper is needed
		if (    SDRC_Misc.IsAddonLoaded("$DarcMissions:") || SDRC_Misc.IsAddonLoaded("$DarcMissionsDev:") 
		     || SDRC_Misc.IsAddonLoaded("$DarcSpawner:") || SDRC_Misc.IsAddonLoaded("$DarcSpawnerDev:") 
		   )
		{
			//Initialize LootHelper
			SDRC_LootHelper.Setup();
			
			//Initialize AmmoHelper
			SDRC_AmmoHelper.Setup();
		}

		//Initialize EnemyHelper
		SDRC_EnemyHelper.Setup(m_Config.fallbackEnemyFaction);
		
		//Initialize Vehicle
		SDRC_VehicleListHelper.Setup();
				
		//Set debug visibility
		SDRC_DebugHelper.Configure(m_Config.debugShowWaypoints, m_Config.debugShowMarks, m_Config.debugShowSpheres, m_Config.debugShowLines, m_Config.debugShowInfo);

		//All good so far, core has started.
		m_bCoreStarted = true;
		
			
		GetGame().GetCallqueue().CallLater(IsCoreReady, 2000, false);	
		
	}

	void ~SDRC_Core()
	{
		SDRC_Log.Add("[~SDRC_Core] Stopping SDRC_Core", LogLevel.NORMAL);
	}

	
	//------------------------------------------------------------------------------------------------
	private void IsCoreReady()
	{
		//Wait for core to be ready with all stuff
		if (	m_bLocationCacheReady
		     && m_bBuildingCacheReady
		     && SDRC_LootHelper.IsReady()
		     && SDRC_AmmoHelper.IsReady()
		     && SDRC_EnemyHelper.IsReady()
		     && SDRC_VehicleListHelper.IsReady()
		   )
		{		
			//Core initialized properly
			SDRC_Conf.coreInitReady = true;
			SDRC_Log.Add("[SDRC_Core] Core init ready.", LogLevel.NORMAL);
		}
		else
		{
			GetGame().GetCallqueue().CallLater(IsCoreReady, 2000, false);	
			SDRC_Log.Add("[SDRC_Core] Waiting for core init to finalize...", LogLevel.DEBUG);
		}
	}		
	
	//------------------------------------------------------------------------------------------------
	bool IsCoreStarted()
	{
		return m_bCoreStarted;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Show a failed startup
	*/		
	protected void ShowFailure()
	{		
		SDRC_Log.SetLogLevel(DC_LogLevel.ERROR);
		SDRC_Log.Add("[SDRC_Core] ****** Could not initialize DarcCore. Check your logs. ******", LogLevel.ERROR);
		GetGame().GetCallqueue().CallLater(ShowFailure, 10000, false);
	}	
	
	//------------------------------------------------------------------------------------------------		
	void FillBuildingCache()
	{
		//Building caching takes time, so don't do it, if not needed.
		if (SDRC_BuildingHelper.IsBuildingCacheNeeded())
		{	
			//Initialize building cache
			SDRC_BuildingHelper.FillBuildingsCache(m_Config.buildingExcludeFilter);
		}
		SDRC_Log.Add("[SDRC_Core:FillBuildingCache] Done!", LogLevel.DEBUG);
		m_bBuildingCacheReady = true;
		
		//Scan for locations
		GetGame().GetCallqueue().CallLater(FillLocationCache, 2000, false);			
	}
	
	//------------------------------------------------------------------------------------------------		
	void FillLocationCache()
	{
		//Building caching takes time, so don't do it, if not needed.
		if (SDRC_Locations.IsLocationCacheNeeded())
		{	
			//Initialize locations cache
			SDRC_Locations.FillLocationsCache(m_Config.locationAkas, m_Config.buildingAkas);
		}

		SDRC_Log.Add("[SDRC_Core:FillLocationCache] Done!", LogLevel.DEBUG);
		m_bLocationCacheReady = true;
	}	
}