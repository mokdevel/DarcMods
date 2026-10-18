//------------------------------------------------------------------------------------------------
/*!
This is the main Spawner file. 
*/

//------------------------------------------------------------------------------------------------
modded class SDRC_Spawner
{
	const string DC_CONFIG_ADDON_NAME_ARGHFAUNA = "ArghFauna";
	const string DC_CONFIG_FILE_SPAWNER_ARGHFAUNA = "dc_spawnerConfig_ArghFauna.json";
	const int DC_CONFIG_FILE_SPAWNER_ARGHFAUNA_JSONVER = 1;
	
	private ref SDRC_JsonApi2 m_JsonApi_ArghFauna = null;
	private ref SDRC_SpawnerConfig_ArghFauna m_Config_ArghFauna = null;
		
	//------------------------------------------------------------------------------------------------
	override void LoadAddon(out array<ref SDRC_SpawnerConfig> spawnerConfigs)
	{
		SDRC_Log.Add("[SDRC_Spawner:LoadAddon] Loading addon: " + DC_CONFIG_ADDON_NAME_ARGHFAUNA, LogLevel.NORMAL);
		
		m_JsonApi_ArghFauna = new SDRC_JsonApi2(DC_CONFIG_FILE_SPAWNER_ARGHFAUNA);
		m_Config_ArghFauna = new SDRC_SpawnerConfig_ArghFauna();
		
		//Load config
		bool success = m_JsonApi_ArghFauna.Load(m_Config_ArghFauna, SDRC_SpawnerConfig_ArghFauna.Cast(m_Config_ArghFauna), DC_CONFIG_FILE_SPAWNER_ARGHFAUNA_JSONVER, safeUpdate: true);
		if (!success)
		{
			SDRC_Log.Add("[SDRC_Spawner:LoadAddon] Error loading " + DC_CONFIG_FILE_SPAWNER_ARGHFAUNA + ". DarcSpawner not started.", LogLevel.ERROR);
			return;
		}			
		
		if ( (m_Config_ArghFauna.spawnSetList.IsEmpty()) || (m_Config_ArghFauna.spawnSets.IsEmpty()) )
		{
			SDRC_Log.Add("[SDRC_Spawner:LoadAddon] No spawnSets defined. Addon '" + DC_CONFIG_ADDON_NAME_ARGHFAUNA + "' failed to start.", LogLevel.ERROR);
			return;			
		}

		spawnerConfigs.Insert(m_Config_ArghFauna);
		
		super.LoadAddon(spawnerConfigs);
	}
}