//Helpers SDRC_Compat

//------------------------------------------------------------------------------------------------
/*!
Compatibility for Freedom Fighters
*/

modded class SDRC_Compat
{	
	const string DC_COMPATCONFIG_FILE = "dc_compatFFConfig.json";
	const int DC_COMPAT_WAIT_FOR_PLAYERS_TIME = 15;
	const int DC_COMPAT_CLEAN_WAIT_TIME = 15;
	static ref SDRC_CompatFFJsonApi m_DC_CompatJsonApi = null;
	static ref array<string> m_FF_cleanUpList = {};
	
	//------------------------------------------------------------------------------------------------
	/*!
	Add you init code here.
	*/
	override static bool Init()
	{
		super.Init();
		
		m_DC_CompatJsonApi = new SDRC_CompatFFJsonApi(DC_COMPATCONFIG_FILE);		
		m_DC_CompatJsonApi.Load();
		
		SDRC_Log.Add("[SDRC_CompatFF] Initializing compatibility: Freedom Fighters", LogLevel.NORMAL);

		GetGame().GetCallqueue().CallLater(WaitForPlayers, DC_COMPAT_CLEAN_WAIT_TIME*1000, false);
//		GetGame().GetCallqueue().CallLater(Clear, DC_COMPAT_CLEAN_WAIT_TIME*1000, false);
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static void WaitForPlayers()
	{
		if (SDRC_PlayerHelper.PlayerCount() == 0)
		{
			GetGame().GetCallqueue().CallLater(WaitForPlayers, DC_COMPAT_WAIT_FOR_PLAYERS_TIME*1000, false);
			return;
		}
		
		GetGame().GetCallqueue().CallLater(Clear, DC_COMPAT_CLEAN_WAIT_TIME*1000, false);
	}
		
	static void Clear()
	{		
		SDRC_Log.Add("[SDRC_CompatFF] Clearing old mission stuff from map.", LogLevel.NORMAL);
				
		//Clean choppers from map
		SDRC_List cleanUpList = new SDRC_List();
		cleanUpList.id = "CompatFF_Helicopters";
		cleanUpList.modDir.Insert("Prefabs");
		cleanUpList.include.Insert("_flying");
		cleanUpList.include.Insert("Loot");
		cleanUpList.include.Insert("Hvt");
		SDRC_Resources.GetList(m_FF_cleanUpList, "$DarcMissions:", cleanUpList);
		
		m_FF_cleanUpList.Debug();
		
		float mapSize = SDRC_Misc.GetWorldSize();
		vector pos = "0 0 0";
		pos[0] = mapSize/2;
		pos[2] = mapSize/2;
		GetGame().GetWorld().QueryEntitiesBySphere(pos, mapSize, FindEntitiesCallback, null, EQueryEntitiesFlags.ALL);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Call back filter for finding the burning choppers
	*/		
	static bool FindEntitiesCallback(IEntity entity)
	{
		if (entity)
		{
			if (entity.ClassName() == "Vehicle")
			{
				if (entity.GetPrefabData())
				{
					//string resName = entity.GetPrefabData().GetPrefabName();
					//SDRC_Log.Add("[SDRC_CompatFF:FindEntitiesCallback] Found " + resName, LogLevel.DEBUG);
					
					if (SCR_StringHelper.ContainsAny(entity.GetPrefabData().GetPrefabName(), m_FF_cleanUpList) )
					{
						SDRC_Log.Add("[SDRC_CompatFF:FindEntitiesCallback] Removing " + entity, LogLevel.DEBUG);
						SDRC_SpawnHelper.DespawnItem(entity);
						return true;	//Just continue...
					}
				}
			}
		}
		
		return true;
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_CompatFFConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Mission specific
	string comment;
	//Specific
	int hideOutSafeZoneDistance = 300;
	float spawnRateForGreenZones = 0.05; 
}

//------------------------------------------------------------------------------------------------
class SDRC_CompatFFJsonApi : SDRC_JsonApi
{
	ref SDRC_CompatFFConfig conf = new SDRC_CompatFFConfig();
	
	//------------------------------------------------------------------------------------------------
	void SDRC_CompatFFJsonApi(string fileName)
	{
		SetFileName(fileName);
	}
	
	//------------------------------------------------------------------------------------------------
	bool Load(bool createMissingFiles = true)
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(createMissingFiles);		
		if (!loadContext)
		{
			if (!createMissingFiles)
			{
				return false;
			}
			SetDefaults();
			Save();
			return true;
		}
		
		loadContext.ReadValue("", conf);
		return true;
	}	

	//------------------------------------------------------------------------------------------------
	void Save()
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen();
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{	
	}		
}