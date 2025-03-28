//Helpers/SCR_DC_JsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J
// NOTE: Format .json in Notepad++ - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
class SCR_DC_JsonApi : JsonApiStruct
{
	private string m_FileName = "";
	
	//------------------------------------------------------------------------------------------------
	SCR_JsonLoadContext LoadConfig(string fileName)
	{	
		SetFileName(fileName);		
		
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		
		if (SCR_DC_Conf.OVERWRITE_JSON)
		{
			SCR_DC_Log.Add("[SCR_DC_JsonApi] Not release build - overwriting json config on disk.", LogLevel.WARNING);
			return null;
		}
		
		bool success = loadContext.LoadFromFile(m_FileName);
		
		if(!success)
		{
			SCR_DC_Log.Add("[SCR_DC_JsonApi] Config file load failed or not found (" + m_FileName + "). Creating a default config.", LogLevel.ERROR);
			return null;
		}

		SCR_DC_Log.Add("[SCR_DC_JsonApi] Loading configuration from file: " + m_FileName, LogLevel.NORMAL);
		
		return loadContext;
	}	
	
	//------------------------------------------------------------------------------------------------
	SCR_JsonSaveContext SaveConfigOpen(string fileName)
	{
		SetFileName(fileName);
		
		SCR_JsonSaveContext saveContext = new SCR_JsonSaveContext();
		
		return saveContext;
	}
	
	//------------------------------------------------------------------------------------------------
	void SaveConfigClose(SCR_JsonSaveContext saveContext)
	{
		string dataString = saveContext.ExportToString();
		ExpandFromRAW(dataString);
		
		SCR_DC_Log.Add("[SCR_DC_JsonApi] This gives some warnings on 'JsonApi Array name='something' found in JSON ... ' . Please ignore.", LogLevel.WARNING);			
		
		if(!saveContext.SaveToFile(m_FileName))
		{
			SCR_DC_Log.Add("[SCR_DC_JsonApi] Config save failed to: " + m_FileName, LogLevel.ERROR);
		}
		else
		{
			SCR_DC_Log.Add("[SCR_DC_JsonApi] Config saved to: " + m_FileName, LogLevel.DEBUG);			
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnError(int errorCode)
	{
		// errorCode is EJsonApiError
		// Event called when pending store operation is finished - callback when error happened
		SCR_DC_Log.Add("[SCR_DC_JsonApi] Error loading config. " + SCR_Enum.GetEnumName(EJsonApiError, errorCode), LogLevel.ERROR);
	}		

	//------------------------------------------------------------------------------------------------
	void SetFileName(string fileName)
	{			
		string path = "";
		string directory = SCR_DC_Conf.CONF_DIRECTORY;
		if (SCR_DC_Conf.missionProfile != "")
		{
			directory = directory + "/" + SCR_DC_Conf.missionProfile
		}
		
		path = "$profile:/" + directory + "/";

		if (!FileIO.MakeDirectory(path))
		{
			SCR_DC_Log.Add("[SCR_DC_JsonApi] Could not create path: " + path, LogLevel.ERROR);
		}
				
		m_FileName = path + fileName;
	}
	
}	