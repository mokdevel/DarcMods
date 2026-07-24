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
	[Attribute(typename.EnumToString(SDRC_EMissionType, SDRC_EMissionType.NONE), UIWidgets.ComboBox, desc: "Mission type", enumType: SDRC_EMissionType)]	
	protected SDRC_EMissionType m_MissionType;	//The type of mission
	
	//NOTE: Params are set in SDRC_EditorAttributeBaseValues in file SDRC_BaseValueListEditorAttribute.c
	[Attribute("-1", desc: "Sub mission index")]	
	protected int m_SubIdx;
			
	protected int m_RequestId;	//ID for the request that you can refer to from other mods.
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Handle slider things when adding to world
	*/
	/* override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		
		if (baseGameMode)		
		{		
			baseGameMode.missionFrame.justAnumber = Math.RandomInt(0, 10);
		}		
		
		SDRC_Log.Add("[SDRC_DarcMissionEditableRequestComp] Dropped the entity: " + owner, LogLevel.DEBUG);	
	}	*/
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Get/set request ID. 
	*/
	SDRC_EMissionType GetRequestId()
	{
		return m_RequestId;
	}	
	
	void SetRequestId(int requestId)
	{
		m_RequestId = requestId;
	}		

	//------------------------------------------------------------------------------------------------
	// Mission specific parameters coming from the request
	//------------------------------------------------------------------------------------------------	
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Return the mission type
	*/
	SDRC_EMissionType GetMissionType()
	{
		return m_MissionType;
	}
	
	void SetMissionType(SDRC_EMissionType missionType)
	{
		m_MissionType = missionType;
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Return the mission type
	*/
	SDRC_EMissionType GetSubIdx()
	{
		return m_SubIdx;
	}
	
	void SetSubIdx(SDRC_EMissionType subIdx)
	{
		m_SubIdx = subIdx;
	}				
}