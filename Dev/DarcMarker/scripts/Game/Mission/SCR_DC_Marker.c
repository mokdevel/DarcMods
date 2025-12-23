//------------------------------------------------------------------------------------------------
/*!
Just a dummy test to show a marker on map.
*/

const string DC_ID_PREFIX = "DCM_";				//The prefix used for marker and missions Id's.

//------------------------------------------------------------------------------------------------
class SCR_DC_Marker
{
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Marker()
	{
		SCR_DC_Log.Add("[SCR_DC_Marker] Starting SCR_DC_Marker", LogLevel.NORMAL);
		SCR_DC_MapMarkersUI.AddMarker("DARC_", "2000 0 2000", "My test marker")
	}	
}