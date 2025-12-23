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
	static ref SCR_JsonLoadContext loadContext;
	static ref SDRC_CtxConfig conf = new SDRC_CtxConfig();
	
	//------------------------------------------------------------------------------------------------
	/*!
	Add you init code here.
	*/
	static bool Init()
	{		
		SDRC_Log.Add("[SDRC_LoadCtx] Initializing..", LogLevel.NORMAL);
		
		m_DC_CompatJsonApi = new SDRC_LoadCtxJsonApi(DC_COMPATCONFIG_FILE);		
		m_DC_CompatJsonApi.Load(conf);
		
		SDRC_Log.Add("[SDRC_LoadCtx] Author: " + conf.author, LogLevel.NORMAL);

		return true;
	}
}

class SDRC_Config : Managed
{
	int vers = -1;
	
	void SetDefaults()
	{	
		vers = 1;
	}
	
	bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_CtxConfig : SDRC_Config
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
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{	
		names = {"Janne", "Jorma"};
		pos = {"1.0 1.0 1.0", "1.1 1.1 1.1"};
	}
	
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_CtxConfig data = SDRC_CtxConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_LoadCtxJsonApi : SDRC_JsonApi2
{
	//------------------------------------------------------------------------------------------------
	void SDRC_LoadCtxJsonApi(string fileName)
	{
		SetFileName(fileName);
	}
	
	//------------------------------------------------------------------------------------------------
	bool Load(Class T, bool createMissingFiles = true)
	{	
		SDRC_Config C = SDRC_Config.Cast(T);
		
		SCR_JsonLoadContext loadContext = LoadConfig(createMissingFiles);		
		if (!loadContext)
		{
			if (!createMissingFiles)
			{
				return false;
			}
			C.SetDefaults();
			
			Save(C);
//			return true;
		}
		
		loadContext = LoadConfig(false);
		
		if (!loadContext)
		{
            Print("ERROR!", LogLevel.ERROR);
			return false;
		}
		
		if (C.vers != DC_FILE_VERSION)
		{
            Print("ERROR!", LogLevel.ERROR);
			return false;
		}
		
		Print(T);
		return true;
	}	
	
	//------------------------------------------------------------------------------------------------
	void Save(Class T)
	{
		bool useTypeDiscriminator = false;
        ContainerSerializationSaveContext saveContext = new ContainerSerializationSaveContext(false);
		saveContext.EnableTypeDiscriminator(useTypeDiscriminator);

        PrettyJsonSaveContainer container = new PrettyJsonSaveContainer;
        saveContext.SetContainer(container);
		
		SDRC_Config C = SDRC_Config.Cast(T);
        if (!C.DoSave(saveContext, T)) 
		{
            Print("ERROR!", LogLevel.ERROR);
            return;
		}
		
/*        if (!saveContext.WriteValue("", T)) 
		{
            Print("ERROR!", LogLevel.ERROR);
            return;
        }*/
        
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
	
	
}