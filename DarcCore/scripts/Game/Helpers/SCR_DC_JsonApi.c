//Helpers/SCR_DC_JsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J
// NOTE: Format .json in Notepad++ - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
class SCR_DC_JsonApi : JsonApiStruct
{
	private string m_fileName = "";
	
	//------------------------------------------------------------------------------------------------
	SCR_JsonLoadContext LoadConfig(string fileName)
	{	
		SetFileName(fileName);		
		
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		
		if (SCR_DC_Core.OVERWRITE_JSON)
		{
			SCR_DC_Log.Add("[SCR_DC_JsonConfig] Not release build - overwriting json config on disk.", LogLevel.WARNING);
			return null;
		}
		
		bool success = loadContext.LoadFromFile(m_fileName);
		
		if(!success)
		{
			SCR_DC_Log.Add("[SCR_DC_JsonConfig] Config file load failed or not found (" + m_fileName + "). Creating a default config.", LogLevel.ERROR);
			return null;
		}

		SCR_DC_Log.Add("[SCR_DC_JsonConfig] Loading configuration from file: " + m_fileName, LogLevel.NORMAL);
		
		return loadContext;
	}	
	
	//------------------------------------------------------------------------------------------------
	SCR_JsonSaveContext SaveConfigOpen(string fileName)
	{
		SetFileName(fileName);
		//m_fileName = "$profile:/" + fileName;
		
		SCR_JsonSaveContext saveContext = new SCR_JsonSaveContext();
		
		return saveContext;
	}
	
	//------------------------------------------------------------------------------------------------
	void SaveConfigClose(SCR_JsonSaveContext saveContext)
	{
		string dataString = saveContext.ExportToString();
		ExpandFromRAW(dataString);
		
		SCR_DC_Log.Add("[SCR_DC_JsonConfig] This gives some warnings on 'JsonApi Array name='something' found in JSON ... ' . Please ignore.", LogLevel.WARNING);			
		
		if(!saveContext.SaveToFile(m_fileName))
		{
			SCR_DC_Log.Add("[SCR_DC_JsonConfig] Config save failed to: " + m_fileName, LogLevel.ERROR);
		}
		else
		{
			SCR_DC_Log.Add("[SCR_DC_JsonConfig] Config saved to: " + m_fileName, LogLevel.DEBUG);			
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnError(int errorCode)
	{
		// errorCode is EJsonApiError
		// Event called when pending store operation is finished - callback when error happened
		SCR_DC_Log.Add("[SCR_DC_JsonConfig] Error loading config. " + SCR_Enum.GetEnumName(EJsonApiError, errorCode), LogLevel.ERROR);
	}		
	
	//------------------------------------------------------------------------------------------------
	void SetFileName(string fileName)
	{			
		if (SCR_DC_Core.RELEASE)
		{
			m_fileName = "$profile:/" + fileName;
		}
		else
		{
			m_fileName = fileName;
		}
	}
	
}	