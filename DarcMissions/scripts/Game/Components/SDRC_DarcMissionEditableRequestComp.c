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
	
	void SetSubIdx(int subIdx)
	{
		m_SubIdx = subIdx;
	}				
}

//------------------------------------------------------------------------------------------------
/**
Attribute base for slider values for other attributes to inherent from
*/
[BaseContainerProps()]
class SDRC_MissionSubIdxEditorAttributeBaseValues : SCR_EditorAttributeBaseValues
{
	override void GetSliderValues(out string sliderValueFormating, out float min, out float max, out float step, out int decimals)
	{		
		SDRC_RplGMComp rplGMComp = SDRC_RplGMComp.GetInstance();

		int num = rplGMComp.missionIndexMax;
	
		m_fMin = -1;
		m_fMax = num;
		m_fStep = 1;
		m_iDecimals = 0;
		
		sliderValueFormating = m_sSliderValueFormating;
		min = m_fMin;
		max = m_fMax;
		step = m_fStep;
		decimals = m_iDecimals;
	}
};

//------------------------------------------------------------------------------------------------
//! Attribute for mission subIdx
[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class SDRC_MissionSubTypeAttribute : SCR_BaseValueListEditorAttribute
{
	//------------------------------------------------------------------------------------------------
	override SCR_BaseEditorAttributeVar ReadVariable(Managed item, SCR_AttributesManagerEditorComponent manager)
	{
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity)
			return null;

		IEntity owner = editableEntity.GetOwner();
		if (!owner)
			return null;

		SDRC_DarcMissionEditableRequestComp missionComp = SDRC_DarcMissionEditableRequestComp.Cast(owner.FindComponent(SDRC_DarcMissionEditableRequestComp));
		if (!missionComp)
			return null;

		int subIdx; 
		subIdx = missionComp.GetSubIdx();
		
		//Modify the scale with a hacky-hack way. 
		SDRC_RplGMComp rplGMComp = SDRC_RplGMComp.GetInstance();
		
		//Get the mission type enum value and use it as the index for missionIndexMax
		int index = missionComp.GetMissionType();
		int value = 15;	//Setting just a default here
		if ( (index > 0) && (index < rplGMComp.missionIndexMaxList.Count()) )
		{
			value = rplGMComp.missionIndexMaxList[index];
		}
			
		rplGMComp.missionIndexMax = value;	//TBD: For this to work on clients, it needs to be replicated. 
		SDRC_Log.Add("[SDRC_MissionSubTypeAttribute] Editing value: " + value, LogLevel.DEBUG);			
		
		return SCR_BaseEditorAttributeVar.CreateInt(subIdx);
	}

	//------------------------------------------------------------------------------------------------
	// Update the subIdx value when moving the slider
	
	override void WriteVariable(Managed item, SCR_BaseEditorAttributeVar var, SCR_AttributesManagerEditorComponent manager, int playerID)
	{
		if (!var)
			return;

		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(item);
		if (!editableEntity)
			return;

		IEntity owner = editableEntity.GetOwner();
		if (!owner)
			return;

		SDRC_DarcMissionEditableRequestComp missionComp = SDRC_DarcMissionEditableRequestComp.Cast(owner.FindComponent(SDRC_DarcMissionEditableRequestComp));
		if (!missionComp)
			return;

		missionComp.SetSubIdx(var.GetInt());
	}
}

//------------------------------------------------------------------------------------------------
// Update the description. 
// This is a bit hacky. 

[BaseContainerProps(), BaseContainerCustomStringTitleField("Using base class even if not recommended.")]
class SDRC_SCR_BaseAttributeDynamicDescription : SCR_BaseAttributeDynamicDescription
{	
	override void GetDescriptionData(notnull SCR_BaseEditorAttribute attribute, notnull SCR_BaseEditorAttributeUIComponent attributeUi, out SCR_EditorAttributeUIInfo uiInfo, out string param1 = string.Empty, out string param2 = string.Empty, out string param3 = string.Empty)
	{
		SDRC_RplGMComp rplGMComp = SDRC_RplGMComp.GetInstance();
		SCR_BaseEditorAttributeVar var = attribute.GetVariable();		
		
		int index = var.GetInt();
		string description = "Random";
		if (index > -1)
		{
			description = rplGMComp.m_MissionData[1].names[index];
		}
		m_DescriptionDisplayInfo.SetDescription("Title: " + description);
		uiInfo = m_DescriptionDisplayInfo;
	}	
}