//Helpers SDRC_SpawnerJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

//------------------------------------------------------------------------------------------------
class SDRC_DeathMarkerConfig : SDRC_Config
{
	//Default information
	string author = "darc";
	//Spawner specific
	int markerLifeTime;				//The lifetime of the marker after which the 
	bool visibleOnlyToFaction;		//If true, the marker is only visible for other members in the same faction.
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_DeathMarkerConfig data = SDRC_DeathMarkerConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------

	override void SetDefaults()
	{		
		super.SetDefaults();	

		markerLifeTime = 600;
		visibleOnlyToFaction = false;
	}	
}
