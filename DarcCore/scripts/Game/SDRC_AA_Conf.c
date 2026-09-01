//Helpers SDRC_Core.c

//------------------------------------------------------------------------------------------------
/*!
Stupid file to handle a few common variables accross multiple mods
*/

#define SDRC_RELEASE

#ifdef SDRC_RELEASE
	const bool SHOW_DEBUG_BOOL = false;		//Controls if debug visuals are to be shown. Disable for DarcMissions release.
#else
	const bool SHOW_DEBUG_BOOL = true;		//Controls if debug visuals are to be shown. Disable for DarcMissions release.
#endif

//#define DEBUG_DO_NOT_CLEAR		//Leave old visual debug lines etc on restart
//#define NEW_VERSION_WIP			//New functionality (..that affects jsons)
//#define EXPERIMENTAL

sealed class SDRC_Conf
{
	const string CONF_DIRECTORY = "DarcMods";
	const string CORE_CONFIG_FILE = "dc_coreConfig.json";
	static string subDir = "";								//Updated in SDRC_Core_BaseGameMode
	static string subDirPath = "";							//Updated in SDRC_Core_BaseGameMode
	static bool coreHasStarted = false;
	static bool coreInitReady = false;
	const int MISSION_RUN_DELAY = 5000;						//Time between INIT and SPAWN states.
	const int PERSISTENCE_DELAY = 6000;						//Time to delay the persistence setting for spawned entities. 
	const int DESPAWN_ENTITY_USED_FOR_SIZE_DELAY = 1000;	//Time to delay the deletion of an entity that was used for defining the size or weapon used for finding the mag.
	const int AI_SETTING_DELAY = 10000;
	const int POSITION_RANDOMIZATION = 150;					//Randomization for position. 
	const int SPAWN_ITEM_DELAY = 1000;						//Time used between AI/item spawns for example for Roadblock items
	static ref array<string> errorList = {};				//Errors reported and spamchatted to players 
	
	//TBD: const int MISSION_END_TIME = 5000;				//Time to set for mission cycle when mission is set to end.
	
//Release options	
#ifdef SDRC_RELEASE
	const bool RELEASE = true;							//Define for release compilation to workshop
	const string DEFAULT_DIR = "default";
	const bool SDRC_ENABLE_DARCSPAWNER = true;			//Enable the spawner mod during development
	const bool SDRC_ENABLE_DARCMISSIONS = true;			//Enable the missions mod during development
	const bool SDRC_ENABLE_DARCDEATHMARKER = true;		//Enable death marker mod during development
	const bool SDRC_ENABLE_DARCSTORIES = false;			//Enable stories mod during development
	const bool SDRC_ENABLE_DARCCHOPPER = true;			//Enable chopper mod during development
//	#define SDRC_CREATE_EXAMPLE_NONVALIDAREA			//If enabled, creates a default configuration
	const bool OVERWRITE_JSON = false;					//Writes a new conf even if it exists
	const bool SHOW_VALID_MISSION_AREAS = false;		//Debug markers for mission position testing drawn on the map
	const bool SHOW_MARKER_FOR_LOCATION = false;		//Show a debug marker for locations
	const bool SHOW_DEBUG_INFO = false;					//Sets the default visibility for debug UI
	const bool SHOW_DEBUG = SHOW_DEBUG_BOOL;		
//	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.NORMAL;
	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.DEBUG;
	//TBD: const bool DISABLE_PERSISTENCE = true;
	const int LIST_LOAD_DELAY = 100;
#endif
		
//Development time options	
#ifndef SDRC_RELEASE
	const bool RELEASE = false;
	const string DEFAULT_DIR = "debug";
	const bool SDRC_ENABLE_DARCSPAWNER = false;
	const bool SDRC_ENABLE_DARCMISSIONS	= true;
	const bool SDRC_ENABLE_DARCDEATHMARKER = false;
	const bool SDRC_ENABLE_DARCSTORIES = false;
	const bool SDRC_ENABLE_DARCCHOPPER = true;
//	#define SDRC_CREATE_EXAMPLE_NONVALIDAREA
	const bool OVERWRITE_JSON = true;
//	const bool OVERWRITE_JSON = false;
	const bool SHOW_VALID_MISSION_AREAS = false;
	const bool SHOW_MARKER_FOR_LOCATION = false;
	const bool SHOW_DEBUG_INFO = true;
	const bool SHOW_DEBUG = SHOW_DEBUG_BOOL;	
	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.DEBUG;
//	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.ALL;
	//TBD: const bool DISABLE_PERSISTENCE = true;	
	const int LIST_LOAD_DELAY = 10;
#endif
}