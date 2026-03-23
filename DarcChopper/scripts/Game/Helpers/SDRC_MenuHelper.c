class SDRC_MenuHelper
{
	static const string MODULE_NAME = "SDRC_EnableSetting";
	static const string FLYPATH_KEY = "m_bShowFlyPath";

	//------------------------------------------------------------------------------------------------
	static bool GetShowFlyPath()
	{
		BaseContainer module = GetGame().GetGameUserSettings().GetModule(MODULE_NAME);
		if (!module) 
		{
			return false;
		}
		bool val; 
		module.Get(FLYPATH_KEY, val); 
		return val;
	}
	
	static void SetShowFlyPath(bool enabled)
	{
		BaseContainer module = GetGame().GetGameUserSettings().GetModule(MODULE_NAME);
		if (!module) 
		{
			return;
		}
		module.Set(FLYPATH_KEY, enabled);
	}
}