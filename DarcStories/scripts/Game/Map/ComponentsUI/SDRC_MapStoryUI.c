class SDRC_MapStoryUI : SCR_MapRulerUI
{
	const string TOOL_MENU_ICONSET = "{A07F0C1D34FDFD4E}UI/Textures/Icons/SDRC_icons_mapMarkersUInew.imageset";
	const string TOOL_MENU_ICON_NAME = "icon_DeathMarker_map";
	const string CLOCK_ICONSET_NAME = "{3FF54281E7F02F92}UI/Textures/IconsClock/SDRC_iconsClock.imageset";
	
	// Widgets
	protected ImageWidget m_wImageStoryBackground;
	protected ImageWidget m_wImageStoryImage;
	protected TextWidget m_wStoryBrief;
	protected TextWidget m_wStoryTitle;
	protected TextWidget m_wStoryTitleText;
	protected TextWidget m_wStoryChapterTitle;
	protected TextWidget m_wStoryStatus;
	protected TextWidget m_wStoryStatusTitle;
	protected TextWidget m_wStoryText;
	protected string imageNameOld;
		
	//------------------------------------------------------------------------------------------------
	override void Init()
	{
		m_sRootWidgetName = "StoryFrame";
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
			m_wImageStoryBackground.SetEnabled(visible);
			m_wImageStoryBackground.SetVisible(visible);
			m_wImageStoryImage.SetEnabled(visible);
			m_wImageStoryImage.SetVisible(visible);
			m_wStoryBrief.SetEnabled(visible);
			m_wStoryBrief.SetVisible(visible);
			m_wStoryTitle.SetEnabled(visible);
			m_wStoryTitle.SetVisible(visible);
			m_wStoryTitleText.SetEnabled(visible);
			m_wStoryTitleText.SetVisible(visible);
			m_wStoryChapterTitle.SetEnabled(visible);
			m_wStoryChapterTitle.SetVisible(visible);
			m_wStoryStatus.SetEnabled(visible);
			m_wStoryStatus.SetVisible(visible);
			m_wStoryStatusTitle.SetEnabled(visible);
			m_wStoryStatusTitle.SetVisible(visible);
			m_wStoryText.SetEnabled(visible);
			m_wStoryText.SetVisible(visible);
			
			if (!m_wImageStoryBackground)
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
				m_fAngle = m_wImageStoryImage.GetRotation();
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

		m_wImageStoryBackground = ImageWidget.Cast(m_RootWidget.FindAnyWidget("StoryBackground"));		
		m_wImageStoryImage = ImageWidget.Cast(m_RootWidget.FindAnyWidget("StoryImage"));		
		m_wStoryBrief = TextWidget.Cast(m_RootWidget.FindAnyWidget("StoryBrief"));
		m_wStoryTitle = TextWidget.Cast(m_RootWidget.FindAnyWidget("StoryTitle"));
		m_wStoryTitleText = TextWidget.Cast(m_RootWidget.FindAnyWidget("StoryTitleText"));
		m_wStoryChapterTitle = TextWidget.Cast(m_RootWidget.FindAnyWidget("StoryChapterTitle"));
		m_wStoryStatus = TextWidget.Cast(m_RootWidget.FindAnyWidget("StoryStatus"));
		m_wStoryStatusTitle = TextWidget.Cast(m_RootWidget.FindAnyWidget("StoryStatusTitle"));
		m_wStoryText = TextWidget.Cast(m_RootWidget.FindAnyWidget("StoryText"));
				
		SDRC_RplStoryComp storyComp = SDRC_RplStoryComp.GetInstance();
		if (storyComp)
		{
			m_wStoryTitleText.SetText(storyComp.GetStoryTitle());
			m_wStoryChapterTitle.SetText(storyComp.GetTitle());
			m_wStoryText.SetText(storyComp.GetText());
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

			string imageName = GetClockImageName(timeLeft);
			//TBD: If it has not changed, do not reload		
			if (imageName != imageNameOld)
			{
				bool imageLoaded = m_wImageStoryImage.LoadImageFromSet(0, CLOCK_ICONSET_NAME, imageName);
				if (imageLoaded)
				{
					imageNameOld = imageName;
					m_wImageStoryImage.SetImage(0);
				}
			}
							
			m_wStoryChapterTitle.SetText(storyComp.GetTitle() + " - " + timeLeft);
			m_wStoryStatus.SetText(storyComp.GetSuccess());
		}
		
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
