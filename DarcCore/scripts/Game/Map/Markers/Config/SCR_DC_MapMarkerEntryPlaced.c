//------------------------------------------------------------------------------------------------
//! Marker entry which can be placed through map
[BaseContainerProps(), SCR_MapMarkerTitle()]
class SCR_DC_MapMarkerEntryPlaced : SCR_MapMarkerEntryPlaced
{
	//------------------------------------------------------------------------------------------------
	override SCR_EMapMarkerType GetMarkerType()
	{
	 	return SCR_EMapMarkerType.DARC_MISSION;
	}
	
}
