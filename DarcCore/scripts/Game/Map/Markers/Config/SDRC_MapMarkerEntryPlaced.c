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
