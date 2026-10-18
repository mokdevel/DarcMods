//------------------------------------------------------------------------------------------------
/*!
This is the main Spawner file. 
*/

//------------------------------------------------------------------------------------------------
modded class SDRC_Spawner
{
	const string DC_CONFIG_ADDON_NAME_OUTBREAKZOMBIES = "OutbreakZombies";
	const string DC_CONFIG_FILE_SPAWNER_OUTBREAKZOMBIES = "dc_spawnerConfig_OutbreakZombies.json";
	const int DC_CONFIG_FILE_SPAWNER_OUTBREAKZOMBIES_JSONVER = 1;
	
	private ref SDRC_JsonApi2 m_JsonApi_OutbreakZombies = null;
	private ref SDRC_SpawnerConfig_OutbreakZombies m_Config_OutbreakZombies = null;
		
	//------------------------------------------------------------------------------------------------
	override void LoadAddon(out array<ref SDRC_SpawnerConfig> spawnerConfigs)
	{
		SDRC_Log.Add("[SDRC_Spawner:LoadAddon] Loading addon: " + DC_CONFIG_ADDON_NAME_OUTBREAKZOMBIES, LogLevel.NORMAL);
		
		m_JsonApi_OutbreakZombies = new SDRC_JsonApi2(DC_CONFIG_FILE_SPAWNER_OUTBREAKZOMBIES);
		m_Config_OutbreakZombies = new SDRC_SpawnerConfig_OutbreakZombies();
		
		//Load config
		bool success = m_JsonApi_OutbreakZombies.Load(m_Config_OutbreakZombies, SDRC_SpawnerConfig_OutbreakZombies.Cast(m_Config_OutbreakZombies), DC_CONFIG_FILE_SPAWNER_OUTBREAKZOMBIES_JSONVER, safeUpdate: true);
		if (!success)
		{
			SDRC_Log.Add("[SDRC_Spawner:LoadAddon] Error loading " + DC_CONFIG_FILE_SPAWNER_OUTBREAKZOMBIES + ". DarcSpawner not started.", LogLevel.ERROR);
			return;
		}			
		
		if ( (m_Config_OutbreakZombies.spawnSetList.IsEmpty()) || (m_Config_OutbreakZombies.spawnSets.IsEmpty()) )
		{
			SDRC_Log.Add("[SDRC_Spawner:LoadAddon] No spawnSets defined. Addon '" + DC_CONFIG_ADDON_NAME_OUTBREAKZOMBIES + "' failed to start.", LogLevel.ERROR);
			return;			
		}

		spawnerConfigs.Insert(m_Config_OutbreakZombies);
		
		super.LoadAddon(spawnerConfigs);
	}
}