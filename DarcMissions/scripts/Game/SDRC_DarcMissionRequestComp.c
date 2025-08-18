[ComponentEditorProps(category: "GameScripted/Editor (Editables)", description: "", icon: "WBData/ComponentEditorProps/componentEditor.png")]
class SDRC_DarcMissionRequestCompClass : SCR_EditableSystemComponentClass 
{ 
}

SDRC_DarcMissionRequestCompClass g_DarcMissionRequestClass;
 
//------------------------------------------------------------------------------------------------
//[BaseContainerProps()]
//[ComponentEditorProps(category: "GameScripted/Editor (Editables)", description: "", icon: "WBData/ComponentEditorProps/componentEditor.png")]
//! @ingroup Editable_Entities
class SDRC_DarcMissionRequestComp : SCR_EditableSystemComponent
{
	[Attribute(typename.EnumToString(DC_EMissionType, DC_EMissionType.OCCUPATION), UIWidgets.ComboBox, desc: "Mission type", enumType: DC_EMissionType)]	
	protected DC_EMissionType missionType;	//The type of mission

#ifndef SDRC_RELEASE
	static protected string param = "1, 10, 1";
	[Attribute("3", UIWidgets.EditBox, "Alpha of the lines, when highlighted or selected.", param)]	
	protected int m_MissionIdx;

	[Attribute(desc: "If charge should be armed by default when spawned by the GM.")]
	protected bool m_bArmedByDefault;

	[Attribute("120", desc: "Default fuze time that will be used when object is spawned by GM.")]
	protected float m_fDefaultFuzeTime;		
	
	void SDRC_DarcMissionRequestComp(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		param = "1, 20, 1";
		super.this;
	}
	
	override protected void EOnActivate(IEntity owner)
	{
		param = "1, 70, 1";
		super.EOnActivate(owner);
	}

	override protected void OnPostInit(IEntity owner)
	{
		param = "1, 50, 1";
		super.OnPostInit(owner);
	}
	
	override protected void EOnInit(IEntity owner)
	{
		param = "1, 30, 1";
		super.EOnInit(owner);
	}
	
	//TBD: For future needs shall add
	//protected int missionIndex;			//Index of the mission details
	//protected string subDir;				//Location for the mission conf to load. Overrides subDir defined by core
#endif
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Return the mission type
	*/
	DC_EMissionType GetMissionType()
	{
		return missionType;
	}
}