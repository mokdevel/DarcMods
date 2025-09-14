//Helpers SDRC_StoriesClasses.c

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
	STORY_WAITING,		//Waiting before starting a mission
	STORY_START,		//Select and start the story. Check that story is usabled.
	CHAPTER_START,		//Select chapter. Spawn the right mission entity.
	CHAPTER_INIT,		//Initialize the chapter. This is done as a delayed action for the mission entity.
	CHAPTER_WAITING,	//Final check to see that mission started properly
	CHAPTER_ACTIVE,		//Normal state running the chapter.
	CHAPTER_DONE,		//Chapter is over. Either win or lose.
	CHAPTER_OVER,
	WAITING_FOR_NEXT,
	STORY_END,
};

enum DC_ENextChapter
{
	NONE = 0,
	WIN = -2,
	LOSE = -1,
}

//------------------------------------------------------------------------------------------------
class SDRC_Story : Managed
{
	int id;
	int chapterId = 0;						//The id of the chapter
	string comment;
	string title = "";
	ref array<string> dependencies = {};	//List of mods needed for the story
	//Set outside of Set()
	//DC_EStoryState state = DC_EStoryState.NONE;
	ref array<ref SDRC_Chapter> chapters = {};
	
	void Set(int id_, int chapterId_, string comment_, string title_, array<string> dependencies_)
	{
		id = id_;
		chapterId = chapterId_;
		comment = comment_;
		title = title_;
		dependencies = dependencies_;
		//NOTE: state handled outside
		//NOTE: chapters are added separately
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns the index of the chapters table where chapterId points
	*/	
	int GetIndex(int chapterId)
	{
		int idx = 0;
		
		foreach (SDRC_Chapter chapter : chapters)
		{
			if (chapter.id == chapterId)
			{
				return idx;
			}
			
			idx++;
		}
		
		//SDRC_Log.Add("[SDRC_Story:GetIndex] Wrong chapterId (" + chapterId + ") in story : " + id, LogLevel.ERROR);
		return -1;		
	}	
	
	//------------------------------------------------------------------------------------------------
/*	void GetState()
	{	
		return state;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetState(DC_EStoryState stateToSet)
	{	
		state = stateToSet;

		//If chapter is over, 		
		if (state == DC_EStoryState.CHAPTER_OVER)
		{
			GetGame().GetCallqueue().CallLater(StartNewChapter, m_Config.chapterTimeBetween*1000, false);
		}
	}*/
}

//------------------------------------------------------------------------------------------------
class SDRC_Chapter : Managed
{
	int id;
	ref array<int> nextChapter = {};			//Where to go after a win, lose		
	DC_EMissionType missionType;		//Type of the mission
	int subIdx;							//Sub mission index
	int activeTime;
	string title = "";
	string text = "";
	string textWin;
	string textLose;
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();		
	//Default values not to be changed
	DC_EMissionSuccess success = DC_EMissionSuccess.UNKNOWN;
	int chapterEndTime = 0;
		
	void Set(int id_, array<int> nextChapter_, DC_EMissionType missionType_, int subIdx_, int activeTime_, string title_, string text_, string textWin_, string textLose_)
	{
		id = id_;
		missionType = missionType_;
		subIdx = subIdx_;
		activeTime = activeTime_;
		nextChapter = nextChapter_;
		title = title_;
		text = text_;
		textWin = textWin_;
		textLose = textLose_;
	}		
}