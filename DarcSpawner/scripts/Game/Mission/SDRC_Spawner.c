//------------------------------------------------------------------------------------------------
/*!
This is the main Spawner file. 
*/

//------------------------------------------------------------------------------------------------

const string DC_CONFIG_FILE_SPAWNER = "dc_spawnerConfig.json";
const int DC_CONFIG_FILE_SPAWNER_JSONVER = 3;

//------------------------------------------------------------------------------------------------
class SDRC_Spawner
{
	private ref SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_CONFIG_FILE_SPAWNER);
	
	private ref array<ref SDRC_SpawnerConfig> m_SpawnerConfigs = {};
	
//	private ref SDRC_SpawnerConfig m_Config = new SDRC_SpawnerConfig();	
	private ref SDRC_SpawnerConfig m_Config = null;
	
	private int m_iSpawnerConfigIndex = 0;				//Which spawner we're spawning
	private bool m_bHasRoadNetwork = true;				//Check if road network is available	
	
	protected ref array<IEntity> m_EntityList = {};		//Entities (e.g. cars, tents, ..) spawned
	private int m_spawnSetID;

	//Run time parameters that needs to be reset for each new spawner run
	private int m_spawnIdx;
	private int m_spawnCount;
	private int m_failCount;							//Counter for failed spawns
	private int m_failLimit;							//Limit for fails before stopping. This is 2* maximum amount.
	private ref array<vector> m_positionsUsed = {};		//When using position spawning, avoid spawning to same positions.
		
	//------------------------------------------------------------------------------------------------
	void SDRC_Spawner()
	{
		SDRC_Log.Add("[SDRC_Spawner] Starting SDRC_Spawner", LogLevel.NORMAL);
		
		//If addons present, load those
		if (SDRC_Misc.IsAddonLoaded("$DarcSpawner_*:"))
		{
			LoadAddon(m_SpawnerConfigs);
		}
		else
		{
			//Load config
			bool success = m_JsonApi.Load(m_Config, SDRC_SpawnerConfig.Cast(m_Config), DC_CONFIG_FILE_SPAWNER_JSONVER, safeUpdate: true);
			if (!success)
			{
				SDRC_Log.Add("[SDRC_Spawner] Error loading " + DC_CONFIG_FILE_SPAWNER + ". DarcSpawner not started.", LogLevel.ERROR);
				return;
			}
		}
		
		if (m_SpawnerConfigs.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_Spawner] No spawner configs defined. DarcSpawner not started.", LogLevel.ERROR);
		}

		//Check if RoadNetworkManager is available.
		if (!SDRC_RoadHelper.GetRoadNetworkManager())
		{
			m_bHasRoadNetwork = false;
			SDRC_Log.Add("[SDRC_Spawner] RoadNetworkManager not defined. Entities will not be spawned on roads.", LogLevel.WARNING);
		}
				
		StartSpawner();
	}		
		
	//------------------------------------------------------------------------------------------------
	void ~SDRC_Spawner()
	{
		SDRC_Log.Add("[~SDRC_Spawner] Stopping SDRC_Spawner", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	A class to be overriden by the addon.
	*/	
	void LoadAddon(out array<ref SDRC_SpawnerConfig> spawnerConfigs)
	{
	}	
	
	//------------------------------------------------------------------------------------------------
	void StartSpawner()
	{	
		if (m_iSpawnerConfigIndex > m_SpawnerConfigs.Count() - 1)
		{
			SDRC_Log.Add("[SDRC_Spawner:StartSpawner] All done, stopping.", LogLevel.NORMAL);
			return;
		}
		
		//Set the spawner config
		m_Config = m_SpawnerConfigs[m_iSpawnerConfigIndex];
		//Reset values for each spawner.
		m_spawnIdx = 0;
		m_spawnCount = 0;
		m_failCount = 0;						//Counter for failed spawns
		m_failLimit = 0;						//Limit for fails before stopping. This is 2* maximum amount.
		array<vector> m_positionsUsed = {};		//When using position spawning, avoid spawning to same positions.
		
		SDRC_Log.Add("[SDRC_Spawner] --- Starting spawner: " + m_Config.comment, LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Spawner] Spawner author: " + m_Config.author, LogLevel.NORMAL);
		
		if ( (m_Config.spawnSetList.IsEmpty()) || (m_Config.spawnSets.IsEmpty()) )
		{
			SDRC_Log.Add("[SDRC_Spawner] No spawnSets defined.", LogLevel.ERROR);
			return;			
		}

		//Max amount of spawnNames to spawn
		m_spawnCount = m_Config.spawnCount;		
		if (m_spawnCount == 0)
		{			
			m_spawnCount = (SDRC_Misc.GetWorldSize() * m_Config.spawnWorldSizeMultiplier) / 1000;
			SDRC_Log.Add("[SDRC_Spawner] m_spawnCount = Worldsize: " + SDRC_Misc.GetWorldSize() + " * " + m_Config.spawnWorldSizeMultiplier, LogLevel.SPAM);
		}		
		m_failLimit = m_spawnCount * 2;
		
		SDRC_Log.Add("[SDRC_Spawner] Maximum spawnCount: " + m_spawnCount, LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_Spawner] spawnSets defines: " + m_Config.spawnSets.Count(), LogLevel.DEBUG);
		
		Run();
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Run the spawner. Spawn items with some delay.
	*/	
	void Run()
	{
		SDRC_Log.Add("[SDRC_Spawner:Run] Running", LogLevel.SPAM);

		if (m_spawnIdx < m_spawnCount)		
		{	
			if (Spawn())
			{
				m_spawnIdx++;
			}
			else
			{
				m_failCount++;
			}
			
			//TBD: Do we need m_failCount checking ?
			if (m_failCount < m_failLimit)
			{
				GetGame().GetCallqueue().CallLater(Run, 3000, false);
			}
			else
			{
				SDRC_Log.Add("[SDRC_Spawner:Run] Spawned " + m_EntityList.Count() + "/" + m_spawnCount + ". Some spawns failed.", LogLevel.NORMAL);
			}
		}				
		else
		{
			SDRC_Log.Add("[SDRC_Spawner:Run] Spawned " + m_EntityList.Count() + "/" + m_spawnCount, LogLevel.NORMAL);
			
			//Start another spawner
			m_iSpawnerConfigIndex++;
			StartSpawner();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn an entity to random location. Adds items in to it.
	*/	
	private bool Spawn()
	{
		IEntity entity;
		bool isVehicle = false;
		array<SDRC_Location> locations = {};
		SDRC_Location location = null;
		vector pos;
		bool snap = true;
		float emptyPosRadius = 50;
		
		//Select a random spawnSet
		int idx = m_Config.spawnSetList.GetRandomElement();
		SDRC_SpawnSet spawnSet = m_Config.spawnSets[idx];

		SDRC_Log.Add("[SDRC_Spawner:Spawn] Using index: " + idx, LogLevel.DEBUG);

		//Select random entity
		string entityToSpawn = spawnSet.entities.GetRandomElement();		
		
		if (entityToSpawn.Contains("Vehicle"))
		{
			isVehicle = true;
		}
		
		//Spawn entities one by one.
		//Find from location types
		if (!spawnSet.locationTypes.IsEmpty())
		{
			SDRC_Locations.GetLocationsCached(locations, spawnSet.locationTypes);
			
			if (locations.IsEmpty())
			{
				SDRC_Log.Add("[SDRC_Spawner:Spawn] No locations found. Check your conf.", LogLevel.ERROR);
				return false;				
			}
			
			//SDRC_Locations.GetLocations(locations, spawnSet.locationTypes);
			location = locations.GetRandomElement();
			pos = location.pos;
			SDRC_Log.Add("[SDRC_Spawner:Spawn] Chosen location: " + location.displayName + " (" + pos + ")", LogLevel.DEBUG);
		
//			if (m_bHasRoadNetwork && isVehicle)
			if (m_bHasRoadNetwork && spawnSet.spawnOnRoad)
			{
				SDRC_RoadPos roadPos = new SDRC_RoadPos();
				vector tmpPos = SDRC_RoadHelper.FindClosestRoadposToPos(roadPos, pos);
				SDRC_Log.Add("[SDRC_Spawner:Spawn] tmpPos: " + tmpPos, LogLevel.SPAM);			
				if (tmpPos != "0 0 0")
				{
					pos = tmpPos;
				}
			}
			else
			{
				//Randomize position and try to find a spot not in water.
				SDRC_Log.Add("[SDRC_Spawner:Spawn] Randomizing position", LogLevel.SPAM);
				for (int i = 0; i < 5; i++)
				{
					pos = SDRC_Misc.RandomizePos(pos, spawnSet.spawnRndRadius);
					if (!SDRC_Misc.IsPosInWater(pos))
					{
						break;
					}
				}
				
			}
		}
		else //Use positions
		{
			if (!spawnSet.positions.IsEmpty())
			{
				pos = spawnSet.positions.GetRandomElement();
				emptyPosRadius = -1;	//Spawn to exact position
				
				if (pos[1] != 0)	//If height is not 0, snap to exact pos
				{
					snap = false;
				}
				SDRC_Log.Add("[SDRC_Spawner:Spawn] Chosen position: " + " (" + pos + ")", LogLevel.DEBUG);
				
				
				//Avoid spawning to same position.
				//Check if we already spawned here?
				vector rpos;	//Rounded value
				rpos[0] = (int)pos[0];
				rpos[1] = (int)pos[1];
				rpos[2] = (int)pos[2];
				if (m_positionsUsed.Contains(rpos))
				{
					return false;
				}
				
				m_positionsUsed.Insert(rpos);
			}
			else
			{
				SDRC_Log.Add("[SDRC_Spawner:Spawn] No positions, nor locations defined. Check you conf.", LogLevel.ERROR);
				return false;				
			}
		}
		
		if (!SDRC_Misc.IsPosInWater(pos))
		{		
			if (location)
			{
				SDRC_Log.Add("[SDRC_Spawner:Spawn] Spawning " + entityToSpawn + " to " + WidgetManager.Translate(location.displayName) + " (" + pos + ")", LogLevel.NORMAL);
			}
			else
			{
				SDRC_Log.Add("[SDRC_Spawner:Spawn] Spawning " + entityToSpawn + " to (" + pos + ")", LogLevel.NORMAL);
			}
			
			float rotation = SDRC_Misc.RandomFloat(0, 360);
//			if (m_bHasRoadNetwork && isVehicle)
			if (m_bHasRoadNetwork && spawnSet.spawnOnRoad)
			{
				entity = SDRC_SpawnHelper.SpawnItem(pos, entityToSpawn, rotation, -1, snap);
			}
			else
			{
				entity = SDRC_SpawnHelper.SpawnItem(pos, entityToSpawn, rotation, emptyPosRadius, snap);
			}
			
			if (entity != NULL)
			{ 
				m_EntityList.Insert(entity);
				
				//Disable arsenal
				if (isVehicle)
				{
					SDRC_VehicleHelper.DisableVehicleArsenal(entity, entityToSpawn, spawnSet.disableArsenal);
					SDRC_VehicleHelper.EmptyStorage(entity);					
				}

				//Add loot if it is defined. 
				if (spawnSet.loot)
				{
					//Set the spawned entity as the target.
					spawnSet.loot.box = entity;
					SDRC_LootHelper.SpawnItemsToStorage(entity, spawnSet.loot.items, spawnSet.loot.itemChance);
				}
					
				if (spawnSet.showMarker)
				{
					SDRC_MapMarkerHelper.CreateMapMarker(entity.GetOrigin(), spawnSet.markerIdx, "", "", markerTypeString: spawnSet.markerType);
				}
				
				SDRC_DebugHelper.AddDebugPosEntity(entity, ARGB(50, 255, 0, 255));
			}
			else
			{
				SDRC_Log.Add("[SDRC_Spawner:Spawn] Could not spawn: " + entityToSpawn, LogLevel.ERROR);	
				return false;
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_Spawner:Spawn] Position in water: " + pos, LogLevel.ERROR);	
			return false;
		}
		
		return true;
	}
}