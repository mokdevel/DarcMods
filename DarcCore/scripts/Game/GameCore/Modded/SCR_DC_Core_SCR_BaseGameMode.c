//Modded/SCR_DC_Core_BaseGameMode.c

modded class SCR_BaseGameMode 
{
	//private 
	private ref SCR_DC_Core m_SCR_DC_Core;
	private SCR_DC_DebugEntity m_SCR_DC_DebugEntity;
	private SCR_DC_RplHintEntity m_SCR_DC_RplHintEntity;
	
    override void OnGameStart()
    {
        super.OnGameStart();
		
		SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode:OnGameStart]", LogLevel.DEBUG);
		
		if(IsMaster())
		{
			SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode:IsMaster] OnGameStart", LogLevel.SPAM);        
		
			//Initialize the SCR_DC_DebugEntity
			SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode] Creating SCR_DC_DebugEntity", LogLevel.NORMAL);        
			m_SCR_DC_DebugEntity = SCR_DC_DebugEntity.Cast(GetGame().SpawnEntity(SCR_DC_DebugEntity, GetGame().GetWorld(), null));

			//Initialize the SCR_DC_HintEntity
			SCR_DC_Log.Add("[SCR_DC_Hint_BaseGameMode] Creating SCR_DC_HintEntity", LogLevel.NORMAL);
			
/*	        EntitySpawnParams params = EntitySpawnParams();
	        params.TransformMode = ETransformMode.WORLD;			
//	        params.Transform[3] = "900 0 2600";
	        params.Transform[3] = "0 0 0";	*/
			
			Resource resource = Resource.Load("{CF2D9888CE3C5AD7}Prefabs/Helpers/RPLHintHelper.et");
			if (!resource.IsValid())
			{
				SCR_DC_Log.Add("[SCR_DC_Hint_BaseGameMode] Failed to create SCR_DC_RplHintEntity", LogLevel.NORMAL);
				return;
			}
	
			m_SCR_DC_RplHintEntity = SCR_DC_RplHintEntity.Cast(GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld()));
						
			//Initialize core
			m_SCR_DC_Core = new SCR_DC_Core();
		}
		else 
		{
			SCR_DC_Log.Add("[SCR_DC_Core_BaseGameMode:NonMaster] Core not needed for client.", LogLevel.DEBUG);        
		}
		
		SCR_DC_LootHelper.Setup();
    }	
}
	
//------------------------------------------------------------------------------------------------
class SCR_DC_CoreConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	DC_LogLevel logLevel;
	bool debugShowWaypoints = true;
	bool debugShowMarks = true;	
	bool debugShowSpheres = true;	
}

//------------------------------------------------------------------------------------------------
class SCR_DC_CoreJsonApi : SCR_DC_JsonApi
{
	const string DC_CONFIG_FILE = "dc_coreConfig.json";
	ref SCR_DC_CoreConfig conf = new SCR_DC_CoreConfig;

	//------------------------------------------------------------------------------------------------
	void SCR_DC_SpawnerJsonConfig()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_CONFIG_FILE);
		
		if(!loadContext)
		{
			SetDefaults();
			Save();
			return;
		}
		
		loadContext.ReadValue("", conf);
	}	

	//------------------------------------------------------------------------------------------------
	void Save()
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen(DC_CONFIG_FILE);
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}	
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
//		conf.logLevel = DC_LogLevel.DEBUG;	
		conf.logLevel = SCR_DC_Conf.DEFAULT_LOGLEVEL;	
	}
};
