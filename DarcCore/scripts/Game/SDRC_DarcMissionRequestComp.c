class SDRC_DarcMissionRequestCompClass : ScriptComponentClass { }
SDRC_DarcMissionRequestCompClass g_DarcMissionRequestClass;
 
//------------------------------------------------------------------------------------------------
class SDRC_DarcMissionRequestComp : ScriptComponent
{
	[Attribute(typename.EnumToString(DC_EMissionType, DC_EMissionType.OCCUPATION), UIWidgets.ComboBox, desc: "Mission type", enumType: DC_EMissionType)]	
	protected DC_EMissionType missionType;// = DC_EMissionType.OCCUPATION;
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Return the mission type
	*/
	DC_EMissionType GetMissionType()
	{
		return missionType;
	}
}