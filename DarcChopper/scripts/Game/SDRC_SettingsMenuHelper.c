//SDRC_SettingsMenuHelper.c

/* This piece of code is a modified version from Aiming Deadzone way of implementing */

//------------------------------------------------------------------------------------------------
class SDRC_SettingsMenuHelper
{
	static const string MODULE_NAME = "SDRC_EnableSetting";
	static const string ENABLED_KEY = "m_bShowFlyPath";

	static bool GetShowFlyPathEnabled()
	{
		BaseContainer module = GetGame().GetGameUserSettings().GetModule(MODULE_NAME);
		if (!module) 
		{
			return false;
		}
		bool val; 
		module.Get(ENABLED_KEY, val); return val;
	}
	static void SetShowFlyPathEnabled(bool enabled)
	{
		BaseContainer module = GetGame().GetGameUserSettings().GetModule(MODULE_NAME);
		if (!module) 
		{
			return;
		}
		module.Set(ENABLED_KEY, enabled);
	}
	

/*	static SCR_PlayerController GetPlayerControllerByUUID(UUID guid)
	{
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		UUID playerUUID;
		
		if (playerController) {
			playerUUID = SCR_PlayerIdentityUtils.GetPlayerIdentityId(playerController.GetPlayerId());
			if (playerUUID == guid)
				return playerController;
		}
		
		const PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return null;
			
		array<int> playerIds();
		playerManager.GetAllPlayers(playerIds);
			
		foreach (int playerId : playerIds) {
			playerUUID = SCR_PlayerIdentityUtils.GetPlayerIdentityId(playerId);
			if (guid == playerUUID) {
				playerController = SCR_PlayerController.Cast(playerManager.GetPlayerController(playerId));
				break;
			}
		}
			
		return playerController;
	}*/
}