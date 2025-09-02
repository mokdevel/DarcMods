class SDRC_MapStoryUI : SCR_MapRulerUI
{
	const string TOOL_MENU_ICONSET = "{A07F0C1D34FDFD4E}UI/Textures/Icons/SDRC_icons_mapMarkersUInew.imageset";
	const string TOOL_MENU_ICON_NAME = "icon_DeathMarker_map";
	
	// Widgets
	protected TextWidget m_wTitle;
	protected TextWidget m_wText;
	protected ImageWidget m_wImageBack;
	
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
	//! Set visibility
	//! \param[in] visible is true/false switch
	//! \param[in] saveState determines whether this is visibility set during closing of the map, so the pos and rotation should be saved
	override protected void SetVisible(bool visible, bool saveState = false)
	{
		//If story is not running yet, show nothing
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		if (baseGameMode)
		{
			if (!baseGameMode.storiesFrame)
			{
				return;
			}
		}
		
		m_bIsVisible = visible;
		m_wFrame.SetEnabled(visible);
		m_wFrame.SetVisible(visible);
		
		if (visible)
		{
			m_wImage.SetEnabled(visible);
			m_wImage.SetVisible(visible);
			m_wImageBack.SetEnabled(visible);
			m_wImageBack.SetVisible(visible);
			m_wTitle.SetEnabled(visible);
			m_wTitle.SetVisible(visible);
			m_wText.SetEnabled(visible);
			m_wText.SetVisible(visible);
			
			if (!m_wImage)
				return;

			ChimeraCharacter player = ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
			if (!player || !player.GetCharacterController())
				return;

			CharacterControllerComponent controller = player.GetCharacterController();
			if (!controller)
				return;

			IEntity mapItem = controller.GetAttachedGadgetAtLeftHandSlot();
			if (!mapItem)
				return;

			SCR_MapGadgetComponent mapGadget = SCR_MapGadgetComponent.Cast(mapItem.FindComponent(SCR_MapGadgetComponent));
			if (!mapGadget)
				return;

/*			m_fRulerLength = mapGadget.GetRulerLength();
			if (m_wImage.LoadImageTexture(0, mapGadget.GetProtractorTexture()))
				m_wImage.SetImage(0);*/

			float zoomVal = m_MapEntity.GetCurrentZoom();
			m_fSizeCoef = 1000 / (m_fRulerLength / m_fBaseImageSize[0]); // (ruler real length%) / 1000 pix(meters)
			float sizeVal = m_wWorkspace.DPIUnscale(zoomVal * m_fSizeCoef);
			SetSize(sizeVal, sizeVal);
			
			if (m_fPosX == 0 && m_fPosY == 0)
			{
				float sizeX, sizeY;
				m_MapEntity.GetMapWidget().GetScreenSize(sizeX, sizeY);
				m_fPosX = sizeX * 0.5;
				m_fPosY = sizeY * 0.5;
				
				m_MapEntity.ScreenToWorld(m_fPosX, m_fPosY, m_fWorldX, m_fWorldY);
			}
			
/*			FrameSlot.SetPos(m_wFrame, m_wWorkspace.DPIUnscale(m_fPosX), m_wWorkspace.DPIUnscale(m_fPosY));
			m_wImage.SetRotation(m_fAngle);
			m_vMapPan = m_MapEntity.GetCurrentPan();*/
			
			m_MapEntity.GetOnMapZoom().Insert(OnMapZoom);	// zoom for scaling
			m_MapEntity.GetOnMapPan().Insert(OnMapPan);		// pan for scaling
		}
		else
		{
			if (saveState)	// save angle
			{
				m_fAngle = m_wImage.GetRotation();
			}
			else 
			{
				m_fAngle = 0;
				m_fPosX = 0;
				m_fPosY = 0;
			}

			m_MapEntity.GetOnMapZoom().Remove(OnMapZoom);	// zoom for scaling
			m_MapEntity.GetOnMapPan().Remove(OnMapPan);		// zoom for scaling
		}
		
		if (m_ToolMenuEntry)
			m_ToolMenuEntry.SetActive(visible);
	}	

	//------------------------------------------------------------------------------------------------
	override void OnMapOpen(MapConfiguration config)
	{
		// refresh widgets
		m_wFrame = m_RootWidget.FindAnyWidget("StoryFrame");
		m_wImage = ImageWidget.Cast(m_RootWidget.FindAnyWidget("StoryImage"));		
		m_wImageBack = ImageWidget.Cast(m_RootWidget.FindAnyWidget("StoryBackground"));		
		m_wTitle = TextWidget.Cast(m_RootWidget.FindAnyWidget("StoryTitle"));
		m_wText = TextWidget.Cast(m_RootWidget.FindAnyWidget("StoryText"));
		
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		if (baseGameMode)
		{
			if (baseGameMode.storiesFrame)
			{
				m_wTitle.SetText(baseGameMode.storiesFrame.m_Chapter.title);
				m_wText.SetText(baseGameMode.storiesFrame.m_Chapter.story);
			}
		}

		super.OnMapOpen(config);
	}
		
	//------------------------------------------------------------------------------------------------
	// constructor
	void SCR_MapStoryUI()
	{
		m_bHookToRoot = true;
	}
}
