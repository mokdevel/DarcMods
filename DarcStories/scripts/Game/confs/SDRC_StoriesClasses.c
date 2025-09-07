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
	STORY_START,		//Start the mission
	CHAPTER_START,
	CHAPTER_INIT,
	CHAPTER_ACTIVE,
	CHAPTER_DONE,
	CHAPTER_OVER,
	WAITING_FOR_NEXT,
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
		
	void Set(int id_, array<int> nextChapter_, DC_EMissionType missionType_, int subIdx_, int activeTime_, string title_, string text_, string textWin_, string textLose_)
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