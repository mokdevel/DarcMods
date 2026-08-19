//------------------------------------------------------------------------------------------------
/*!
This is the main Core file. 
*/

//------------------------------------------------------------------------------------------------
class SDRC_EmptyPos : Managed
{
	int limit;
	//Obstruction object filters
	ref array<string> ignoreFilter = {};	//Ignore these classes or resourcename. Small items that do not obstruct.
	ref array<string> stopFilter = {};		//Large objects that will stop search immediately. Like big rocks.
	ref array<string> classFilter = {};		//Classes that obstruct and are counted towards the limit.
	ref array<string> objectFilter = {};	//Objects that obstruct and are counted towards the limit.
}

//------------------------------------------------------------------------------------------------
enum SDRC_ECoreScanState
{
	INIT,
	ENEMY,
	VEHICLE,
	LOOT,
	AMMO,
	BUILDINGCACHE,
	LOCATIONCACHE,
	AREACACHE,
	DUMPCACHE,
	READY
};

//------------------------------------------------------------------------------------------------
class SDRC_Core
{
	private const string DC_CONFIG_FILE_CORE = SDRC_Conf.CORE_CONFIG_FILE;
	private const int DC_CONFIG_FILE_CORE_JSONVER = 2;
	
	private ref SDRC_JsonApi2 m_DC_CoreJsonApi = new SDRC_JsonApi2(DC_CONFIG_FILE_CORE);	
	ref SDRC_CoreConfig m_Config = new SDRC_CoreConfig();	
	
	private bool m_bCoreStarted = false;
	private bool m_bCoreInitStarted = false;
	
	private bool m_bListScanReady = false;
	private int m_iListIndex = 0;
	private SDRC_ECoreScanState m_iScanState = SDRC_ECoreScanState.INIT;
	
	private ref array<string> m_sAddonList = {};
	private ref array<string> m_sFactionList = {};
	//------------------------------------------------------------------------------------------------
	void SDRC_Core()
	{
		SDRC_Log.Add("[SDRC_Core] Starting SDRC_Core", LogLevel.NORMAL);
		
		if ( SDRC_Misc.IsAddonLoaded("$DarcCore:") && SDRC_Misc.IsAddonLoaded("$DarcCoreDev:") )
		{
			//Duplicate core running. This shall not happen ever!
			SDRC_Log.Add("[SDRC_Core] ----------------------------------------------------------------", LogLevel.ERROR);
			SDRC_Log.Add("[SDRC_Core] You have two versions of DarcCore running (DEV/PROD). Aborting.", LogLevel.ERROR);
			SDRC_Log.Add("[SDRC_Core] This is most likely due to you having mods with dependencies to \n PROD while testing DEV versions.", LogLevel.ERROR);
			SDRC_Log.Add("[SDRC_Core] ----------------------------------------------------------------", LogLevel.ERROR);
			
			ShowFailure();
			return;
		}		
		
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

		//Set the full path
		SDRC_Conf.subDirPath = "$profile:/" + SDRC_Conf.CONF_DIRECTORY + "/" + SDRC_Conf.subDir + "/";
		
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
		
		int factionCount = SDRC_FactionHelper.GetFactionKeyList(m_sFactionList);
		SDRC_Log.Add("[SDRC_Core] Factions available: " + factionCount + " - " + m_sFactionList, LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] Fallback faction: " + m_Config.fallbackEnemyFaction, LogLevel.NORMAL);
		
		SDRC_Misc.GetAddonList(m_sAddonList, true);
		
		SDRC_Log.Add("[SDRC_Core] -------------------------------------", LogLevel.NORMAL);

//		GetGame().GetCallqueue().CallLater(FillBuildingCache, 2000, false);	//NOTE: If cache is to be filled, it is checked inside the function below
						
		//Set debug visibility
		SDRC_DebugHelper.Configure(m_Config.debugShowWaypoints, m_Config.debugShowMarks, m_Config.debugShowSpheres, m_Config.debugShowLines, m_Config.debugShowInfo);

		//Run the init - fill lists, fill caches, etc ... 
		InitLoop();
		
		//All good so far, core init has started.
		m_bCoreInitStarted = true;
			
//		GetGame().GetCallqueue().CallLater(IsCoreReady, 2000, false);	
	}

	void ~SDRC_Core()
	{
		SDRC_Log.Add("[~SDRC_Core] Stopping SDRC_Core", LogLevel.NORMAL);
	}
	
/*	//------------------------------------------------------------------------------------------------
	private void IsCoreReady()
	{
		//Wait for core to be ready with all stuff
		if (m_bCoreStarted)
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
	}		*/
	
