//------------------------------------------------------------------------------------------------
/*!
This is the main Spawner file. 
*/

//------------------------------------------------------------------------------------------------
modded class SDRC_Spawner
{
	const string DC_CONFIG_ADDON_NAME_OUTBREAKCREATURES = "OutbreakCreatures";
	const string DC_CONFIG_FILE_SPAWNER_OUTBREAKCREATURES = "dc_spawnerConfig_OutbreakCreatures.json";
	const int DC_CONFIG_FILE_SPAWNER_OUTBREAKCREATURES_JSONVER = 1;
	
	private ref SDRC_JsonApi2 m_JsonApi_OutbreakCreatures = null;
	private ref SDRC_SpawnerConfig_OutbreakCreatures m_Config_OutbreakCreatures = null;
		
	//------------------------------------------------------------------------------------------------
	override void LoadAddon(out array<ref SDRC_SpawnerConfig> spawnerConfigs)
	{
		SDRC_Log.Add("[SDRC_Spawner:LoadAddon] Loading addon: " + DC_CONFIG_ADDON_NAME_OUTBREAKCREATURES, LogLevel.NORMAL);
		
		m_JsonApi_OutbreakCreatures = new SDRC_JsonApi2(DC_CONFIG_FILE_SPAWNER_OUTBREAKCREATURES);
		m_Config_OutbreakCreatures = new SDRC_SpawnerConfig_OutbreakCreatures();
		
		//Load config
		bool success = m_JsonApi_OutbreakCreatures.Load(m_Config_OutbreakCreatures, SDRC_SpawnerConfig_OutbreakCreatures.Cast(m_Config_OutbreakCreatures), DC_CONFIG_FILE_SPAWNER_OUTBREAKCREATURES_JSONVER, safeUpdate: true);
		if (!success)
		{
			SDRC_Log.Add("[SDRC_Spawner:LoadAddon] Error loading " + DC_CONFIG_FILE_SPAWNER_OUTBREAKCREATURES + ". DarcSpawner not started.", LogLevel.ERROR);
			return;
		}			
		
		if ( (m_Config_OutbreakCreatures.spawnSetList.IsEmpty()) || (m_Config_OutbreakCreatures.spawnSets.IsEmpty()) )
		{
			SDRC_Log.Add("[SDRC_Spawner:LoadAddon] No spawnSets defined. Addon '" + DC_CONFIG_ADDON_NAME_OUTBREAKCREATURES + "' failed to start.", LogLevel.ERROR);
			return;			
		}

		spawnerConfigs.Insert(m_Config_OutbreakCreatures);
		
		super.LoadAddon(spawnerConfigs);
	}
}