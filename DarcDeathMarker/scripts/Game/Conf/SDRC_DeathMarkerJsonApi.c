//Helpers SDRC_SpawnerJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

//------------------------------------------------------------------------------------------------
class SDRC_DeathMarkerConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Spawner specific
	int markerLifeTime;				//The lifetime of the marker after which the 
	bool visibleOnlyToFaction;		//If true, the marker is only visible for other members in the same faction.
}

//------------------------------------------------------------------------------------------------
class SDRC_DeathMarkerJsonApi : SDRC_JsonApi
{
	const string DC_CONFIG_FILE = "dc_deathMarkerConfig.json";
	ref SDRC_DeathMarkerConfig conf = new SDRC_DeathMarkerConfig();

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
		conf.markerLifeTime = 600;
		conf.visibleOnlyToFaction = false;
	}	
}
