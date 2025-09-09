//Helpers SDRC_StoryConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

	//Release options
	#ifdef SDRC_RELEASE
	#endif
	
	//Development time options
	#ifndef SDRC_RELEASE	
	#endif

//------------------------------------------------------------------------------------------------
class SDRC_StoryJsonApi : SDRC_JsonApi
{
	private string m_sStoryFileName;
		
	ref SDRC_Story conf = new SDRC_Story();

	//------------------------------------------------------------------------------------------------
	void SDRC_StoryJsonApi(string fileName = "")
	{
		m_sStoryFileName = fileName;
	}
	
	//------------------------------------------------------------------------------------------------
	void Load(bool respectOverWrite = true)
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(m_sStoryFileName, respectOverWrite);
		if (!loadContext)
		{
			SetDefaults();
			Save("");
			return;
		}
		
		loadContext.ReadValue("", conf);
	}	

	//------------------------------------------------------------------------------------------------
	void Save(string data)
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen(m_sStoryFileName);
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		#ifdef SDRC_RELEASE
		#endif	

		#ifndef SDRC_RELEASE				
		#endif
	};
	//------------------------------------------------------------------------------------------------
	// New methods
	//------------------------------------------------------------------------------------------------
	void SetStoryFileName(string fileName)
	{
		m_sStoryFileName = fileName;
	}
	
	//------------------------------------------------------------------------------------------------
	void LoadStory()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(m_sStoryFileName);
		if (!loadContext)
		{
			return;
		}
		
		loadContext.ReadValue("", conf);
	}	
	
}