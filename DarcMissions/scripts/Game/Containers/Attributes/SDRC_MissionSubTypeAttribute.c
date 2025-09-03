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

		SDRC_DarcMissionRequestComp missionComp = SDRC_DarcMissionRequestComp.Cast(owner.FindComponent(SDRC_DarcMissionRequestComp));
		if (!missionComp)
			return null;

		int missionType; 
		missionType = missionComp.GetMissionType();
		
		return SCR_BaseEditorAttributeVar.CreateFloat(missionType);
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

		SDRC_DarcMissionRequestComp missionComp = SDRC_DarcMissionRequestComp.Cast(owner.FindComponent(SDRC_DarcMissionRequestComp));
		if (!missionComp)
			return;

		missionComp.SetMissionType(var.GetFloat());
	}
}