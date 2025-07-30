class SDRC_DarcMissionRequestCompClass : ScriptComponentClass { }
SDRC_DarcMissionRequestCompClass g_DarcMissionRequestClass;
 
//------------------------------------------------------------------------------------------------
class SDRC_DarcMissionRequestComp : ScriptComponent
{
	[Attribute(typename.EnumToString(DC_EMissionType, DC_EMissionType.OCCUPATION), UIWidgets.ComboBox, desc: "Mission type", enumType: DC_EMissionType)]	
	protected DC_EMissionType missionType;	//The type of mission
	
	//TBD: For future needs shall add
	//protected int missionIndex;			//Index of the mission details
	//protected string subDir;				//Location for the mission conf to load. Overrides subDir defined by core
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Return the mission type
	*/
	DC_EMissionType GetMissionType()
	{
		return missionType;
	}
}