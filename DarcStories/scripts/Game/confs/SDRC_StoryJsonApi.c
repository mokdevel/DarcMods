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
//Stages in the state machine
enum DC_EStoryState
{
	NONE,		//Unknown state. Nothing should be run at this state.
	INIT,		//The mission is being init. This automatically set when object is created.
	ACTIVE,		//Normal state when mission is running.	
	END,		//Mission is ending. Things are cleaned, despawned etc.
	EXIT,		//State to inform the MissionFrame that the mission should be destroyed.
	FAILED		//Mission startup has failed, delete mission
};

//------------------------------------------------------------------------------------------------
class SDRC_Chapter : Managed
{
	int id;
	DC_EMissionType missionType;		//Type of the mission
	int index;							//Sub mission	
	string comment;
	string title;
	string intro;
	string wintro;
	string losetro;
	int activeTime;
	ref array<int> nextChapter = {};			//Where to go after a win, lose		
	ref SDRC_MissionConfigGeneral missionConf = new SDRC_MissionConfigGeneral();		
	
	void Set(int id_, DC_EMissionType missionType_, string comment_, string title_, string intro_, string wintro_, string losetro_, int activeTime_, array<int> nextChapter_)
	{
		id = id_;
		missionType = missionType_;
		comment = comment_;
		title = title_;
		intro = intro_;
		wintro = wintro_;
		losetro = losetro_;
		activeTime = activeTime;
		nextChapter = nextChapter_;
	}		
}

//------------------------------------------------------------------------------------------------
class SDRC_Story : Managed
{
	int id;
	string comment;
	string title;
	string intro;
	string wintro;
	string losetro;
	//Set outside of Set()
	int index = 0;
	DC_EStoryState state = DC_EStoryState.INIT;
	ref array<SDRC_Chapter> chapters = {};
	
	void Set(int id_, string comment_, string title_, string intro_, string wintro_, string losetro_, )
	{
		id = id_;
		comment = comment_;
		title = title_;
		intro = intro_;
		wintro = wintro_;
		losetro = losetro_;
		//NOTE: index handled outside
		//NOTE: state handled outside
		//NOTE: chapters are added separately
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_StoryJsonApi : SDRC_JsonApi
{
//	const string DC_MISSIONCONFIG_FILE = "story/dc_storyConfig_00.json";
	const string DC_MISSIONCONFIG_FILE = "dc_storyConfig_00.json";
		
	ref SDRC_Story conf = new SDRC_Story();

	//------------------------------------------------------------------------------------------------
	void SDRC_StoryJsonApi()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_MISSIONCONFIG_FILE);
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
		SCR_JsonSaveContext saveContext = SaveConfigOpen(DC_MISSIONCONFIG_FILE);
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
		
		conf.Set(
			0, 
			"id 0:",
			"Story Title",
			"Story intro",
			"Story wintro",
			"Story losetro",
		);
		
		conf.chapters.Insert(Chapter00());
		conf.chapters.Insert(Chapter01());
	};
	
	SDRC_Chapter Chapter00()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			0, DC_EMissionType.CRASHSITE,
			"id 0:",
			"Chapter Title",
			"Chapter intro",
			"Chapter wintro",
			"Chapter losetro",
			SDRC_STORIES_CHAPTER_TIME_DEFAULT,
			{1,-1},
		);
	
		return chapter;
	};	
	
	SDRC_Chapter Chapter01()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			1, DC_EMissionType.CRASHSITE,
			"id 1:",
			"Chapter Title",
			"Chapter intro",
			"Chapter wintro",
			"Chapter losetro",
			SDRC_STORIES_CHAPTER_TIME_DEFAULT,
			{1,-1},
		);
	
		return chapter;
	};	
}