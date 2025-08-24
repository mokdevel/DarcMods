//Helpers SDRC_StoriesFrameConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

	//Release options
	#ifdef SDRC_RELEASE
		private const int SDRC_STORIESFRAME_START_DELAY = 1*60;						//Time to wait before spawning the first mission (seconds)
		private const int SDRC_STORIESFRAME_CYCLE_TIME_DEFAULT = 30;
		private const int SDRC_STORIESFRAME_CYCLE_TIME_LIMIT = 20;					//The cycle to run the mission frame. 
	#endif
	
	//Development time options
	#ifndef SDRC_RELEASE	
		private const int SDRC_STORIESFRAME_START_DELAY = 5;						//Time to wait before spawning the first mission (seconds)
		private const int SDRC_STORIESFRAME_CYCLE_TIME_DEFAULT = 10;
		private const int SDRC_STORIESFRAME_CYCLE_TIME_LIMIT = 10;					//The cycle to run the mission frame. 
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
	DC_EMissionType missionType;
	string comment;
	string title;
	string intro;
	string endtro;
	ref array<int> nextChapter = {};			//Where to go after a win, lose	
	
	void Set(int id_, DC_EMissionType missionType_, string comment_, string title_, string intro_, string endtro_, array<int> nextChapter_)
	{
		id = id_;
		missionType = missionType_;
		comment = comment_;
		title = title_;
		intro = intro_;
		endtro = endtro_;
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
	string endtro;
	//Set outside of Set()
	int index = 0;
	DC_EStoryState state = DC_EStoryState.INIT;
	ref array<SDRC_Chapter> chapters = {};
	
	void Set(int id_, string comment_, string title_, string intro_, string endtro_)
	{
		id = id_;
		comment = comment_;
		title = title_;
		intro = intro_;
		endtro = endtro_;
		//NOTE: index handled outside
		//NOTE: state handled outside
		//NOTE: chapters are added separately
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_StoriesFrameConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Mission specific
	string comment;
	int storiesStartDelay;			//Time to wait before spawning the first mission (seconds).
	//Timing specific
	int storiesFrameCycleTime;		//The cycle time to manage mission spawning, deletion etc... (seconds)	
	
	ref array<ref int> storiesList = {};	//The indexes of stories
	ref array<SDRC_Story> stories = {};		//List stories	
}

//------------------------------------------------------------------------------------------------
class SDRC_StoriesFrameJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_storiesConfig.json";
		
	ref SDRC_StoriesFrameConfig conf = new SDRC_StoriesFrameConfig();

	//------------------------------------------------------------------------------------------------
	void SDRC_MissionFrameJsonApi()
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

		if (conf.storiesFrameCycleTime < SDRC_STORIESFRAME_CYCLE_TIME_LIMIT)
		{
			SDRC_Log.Add("[SDRC_StoriesFrameConfig] storiesFrameCycleTime is less than " + SDRC_STORIESFRAME_CYCLE_TIME_LIMIT + ". This could lead to performance issues.", LogLevel.WARNING);
		}
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
		conf.comment = "Simple comment, not used in game";
		conf.storiesStartDelay = SDRC_STORIESFRAME_START_DELAY;		
		conf.storiesFrameCycleTime = SDRC_MISSIONFRAME_CYCLE_TIME;
		conf.storiesList = {0};
		
		#ifdef SDRC_RELEASE
		#endif	

		#ifndef SDRC_RELEASE				
		#endif
		
		//----------------------------------------------------
		conf.stories.Insert(Story0());
	}
		
	//----------------------------------------------------
	SDRC_Story Story0()
	{
		ref SDRC_Story story = new SDRC_Story();
		story.Set(
			0, 
			"id 0:",
			"Story Title",
			"Story intro",
			"Story endtro",
		);
		
		ref SDRC_Chapter chapter = SDRC_Chapter();
		chapter.Set(
			0, DC_EMissionType.CRASHSITE,
			"id 0:",
			"Chapter Title",
			"Chapter intro",
			"Chapter endtro",
			{1,10},
		);
		story.chapters.Insert(chapter);
		
		return story;		
	}
}