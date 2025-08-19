//SDRC_DarcMissionEditableRequestComp.c

//------------------------------------------------------------------------------------------------
/*!
Component to modify mission related information in WB. Used with GM placable missions.
*/
[ComponentEditorProps(category: "GameScripted/Editor (Editables)", description: "", icon: "")]
class SDRC_DarcMissionEditableRequestCompClass : SCR_EditableSystemComponentClass { }
SDRC_DarcMissionEditableRequestCompClass g_DarcMissionEditableRequestClass;
 
//------------------------------------------------------------------------------------------------
//! @ingroup Editable_Entities
class SDRC_DarcMissionEditableRequestComp : SCR_EditableSystemComponent
{
	[Attribute(typename.EnumToString(DC_EMissionType, DC_EMissionType.OCCUPATION), UIWidgets.ComboBox, desc: "Mission type", enumType: DC_EMissionType)]	
	protected DC_EMissionType m_MissionType;	//The type of mission

#ifndef SDRC_RELEASE
	static int max = 12;
	static private string param = "1, " + max + ", 1";
	[Attribute("3", UIWidgets.EditBox, "Test mission index variable", param)]	
	protected int m_MissionIdx;
#endif
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Return the mission type
	*/
	DC_EMissionType GetMissionType()
	{
		return m_MissionType;
	}
}