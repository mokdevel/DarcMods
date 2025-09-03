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