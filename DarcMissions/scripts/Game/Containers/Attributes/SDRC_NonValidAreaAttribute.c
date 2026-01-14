//! Attribute for mission subIdx
[BaseContainerProps(), SCR_BaseEditorAttributeCustomTitle()]
class SDRC_NonValidAreaAttribute : SCR_BaseValueListEditorAttribute
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

		SCR_EffectsModuleAreaMeshComponent nvaComp = SCR_EffectsModuleAreaMeshComponent.Cast(owner.FindComponent(SCR_EffectsModuleAreaMeshComponent));
		if (!nvaComp)
			return null;

		float width; 
		float height; 
		nvaComp.GetDimensions2D(width, height);
		
		return SCR_BaseEditorAttributeVar.CreateFloat(width);
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

		SDRC_EffectsModuleAreaMeshComponent nvaComp = SDRC_EffectsModuleAreaMeshComponent.Cast(owner.FindComponent(SDRC_EffectsModuleAreaMeshComponent));
		if (!nvaComp)
			return;

		nvaComp.SetWidth(var.GetFloat());
	}
}