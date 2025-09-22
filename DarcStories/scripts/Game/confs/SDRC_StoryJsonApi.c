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
		SetFileName(fileName);
	}
	
	//------------------------------------------------------------------------------------------------
	void Load(bool respectOverWrite = true)
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(respectOverWrite);
		if (!loadContext)
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
		SCR_JsonSaveContext saveContext = SaveConfigOpen();
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
	
	//------------------------------------------------------------------------------------------------
	void LoadStory()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig();
		if (!loadContext)
		{
			return;
		}
		
		loadContext.ReadValue("", conf);
	}	
	
}