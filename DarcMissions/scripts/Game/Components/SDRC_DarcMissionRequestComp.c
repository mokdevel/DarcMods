//SDRC_DarcMissionRequestComp.c

//------------------------------------------------------------------------------------------------
/*!
Component to set/get mission related information. Used with GM placable missions.
*/
[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
class SDRC_DarcMissionRequestCompClass : ScriptGameComponentClass { }
SDRC_DarcMissionRequestCompClass g_DarcMissionRequestClass;
 
//------------------------------------------------------------------------------------------------
class SDRC_DarcMissionRequestComp : ScriptGameComponent
{
	protected DC_EMissionType m_MissionType;	//The type of mission
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Return the mission type
	*/
	DC_EMissionType GetMissionType()
	{
		return m_MissionType;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Return the mission type
	*/
	void SetMissionType(DC_EMissionType missionType)
	{
		m_MissionType = missionType;
	}		
}