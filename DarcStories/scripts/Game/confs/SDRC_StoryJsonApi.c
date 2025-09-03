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
	ERROR = -1,
	NONE = 0,			//Unknown state. Nothing should be run at this state.
	INIT,				//Story frame is being init
	STORY_WAITING,
//	STORY_START,
	CHAPTER_START,
	CHAPTER_INIT,
	CHAPTER_ACTIVE,
	CHAPTER_DONE,
	CHAPTER_OVER,
	WAITING_FOR_NEXT,
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
	int index = 0;							//The story index pointing to a chapter
	//Set outside of Set()
//	DC_EStoryState state = DC_EStoryState.NONE;
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
	string title = "";
	string text = "";
	string textWin;
	string textLose;
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();		
		
	void Set(int id_, DC_EMissionType missionType_, int subIdx_, int activeTime_, array<int> nextChapter_, string title_, string text_, string textWin_, string textLose_)
	{
		id = id_;
		missionType = missionType_;
		subIdx = subIdx_;
		activeTime = activeTime;
		nextChapter = nextChapter_;
		title = title_;
		text = text_;
		textWin = textWin_;
		textLose = textLose_;
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
		
		conf.chapters.Insert(Chapter01());
		conf.chapters.Insert(Chapter02());
		conf.chapters.Insert(Chapter03());
	};

	SDRC_Chapter Chapter01()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			//1, DC_EMissionType.SQUATTERS, 0, 
			1, DC_EMissionType.OCCUPATION, 3,
			SDRC_STORIES_CHAPTER_TIME_DEFAULT,
			{1, 1},
			"Chapter 1: Burning Metal"
			,"The team deployed at dawn. A vehicle crash on a coastal road had drawn intelligence attention — reports suggested the wreck contained documents tied to a high-value individual. The island, dotted with small cities and dense forests, offered both cover and danger. \n\nLanding near the twisted wreck, the squad swept the site. The squad had only minutes before hostile patrols noticed them."
			,"Amid burnt metal and shattered glass, a briefcase survived the flames. Inside, fragments of coordinates and coded notes hinted at movements and safehouses. The team exfiltrated with the documents intact, slipping back into the forest before addition lmilitias closed in. The intel raised more questions than answers, but it was enough to point them deeper into the island’s shadows. \n The trail to the target was not lost—just hidden further, deeper, and more dangerous than before."
			,"Pinned down by enemy patrols, the team was forced to retreat, leaving the documents behind. Flames consumed the wreck as they withdrew, erasing vital clues. The trail to the target was not lost—just hidden further, deeper, and more dangerous than before. \n With the documents lost in the flames, command redirected the squad. Reports indicated that another cache of intel had been smuggled to a militia roadblock deeper inland. The squad advanced along cracked asphalt, the humid air thick with the scent of pine and salt."
		);
		
		chapter.general.SetDefaults(comment_: "Chapter 1", title_: "Chapter 1: Burning Metal", info_: "See map for more information");
	
		return chapter;
	};	
		
	SDRC_Chapter Chapter02()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			2, DC_EMissionType.ROADBLOCK, 1,
			SDRC_STORIES_CHAPTER_TIME_DEFAULT,
			{3, DC_ENextChapter.LOSE},
			"Chapter 2: The Roadblock"
			,"The roadblock was crude but fortified—sandbags, steel sheets, and armed guards scanning every passing vehicle. If the squad could breach it, the missing link might be recovered."
			,"After a precise assault, the roadblock fell. Hidden with the commander, the squad uncovered a satchel of coded manifests. Among the pages: a repeated reference to a “Yellow Sector” and clandestine supply runs—threads leading back to the target."
			,"The attack collapsed under heavy resistance. The squad was forced to withdraw into the forest, leaving the roadblock intact. The intel remained out of reach, and the trail grew colder, leaving the target’s whereabouts shrouded in silence."
		);
		
		chapter.general.SetDefaults(comment_: "Chapter 2", title_: "Chapter 2: The Roadblock", info_: "See map for more information");
	
		return chapter;
	};	
	
	SDRC_Chapter Chapter03()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			3, DC_EMissionType.HVTVIP, 3, 
			SDRC_STORIES_CHAPTER_TIME_DEFAULT,
			{DC_ENextChapter.WIN, DC_ENextChapter.LOSE},
			"Chapter 3: The Hit"
			,"With the salvaged documents secure, the squad moved inland. The coded coordinates pointed towards %l. Recon drones showed faint activity. It seemed to be a staging post, possibly linked to the target’s network. \n The squad prepared: setting observation posts, marking entry routes, and coordinating fire support. Every step forward brought them closer to the unseen figure that loomed over the mission."
			,"The team infiltrated quietly, seizing encrypted files and maps before the enemy realized their presence. The evidence confirmed patterns of movement—the target’s trail was narrowing, close enough to prepare for elimination."
			,"A misstep triggered alarms. Gunfire lit the station, forcing a hasty retreat. In the chaos, the enemy managed to destroy part of their own intel. The squad escaped, but with gaps in the puzzle—the path to the target now unclear."
		);
	
		chapter.general.SetDefaults(comment_: "Chapter 3: The Hit", info_: "See map for more information");
		
		return chapter;
	};	
}