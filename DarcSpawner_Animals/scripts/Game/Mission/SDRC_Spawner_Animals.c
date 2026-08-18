//------------------------------------------------------------------------------------------------
/*!
This is the main Spawner file. 
*/

//------------------------------------------------------------------------------------------------
modded class SDRC_Spawner
{
	const string DC_CONFIG_ADDON_NAME_ANIMALS = "Animals";
	const string DC_CONFIG_FILE_SPAWNER_ANIMALS = "dc_spawnerConfig_Animals.json";
	const int DC_CONFIG_FILE_SPAWNER_ANIMALS_JSONVER = 2;
	
	private ref SDRC_JsonApi2 m_JsonApi_Animals = null;
	private ref SDRC_SpawnerConfig_Animals m_Config_Animal = null;
		
	//------------------------------------------------------------------------------------------------
	override void LoadAddon(out array<ref SDRC_SpawnerConfig> spawnerConfigs)
	{
		SDRC_Log.Add("[SDRC_Spawner:LoadAddon] Loading addon: " + DC_CONFIG_ADDON_NAME_ANIMALS, LogLevel.NORMAL);
		
		m_JsonApi_Animals = new SDRC_JsonApi2(DC_CONFIG_FILE_SPAWNER_ANIMALS);
		m_Config_Animal = new SDRC_SpawnerConfig_Animals();
		
		//Load config
		bool success = m_JsonApi_Animals.Load(m_Config_Animal, SDRC_SpawnerConfig_Animals.Cast(m_Config_Animal), DC_CONFIG_FILE_SPAWNER_ANIMALS_JSONVER, safeUpdate: true);
		if (!success)
		{
			SDRC_Log.Add("[SDRC_Spawner:LoadAddon] Error loading " + DC_CONFIG_FILE_SPAWNER_ANIMALS + ". DarcSpawner not started.", LogLevel.ERROR);
			return;
		}			
		
		if ( (m_Config_Animal.spawnSetList.IsEmpty()) || (m_Config_Animal.spawnSets.IsEmpty()) )
		{
			SDRC_Log.Add("[SDRC_Spawner:LoadAddon] No spawnSets defined. Addon '" + DC_CONFIG_ADDON_NAME_ANIMALS + "' failed to start.", LogLevel.ERROR);
			return;			
		}

//		CopyData(mainConf, SDRC_SpawnerConfig.Cast(m_Config_Animal));
		
		spawnerConfigs.Insert(m_Config_Animal);
		
		super.LoadAddon(spawnerConfigs);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Copy data from addon the main spawner configuration
	*/	
	void CopyData(out SDRC_SpawnerConfig mainConf, SDRC_SpawnerConfig sourceConf)
	{
		mainConf.version = -1;
		mainConf.author = sourceConf.author + ", ";
		mainConf.comment = sourceConf.comment + ". ";
		mainConf.spawnWorldSizeMultiplier = sourceConf.spawnWorldSizeMultiplier;
		//Take the largest count as the max
		if (sourceConf.spawnCount > mainConf.spawnCount)
		{
			mainConf.spawnCount = sourceConf.spawnCount;
		}
		
		int biggestIndex = SDRC_Misc.FindMaxArrayValue(mainConf.spawnSetList);

		foreach (int setListIndex : m_Config_Animal.spawnSetList)
		{
			//Copy the spawnSetList
			mainConf.spawnSetList.Insert(setListIndex + biggestIndex);
		}		
				
		foreach (int i, SDRC_SpawnSet spawnSet : m_Config_Animal.spawnSets)
		{
			//Copy the spawnSets
			ref SDRC_SpawnSet spawnSetTmp = new SDRC_SpawnSet();
			spawnSetTmp = spawnSet;
			mainConf.spawnSets.Insert(spawnSetTmp);
		}		
	}
}