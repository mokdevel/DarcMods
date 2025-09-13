//------------------------------------------------------------------------------------------------
/*!
This is the map entity that shows the mission briefing. This is based on SCR_MapRulerUI.
*/

class SDRC_MapStoryUI : SCR_MapUIBaseComponent
{
	const string TOOL_MENU_ICONSET = "{A07F0C1D34FDFD4E}UI/Textures/Icons/SDRC_icons_mapMarkersUInew.imageset";
	const string TOOL_MENU_ICON_NAME = "icon_DeathMarker_map";
	const string CLOCK_ICONSET_NAME = "{3FF54281E7F02F92}UI/Textures/IconsClock/SDRC_iconsClock.imageset";
	
	string m_sRootWidgetName;	
	string m_sImageWidgetName; 
	string m_sToolMenuIconName;
	
	[Attribute("", UIWidgets.Object, desc: "Array of sizes, multiplicator values of default length of 1km")]
	protected ref array<ref int> m_aSizesArray; 

	protected bool m_bIsVisible;
	protected bool m_bWantedVisible;
	protected bool m_bIsDragged;
	protected bool m_bIsZooming;
	protected int m_iCurrentSizeIndex;			// current id of m_SizesArray
	protected int m_iSizesCount;				// count of m_SizesArray
	protected float m_fPosX, m_fPosY;			// widget position in scaled screen coords
	protected float m_fWorldX, m_fWorldY;		// widget world pos
	protected float m_fBaseImageSize[2];		// for calculation of ruler scaling
	protected float m_fSizeCoef;				// ruler size coefficient based on image size and ruler length 
	protected vector m_vMapPan;
	protected SCR_MapToolEntry m_ToolMenuEntry;
	
	// Widgets
	protected Widget m_wFrame;
	protected ImageWidget m_wImage;
	protected WorkspaceWidget m_wWorkspace;
	
	// Widgets - story specific
	protected ImageWidget m_wImageStoryBackground;
	protected ImageWidget m_wImageStoryLines;
	protected ImageWidget m_wImageStoryClock;
	protected TextWidget m_wStoryBrief;
	protected TextWidget m_wStoryTitle;
	protected TextWidget m_wStoryTitleText;
	protected TextWidget m_wStoryChapterTitle;
	protected TextWidget m_wStoryStatus;
	protected TextWidget m_wStoryStatusTitle;
	protected TextWidget m_wStoryText;
	
	// Additional
	protected string imageNameOld;
	protected bool m_bForceUpdate;
	
