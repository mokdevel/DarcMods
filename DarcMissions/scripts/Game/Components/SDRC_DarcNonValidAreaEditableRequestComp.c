//SDRC_DarcMissionEditableRequestComp.c

//------------------------------------------------------------------------------------------------
/*!
Component to modify mission related information via GM. Used with GM placable missions.

This is a component in SDRC_DarcMissionGM and should be the editable functionality.

TBD: The editing part is not working at the moment.
*/

//------------------------------------------------------------------------------------------------
[ComponentEditorProps(category: "GameScripted/Editor (Editables)", description: "", icon: "WBData/ComponentEditorProps/componentEditor.png")]
class SDRC_DarcNonValidAreaEditableRequestCompClass : SCR_EditableSystemComponentClass { }
SDRC_DarcNonValidAreaEditableRequestCompClass g_DarcNonValidAreaEditableRequestClass;
 
//------------------------------------------------------------------------------------------------
//! @ingroup Editable_Entities
class SDRC_DarcNonValidAreaEditableRequestComp : SCR_EditableSystemComponent
{
	[Attribute(typename.EnumToString(SDRC_EMissionType, SDRC_EMissionType.NONE), UIWidgets.ComboBox, desc: "Mission type", enumType: SDRC_EMissionType)]	
	protected SDRC_EMissionType m_MissionType;	//The type of mission
	
	//NOTE: Params are set in SDRC_EditorAttributeBaseValues in file SDRC_BaseValueListEditorAttribute.c
	[Attribute("100", desc: "Radius")]	
	protected float m_width;

	//------------------------------------------------------------------------------------------------
	// Mission specific parameters coming from the request
	//------------------------------------------------------------------------------------------------	
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Return the mission type. This is not used for NonValidArea but left here for compatibility.
	*/
	SDRC_EMissionType GetMissionType()
	{
		return m_MissionType;
	}
	
	void SetMissionType(SDRC_EMissionType missionType)
	{
		m_MissionType = missionType;
	}					
}