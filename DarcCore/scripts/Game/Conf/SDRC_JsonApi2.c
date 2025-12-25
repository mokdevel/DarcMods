//Helpers/SDRC_JsonApi2.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J
// NOTE: Format .json in Notepad++ - press Ctrl+Alt+Shift+M

//#define CTX

const int DC_FILE_VERSION = 1;

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
	bool Load(Class T, bool createMissingFiles = true)
	{	
	#ifndef CTX
		SDRC_MissionConfig C = SDRC_MissionConfig.Cast(T);
	#else
		SDRC_MissionConfig2 C = SDRC_MissionConfig2.Cast(T);
	#endif
		
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
		
		if (C.version != DC_FILE_VERSION)
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
		
	#ifndef CTX		
		SDRC_MissionConfig C = SDRC_MissionConfig.Cast(T);
	#else
		SDRC_MissionConfig2 C = SDRC_MissionConfig2.Cast(T);
	#endif
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
					
	//------------------------------------------------------------------------------------------------
	/*!
	Load the json config	
	\param createMissingFiles Defines if the SDRC_Conf.OVERWRITE_JSON setting is to be respected. If false, overwrite of files will not happen.
	*/
	SCR_JsonLoadContext LoadConfig(bool createMissingFiles = true)
	{	
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		
		if (SDRC_Conf.OVERWRITE_JSON && createMissingFiles)
		{
			SDRC_Log.Add("[SDRC_JsonApi2] Not release build - overwriting json config on disk.", LogLevel.WARNING);
			return null;
		}
		
		bool success = loadContext.LoadFromFile(m_FileName);
		
		if (!success)
		{
			if (createMissingFiles)
			{
				SDRC_Log.Add("[SDRC_JsonApi2] Config file load failed or not found (" + m_FileName + "). Creating a default config.", LogLevel.ERROR);
			}
			else
			{
				SDRC_Log.Add("[SDRC_JsonApi2] Config file load failed or not found (" + m_FileName + ").", LogLevel.ERROR);
			}
			return null;
		}

		SDRC_Log.Add("[SDRC_JsonApi2] Loading configuration from file: " + m_FileName, LogLevel.NORMAL);
		
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
	void CreateMissionFiles()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void LoadMissionFiles()
	{
		//Load mission files
/*		foreach (string missionFile : conf.missionFiles)
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
		}*/
	}		
}	