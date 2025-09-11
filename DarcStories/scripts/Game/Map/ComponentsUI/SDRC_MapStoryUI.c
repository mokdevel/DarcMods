class SDRC_MapStoryUI : SCR_MapRulerUI
{
	const string TOOL_MENU_ICONSET = "{A07F0C1D34FDFD4E}UI/Textures/Icons/SDRC_icons_mapMarkersUInew.imageset";
	const string TOOL_MENU_ICON_NAME = "icon_DeathMarker_map";
	const string CLOCK_ICONSET_NAME = "{3FF54281E7F02F92}UI/Textures/IconsClock/SDRC_iconsClock.imageset";
	
	// Widgets
	protected TextWidget m_wTitle;
	protected TextWidget m_wText;
	protected ImageWidget m_wImageStory;
	protected ImageWidget m_wImageBack;
	protected string imageNameOld;
	
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
//		super.SetVisible(visible, saveState);
		
		saveState = true;
		m_bIsVisible = visible;
		m_wFrame.SetEnabled(visible);
		m_wFrame.SetVisible(visible);
		
		if (visible)
		{
			m_wImageStory.SetEnabled(visible);
			m_wImageStory.SetVisible(visible);
			m_wImageBack.SetEnabled(visible);
			m_wImageBack.SetVisible(visible);
			m_wTitle.SetEnabled(visible);
			m_wTitle.SetVisible(visible);
			m_wText.SetEnabled(visible);
			m_wText.SetVisible(visible);
			
			if (!m_wImageBack)
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

			float zoomVal = m_MapEntity.GetCurrentZoom();
//			m_fSizeCoef = 1000 / (m_fRulerLength / m_fBaseImageSize[0]); // (ruler real length%) / 1000 pix(meters)
			m_fSizeCoef = 1000 / (600 / m_fBaseImageSize[0]); // (image real length%) / 1000 pix(meters)
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
			
			m_MapEntity.GetOnMapZoom().Insert(OnMapZoom);	// zoom for scaling
			m_MapEntity.GetOnMapPan().Insert(OnMapPan);		// pan for scaling
		}
		else
		{
			if (saveState)	// save angle
			{
				m_fAngle = m_wImageStory.GetRotation();
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
		imageNameOld = "";
		
		// refresh widgets
		m_wFrame = m_RootWidget.FindAnyWidget("StoryFrame");
		m_wImageStory = ImageWidget.Cast(m_RootWidget.FindAnyWidget("StoryImage"));		
		m_wImageBack = ImageWidget.Cast(m_RootWidget.FindAnyWidget("StoryBackground"));		
		m_wTitle = TextWidget.Cast(m_RootWidget.FindAnyWidget("StoryTitle"));
		m_wText = TextWidget.Cast(m_RootWidget.FindAnyWidget("StoryText"));
		
		SDRC_RplStoryComp storyComp = SDRC_RplStoryComp.GetInstance();
		if (storyComp)
		{
			m_wTitle.SetText(storyComp.GetTitle());
			m_wText.SetText(storyComp.GetText());
		}						
		super.OnMapOpen(config);		
	}

	//------------------------------------------------------------------------------------------------
	override void Update(float timeSlice)
	{
		int timeLeft = 100;
		SDRC_RplStoryComp storyComp = SDRC_RplStoryComp.GetInstance();
		if (storyComp)
		{
			timeLeft = storyComp.GetTimeLeft();
		}

		string imageName = GetClockImageName(timeLeft);
		//TBD: If it has not changed, do not reload		
		if (imageName != imageNameOld)
		{
			bool imageLoaded = m_wImageStory.LoadImageFromSet(0, CLOCK_ICONSET_NAME, imageName);
			if (imageLoaded)
			{
				imageNameOld = imageName;
				m_wImageStory.SetImage(0);
			}
		}
						
		m_wTitle.SetText(storyComp.GetTitle() + " - " + timeLeft);
		super.Update(timeSlice);
	}
			
	//------------------------------------------------------------------------------------------------
	// constructor
	void SCR_MapStoryUI()
	{
		m_bHookToRoot = true;
	}
	
	//------------------------------------------------------------------------------------------------
	// GetClockImageName
	
	string GetClockImageName(int percent)
	{
		array<string> names = {"clock01", "clock02", "clock03", "clock04", "clock05", "clock06", "clock07", "clock08", "clock09", "clock10"};
		
		int idx = percent/10;
		if (idx < 0)
		{
			idx = 0;
		}
		if (idx > 9)
		{
			idx = 9;
		}
		
		return names[idx];		
	}
}
