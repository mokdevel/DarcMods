//Helpers SDRC_LoadCtx

//------------------------------------------------------------------------------------------------
/*!
Compatibility for Freedom Fighters
*/

const int DC_FILE_VERSION = 1;

//------------------------------------------------------------------------------------------------
class SDRC_LoadCtx
{	
	const string DC_COMPATCONFIG_FILE = "dc_ctx.json";
	const int DC_COMPAT_WAIT_FOR_PLAYERS_TIME = 15;
	const int DC_COMPAT_CLEAN_WAIT_TIME = 15;
	static ref SDRC_LoadCtxJsonApi m_DC_CompatJsonApi = null;
	static ref array<string> m__cleanUpList = {};
	
	//------------------------------------------------------------------------------------------------
	/*!
	Add you init code here.
	*/
	static bool Init()
	{
		m_DC_CompatJsonApi = new SDRC_LoadCtxJsonApi(DC_COMPATCONFIG_FILE);		
		m_DC_CompatJsonApi.Load();
		
		SDRC_Log.Add("[SDRC_LoadCtx] Initializing..", LogLevel.NORMAL);

		return true;
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_CtxConfig : Managed
{
	//Default information
	int version = -1;
	string author = "darc";
	//Mission specific
	string comment;
	//Specific
	int hideOutSafeZoneDistance = 300;
	float spawnRateForGreenZones = 0.15; 
	bool setEnemyFactionAutomatically = true;	//Automatically set enemy faction from . (WIP)
	bool rewardPerUser = false;					//Shall reward be set per user or for a group
	int rewardDefault = 500;					//Default reward unless specific reward has been set in a mission. (WIP)
	ref array<string> names = {};
	ref array<string> pos = {};
}

//------------------------------------------------------------------------------------------------
class SDRC_LoadCtxJsonApi : SDRC_JsonApi2
{
	ref SDRC_CtxConfig conf = new SDRC_CtxConfig();
	
	//------------------------------------------------------------------------------------------------
	void SDRC_LoadCtxJsonApi(string fileName)
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
//			return true;
		}
		
		loadContext = LoadConfig(false);
		
		if (!loadContext)
		{
            Print("ERROR!", LogLevel.ERROR);
			return false;
		}
		
		if (conf.version != DC_FILE_VERSION)
		{
            Print("ERROR!", LogLevel.ERROR);
			return false;
		}
		
		Print(conf);
		return true;
	}	
	
	//------------------------------------------------------------------------------------------------
	void Save()
	{
		bool useTypeDiscriminator = false;
        ContainerSerializationSaveContext saveContext = new ContainerSerializationSaveContext(false);
		saveContext.EnableTypeDiscriminator(useTypeDiscriminator);

        PrettyJsonSaveContainer container = new PrettyJsonSaveContainer;
        saveContext.SetContainer(container);
		
        if (!saveContext.WriteValue("", conf)) 
		{
            Print("ERROR!", LogLevel.ERROR);
            return;
        }
        
		container.SaveToFile(GetFileName());
    }				
		
/*		
		
		SCR_JsonSaveContext saveContext = SaveConfigOpen();		
		saveContext.WriteValue("", conf);
		
		string stringValue = "data";
		int integerValue = 123;
		
		PrettyJsonSaveContainer container = new PrettyJsonSaveContainer();		
		
//		BaseJsonSerializationSaveContainer container = new BaseJsonSerializationSaveContainer();
		container.SetMaxDecimalPlaces(3);
		container.
//		container.StartObject("aa");
		container.WriteValue("value", 0.12345); // "0.123"
		container.EndObject();
		
		
		saveContext.WriteValue("", container);
		saveContext.WriteValue("key1", stringValue);
		saveContext.WriteValue("key2", integerValue);

//		// process saved data (export, send, save...), in this case data are exported as json string
//		string dataString = saveContext.ExportToString();
		
		SaveConfigClose(saveContext);
	}*/
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{	
		conf.names = {"Janne", "Jorma"};
		conf.pos = {"1.0 1.0 1.0", "1.1 1.1 1.1"};
	}		
}