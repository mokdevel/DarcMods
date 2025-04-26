//Helpers SCR_DC_Core.c

//------------------------------------------------------------------------------------------------
/*!
Stupid file to handle a few common variables accross multiple mods
*/

#define SCR_DC_RELEASE

sealed class SCR_DC_Conf
{
const string CONF_DIRECTORY = "DarcMods";
	static string missionProfile = "";
	
#ifdef SCR_DC_RELEASE
//	#define SCR_DC_DISABLE_DARCSPAWNER				//Disable the spawner mod during development
//	#define SCR_DC_DISABLE_DARCMISSIONS				//Disable the missions mod during development
	const bool RELEASE = true;						//Define for release compilation to workshop
	const bool OVERWRITE_JSON = false;				//Writes a new conf even if it exists
	const bool SHOW_VALID_MISSION_AREAS = false;	//Debug markers for mission position testing drawn on the map
	const bool SHOW_MARKER_FOR_LOCATION = false;	//Show a debug marker for locations
	const bool FIRST_MISSION_HAS_SPAWNED = false;	//Has the first mission (=dynamic) spawned. Setting to true will spawn a static mission first
	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.NORMAL
//	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.DEBUG
#endif
		
#ifndef SCR_DC_RELEASE	//Development time options
	#define SCR_DC_DISABLE_DARCSPAWNER				//Disable the spawner mod during development
//	#define SCR_DC_DISABLE_DARCMISSIONS				//Disable the missions mod during development
	const bool RELEASE = false;						//Define for release compilation to workshop
	const bool OVERWRITE_JSON = true;				//Writes a new conf even if it exists
//	const bool OVERWRITE_JSON = false;				//Writes a new conf even if it exists
	const bool SHOW_VALID_MISSION_AREAS = false;	//Debug markers for mission position testing drawn on the map
	const bool SHOW_MARKER_FOR_LOCATION = false;	//Show a debug marker for locations
	const bool FIRST_MISSION_HAS_SPAWNED = true;	//Has the first mission (=dynamic) spawned. 
	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.DEBUG
#endif
}