	//------------------------------------------------------------------------------------------------
	override void Init()
	{
		m_sRootWidgetName = "StoryFrame";
		m_sImageWidgetName = "StoryLines";
		
		m_fPosX = 0;
		m_fPosY = 0;
		m_wWorkspace = GetGame().GetWorkspace();
		
		SCR_MapToolMenuUI toolMenu = SCR_MapToolMenuUI.Cast(m_MapEntity.GetMapUIComponent(SCR_MapToolMenuUI));
		if (toolMenu)
		{
			m_ToolMenuEntry = toolMenu.RegisterToolMenuEntry(TOOL_MENU_ICONSET, TOOL_MENU_ICON_NAME, 100); // add to menu		
			m_ToolMenuEntry.m_OnClick.Insert(ToggleVisible);
			m_ToolMenuEntry.SetEnabled(true);
			
//			GetGame().GetInputManager().AddActionListener("MapToolProtractor", EActionTrigger.DOWN, OnInputQuickBind);
			GetGame().GetInputManager().AddActionListener("MapToolBriefing", EActionTrigger.DOWN, OnInputQuickBind);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Set visibility
	//! \param[in] visible is true/false switch
	//! \param[in] saveState determines whether this is visibility set during closing of the map, so the pos and rotation should be saved
	protected void SetVisible(bool visible, bool saveState = false)
	{
		m_bIsVisible = visible;
		m_wFrame.SetEnabled(visible);
		m_wFrame.SetVisible(visible);
		
		if (visible)
		{
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
			m_fSizeCoef = 1000 / (1024 / m_fBaseImageSize[0]); // (image real length%) / 1000 pix(meters)
			
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

			FrameSlot.SetPos(m_wFrame, m_wWorkspace.DPIUnscale(m_fPosX), m_wWorkspace.DPIUnscale(m_fPosY));
			m_vMapPan = m_MapEntity.GetCurrentPan();
			
			m_bForceUpdate = true;
			
			m_MapEntity.GetOnMapZoom().Insert(OnMapZoom);	// zoom for scaling
			m_MapEntity.GetOnMapPan().Insert(OnMapPan);		// pan for scaling
		}
		else
		{
			m_MapEntity.GetOnMapZoom().Remove(OnMapZoom);	// zoom for scaling
			m_MapEntity.GetOnMapPan().Remove(OnMapPan);		// zoom for scaling
		}
		
		if (m_ToolMenuEntry)
			m_ToolMenuEntry.SetActive(visible);
	}	

	//------------------------------------------------------------------------------------------------
	override void OnMapOpen(MapConfiguration config)
	{	
		super.OnMapOpen(config);		
				
		imageNameOld = "";
		
		// refresh widgets
		m_wFrame = m_RootWidget.FindAnyWidget(m_sRootWidgetName);
		m_wImage = ImageWidget.Cast(m_RootWidget.FindAnyWidget(m_sImageWidgetName));
		
		// refresh story widgets
		m_wImageStoryBackground = ImageWidget.Cast(m_RootWidget.FindAnyWidget("StoryBackground"));		
		m_wImageStoryLines = ImageWidget.Cast(m_RootWidget.FindAnyWidget("StoryLines"));		
		m_wImageStoryClock = ImageWidget.Cast(m_RootWidget.FindAnyWidget("StoryClock"));		
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
		
		int x, y;
		m_wImage.GetImageSize(0, x, y);
		m_fBaseImageSize[0] = x;
		m_fBaseImageSize[1] = y;
		
		m_iSizesCount = m_aSizesArray.Count();
		
		if ( SCR_MapToolInteractionUI.Cast(m_MapEntity.GetMapUIComponent(SCR_MapToolInteractionUI)) )	// if dragging available, add callback
		{
			SCR_MapToolInteractionUI.GetOnDragWidgetInvoker().Insert(OnDragWidget);
			SCR_MapToolInteractionUI.GetOnActivateToolInvoker().Insert(OnActivateTool);
		}
				
		SetVisible(m_bWantedVisible);	// restore last visible state
		OnMapZoom(m_MapEntity.GetCurrentZoom());	
	}

	//------------------------------------------------------------------------------------------------
	override void OnMapClose(MapConfiguration config)
	{				
		m_bWantedVisible = m_bIsVisible;	// visibility state
		SetVisible(false, true);
		
		super.OnMapClose(config);
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
			//If it has not changed, do not reload		
			if (imageName != imageNameOld)
			{
				bool imageLoaded = m_wImageStoryClock.LoadImageFromSet(0, CLOCK_ICONSET_NAME, imageName);
				if (imageLoaded)
				{
					imageNameOld = imageName;
					m_wImageStoryClock.SetImage(0);
				}
				
				m_wStoryTitleText.SetText(storyComp.GetStoryTitle());
				m_wStoryChapterTitle.SetText(storyComp.GetTitle() + " - " + timeLeft);
				m_wStoryText.SetText(storyComp.GetText());				
				m_wStoryStatus.SetText(storyComp.GetSuccess());
				
			}
							
		}
		
//		if ( (m_bIsVisible && m_bIsDragged) || (m_bForceUpdate) )
		if (m_bIsVisible && m_bIsDragged)
		{
			m_bForceUpdate = false;
			// save position for map reopen
			float sizeX, sizeY;
			m_wFrame.GetScreenPos(m_fPosX, m_fPosY);
			m_wFrame.GetScreenSize(sizeX, sizeY);
			m_fPosX = m_fPosX + sizeX * 0.5;
			m_fPosY = m_fPosY + sizeY * 0.5;
			
			m_MapEntity.ScreenToWorld(m_fPosX, m_fPosY, m_fWorldX, m_fWorldY);
		}
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
	
	//------------------------------------------------------------------------------------------------
	// Things from SCR_MapRulerUI
	//------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------
	//! Set size of image, multiply by current size mode
	//! \param[in] x is unscaled size in px
	//! \param[in] y is unscaled size in px
	//! \param[in] nextSize determines whether current size is kept or swapped to the next one in size array
	protected void SetSize(float x, float y, bool nextSize = false)
	{
		if (nextSize)
		{
			if (m_iCurrentSizeIndex < m_iSizesCount - 1)
				m_iCurrentSizeIndex++;
			else 
				m_iCurrentSizeIndex = 0; 
		}
		
		m_wImage.SetSize(x * m_aSizesArray[m_iCurrentSizeIndex], y * m_aSizesArray[m_iCurrentSizeIndex]);

	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnInputQuickBind(float value, EActionTrigger reason)
	{
		if (!GetGame().GetInputManager().IsUsingMouseAndKeyboard() && IsToolMenuFocused())
			return;	
		ToggleVisible();
	}
		
	//------------------------------------------------------------------------------------------------
	//! Visibility toggle
	protected void ToggleVisible()
	{
		if (!m_bIsVisible)
			SetVisible(true);
		else
			SetVisible(false);
	}
	
	//------------------------------------------------------------------------------------------------
	//! SCR_MapEntity event
	protected void OnMapZoom(float zoomVal)
	{
		float sizeVal = m_wWorkspace.DPIUnscale(zoomVal * m_fSizeCoef);
		SetSize(sizeVal, sizeVal);
		m_bIsZooming = true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! SCR_MapEntity event
	protected void OnMapPan(float x, float y, bool adjustedPan)
	{
		if (m_bIsZooming)
		{
			m_bIsZooming = false;
			
			float screenX, screenY;
			m_MapEntity.WorldToScreen( m_fWorldX, m_fWorldY, screenX, screenY, true );
				
			m_fPosX = screenX;
			m_fPosY = screenY;
		}
		else 
		{
			m_fPosX -= m_vMapPan[0] - m_wWorkspace.DPIScale(x);
			m_fPosY -= m_vMapPan[1] - m_wWorkspace.DPIScale(y);
		}
			
		FrameSlot.SetPos(m_wFrame, m_wWorkspace.DPIUnscale(m_fPosX), m_wWorkspace.DPIUnscale(m_fPosY));	
		m_vMapPan = m_MapEntity.GetCurrentPan();
	}
	
	//------------------------------------------------------------------------------------------------
	//! SCR_MapToolInteractionUI event
	protected void OnDragWidget(Widget widget)
	{
		if (widget == m_wFrame)
			m_bIsDragged = true;
		else 
			m_bIsDragged = false;
	}
	
	//------------------------------------------------------------------------------------------------
	//! SCR_MapToolInteractionUI event
	protected void OnActivateTool(Widget widget)
	{
		if (!m_wImage)
			return;

		m_wImage.SetRotation(0);
	}	
}
