[ComponentEditorProps(category: "GameScripted/DarcChopper", description: "DarcChopper User Settings Sync")]
class SDRC_DarcChopperSettingsMenuComponentClass : ScriptComponentClass {}

//------------------------------------------------------------------------------------------------
class SDRC_DarcChopperSettingsMenuComponent : ScriptComponent
{
	protected RplComponent m_RplComponent;
	protected bool m_bLastEnabled;

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		SDRC_Log.Add("[SDRC_DarcChopperSettingsMenuComponent:EOnInit] Here!", LogLevel.NORMAL);		
		
		m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		ReadCurrent();

		SCR_PlayerController controller = SCR_PlayerController.Cast(owner);
		if (!controller)
			return;
		
		controller.GetOnOwnershipChangedInvoker().Insert(OnOwnershipChanged);

		if (Replication.IsServer() && !System.IsConsoleApp())
			OnOwnershipChanged(false, true);
	}

	//------------------------------------------------------------------------------------------------
	protected void OnOwnershipChanged(bool changing, bool becameOwner)
	{
		GetGame().OnUserSettingsChangedInvoker().Remove(OnUserSettingsChanged);
		if (!becameOwner)
			return;

		GetGame().OnUserSettingsChangedInvoker().Insert(OnUserSettingsChanged);
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}

	//------------------------------------------------------------------------------------------------
	protected void OnUserSettingsChanged()
	{
		SDRC_Log.Add("[SDRC_DarcChopperSettingsMenuComponent:OnUserSettingsChanged] Here!", LogLevel.NORMAL);		
		
		bool b = SDRC_SettingsMenuHelper.GetShowFlyPathEnabled();

		if (b != m_bLastEnabled)
		{
			m_bLastEnabled = b;
			RequestSaveSetting(b);
		}
	}

	//------------------------------------------------------------------------------------------------
	void OnSettingRequested(bool b)
	{
		if (!m_RplComponent || m_RplComponent.IsMaster())
			RPC_OnSettingRequested(b);
		else
			Rpc(RPC_OnSettingRequested, b);
	}

	//------------------------------------------------------------------------------------------------
	void RequestSaveSetting(bool b)
	{
		if (!m_RplComponent || m_RplComponent.IsMaster())
			RPC_SaveSettingRequested(b);
		else
			Rpc(RPC_SaveSettingRequested, b);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RPC_OnSettingRequested(bool b)
	{
		m_bLastEnabled = b;

		SDRC_SettingsMenuHelper.SetShowFlyPathEnabled(b);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RPC_RequestSetting()
	{
		SCR_PlayerController controller = SCR_PlayerController.Cast(GetOwner());
		if (!controller)
			return;

		UUID guid = SCR_PlayerIdentityUtils.GetPlayerIdentityId(controller.GetPlayerId());
		if (guid.IsNull())
			return;

/*		ADZ_RESTCalls restctx = ADZ_RESTCalls.GetInstance();
        if (!restctx)
            return;
        array<string> keys = { 
			"ShowFlyPath", 
		};
        restctx.RequestSetting(guid, keys);*/
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RPC_SaveSettingRequested(bool b)
	{
		SCR_PlayerController controller = SCR_PlayerController.Cast(GetOwner());
		if (!controller)
			return;

		UUID guid = SCR_PlayerIdentityUtils.GetPlayerIdentityId(controller.GetPlayerId());
		if (guid.IsNull())
			return;

/*		ADZ_RESTCalls restctx = ADZ_RESTCalls.GetInstance();
		if (!restctx)
			return;

		restctx.SaveSetting(guid, b);*/
	}

	//------------------------------------------------------------------------------------------------
	protected void ReadCurrent()
	{
		m_bLastEnabled = SDRC_SettingsMenuHelper.GetShowFlyPathEnabled();
	}
};