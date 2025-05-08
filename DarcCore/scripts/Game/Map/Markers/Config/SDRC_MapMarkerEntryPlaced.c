//------------------------------------------------------------------------------------------------
//! Marker entry which can be placed through map

enum DC_EMissionIcon
{
	MISSION,
	CRASHSITE,
	TARGET_X,
	TARGET_O,
	REDCROSS,
	REDCROSS_SMALL,
	BLACK_X_SMALL,
	N_MISSION,
	N_X,
	N_HOUSE,
	N_FIRE,
	N_FENCE,
	N_CAR,
	N_HELI,
	N_HUNTER,
	N_PATROL,
	N_RADIOACTIVE
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
