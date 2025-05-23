//------------------------------------------------------------------------------------------------
/*!
This is the main Core file. 
*/

//------------------------------------------------------------------------------------------------
class SDRC_Core
{
	private ref SDRC_CoreJsonApi m_DC_CoreConfig = new SDRC_CoreJsonApi();	
	private ref SDRC_CoreConfig m_Config;	
	private ref array<string> m_sAddonList = {};
	private ref array<string> m_sFactionList = {};
	//------------------------------------------------------------------------------------------------
	void SDRC_Core()
	{
		SDRC_Log.Add("[SDRC_Core] Starting SDRC_Core", LogLevel.NORMAL);
		
		//Load configuration from file
		m_DC_CoreConfig.Load();
		m_Config = m_DC_CoreConfig.conf;

		SDRC_Log.SetLogLevel(m_Config.logLevel);

		//Print general information
		SDRC_Conf.subDir = m_Config.subDir;
		SDRC_Log.Add("[SDRC_Core] -------- General information --------", LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] Conf destination: $profile:/" + SDRC_Conf.CONF_DIRECTORY + "/" + SDRC_Conf.subDir, LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] World name: " + SDRC_Misc.GetWorldName(true), LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] World size: " + SDRC_Misc.GetWorldSize(), LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Core] World has ocean: " + GetGame().GetWorld().IsOcean(), LogLevel.NORMAL);
		int factionCount = SDRC_Misc.GetFactionList(m_sFactionList);
		SDRC_Log.Add("[SDRC_Core] Factions available: " + factionCount + " - " + m_sFactionList, LogLevel.NORMAL);
		SDRC_Misc.GetAddonList(m_sAddonList, true);
		SDRC_Log.Add("[SDRC_Core] -------------------------------------", LogLevel.NORMAL);

		SDRC_EnemyHelper.SetDefaultEnemyFaction(m_Config.fallbackEnemyFaction);
		
		GetGame().GetCallqueue().CallLater(FillBuildingCache, 2000, false);			
		
		//Initialize LootHelper
		SDRC_LootHelper.Setup();
		
		//Initialize EnemyHelper
		SDRC_EnemyHelper.Setup();
//		SDRC_EnemyHelper.SelectEnemy("G_REMNANTS", {"FIA"});
//		SDRC_EnemyHelper.SelectEnemy("G_REMNANTS", {"USSR"});
//		SDRC_EnemyHelper.SelectEnemy("G_REMNANTS", {"US"});
		
		//Set debug visibility
		SDRC_DebugHelper.Configure(m_Config.debugShowWaypoints, m_Config.debugShowMarks, m_Config.debugShowSpheres);							
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
	}
}
	
//------------------------------------------------------------------------------------------------
class SDRC_CoreConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	DC_LogLevel logLevel;
	string subDir;						//Directory specifying a certain conf for play. For example "Escapists"	
	bool debugShowWaypoints = true;
	bool debugShowMarks = true;	
	bool debugShowSpheres = true;
	string fallbackEnemyFaction = "USSR";
	ref array<string> buildingExcludeFilter = {};
}

//------------------------------------------------------------------------------------------------
class SDRC_CoreJsonApi : SDRC_JsonApi
{
	const string DC_CONFIG_FILE = "dc_coreConfig.json";
	ref SDRC_CoreConfig conf = new SDRC_CoreConfig();

	//------------------------------------------------------------------------------------------------
	void SDRC_SpawnerJsonConfig()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_CONFIG_FILE);
		
		if (!loadContext)
		{
			SetDefaults();
			Save();
			return;
		}
		
		loadContext.ReadValue("", conf);
	}	

	//------------------------------------------------------------------------------------------------
	void Save()
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen(DC_CONFIG_FILE);
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}	
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
//		conf.logLevel = DC_LogLevel.DEBUG;	
		conf.logLevel = SDRC_Conf.DEFAULT_LOGLEVEL;
		conf.subDir = SDRC_Conf.DEFAULT_DIR;		
		conf.buildingExcludeFilter = {
			"BrickPile", "WoodPile", "Hotbed", "Henhouse", "PhoneBooth",
			"AmmoDump", "ElectricCabinet", "ControlBox110kV", "LightBeacon",
			"PierConcrete", "PierWooden", "Pier_", "SeaBollard", 
			"ConcreteRetainingWall", "StoneRetainingWall", "ConcreteBridge",
			"Calvary", "Castle_", "Davit_",
			"GraveStone", "Grave_", "GraveFence",
			"MooringDolphin",
			"HouseRuin",
			//These from Kunar map
			"Wall_2", "Wall_5", "militarywall", "fort_rampart_", "CNCBlock", "MEStairs"
		}
	}
};