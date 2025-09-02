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

enum DC_ENextChapter
{
	WIN = -2,
	LOSE = -1,
}

//------------------------------------------------------------------------------------------------
class SDRC_Story : Managed
{
	int id;
	string comment;
	ref array<string> dependencies = {};	//List of mods needed for the story
	int index = 0;
	//Set outside of Set()
	DC_EStoryState state = DC_EStoryState.INIT;
	ref array<ref SDRC_Chapter> chapters = {};
	
	void Set(int id_, string comment_, array<string> dependencies_, int index_)
	{
		id = id_;
		comment = comment_;
		dependencies = dependencies_;
		index = index_;
		//NOTE: state handled outside
		//NOTE: chapters are added separately
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_Chapter : Managed
{
	int id;
	DC_EMissionType missionType;		//Type of the mission
	int subIdx;							//Sub mission index
	int activeTime;
	ref array<int> nextChapter = {};			//Where to go after a win, lose		
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();		
		
	void Set(int id_, DC_EMissionType missionType_, int subIdx_, int activeTime_, array<int> nextChapter_)
	{
		id = id_;
		missionType = missionType_;
		subIdx = subIdx_;
		activeTime = activeTime;
		nextChapter = nextChapter_;
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
			{},
			0
		);
		
		conf.chapters.Insert(Chapter00());
		conf.chapters.Insert(Chapter01());
		conf.chapters.Insert(Chapter02());
	};

	SDRC_Chapter Chapter00()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
//			1, DC_EMissionType.SQUATTERS, 0, 
			1, DC_EMissionType.OCCUPATION, 3, 
			SDRC_STORIES_CHAPTER_TIME_DEFAULT,
			{1, 1},
		);
		
		chapter.general.SetDefaults(comment_: "Chapter 0", title_: "Story begins");
	
		return chapter;
	};	
		
	SDRC_Chapter Chapter01()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			1, DC_EMissionType.OCCUPATION, 3, 
			SDRC_STORIES_CHAPTER_TIME_DEFAULT,
			{1, DC_ENextChapter.LOSE},
		);
		
		chapter.general.SetDefaults(comment_: "Chapter 1", title_: "Chapter 1");
	
		return chapter;
	};	
	
	SDRC_Chapter Chapter02()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			2, DC_EMissionType.ROADBLOCK, 1, 
			SDRC_STORIES_CHAPTER_TIME_DEFAULT,
			{DC_ENextChapter.WIN, DC_ENextChapter.LOSE},
		);
	
		chapter.general.SetDefaults(comment_: "Chapter 1");
		
		return chapter;
	};	
}