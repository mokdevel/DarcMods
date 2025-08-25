//SDRC_DarcMissionEditableRequestComp.c

//------------------------------------------------------------------------------------------------
/*!
Component to modify mission related information via GM. Used with GM placable missions.

This is a component in SDRC_DarcMissionGM and should be the editable functionality. Parameters are then passed to SDRC_DarcMissionRequestComp.st

TBD: The editing part if not working at the moment.
*/

//------------------------------------------------------------------------------------------------
[ComponentEditorProps(category: "GameScripted/Editor (Editables)", description: "", icon: "")]
class SDRC_DarcMissionEditableRequestCompClass : SCR_EditableSystemComponentClass { }
SDRC_DarcMissionEditableRequestCompClass g_DarcMissionEditableRequestClass;
 
//------------------------------------------------------------------------------------------------
//! @ingroup Editable_Entities
class SDRC_DarcMissionEditableRequestComp : SCR_EditableSystemComponent
{
	[Attribute(typename.EnumToString(DC_EMissionType, DC_EMissionType.OCCUPATION), UIWidgets.ComboBox, desc: "Mission type", enumType: DC_EMissionType)]	
	protected DC_EMissionType m_MissionType;	//The type of mission

	static int max = Math.RandomInt(0,12);
	static private string param = "-1, " + max + ", 1";
	[Attribute("-1", UIWidgets.EditBox, desc: "This should be editable", param)]	
	protected int m_MissionSubIdx;
			
	//------------------------------------------------------------------------------------------------
	/*!	
	Return the mission type
	*/
	DC_EMissionType GetMissionType()
	{
		return m_MissionType;
	}
}