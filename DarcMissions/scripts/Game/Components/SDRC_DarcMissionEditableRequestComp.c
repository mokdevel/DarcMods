//SDRC_DarcMissionEditableRequestComp.c

//------------------------------------------------------------------------------------------------
/*!
Component to modify mission related information via GM. Used with GM placable missions.

This is a component in SDRC_DarcMissionGM and should be the editable functionality.

TBD: The editing part is not working at the moment.
*/

//------------------------------------------------------------------------------------------------
enum SDRC_EEditModeForMission
{
	UNKNOWN = 0,
	IN_EDITING,
	NO_ONE_IS_EDITING,	
};

//------------------------------------------------------------------------------------------------
[ComponentEditorProps(category: "GameScripted/Editor (Editables)", description: "", icon: "WBData/ComponentEditorProps/componentEditor.png")]
class SDRC_DarcMissionEditableRequestCompClass : SCR_EditableEntityComponentClass { }
SDRC_DarcMissionEditableRequestCompClass g_DarcMissionEditableRequestClass;
 
//------------------------------------------------------------------------------------------------
//! @ingroup Editable_Entities
class SDRC_DarcMissionEditableRequestComp : SCR_EditableEntityComponent
{
	[Attribute(typename.EnumToString(SDRC_EMissionType, SDRC_EMissionType.NONE), UIWidgets.ComboBox, desc: "Mission type", enumType: SDRC_EMissionType)]	
	protected SDRC_EMissionType m_MissionType;	//The type of mission
	
	//NOTE: Params are set in SDRC_EditorAttributeBaseValues in file SDRC_BaseValueListEditorAttribute.c
	[Attribute("-1", desc: "Sub mission index")]	
	protected int m_SubIdx;

	[RplProp(onRplName: "OnUpdateBeingEdited")]	
	protected SDRC_EEditModeForMission m_eBeingEdited = SDRC_EEditModeForMission.UNKNOWN;
	
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
	/*!	
	Get/set if details are being edited.
	*/
	SDRC_EEditModeForMission IsBeingEdited()
	{
		return m_eBeingEdited;
	}	
	
	void SetBeingEdited(SDRC_EEditModeForMission edited)
	{
		SDRC_Log.Add("[SDRC_DarcMissionEditableRequestComp:SetBeingEdited] State to: " + edited, LogLevel.NORMAL);
		int playerId = GetGame().GetPlayerController().GetPlayerId();
		RpcDo_UpdateBeingEdited(playerId, edited);
		Rpc(RpcDo_UpdateBeingEdited, playerId, edited);
	}		
	
	void OnUpdateBeingEdited()
	{
		SDRC_Log.Add("[SDRC_DarcMissionEditableRequestComp:OnUpdateBeingEdited] Updated.", LogLevel.NORMAL);
	}	
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_UpdateBeingEdited(int playerID, SDRC_EEditModeForMission edited)
	{
		SDRC_Log.Add("[SDRC_DarcMissionEditableRequestComp:RpcDo_UpdateBeingEdited] Edit state changed to: " + edited + " by: " + playerID, LogLevel.DEBUG);	
		m_eBeingEdited = edited;

		Replication.BumpMe();
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
		SDRC_RplPlayerComp playerComponent = SDRC_RplPlayerComp.FindLocalInstance();

		int num = rplGMComp.m_MissionIndexMax;
	
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

		//Inform server that we're editing the entity
		missionComp.SetBeingEdited(SDRC_EEditModeForMission.IN_EDITING);
		
		SDRC_Log.Add("[SDRC_Editable:ReadVariable] Owner: " + owner + " missionComp: " + missionComp + " EntityID: " + owner.GetID(), LogLevel.NORMAL);
		
		//Get subIdx
		int subIdx = missionComp.GetSubIdx();
		//Get the mission type enum value and use it as the index for finding the biggest index
		int missionType = missionComp.GetMissionType();
		
		//Modify the scale with a hacky-hack way. 
		SDRC_RplGMComp rplGMComp = SDRC_RplGMComp.GetInstance();
		SDRC_RplPlayerComp playerComponent = SDRC_RplPlayerComp.FindLocalInstance();
		
		if ( (playerComponent.m_MissionData.IsEmpty()) || (!playerComponent.IsMissionDataReady()) )
		{
			rplGMComp.RpcDo_UpdateMissionIndexMax(-1);
		}
		else
		{
			rplGMComp.RpcDo_UpdateMissionIndexMax(playerComponent.m_MissionData[missionType].maxIndex);
		}
		rplGMComp.RpcDo_UpdateMissionType(missionType);
		
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

		SDRC_Log.Add("[SDRC_Editable:WriteVariable] Owner: " + owner + " missionComp: " + missionComp + " EntityID: " + owner.GetID(), LogLevel.NORMAL);
				
		//Inform server that we're done with the editing
		missionComp.SetBeingEdited(SDRC_EEditModeForMission.NO_ONE_IS_EDITING);
	}
	
}

//------------------------------------------------------------------------------------------------
// Sync that we're editing to server so that the mission is not picked to be spawned while someone is accessing it.

modded class SCR_AttributesManagerEditorComponent
{
	override void ConfirmEditing()
	{	
		super.ConfirmEditing();
		
		if (!m_aEditedItems)
			return;
		
		if (m_aEditedItems.IsEmpty())
			return;
		
		SCR_EditableEntityComponent editableEntity = SCR_EditableEntityComponent.Cast(m_aEditedItems[0]);
		if (!editableEntity)
			return;

		IEntity owner = editableEntity.GetOwner();
		if (!owner)
			return;
		
		SDRC_DarcMissionEditableRequestComp missionComp = SDRC_DarcMissionEditableRequestComp.Cast(owner.FindComponent(SDRC_DarcMissionEditableRequestComp));
		if (missionComp)
		{
			SDRC_Log.Add("[SDRC_DarcMissionEditableRequestComp:SCR_AttributesManagerEditorComponent] Confirm editing.", LogLevel.NORMAL);
			
			SDRC_Log.Add("[SDRC_Editable:ConfirmEditing] Owner: " + owner + " missionComp: " + missionComp + " EntityID: " + owner.GetID(), LogLevel.NORMAL);
			
			//Inform server that we're done with the editing
			missionComp.SetBeingEdited(SDRC_EEditModeForMission.NO_ONE_IS_EDITING);
		}
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

		SDRC_RplPlayerComp playerComponent = SDRC_RplPlayerComp.FindLocalInstance();
				
		int index = var.GetInt();
		string description = "Randomly selected.";
		
		SDRC_Log.Add("[SDRC_DarcMissionEditableRequestComp:GetDescriptionData] m_MissionData count: " + playerComponent.m_MissionData.Count(), LogLevel.NORMAL);	
		
		if (!playerComponent.IsMissionDataReady())
		{
			description = "Mission data not available";
		}
		
		if (index > -1)
		{
			description = playerComponent.m_MissionData[rplGMComp.m_MissionType].names[index];			
			description = SDRC_MissionHelper.CreateInfo(description, "LOCATION", "DESTINATION");
		}
		m_DescriptionDisplayInfo.SetDescription("Mission: " + description);
		uiInfo = m_DescriptionDisplayInfo;
	}	
}