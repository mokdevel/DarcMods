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

const string SDRC_MODNAME = "DarcSpawner";

//------------------------------------------------------------------------------------------------
class SDRC_Spawner
{
	ref SDRC_SpawnerJsonApi m_DC_SpawnerConfig = new SDRC_SpawnerJsonApi();
	ref SDRC_SpawnerConfig m_Config;
	private ref array<MapItem> m_Locations = {};	
	protected ref array<IEntity> m_EntityList = {};		//Entities (e.g. cars, tents, ..) spawned
	private int m_spawnSetID;
	private int m_spawnIdx = 0;
	private int m_spawnCount;
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Spawner()
	{
		SDRC_Log.Add("[SDRC_Spawner] Starting SDRC_Spawner", LogLevel.NORMAL);
		
		//Load configuration from file
		m_DC_SpawnerConfig.Load();
		m_Config = m_DC_SpawnerConfig.conf;

		//Select which spawnSet to use. -1 for a random one.
		m_spawnSetID = m_Config.spawnSetID;
		
		if (m_spawnSetID == -1)
		{
			m_spawnSetID = Math.RandomInt(0, m_Config.spawnSets.Count() - 1);
		}		
		SDRC_Log.Add("[SDRC_Spawner] Using spawnSet: " + m_spawnSetID, LogLevel.NORMAL);

		//Max amount of spawnNames to spawn
		m_spawnCount = m_Config.spawnSets[m_spawnSetID].spawnCount;
		
		if (m_spawnCount == 0)
		{			
			m_spawnCount = (SDRC_Misc.GetWorldSize() * m_Config.spawnWorldSizeMultiplier) / 1000;
			SDRC_Log.Add("[SDRC_Spawner] m_spawnCount = Worldsize: " + SDRC_Misc.GetWorldSize() + " * " + m_Config.spawnWorldSizeMultiplier, LogLevel.DEBUG);			
		}		
		SDRC_Log.Add("[SDRC_Spawner] Maximum spawnCount: " + m_spawnCount, LogLevel.DEBUG);
						
		//Find a locations
		SDRC_Locations.GetLocations(m_Locations, m_Config.spawnSets[m_spawnSetID].locationTypes);
		
		//Check if RoadNetworkManager is available. 		
		if (!SDRC_RoadHelper.GetRoadNetworkManager())
		{
			m_Config.spawnOnRoad = false;
			SDRC_Log.Add("[SDRC_Spawner] RoadNetworkManager not defined. Vehicles will not be spawned on roads.", LogLevel.ERROR);
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
			if (Math.RandomFloat(0, 1) < m_Config.spawnSets[m_spawnSetID].spawnChance)
			{
				Spawn();
			}
			m_spawnIdx++;
			
			GetGame().GetCallqueue().CallLater(Run, 3000, false);
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
	private void Spawn()
	{					
		IEntity entity;

		//Spawn entities one by one.
		MapItem location = m_Locations.GetRandomElement();
		vector pos = location.GetPos();
		
		SDRC_Log.Add("[SDRC_Spawner:Spawn] Chosen location: " + SCR_StringHelper.Translate(location.Entity().GetName()) + " (" + pos + ")", LogLevel.DEBUG);
		
		if (m_Config.spawnOnRoad)
		{
			SDRC_RoadPos roadPos = new SDRC_RoadPos();
			vector tmpPos = SDRC_RoadHelper.FindClosestRoadposToPos(roadPos, pos);
			SDRC_Log.Add("[SDRC_Spawner:Spawn] tmpPos: " + tmpPos, LogLevel.DEBUG);			
			if (tmpPos != "0 0 0")
			{
				pos = tmpPos;
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_Spawner:Spawn] Randomizing position", LogLevel.DEBUG);			
			pos = SDRC_Misc.RandomizePos(pos, m_Config.spawnRndRadius);
		}
		
		if (!SDRC_Misc.IsPosInWater(pos))
		{		
			string entityToSpawn = m_Config.spawnSets[m_spawnSetID].spawnNames.GetRandomElement();		
			SDRC_Log.Add("[SDRC_Spawner:Spawn] Spawning " + entityToSpawn + " to " + SCR_StringHelper.Translate(location.Entity().GetName()), LogLevel.NORMAL);				
			
			float rotation = Math.RandomFloat(0, 360);
			if (m_Config.spawnOnRoad)
			{
				entity = SDRC_SpawnHelper.SpawnItem(pos, entityToSpawn, rotation, -1);
			}
			else
			{
				entity = SDRC_SpawnHelper.SpawnItem(pos, entityToSpawn, rotation);
			}
			
			if (entity != NULL)
			{ 
				m_EntityList.Insert(entity);
				SDRC_DebugHelper.AddDebugPos(entity, Color.VIOLET);
				
				SDRC_LootHelper.SpawnItemsToStorage(entity, m_Config.spawnSets[m_spawnSetID].itemNames, m_Config.spawnSets[m_spawnSetID].itemChance);
				//Disable arsenal
				SDRC_SpawnHelper.DisableVehicleArsenal(entity, entityToSpawn, m_Config.disableArsenal);
					
				if (m_Config.showMarker)
				{
					SDRC_MapMarkerHelper.CreateMapMarker(entity.GetOrigin(), m_Config.markerIdx, "", "", markerTypeString: m_Config.markerType);
				}
			}
			else
			{
				SDRC_Log.Add("[SDRC_Spawner:Spawn] Could not spawn: " + entityToSpawn, LogLevel.ERROR);	
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_Spawner:Spawn] Position in water: " + pos, LogLevel.ERROR);	
		}
	}
}