	//------------------------------------------------------------------------------------------------
	bool IsCoreInitStarted()
	{
		return m_bCoreInitStarted;
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
	/*!
	Do scanning of loot, enemies, etc...
	*/		
	void InitLoop()
	{
		bool fullScan = false;
		bool allDone = false;
		
		//Limit where Loot and Ammo helper is needed
		if (    SDRC_Misc.IsAddonLoaded("$DarcMissions:") || SDRC_Misc.IsAddonLoaded("$DarcMissionsDev:") 
		     || SDRC_Misc.IsAddonLoaded("$DarcSpawner:") || SDRC_Misc.IsAddonLoaded("$DarcSpawnerDev:") 
		     || SDRC_Misc.IsAddonLoaded("$DarcChopperCompatFF:") || SDRC_Misc.IsAddonLoaded("$DarcChopperCompatFFDev:") 
		   )
		{
			fullScan = true;			
		}
	
		switch (m_iScanState)
		{
			case SDRC_ECoreScanState.INIT:
				//Add very specific startup things here
				m_iScanState++;
				break;
			case SDRC_ECoreScanState.ENEMY:
				//Initialize EnemyHelper
				if (SDRC_EnemyHelper.Scan(m_iListIndex, m_Config.fallbackEnemyFaction))
				{
					m_iListIndex = 0;
					m_iScanState++;
				}
				else
				{
					m_iListIndex++;
				}
				break;
			case SDRC_ECoreScanState.VEHICLE:
				if (fullScan)
				{
					//Initialize Vehicle list
					if (SDRC_VehicleListHelper.Scan(m_iListIndex))
					{
						m_iListIndex = 0;
						m_iScanState++;
					}
					else
					{
						m_iListIndex++;
					}
				}
				else //Move to next state if fullscan was not expected
				{
					m_iScanState++;
				}
				break;
			case SDRC_ECoreScanState.LOOT:
				if (fullScan)
				{
					//Initialize LootHelper
					if (SDRC_LootHelper.Scan(m_iListIndex))
					{
						m_iListIndex = 0;
						m_iScanState++;
					}
					else
					{
						m_iListIndex++;
					}
				}
				else //Move to next state if fullscan was not expected
				{
					m_iScanState++;
				}
				break;
			case SDRC_ECoreScanState.AMMO:
				if (fullScan)
				{
					//Initialize AmmoHelper
					SDRC_AmmoHelper.Setup();
					m_iScanState++;
				}
				else //Move to next state if fullscan was not expected
				{
					m_iScanState++;
				}
				break;
			case SDRC_ECoreScanState.BUILDINGCACHE:
				FillBuildingCache();	//This needs to be done first as buildings are needed in FillLocationCache() part.
				m_iScanState++;
				break;
			case SDRC_ECoreScanState.LOCATIONCACHE:
				FillLocationCache();
				m_iScanState++;
				break;
			case SDRC_ECoreScanState.AREACACHE:
				FillAreaCache();
				m_iScanState++;
				break;
			case SDRC_ECoreScanState.DUMPCACHE:
				SDRC_Locations.DumpLocationsCache();
				m_iScanState++;
				break;
			case SDRC_ECoreScanState.READY:
				allDone = true;
			
				//Everything related to core start up has been done! 
				SDRC_Conf.coreInitReady = true;
				SDRC_Log.Add("[SDRC_Core] Core init ready.", LogLevel.NORMAL);
				m_bCoreStarted = true;
				break;
		}
		
		if (!allDone)
		{
			GetGame().GetCallqueue().CallLater(InitLoop, SDRC_Conf.LIST_LOAD_DELAY, false);	
		}
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
	}
	
	//------------------------------------------------------------------------------------------------		
	void FillLocationCache()
	{
		//Location caching takes time, so don't do it, if not needed.
		if (SDRC_Locations.IsLocationCacheNeeded())
		{	
			//Initialize locations cache
			SDRC_Locations.FillLocationsCache(m_Config.locationAkas, m_Config.buildingAkas);
		}

		SDRC_Log.Add("[SDRC_Core:FillLocationCache] Done!", LogLevel.DEBUG);
	}	
	
	//------------------------------------------------------------------------------------------------		
	void FillAreaCache()
	{
		//Area caching takes time, so don't do it, if not needed. It is not needed if locations are not needed.
		if (SDRC_Locations.IsLocationCacheNeeded())
		{	
			//Initialize locations cache
			SDRC_Locations.FillAreaCache(m_Config.locationAkas, m_Config.buildingAkas);
		}

		SDRC_Log.Add("[SDRC_Core:FillAreaCache] Done!", LogLevel.DEBUG);
	}	
}