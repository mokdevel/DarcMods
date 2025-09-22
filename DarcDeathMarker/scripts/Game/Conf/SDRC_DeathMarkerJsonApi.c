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
	ref SDRC_DeathMarkerConfig conf = new SDRC_DeathMarkerConfig();

	//------------------------------------------------------------------------------------------------
	void SDRC_DeathMarkerJsonApi(string fileName)
	{
		SetFileName(fileName);
	}
	
	//------------------------------------------------------------------------------------------------
	bool Load(bool createMissingFiles = true)
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(createMissingFiles);		
		if (!loadContext)
		{
			if (!createMissingFiles)
			{
				return false;
			}
			SetDefaults();
			Save();
			return true;
		}
		
		loadContext.ReadValue("", conf);
		return true;
	}	

	//------------------------------------------------------------------------------------------------
	void Save()
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen();
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
