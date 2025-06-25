//------------------------------------------------------------------------------------------------
//! Marker entry which can be placed through map

enum DC_EMissionIcon
{
	MISSION,			//0
	CRASHSITE,
	TARGET_X,
	TARGET_O,
	UNUSED_1,
	UNUSED_2,			//5
	UNUSED_3,
	REDCROSS,
	REDCROSS_SMALL,		//8
	BLACK_X_SMALL,
	N_MISSION,
	GM_MISSION_X_MAP,
	GM_MISSION_SQUATTERS_MAP,
	GM_MISSION_CRASHSITE_MAP,
	GM_MISSION_OCCUPATION_MAP,
	GM_MISSION_CONVOY_MAP,
	GM_MISSION_HELICOPTER_MAP,
	GM_MISSION_HUNTER_MAP,
	GM_MISSION_PATROL_MAP,
	GM_MISSION_RADIOACTIVE_MAP,
	GM_MISSION_ROADBLOCK_MAP
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
