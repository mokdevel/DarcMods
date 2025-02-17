//------------------------------------------------------------------------------------------------
//! Marker entry which can be placed through map

enum DC_EMissionIcon
{
	CIRCLE,
	MISSION,
	DEBUG,
	CRASHSITE,
	TARGET_X,
	TARGET_O,
	DEBUG_SMALL,
	REDCROSS,
	REDCROSS_SMALL,
};

[BaseContainerProps(), SCR_MapMarkerTitle()]
class SCR_DC_MapMarkerEntryPlaced : SCR_MapMarkerEntryPlaced
{
	//------------------------------------------------------------------------------------------------
	override SCR_EMapMarkerType GetMarkerType()
	{
	 	return SCR_EMapMarkerType.DARC_MISSION;
	}
	
}
