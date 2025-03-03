//------------------------------------------------------------------------------------------------
/*!
This is the main Spawner file. 

Add this to your StartGameTrigger or use SCR_DC_GameCoreBase.c

	//------------------------------------------------------------------------------------------------
	// user script
	private bool m_GameHasStarted = false;
	private ref SCR_DC_Spawner DCSpawner;

	override void EOnActivate(IEntity owner)
	{
		if (!m_GameHasStarted)
		{
			m_GameHasStarted = true;		
			DCSpawner = new SCR_DC_Spawner(true);
			DCSpawner.Run();
		}
	}
	//------------------------------------------------------------------------------------------------
*/

//------------------------------------------------------------------------------------------------
class SCR_DC_Spawner
{
	ref SCR_DC_SpawnerJsonApi m_DC_SpawnerConfig = new SCR_DC_SpawnerJsonApi();
	ref SCR_DC_SpawnerConfig m_Config;
	private ref array<MapItem> m_Locations = {};	
	protected ref array<IEntity> m_EntityList = {};		//Entities (e.g. cars, tents, ..) spawned
	private int m_spawnSetID;
	private int m_spawnIdx = 0;
	private int m_spawnCount;
	
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Spawner()
	{
		SCR_DC_Log.Add("[SCR_DC_Spawner] Starting SCR_DC_Spawner", LogLevel.NORMAL);
		
		//Load configuration from file
		m_DC_SpawnerConfig.Load();
		m_Config = m_DC_SpawnerConfig.conf;

		SCR_DC_Log.SetLogLevel(m_Config.logLevel);
		if (!SCR_DC_Core.RELEASE)
		{
			SCR_DC_Log.SetLogLevel(DC_LogLevel.DEBUG);						//Remove in production
		}

		//Select which spawnSet to use. -1 for a random one.
		m_spawnSetID = m_Config.spawnSetID;
		
		if (m_spawnSetID == -1)
		{
			m_spawnSetID = Math.RandomInt(0, m_Config.spawnSets.Count() - 1);
		}		
		SCR_DC_Log.Add("[SCR_DC_Spawner] Using spawnSet: " + m_spawnSetID, LogLevel.DEBUG);

		//Max amount of spawnNames to spawn
		m_spawnCount = m_Config.spawnSets[m_spawnSetID].spawnCount;
		
		if (m_spawnCount == 0)
		{			
			m_spawnCount = (SCR_DC_Misc.GetWorldSize() * m_Config.spawnWorldSizeMultiplier) / 1000;
			SCR_DC_Log.Add("[SCR_DC_Spawner] m_spawnCount = Worldsize: " + SCR_DC_Misc.GetWorldSize() + " * " + m_Config.spawnWorldSizeMultiplier, LogLevel.DEBUG);			
		}		
		SCR_DC_Log.Add("[SCR_DC_Spawner] Maximum spawnCount: " + m_spawnCount, LogLevel.DEBUG);
						
		//Find a locations
		SCR_DC_Locations.GetLocations(m_Locations, m_Config.spawnSets[m_spawnSetID].locationTypes);
		
		foreach(MapItem loc : m_Locations)
		{
			if (loc) 
			{
				SCR_DC_DebugHelper.AddDebugPos(loc);
			}
		}		
	}

	void ~SCR_DC_Spawner()
	{
		SCR_DC_Log.Add("[SCR_DC_Spawner] Stopping SCR_DC_Spawner", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Run the spawner. Spawn items with some delay.
	*/	
	void Run()
	{
		SCR_DC_Log.Add("[SCR_DC_Spawner:Run] Running", LogLevel.DEBUG);

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
			SCR_DC_Log.Add("[SCR_DC_Spawner:Run] Spawned " + m_EntityList.Count() + "/" + m_spawnCount + ". All done, stopping.", LogLevel.NORMAL);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn an entity to random location. Adds items in to it.
	*/	
	private void Spawn()
	{					
		IEntity entity;

		//Spawn entities one by one. Sets missions active once ready.		
		MapItem location = m_Locations.GetRandomElement();		
//		string entityToSpawn = m_Config.spawnSets[m_spawnSetID].spawnNames[m_spawnIdx];
		string entityToSpawn = m_Config.spawnSets[m_spawnSetID].spawnNames.GetRandomElement();
		
		SCR_DC_Log.Add("[SCR_DC_Spawner:Spawn] Spawning " + entityToSpawn + " to " + SCR_StringHelper.Translate(location.Entity().GetName()), LogLevel.NORMAL);				
		
		float rotation = Math.RandomFloat(0, 360);
				
		entity = SCR_DC_SpawnHelper.SpawnItem(SCR_DC_Misc.RandomizePos(location.Entity().GetOrigin(), m_Config.spawnRndRadius), entityToSpawn, rotation);
			
		if (entity != NULL)
		{ 
			m_EntityList.Insert(entity);
			SCR_DC_DebugHelper.AddDebugPos(entity, Color.VIOLET);
			
			SCR_DC_LootHelper.SpawnItemsToStorage(entity, m_Config.spawnSets[m_spawnSetID].itemNames, m_Config.spawnSets[m_spawnSetID].itemChance);			
			SCR_DC_MapMarkerHelper.CreateMapMarker(entity.GetOrigin(), DC_EMissionIcon.REDCROSS_SMALL, "", "");
		}
		else
		{
			SCR_DC_Log.Add("[SCR_DC_Spawner:Spawn] Could not spawn: " + entityToSpawn, LogLevel.ERROR);	
		}
	}
	

}