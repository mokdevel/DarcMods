//------------------------------------------------------------------------------------------------
modded class SCR_PopUpNotification
{
	protected DC_EMissionIcon m_Icon;
	protected ImageWidget m_wImageWidget;
	protected ImageWidget m_wImageWidgetShadow;

	//------------------------------------------------------------------------------------------------
	/*!	
	Set the icon for a hint
	*/
	void SetIcon(DC_EMissionIcon icon)
	{
		m_Icon = icon;
	}				
}

//------------------------------------------------------------------------------------------------
class SDRC_SCR_PopUpNotificationClass : SCR_PopUpNotificationClass
{
};

//------------------------------------------------------------------------------------------------
//! Takes care of dynamic and static onscreen popups
class SDRC_SCR_PopUpNotification : SCR_PopUpNotification
{
	protected static const ResourceName SDRC_LAYOUT_NAME = "{92F50A7B6329D030}UI/layouts/Common/SDRC_PopupUI.layout";
	
	//------------------------------------------------------------------------------------------------
	override static SCR_PopUpNotification GetInstance()
	{
		if (!s_Instance)
		{
			BaseWorld world = GetGame().GetWorld();

			if (world)
				s_Instance = SDRC_SCR_PopUpNotification.Cast(GetGame().SpawnEntity(SDRC_SCR_PopUpNotification, world));
		}

		return s_Instance;
	}

	//------------------------------------------------------------------------------------------------
	override protected void ProcessInit()
	{
//		super.ProcessInit();
		
		if (!GetGame().GetHUDManager())
			return;

		PlayerController pc = GetGame().GetPlayerController();

		if (!pc || !pc.GetControlledEntity())
			return;

		Widget root = GetGame().GetHUDManager().CreateLayout(SDRC_LAYOUT_NAME, EHudLayers.MEDIUM, 0);

		if (!root)
			return;

		// Init can be safely processed
		GetGame().GetCallqueue().Remove(ProcessInit);

		// Initialize popups UI
		m_wPopupMsg = RichTextWidget.Cast(root.FindAnyWidget("Popup"));
		m_wPopupMsgSmall = RichTextWidget.Cast(root.FindAnyWidget("PopupSmall"));
		m_wStatusProgress = ProgressBarWidget.Cast(root.FindAnyWidget("Progress"));
		m_fDefaultAlpha = m_wPopupMsg.GetColor().A();
		m_wPopupMsg.SetVisible(false);
		m_wPopupMsgSmall.SetVisible(false);
		m_wStatusProgress.SetVisible(false);

		//Darc changes		
		m_wImageWidget = ImageWidget.Cast(root.FindAnyWidget("Icon"));
		m_wImageWidgetShadow = ImageWidget.Cast(root.FindAnyWidget("IconShadow"));
//		LoadIcon();
		//Darc changes - end
		
		GetGame().GetCallqueue().CallLater(SetDefaultHorizontalPosition, 500);

		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());

		if (playerController)
			playerController.m_OnControlledEntityChanged.Insert(RefreshInventoryInvoker);

		RefreshQueue();

		// Popups should not be visible in map
		SCR_MapEntity mapEntity = SCR_MapEntity.GetMapInstance();
		if (mapEntity)
		{
			MapConfiguration config = mapEntity.GetMapConfig();
			if (config)
			{
				Widget mapWidget = config.RootWidgetRef;
				if (mapWidget)
					root.SetZOrder(mapWidget.GetZOrder() - 1);
			}
		}
	}
	//------------------------------------------------------------------------------------------------
	override protected void ShowMsg(notnull SCR_PopupMessage msg)
	{
		if (!m_bIsEnabledInSettings)
			return;
		
//		if (msg == m_ShownMsg)
//			return;

		LoadIcon();
		ShowIcon();
		
		super.ShowMsg(msg);
	}

	//------------------------------------------------------------------------------------------------
	override protected void HideMsg(notnull SCR_PopupMessage msg)
	{
		if (msg == m_ShownMsg)
		{
			FadeWidget(m_wImageWidget, true);
			FadeWidget(m_wImageWidgetShadow, true);
		}

		ShowIcon();
		
		super.HideMsg(msg);
	}
	
	//------------------------------------------------------------------------------------------------
	void ShowIcon()
	{
		if (m_wImageWidget)
		{
			m_wImageWidget.SetVisible(true);
			m_wImageWidgetShadow.SetVisible(true);
/*			if (m_Icon == DC_EMissionIcon.NONE)
			{
				m_wImageWidget.SetVisible(false);
				m_wImageWidgetShadow.SetVisible(false);
			}
			else
			{
				m_wImageWidget.SetVisible(true);
				m_wImageWidgetShadow.SetVisible(true);
			}*/
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	void LoadIcon()
	{
		Widget root = GetGame().GetHUDManager().CreateLayout(SDRC_LAYOUT_NAME, EHudLayers.MEDIUM, 0);

		if (!root)		
			return;
		
		m_wImageWidget = ImageWidget.Cast(root.FindAnyWidget("Icon"));
		m_wImageWidgetShadow = ImageWidget.Cast(root.FindAnyWidget("IconShadow"));
		
		ResourceName texture = SDRC_IconHelper.GetMarkerTexture(m_Icon);		
		
		m_wImageWidget.LoadImageTexture(0, texture);
		m_wImageWidgetShadow.LoadImageTexture(0, texture);
//		ShowIcon();
		
		//By default we hide it.
//		m_wImageWidget.SetVisible(false);
//		m_wImageWidgetShadow.SetVisible(false);

	}	
};
