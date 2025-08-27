class SDRC_MapStoryUI : SCR_MapRulerUI
{
/*	[Attribute(defvalue: "1994", uiwidget: UIWidgets.EditBox, desc: "pix, exact length of the ruler within the provided image, default base length being 1km in world space")]
	float m_fRulerLength;

//	[Attribute("StoryFrame", UIWidgets.EditBox, desc: "Root frame widget name")]
	string m_sRootWidgetName;
	
//	[Attribute("StoryImage", UIWidgets.EditBox, desc: "Ruler image widget name")]
	string m_sImageWidgetName; 
	
	//[Attribute("ruler", UIWidgets.EditBox, desc: "Toolmenu imageset quad name")]
	string m_sToolMenuIconName;
*/	
	const string TOOL_MENU_ICONSET = "{A07F0C1D34FDFD4E}UI/Textures/Icons/SDRC_icons_mapMarkersUInew.imageset";
	const string TOOL_MENU_ICON_NAME = "icon_DeathMarker_map";
	
	//------------------------------------------------------------------------------------------------
	override void Init()
	{
		m_sRootWidgetName = "StoryFrame";
		m_sImageWidgetName = "StoryImage";
		
		m_fPosX = 0;
		m_fPosY = 0;
		m_wWorkspace = GetGame().GetWorkspace();
		
		SCR_MapToolMenuUI toolMenu = SCR_MapToolMenuUI.Cast(m_MapEntity.GetMapUIComponent(SCR_MapToolMenuUI));
		if (toolMenu)
		{
			m_ToolMenuEntry = toolMenu.RegisterToolMenuEntry(TOOL_MENU_ICONSET, TOOL_MENU_ICON_NAME, 100); // add to menu		
			m_ToolMenuEntry.m_OnClick.Insert(ToggleVisible);
			m_ToolMenuEntry.SetEnabled(true);
			
			GetGame().GetInputManager().AddActionListener("MapToolProtractor", EActionTrigger.DOWN, OnInputQuickBind);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// constructor
	void SCR_MapStoryUI()
	{
		m_bHookToRoot = true;
	}
}
