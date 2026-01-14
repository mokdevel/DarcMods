//Helpers/SDRC_JsonApi2.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J
// NOTE: Format .json in Notepad++ - press Ctrl+Alt+Shift+M

//#define CTX

const int DC_FILE_VERSION = 1;

//------------------------------------------------------------------------------------------------
class SDRC_Config : Managed
{
	int version = -1;
	
	bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
	}
		
	void SetDefaults()
	{	
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_JsonApi2 : JsonApiStruct
{
	private string m_FileName = "";

	//------------------------------------------------------------------------------------------------
	void SDRC_JsonApi2(string fileName)
	{
		SetFileName(fileName);
	}
	
	//------------------------------------------------------------------------------------------------
	bool Load(Managed T, SDRC_Config C, int version, bool createMissingFiles = true)
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(createMissingFiles);		
		if (!loadContext)
		{
			if (!createMissingFiles)
			{
				return false;
			}
			C.version = version;
			C.SetDefaults();
			
			Save(C, SDRC_Config.Cast(C));
			loadContext = LoadConfig(false);
//			return true;
		}
		
		if (!loadContext)
		{
			SDRC_Log.Add("[SDRC_JsonApi2:Load] loadContext is null - " + T, LogLevel.ERROR);
			return false;
		}

		SDRC_Log.Add("[SDRC_JsonApi2:Load] Loading configuration from file: " + GetFileName(), LogLevel.NORMAL);
		
		int versionFromFile;
		loadContext.ReadValue("version", versionFromFile);
		
		if (versionFromFile != version)
		{
			SDRC_Log.Add("[SDRC_JsonApi2:Load] ------------------", LogLevel.ERROR);
			SDRC_Log.Add("[SDRC_JsonApi2:Load] Wrong version number: " + versionFromFile + " (expected: " + version + ") : " + GetFileName(), LogLevel.ERROR);
			SDRC_Log.Add("[SDRC_JsonApi2:Load] Please delete the file and restart to receive an updated one: " + GetFileName(), LogLevel.ERROR);
			SDRC_Log.Add("[SDRC_JsonApi2:Load] ------------------", LogLevel.ERROR);
			return false;
		}		
		
		if(!loadContext.ReadValue("", T))
		{
			SDRC_Log.Add("[SDRC_JsonApi2:Load] Error loading file: " + GetFileName(), LogLevel.ERROR);
			return false;
		}

/*		SDRC_Config M = SDRC_Config.Cast(T);
				
		if (M.version == -1)
		{
			SDRC_Log.Add("[SDRC_JsonApi2:Load] Error loading file: " + GetFileName(), LogLevel.ERROR);
			return false;
		}*/
		
//		Print(T);
		return true;
	}	
	
	//------------------------------------------------------------------------------------------------
	void Save(Managed T, SDRC_Config C)
	{
		bool useTypeDiscriminator = false;
        ContainerSerializationSaveContext saveContext = new ContainerSerializationSaveContext(false);
		saveContext.EnableTypeDiscriminator(useTypeDiscriminator);

        PrettyJsonSaveContainer container = new PrettyJsonSaveContainer;
		container.SetMaxDecimalPlaces(3);
        saveContext.SetContainer(container);

        if (!C.DoSave(saveContext, T)) 
		{
			SDRC_Log.Add("[SDRC_JsonApi2:Save] Save failed - " + T, LogLevel.ERROR);
            return;
		}
        
		if (!container.SaveToFile(GetFileName()))
		{
			SDRC_Log.Add("[SDRC_JsonApi2:Save] Container SaveToFile failed - " + T, LogLevel.ERROR);
		}
    }	
					
	//------------------------------------------------------------------------------------------------
	/*!
	Load the json config	
	\param createMissingFiles Defines if the SDRC_Conf.OVERWRITE_JSON setting is to be respected. If false, overwrite of files will not happen.
	*/
	SCR_JsonLoadContext LoadConfig(bool createMissingFiles = true)
	{	
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext(false);
		
		if (SDRC_Conf.OVERWRITE_JSON && createMissingFiles)
		{
			SDRC_Log.Add("[SDRC_JsonApi2:LoadConfig] Not release build - overwriting json config on disk.", LogLevel.WARNING);
			return null;
		}
		
		bool success = loadContext.LoadFromFile(m_FileName);
		
		if (!success)
		{
			if (createMissingFiles)
			{
				SDRC_Log.Add("[SDRC_JsonApi2:LoadConfig] Config file load failed or not found (" + m_FileName + "). Creating a default config.", LogLevel.ERROR);
			}
			else
			{
				SDRC_Log.Add("[SDRC_JsonApi2:LoadConfig] Config file load failed or not found (" + m_FileName + ").", LogLevel.ERROR);
			}
			return null;
		}

		if (loadContext)
		{
			SDRC_Log.Add("[SDRC_JsonApi2:LoadConfig] Loading configuration from file: " + m_FileName, LogLevel.SPAM);
		}
		
		return loadContext;
	}	
	
	//------------------------------------------------------------------------------------------------
	SCR_JsonSaveContext SaveConfigOpen()
	{
		SCR_JsonSaveContext saveContext = new SCR_JsonSaveContext();
		
		return saveContext;
	}
	
	//------------------------------------------------------------------------------------------------
	void SaveConfigClose(SCR_JsonSaveContext saveContext)
	{
//		string dataString = saveContext.ExportToString();
//		ExpandFromRAW(dataString);
		
		SDRC_Log.Add("[SDRC_JsonApi2] This may give some warnings on 'JsonApi Array name='something' found in JSON ... ' . Please ignore.", LogLevel.WARNING);
		
		if (!saveContext.SaveToFile(m_FileName))
		{
			SDRC_Log.Add("[SDRC_JsonApi2] Config save failed to: " + m_FileName, LogLevel.ERROR);
		}
		else
		{
			SDRC_Log.Add("[SDRC_JsonApi2] Config saved to: " + m_FileName, LogLevel.DEBUG);			
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnError(int errorCode)
	{
		// errorCode is EJsonApiError
		// Event called when pending store operation is finished - callback when error happened
		SDRC_Log.Add("[SDRC_JsonApi2] Error loading config. " + SCR_Enum.GetEnumName(EJsonApiError, errorCode), LogLevel.ERROR);
	}		

	//------------------------------------------------------------------------------------------------
	void SetFileName(string fileName)
	{			
		string path = "";
		string directory = SDRC_Conf.CONF_DIRECTORY;
		if ( (SDRC_Conf.subDir != "") && (fileName != SDRC_Conf.CORE_CONFIG_FILE) )	//This should never be empty
		{
			directory = directory + "/" + SDRC_Conf.subDir
		}
		
		path = "$profile:/" + directory + "/";

		if (!FileIO.MakeDirectory(path))
		{
			SDRC_Log.Add("[SDRC_JsonApi2] Could not create path: " + path, LogLevel.ERROR);
		}
				
		m_FileName = path + fileName;
	}	
	
	//------------------------------------------------------------------------------------------------
	string GetFileName()
	{
		return m_FileName;
	}	
	
	//------------------------------------------------------------------------------------------------
/*	void LoadMissionFiles()
	{
		//Load mission files
		foreach (string missionFile : conf.missionFiles)
		{
			SDRC_ChopperJsonApi jsonApi = new SDRC_ChopperJsonApi(missionFile);		
			if (jsonApi.Load(false))
			{
				foreach (SDRC_Chopper subMission : jsonApi.conf.subMissions)
				{
					conf.subMissions.Insert(subMission);
				}
				foreach (int idx : jsonApi.conf.missionList)
				{
					conf.missionList.Insert(idx);
				}
			}
		}
	}	*/	
}	