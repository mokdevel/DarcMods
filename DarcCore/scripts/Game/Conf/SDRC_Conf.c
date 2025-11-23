//Helpers SDRC_Core.c

//------------------------------------------------------------------------------------------------
/*!
Stupid file to handle a few common variables accross multiple mods
*/

//#define SDRC_RELEASE
//#define EXPERIMENTAL
//#define NEW_VERSION_WIP		//New functionality (..that affects jsons)
#define NEW_PERSISTENCE

sealed class SDRC_Conf
{
	const string CONF_DIRECTORY = "DarcMods";
	const string CORE_CONFIG_FILE = "dc_coreConfig.json";
	static string subDir = "";
	static bool coreHasStarted = false;
	const int MISSION_RUN_DELAY = 5000;						//Time between INIT and SPAWN states.
	const int PERSISTENCE_DELAY = 6000;						//Time to delay the persistence setting for spawned entities. 
	const int DESPAWN_ENTITY_USED_FOR_SIZE_DELAY = 5000;	//Time to delay the deletion of an entity that was used for defining the size or weapon used for finding the mag.
	const int AI_SETTING_DELAY = 10000;
	const int POSITION_RANDOMIZATION = 150;					//Randomization for position. 
	
	//TBD: const int MISSION_END_TIME = 5000;				//Time to set for mission cycle when mission is set to end.
	
//Release options	
#ifdef SDRC_RELEASE
	const string DEFAULT_DIR = "default";
	const bool SDRC_ENABLE_DARCSPAWNER = true;			//Enable the spawner mod during development
	const bool SDRC_ENABLE_DARCMISSIONS = true;			//Enable the missions mod during development
	const bool SDRC_ENABLE_DARCDEATHMARKER = true;		//Enable death marker mod during development
	const bool SDRC_ENABLE_DARCSTORIES = false;			//Enable stories mod during development
//	#define SDRC_CREATE_EXAMPLE_NONVALIDAREA			//If enabled, creates a default configuration
	const bool RELEASE = true;							//Define for release compilation to workshop
	const bool OVERWRITE_JSON = false;					//Writes a new conf even if it exists
	const bool SHOW_VALID_MISSION_AREAS = false;		//Debug markers for mission position testing drawn on the map
	const bool SHOW_MARKER_FOR_LOCATION = false;		//Show a debug marker for locations
//	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.NORMAL;
	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.DEBUG;
	//TBD: const bool DISABLE_PERSISTENCE = true;
#endif
		
//Development time options	
#ifndef SDRC_RELEASE
	const string DEFAULT_DIR = "dummy";
	const bool SDRC_ENABLE_DARCSPAWNER = false;
	const bool SDRC_ENABLE_DARCMISSIONS	= true;
	const bool SDRC_ENABLE_DARCDEATHMARKER = false;
	const bool SDRC_ENABLE_DARCSTORIES = false;
//	#define SDRC_CREATE_EXAMPLE_NONVALIDAREA
	const bool RELEASE = false;					
	const bool OVERWRITE_JSON = true;				
//	const bool OVERWRITE_JSON = false;
	const bool SHOW_VALID_MISSION_AREAS = false;
	const bool SHOW_MARKER_FOR_LOCATION = false;	
	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.DEBUG;
//	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.ALL;
	//TBD: const bool DISABLE_PERSISTENCE = true;	
#endif
}