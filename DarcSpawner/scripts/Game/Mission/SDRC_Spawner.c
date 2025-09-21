//------------------------------------------------------------------------------------------------
/*!
This is the main Spawner file. 

Add this to your StartGameTrigger or use SDRC_GameCoreBase.c

	//------------------------------------------------------------------------------------------------
	// user script
	private bool m_GameHasStarted = false;
	private ref SDRC_Spawner DCSpawner;

	override void EOnActivate(IEntity owner)
	{
		if (!m_GameHasStarted)
		{
			m_GameHasStarted = true;		
			DCSpawner = new SDRC_Spawner(true);
			DCSpawner.Run();
		}
	}
	//------------------------------------------------------------------------------------------------
*/

const string DC_CONFIG_FILE_SPAWNER = "dc_spawnerConfig.json";

//const string SDRC_MODNAME = "DarcSpawner";

//------------------------------------------------------------------------------------------------
class SDRC_Spawner
{
	ref SDRC_SpawnerJsonApi m_DC_SpawnerConfig = new SDRC_SpawnerJsonApi(DC_CONFIG_FILE_SPAWNER);
	ref SDRC_SpawnerConfig m_Config;
	protected ref array<IEntity> m_EntityList = {};		//Entities (e.g. cars, tents, ..) spawned
	private int m_spawnSetID;
	private int m_spawnIdx = 0;
	private int m_spawnCount;
	private int m_failCount = 0;						//Counter for failed spawns
	private int m_failLimit = 0;						//Limit for fails before stopping. This is 2* maximum amount.
	private ref array<vector> m_positionsUsed = {};			//When using position spawning, avoid spawning to same positions.
		
	//------------------------------------------------------------------------------------------------
	void SDRC_Spawner()
	{
		SDRC_Log.Add("[SDRC_Spawner] Starting SDRC_Spawner", LogLevel.NORMAL);
		
		//Load configuration from file
		m_DC_SpawnerConfig.Load();
		m_Config = m_DC_SpawnerConfig.conf;

		if (m_Config.spawnSetList.Count() == 0)
		{
			SDRC_Log.Add("[SDRC_Spawner] No spawnSets defined. Stopping.", LogLevel.ERROR);
			return;			
		}

		//Max amount of spawnNames to spawn
		m_spawnCount = m_Config.containerCount;		
		if (m_spawnCount == 0)
		{			
			m_spawnCount = (SDRC_Misc.GetWorldSize() * m_Config.spawnWorldSizeMultiplier) / 1000;
			SDRC_Log.Add("[SDRC_Spawner] m_spawnCount = Worldsize: " + SDRC_Misc.GetWorldSize() + " * " + m_Config.spawnWorldSizeMultiplier, LogLevel.DEBUG);			
		}		
		m_failLimit = m_spawnCount * 2;
		SDRC_Log.Add("[SDRC_Spawner] Maximum spawnCount: " + m_spawnCount, LogLevel.DEBUG);
		
		//Check if RoadNetworkManager is available. 		
		if (!SDRC_RoadHelper.GetRoadNetworkManager())
		{
			m_Config.spawnOnRoad = false;
			SDRC_Log.Add("[SDRC_Spawner] RoadNetworkManager not defined. Vehicles will not be spawned on roads.", LogLevel.WARNING);
		}
	}

	void ~SDRC_Spawner()
	{
		SDRC_Log.Add("[~SDRC_Spawner] Stopping SDRC_Spawner", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Run the spawner. Spawn items with some delay.
	*/	
	void Run()
	{
		SDRC_Log.Add("[SDRC_Spawner:Run] Running", LogLevel.DEBUG);

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
			
			if (m_failCount < m_failLimit)
			{
				GetGame().GetCallqueue().CallLater(Run, 3000, false);
			}
			else
			{
				SDRC_Log.Add("[SDRC_Spawner:Run] Spawned " + m_EntityList.Count() + "/" + m_spawnCount + ". Some spawns failed. Stopping.", LogLevel.NORMAL);
			}
		}				
		else
		{
			SDRC_Log.Add("[SDRC_Spawner:Run] Spawned " + m_EntityList.Count() + "/" + m_spawnCount + ". All done, stopping.", LogLevel.NORMAL);
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
		array<MapItem> locations = {};	
		MapItem location = null;
		vector pos;
		bool snap = true;
		float emptyPosRadius = 50;
		
		int idx = m_Config.spawnSetList.GetRandomElement();
		SDRC_SpawnSet spawnSet = m_Config.spawnSets[idx];

		SDRC_Log.Add("[SDRC_Spawner:Spawn] Using index: " + idx, LogLevel.DEBUG);
				
		string entityToSpawn = spawnSet.containers.GetRandomElement();		
		
		if (entityToSpawn.Contains("Vehicle"))
		{
			isVehicle = true;
		}
		
		//Spawn entities one by one.
		//Find from location types
		if (!spawnSet.locationTypes.IsEmpty())
		{
			SDRC_Locations.GetLocations(locations, spawnSet.locationTypes);
			location = locations.GetRandomElement();
			pos = location.GetPos();
			SDRC_Log.Add("[SDRC_Spawner:Spawn] Chosen location: " + SCR_StringHelper.Translate(location.Entity().GetName()) + " (" + pos + ")", LogLevel.DEBUG);
		
			if (m_Config.spawnOnRoad && isVehicle)
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
				SDRC_Log.Add("[SDRC_Spawner:Spawn] Randomizing position", LogLevel.SPAM);			
				pos = SDRC_Misc.RandomizePos(pos, m_Config.spawnRndRadius);
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
				SDRC_Log.Add("[SDRC_Spawner:Spawn] Spawning " + entityToSpawn + " to " + SCR_StringHelper.Translate(location.Entity().GetName()), LogLevel.NORMAL);
			}
			else
			{
				SDRC_Log.Add("[SDRC_Spawner:Spawn] Spawning " + entityToSpawn + " to (" + pos + ")", LogLevel.NORMAL);
			}
			
			float rotation = Math.RandomFloat(0, 360);
			if (m_Config.spawnOnRoad && isVehicle)
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
				spawnSet.loot.box = entity;
				
				SDRC_LootHelper.SpawnItemsToStorage(entity, spawnSet.loot.items, spawnSet.loot.itemChance);
				//Disable arsenal
				if (isVehicle)
				{
					SDRC_SpawnHelper.DisableVehicleArsenal(entity, entityToSpawn, m_Config.disableArsenal);
				}
					
				if (spawnSet.showMarker)
				{
					SDRC_MapMarkerHelper.CreateMapMarker(entity.GetOrigin(), spawnSet.markerIdx, "", "", markerTypeString: spawnSet.markerType);
				}
				
				SDRC_DebugHelper.AddDebugPos(entity, ARGB(50, 255, 0, 255));
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