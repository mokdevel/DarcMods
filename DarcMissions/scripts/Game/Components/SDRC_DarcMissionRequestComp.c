//SDRC_DarcMissionRequestComp.c

//------------------------------------------------------------------------------------------------
/*!
Component to set/get mission related information. Used with GM placable missions.

This is a component in SDRC_DarcMissionGM. This component keeps all the information needed for the mission to spawn.
*/

//------------------------------------------------------------------------------------------------
[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
class SDRC_DarcMissionRequestCompClass : ScriptGameComponentClass { }
SDRC_DarcMissionRequestCompClass g_DarcMissionRequestClass;
 
//------------------------------------------------------------------------------------------------
class SDRC_DarcMissionRequestComp : ScriptGameComponent
{
	[Attribute(typename.EnumToString(DC_EMissionType, DC_EMissionType.NONE), UIWidgets.ComboBox, desc: "Mission type", enumType: DC_EMissionType)]	
	protected DC_EMissionType m_MissionType;	//The type of mission
	
	static int max = Math.RandomInt(0,12);
	static private string param = "-1, " + max + ", 1";
	[Attribute("-1", UIWidgets.EditBox, desc: "Test mission index variable", param)]	
	protected int m_MissionSubIdx;				//The sub type of mission	
	
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Get/set mission type
	*/
	DC_EMissionType GetMissionType()
	{
		return m_MissionType;
	}	
	
	void SetMissionType(DC_EMissionType missionType)
	{
		m_MissionType = missionType;
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Get/set mission subIdx
	*/
	DC_EMissionType GetMissionSubIdx()
	{
		return m_MissionSubIdx;
	}	
	
	void SetMissionSubIdx(int missionSubIdx)
	{
		m_MissionSubIdx = missionSubIdx;
	}		
	
}