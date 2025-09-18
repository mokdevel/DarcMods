//Helpers SDRC_StoriesFrameConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

	//Release options
	#ifdef SDRC_RELEASE
		private const int SDRC_STORIESFRAME_START_DELAY = 1*60;						//Time to wait before spawning the first mission (seconds)
		private const int SDRC_STORIESFRAME_CYCLE_TIME_DEFAULT = 10;
		private const int SDRC_STORIESFRAME_CYCLE_TIME_LIMIT = 5;					//The cycle to run the mission frame. 

		private const int SDRC_STORY_TIME_BETWEEN = 20*60;							//Time between stories

		private const int SDRC_STORY_CYCLE_TIME = 10;								//Time for the chapter to be active
		private const int SDRC_CHAPTER_TIME_DEFAULT = 30*60;						//Time for the mission to be active
		private const int SDRC_CHAPTER_TIME_BETWEEN = 10*60;						//Time between chapters
	#endif
	
	//Development time options
	#ifndef SDRC_RELEASE	
		private const int SDRC_STORIESFRAME_START_DELAY = 8;						//Time to wait before spawning the first mission (seconds)
		private const int SDRC_STORIESFRAME_CYCLE_TIME_DEFAULT = 10;
		private const int SDRC_STORIESFRAME_CYCLE_TIME_LIMIT = 5;					//The cycle to run the mission frame. 

		private const int SDRC_STORY_TIME_BETWEEN = 3*60;							//Time between stories

		private const int SDRC_STORY_CYCLE_TIME = 10;								//Time for the chapter to be active
		private const int SDRC_CHAPTER_TIME_DEFAULT = 2*60;							//Time for the chapter to be active
		private const int SDRC_CHAPTER_TIME_BETWEEN = 1*60;							//Time between chapters
	#endif

//------------------------------------------------------------------------------------------------
class SDRC_StoriesFrameConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Mission specific
	string comment;
	int storiesStartDelay;			//Time to wait before spawning the first mission (seconds).
	//StoriesFrame: Timing specific
	int storiesFrameCycleTime;		//The cycle time to run the storiesFrame (seconds)	
	int storyTimeBetween;			//The time between stories
	//Story: Timing specific for all 
	int storyCycleTime;				//The cycle time to run the story (seconds)
	int chapterTimeBetween;			//The time between chapters
	
	ref array<ref string> storiesList = {};		//The indexes of stories
}

//------------------------------------------------------------------------------------------------
class SDRC_StoriesFrameJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_storiesConfig.json";
		
	ref SDRC_StoriesFrameConfig conf = new SDRC_StoriesFrameConfig();

	//------------------------------------------------------------------------------------------------
	void SDRC_StoriesFrameConfig()
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
		conf.storyTimeBetween = SDRC_STORY_TIME_BETWEEN;
		conf.storyCycleTime = SDRC_STORY_CYCLE_TIME;
		conf.chapterTimeBetween = SDRC_CHAPTER_TIME_BETWEEN;
		conf.storiesList = {"dc_storyConfig_00.json", "dc_storyConfig_01.json"};
		
		#ifdef SDRC_RELEASE
		#endif	

		#ifndef SDRC_RELEASE				
		#endif		
	}
	
	//------------------------------------------------------------------------------------------------
	void CreateStories()
	{			
		//Load stories from file
		SDRC_Story00_JsonApi story00_JsonApi = new SDRC_Story00_JsonApi();		
		story00_JsonApi.Load();
		//conf.stories.Insert(story00_JsonApi.conf);		
		
		SDRC_Story01_JsonApi story01_JsonApi = new SDRC_Story01_JsonApi();		
		story01_JsonApi.Load();
		//conf.stories.Insert(story01_JsonApi.conf);
	}	
}