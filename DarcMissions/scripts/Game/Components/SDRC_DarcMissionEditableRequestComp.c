//SDRC_DarcMissionEditableRequestComp.c

//------------------------------------------------------------------------------------------------
/*!
Component to modify mission related information via GM. Used with GM placable missions.

This is a component in SDRC_DarcMissionGM and should be the editable functionality.

TBD: The editing part is not working at the moment.
*/

//------------------------------------------------------------------------------------------------
[ComponentEditorProps(category: "GameScripted/Editor (Editables)", description: "", icon: "WBData/ComponentEditorProps/componentEditor.png")]
class SDRC_DarcMissionEditableRequestCompClass : SCR_EditableSystemComponentClass { }
SDRC_DarcMissionEditableRequestCompClass g_DarcMissionEditableRequestClass;
 
//------------------------------------------------------------------------------------------------
//! @ingroup Editable_Entities
class SDRC_DarcMissionEditableRequestComp : SCR_EditableSystemComponent
{
	[Attribute(typename.EnumToString(DC_EMissionType, DC_EMissionType.NONE), UIWidgets.ComboBox, desc: "Mission type", enumType: DC_EMissionType)]	
	protected DC_EMissionType m_MissionType;	//The type of mission
	
	//NOTE: Params are set in SDRC_EditorAttributeBaseValues in file SDRC_BaseValueListEditorAttribute.c
	[Attribute("-1", desc: "Sub mission index")]	
	protected int m_SubIdx;
			
	protected int m_RequestId;
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Return the mission type
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
	Return the mission type
	*/
	DC_EMissionType GetSubIdx()
	{
		return m_SubIdx;
	}
	
	void SetSubIdx(DC_EMissionType subIdx)
	{
		m_SubIdx = subIdx;
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Get/set mission subIdx
	*/
	DC_EMissionType GetRequestId()
	{
		return m_RequestId;
	}	
	
	void SetRequestId(int requestId)
	{
		m_RequestId = requestId;
	}			
}