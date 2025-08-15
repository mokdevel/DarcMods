//------------------------------------------------------------------------------------------------
//! Marker entry which can be placed through map


//NOTE: If you add a new icon, remember to add texture finding to SDRC_MapMarkerHelper.GetMarkerTexture
enum DC_EMissionIcon
{
	NONE = -1,
	GM_MISSION_X_MAP = 0,
	GM_MISSION_SQUATTERS_MAP,
	GM_MISSION_CRASHSITE_MAP,
	GM_MISSION_OCCUPATION_MAP,
	GM_MISSION_CONVOY_MAP,
	GM_MISSION_HELICOPTER_MAP,
	GM_MISSION_HUNTER_MAP,
	GM_MISSION_PATROL_MAP,
	GM_MISSION_RADIOACTIVE_MAP,
	GM_MISSION_ROADBLOCK_MAP,
	GM_MISSION_HVTVIP_MAP,
	GM_MISSION_HVTITEM_MAP,
	
	//Markers to show on top of win/lose mission
	GM_MISSION_WIN_MAP,
	GM_MISSION_LOSE_MAP,
	
	//Other markers
	ICON_DEATHMARKER_MAP,
	ICON_DEATHMARKER_SMALL_MAP,
	ICON_DEATHMARKER_SMALL_RED_MAP,
	ICON_PLUS_SMALL_MAP,
	ICON_EXCLAMATION_SMALL_MAP,
	ICON_CRATE_SMALL_MAP,
	
	//THESE ARE NOT USABLE AS MARKERS
	ICON_WIN_ROUND = 100,
	ICON_LOSE_ROUND,
};

[BaseContainerProps(), SCR_MapMarkerTitle()]
class SDRC_MapMarkerEntryPlaced : SCR_MapMarkerEntryPlaced
{
	//------------------------------------------------------------------------------------------------
	override SCR_EMapMarkerType GetMarkerType()
	{
	 	return SCR_EMapMarkerType.DARC_MISSION;
	}	
}
