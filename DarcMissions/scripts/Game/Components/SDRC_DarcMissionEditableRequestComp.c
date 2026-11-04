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

	[Attribute("Dummy", desc: "Mission title")]	
	protected string m_Title;
				
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
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		
		if (baseGameMode)		
		{		
			int num = baseGameMode.missionLargestIndexForEdit;
		
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
	}
};

[BaseContainerProps(), BaseContainerCustomStringTitleField("Using base class even if not recommended.")]
class SDRC_SCR_BaseAttributeDynamicDescription : SCR_BaseAttributeDynamicDescription
{	
	int m_iVar = 0;
	
	override void GetDescriptionData(notnull SCR_BaseEditorAttribute attribute, notnull SCR_BaseEditorAttributeUIComponent attributeUi, out SCR_EditorAttributeUIInfo uiInfo, out string param1 = string.Empty, out string param2 = string.Empty, out string param3 = string.Empty)
	{
//		SDRC_DarcMissionEditableRequestComp requestComp = SDRC_DarcMissionEditableRequestComp.Cast(this.FindComponent(SDRC_DarcMissionEditableRequestComp));
//		int id = requestComp.GetRequestId();		
		
//		IEntity entity = this.GetOwner();
		
		m_DescriptionDisplayInfo.SetDescription("dang: " + m_iVar);
		m_iVar++;
		uiInfo = m_DescriptionDisplayInfo;
	}
	
}