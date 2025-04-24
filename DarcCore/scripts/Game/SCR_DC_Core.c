//------------------------------------------------------------------------------------------------
/*!
This is the main Core file. 
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

		GetGame().GetCallqueue().CallLater(FillBuildingCache, 2000, false);			
		
		//Initialize LootHelper
		SCR_DC_LootHelper.Setup();
		
		//Set debug visibility
		SCR_DC_DebugHelper.Configure(m_Config.debugShowWaypoints, m_Config.debugShowMarks, m_Config.debugShowSpheres);							
	}

	void ~SCR_DC_Core()
	{
		SCR_DC_Log.Add("[~SCR_DC_Core] Stopping SCR_DC_Core", LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------		
	void FillBuildingCache()
	{
		//Initialize building cache
		SCR_DC_BuildingHelper.FillBuildingsCache(m_Config.buildingExcludeFilter);
	}
}
	
//------------------------------------------------------------------------------------------------
class SCR_DC_CoreConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	DC_LogLevel logLevel;
	bool debugShowWaypoints = true;
	bool debugShowMarks = true;	
	bool debugShowSpheres = true;
	ref array<string> buildingExcludeFilter = {};
}

//------------------------------------------------------------------------------------------------
class SCR_DC_CoreJsonApi : SCR_DC_JsonApi
{
	const string DC_CONFIG_FILE = "dc_coreConfig.json";
	ref SCR_DC_CoreConfig conf = new SCR_DC_CoreConfig;

	//------------------------------------------------------------------------------------------------
	void SCR_DC_SpawnerJsonConfig()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_CONFIG_FILE);
		
		if(!loadContext)
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
		conf.logLevel = SCR_DC_Conf.DEFAULT_LOGLEVEL;
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