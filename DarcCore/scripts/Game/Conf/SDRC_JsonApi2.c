//Helpers/SDRC_JsonApi2.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J
// NOTE: Format .json in Notepad++ - press Ctrl+Alt+Shift+M

//#define CTX

//------------------------------------------------------------------------------------------------
class SDRC_Config : Managed
{
	int jsonVersion = -1;
	
	bool DoSave(SaveContainerContext saveContext, Class T)
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
	/*!
	Load the json config
	\param T <TBD> 
	\param C <TBD> 
	\param jsonVersion Version of the file. Parameter inside json.
	\param createMissingFiles If true, any missing file will be created.
	\param safeUpdate If true, file is automatically updated to the latest one. A backup of is made of the old file.
	\param silent If true, some logging is hidden
	*/
	bool Load(Managed T, SDRC_Config C, int jsonVersion, bool createMissingFiles = true, bool safeUpdate = false, bool silent = false)
	{	
		if ( (!T) || (!C) )
		{
			SDRC_Log.Add("[SDRC_JsonApi2:Load] Classes missing! ", LogLevel.ERROR);
			return false;
		}
		
		JsonLoadContext loadContext = LoadConfig(createMissingFiles);		
		if (!loadContext)
		{
			if (!createMissingFiles)
			{
				return false;
			}
			C.jsonVersion = jsonVersion;
			C.SetDefaults();
			
			//Compatibility thing for consoles
			if (GetGame().IsPlatformGameConsole())
			{
				//Do NOT save the files but use the data in memory. This will NOT save any configs on filesystem.
				return true;			
			}
			
			Save(C, SDRC_Config.Cast(C));
			
			loadContext = LoadConfig(false);
		}
		
		if (!loadContext)
		{
			SDRC_Log.Add("[SDRC_JsonApi2:Load] loadContext is null - " + T, LogLevel.ERROR);
			return false;
		}

		if (!silent)
		{
			SDRC_Log.Add("[SDRC_JsonApi2:Load] Loading from file: " + GetFileName(), LogLevel.NORMAL);
		}
		
		int versionFromFile;
		loadContext.ReadValue("jsonVersion", versionFromFile);
		
		if (versionFromFile != jsonVersion)
		{
			if (!safeUpdate)
			{			
				SDRC_Log.Add("[SDRC_JsonApi2:Load] ------------------", LogLevel.ERROR);
				SDRC_Log.Add("[SDRC_JsonApi2:Load] Wrong jsonVersion number: " + versionFromFile + " (expected: " + jsonVersion + ") : " + GetFileName(), LogLevel.ERROR);
				SDRC_Log.Add("[SDRC_JsonApi2:Load] Please delete the file and restart to receive an updated one: " + GetFileName(), LogLevel.ERROR);
				SDRC_Log.Add("[SDRC_JsonApi2:Load] ------------------", LogLevel.ERROR);
				
				SDRC_Conf.errorList.Insert(GetFileName() + " has been updated. Delete to receive an updated one."); 
				
				return false;
			}
			else
			{			
				SDRC_Log.Add("[SDRC_JsonApi2:Load] ------------------", LogLevel.WARNING);
				SDRC_Log.Add("[SDRC_JsonApi2:Load] Wrong jsonVersion number: " + versionFromFile + " (expected: " + jsonVersion + ") : " + GetFileName(), LogLevel.WARNING);

				//Do a safe update of the file. Making a backup and automatically updating a new file.			
				string backupFile = "";
				
				for (int i = 0; i < 10; i++)
				{
					backupFile = GetFileName() + "_bak_" + i;
					
					if (!FileIO.FileExists(backupFile))
					{					
						SDRC_Log.Add("[SDRC_JsonApi2:Load] Making a backup: " + backupFile, LogLevel.WARNING);
						FileIO.CopyFile(GetFileName(), backupFile);
						break;
					}
					
					if (i == 9)
					{
						SDRC_Log.Add("[SDRC_JsonApi2:Load] Too many backup files under DarcMods. Please delete the files " + GetFileName() + "_bak_N.", LogLevel.ERROR);
						SDRC_Log.Add("[SDRC_JsonApi2:Load] ------------------", LogLevel.WARNING);
						return false;
					}
				}
				
				SDRC_Log.Add("[SDRC_JsonApi2:Load] Creating an updated file: " + GetFileName(), LogLevel.WARNING);
				SDRC_Log.Add("[SDRC_JsonApi2:Load] ------------------", LogLevel.WARNING);
				
				//Save new file
				C.jsonVersion = jsonVersion;
				C.SetDefaults();
				Save(C, SDRC_Config.Cast(C));
				
				loadContext = LoadConfig(false);
			}
		}		
		
		if (!loadContext.ReadValue("", T))
		{
			SDRC_Log.Add("[SDRC_JsonApi2:Load] Error loading file: " + GetFileName(), LogLevel.ERROR);
			return false;
		}

/*		SDRC_Config M = SDRC_Config.Cast(T);
				
		if (M.jsonVersion == -1)
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
        SaveContainerContext saveContext = new SaveContainerContext(false);
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
	JsonLoadContext LoadConfig(bool createMissingFiles = true)
	{	
		JsonLoadContext loadContext = new JsonLoadContext(false);
		
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
	JsonLoadContext SaveConfigOpen()
	{
		JsonLoadContext saveContext = new JsonLoadContext();
		
		return saveContext;
	}
	
	//------------------------------------------------------------------------------------------------
	void SaveConfigClose(JsonSaveContext saveContext)
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
		if ( (SDRC_Conf.subDir != "") && 					//This should never be empty
			 (fileName != SDRC_Conf.CORE_CONFIG_FILE) )		//Core config shall be in root of CONF_DIRECTORY
		{
			directory = directory + "/" + SDRC_Conf.subDir
		}
		
		path = "$profile:/" + directory + "/";

		if (!FileIO.MakeDirectory(path))
		{
			SDRC_Log.Add("[SDRC_JsonApi2] Could not create path: " + path, LogLevel.ERROR);
		}
		
		m_FileName = path + fileName;		
		
		/* TBD: Check that the part below does not introduce something unexpected.
		
		// if a config does not exist in the path above defined using subDir, then look at the default path
		string tempFilepath = path + fileName;
		if (!FileIO.FileExists(tempFilepath))
		{
			directory = SDRC_Conf.CONF_DIRECTORY + "/" + SDRC_Conf.DEFAULT_DIR;
			path = "$profile:/" + directory + "/";
			
			SDRC_Log.Add("[SDRC_Core] Could not find " + tempFilepath + ", looking for " + path + fileName + " instead", LogLevel.WARNING);
			
			tempFilepath = path + fileName;
		}
		
		m_FileName = tempFilepath;
		*/
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