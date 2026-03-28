//SDRC_GameplaySettingsSubMenu.c

/* This piece of code is a modified version from Aiming Deadzone way of implementing */

//------------------------------------------------------------------------------------------------
class SDRC_EnableSetting : ModuleGameSettings
{
	[Attribute(defvalue: "1", UIWidgets.CheckBox)]
	bool m_bShowFlyPath;
}

//------------------------------------------------------------------------------------------------
modded class SCR_GameplaySettingsSubMenu
{
	//------------------------------------------------------------------------------------------------	
	protected void SDRC_EntryCreate(string module, string setting, string widget)
	{
		SDRC_Log.Add("[SDRC_GameplaySettingsSubMenu:SDRC_EntryCreate] Here!", LogLevel.NORMAL);		
		
		if (!m_wScroll)
			return;
		
		const int idx = m_aSettingsBindings.Insert(new SCR_SettingBindingGameplay(module, setting, widget));
		const SCR_SettingsBindingBase bind = SCR_SettingsBindingBase.Cast(m_aSettingsBindings.Get(idx));
		
		if (!bind)
			return;
		
		bind.LoadEntry(m_wScroll, false, true);
		bind.GetEntryChangedInvoker().Insert(OnMenuItemChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnTabCreate(Widget menuRoot, ResourceName buttonsLayout, int index)
	{
		super.OnTabCreate(menuRoot, buttonsLayout, index);
		
		SDRC_EntryCreate("SDRC_EnableSetting", "m_bShowFlyPath", "ShowFlyPath");
	}
}