//------------------------------------------------------------------------------------------------
/*!
This is the main Core file. 
*/

//------------------------------------------------------------------------------------------------
class SDRC_Core
{
	private ref SDRC_CoreJsonApi m_DC_CoreConfig = new SDRC_CoreJsonApi();	
	private ref SDRC_CoreConfig m_Config;	
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Core()
	{
		SDRC_Log.Add("[SDRC_Core] Starting SDRC_Core", LogLevel.NORMAL);
		
		//Load configuration from file
		m_DC_CoreConfig.Load();
		m_Config = m_DC_CoreConfig.conf;

		SDRC_Log.SetLogLevel(m_Config.logLevel);
		SDRC_AIHelper.SetDefaultEnemyFaction(m_Config.defaultEnemyFaction);

		GetGame().GetCallqueue().CallLater(FillBuildingCache, 2000, false);			
		
		//Initialize LootHelper
		SDRC_LootHelper.Setup();
		
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
	bool debugShowWaypoints = true;
	bool debugShowMarks = true;	
	bool debugShowSpheres = true;
	string defaultEnemyFaction = "USSR";
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
		conf.buildingExcludeFilter = {
			"BrickPile", "WoodPile", "Hotbed", "Henhouse", "PhoneBooth",
			"AmmoDump", "ElectricCabinet", "ControlBox110kV", "LightBeacon",
			"PierConcrete", "PierWooden", "Pier_", "SeaBollard", 
			"ConcreteRetainingWall", "StoneRetainingWall", "ConcreteBridge",
			"Calvary", "Castle_", "Davit_",
			"GraveStone", "Grave_", "GraveFence",
			"MooringDolphin",
			"HouseRuin",
		}
	}
};