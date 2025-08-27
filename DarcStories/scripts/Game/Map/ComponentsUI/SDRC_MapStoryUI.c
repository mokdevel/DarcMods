class SDRC_MapStoryUI : SCR_MapRulerUI
{
	const string TOOL_MENU_ICONSET = "{A07F0C1D34FDFD4E}UI/Textures/Icons/SDRC_icons_mapMarkersUInew.imageset";
	const string TOOL_MENU_ICON_NAME = "icon_DeathMarker_map";
	
	// Widgets
	protected TextWidget m_wTitle;
	protected TextWidget m_wText;
	
	//------------------------------------------------------------------------------------------------
	override void Init()
	{
//		m_sRootWidgetName = "StoryFrame";
//		m_sImageWidgetName = "StoryImage";
		
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
	override void OnMapOpen(MapConfiguration config)
	{
		// refresh widgets
		m_wTitle = TextWidget.Cast(m_RootWidget.FindAnyWidget("StoryTitle"));
		m_wText = TextWidget.Cast(m_RootWidget.FindAnyWidget("StoryText"));
		
		m_wTitle.SetText("Yeah, new title");

		super.OnMapOpen(config);
	}
	
		
	//------------------------------------------------------------------------------------------------
	// constructor
	void SCR_MapStoryUI()
	{
		m_bHookToRoot = true;
	}
}
