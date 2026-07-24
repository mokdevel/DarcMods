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
		//The value is used 
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		
		if (baseGameMode)		
		{		
			//Get the mission type enum value and use it as the index for missionBigIndexForEdit
			int index = missionComp.GetMissionType();
			int value = 15;	//Setting just a default here
			if ( (index > 0) && (index < baseGameMode.missionBigIndex.Count()) )
			{
				value = baseGameMode.missionBigIndex[index];
			}
				
			baseGameMode.missionBigIndexForEdit = value;	//TBD: For this to work on clients, it needs to be replicated. 
			SDRC_Log.Add("[SDRC_MissionSubTypeAttribute] Editing value: " + value, LogLevel.DEBUG);			
		}		
		
		return SCR_BaseEditorAttributeVar.CreateInt(subIdx);
	}

	//------------------------------------------------------------------------------------------------
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