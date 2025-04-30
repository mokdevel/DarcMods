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
};
