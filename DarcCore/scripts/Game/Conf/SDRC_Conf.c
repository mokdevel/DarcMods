//Helpers SDRC_Core.c

//------------------------------------------------------------------------------------------------
/*!
Stupid file to handle a few common variables accross multiple mods
*/

#define SDRC_RELEASE

sealed class SDRC_Conf
{
const string CONF_DIRECTORY = "DarcMods";
	static string missionProfile = "";
	
//Release options	
#ifdef SDRC_RELEASE
	const bool SDRC_ENABLE_DARCSPAWNER = true;		//Enable the spawner mod during development
	const bool SDRC_ENABLE_DARCMISSIONS = true;		//Enable the missions mod during development
	const bool SDRC_ENABLE_DARCDEATHMARKER = true;	//Enable death marker mod during development
//	#define SDRC_CREATE_EXAMPLE_NONVALIDAREA		//If enabled, creates a default configuration
	const bool RELEASE = true;						//Define for release compilation to workshop
	const bool OVERWRITE_JSON = false;				//Writes a new conf even if it exists
	const bool SHOW_VALID_MISSION_AREAS = false;	//Debug markers for mission position testing drawn on the map
	const bool SHOW_MARKER_FOR_LOCATION = false;	//Show a debug marker for locations
//	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.NORMAL;
	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.DEBUG;
	#define USE_POPUP_NOTIFICATION
#endif
		
//Development time options	
#ifndef SDRC_RELEASE
	const bool SDRC_ENABLE_DARCSPAWNER = true;
	const bool SDRC_ENABLE_DARCMISSIONS	= true;
	const bool SDRC_ENABLE_DARCDEATHMARKER = true;
	#define SDRC_CREATE_EXAMPLE_NONVALIDAREA
	const bool RELEASE = false;					
	const bool OVERWRITE_JSON = true;				
//	const bool OVERWRITE_JSON = false;				
	const bool SHOW_VALID_MISSION_AREAS = false;	
	const bool SHOW_MARKER_FOR_LOCATION = false;	
	const DC_LogLevel DEFAULT_LOGLEVEL = DC_LogLevel.DEBUG;
	#define USE_POPUP_NOTIFICATION
#endif
}