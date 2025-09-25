//------------------------------------------------------------------------------------------------
/*!
Some documentation on the finite state machine (FSM)

States (SDRC_EStoryState)
	NONE = 0,			//Unknown state. Nothing should be run at this state.
	INIT,				//Story frame is being init
	STORY_WAITING,		//Waiting before starting a mission
	STORY_START,		//Select and start the story. Check that story is usabled.
	CHAPTER_START,		//Select chapter. Spawn the right mission entity.
	CHAPTER_INIT,		//Initialize the chapter. This is done as a delayed action for the mission entity.
	CHAPTER_WAITING,	//Final check to see that mission started properly. Wait for spawn to be ready by DarcMissions.
	CHAPTER_ACTIVE,		//Normal state running the chapter.
	CHAPTER_DONE,		//Chapter is done. Either win or lose.
	CHAPTER_OVER,		//Chapter is over, waiting for next chapter
	(WAITING_FOR_NEXT,	//Waiting for next chapter - unused)
	STORY_END,			//The final state

				+---------------+
				|NONE			|
				+---------------+
					|
				+---------------+
				|INIT			|
				+---------------+
					: <time: storiesStartDelay>
					|
				+---------------+
	+---------->|STORY_WAITING	|
	|			+---------------+
	|				|
	|			+---------------+
	|			|STORY_START	| -> ERROR (tbd)
	|			+---------------+
	|				|
	|			+---------------+
	|	+---+-->|CHAPTER_START	| -> ERROR
	|	|	|	+---------------+
	|	|	|		|
	|	|	|	+---------------+
	|	|	|	|CHAPTER_INIT	|
	|	|	|	+---------------+
	|	|	|		: <time: 2 sec>
	|	|	|	SetChapterParameters_Delayed() -> ERROR
	|	| fail		|
	|	|	|	+---------------+
	|	|	+---|CHAPTER_WAITING|
	|	|		+---------------+
	|	|			|
	|	|		+---------------+
	|	|		|CHAPTER_ACTIVE	|<----------------------+
	|	|		+---------------+						|
	|	|			|									|
	|	|		mission state: ACTIVE, success unkown --+
	|	|			|
	|	|		mission state: ACTIVE (win/lose), END, EXIT
	|	|			|
	|	|		+---------------+
	|	|		|CHAPTER_DONE	|
	|	|		+---------------+
	|	|			|
	|	|		mission success: DELETED -> ERROR
	|	|			|
	|	|		mission success: WIN, LOSE
	|	|			|
	|	|		+---------------+
	|	|		|CHAPTER_OVER	|
	|	|		+---------------+
	|	|			|		|
	|	|			|	nextChapter: WIN, LOSE
	|	|			|		|
	|	|			|	+---------------+
	|	|			|	|STORY_END		|
	|	|			|	+---------------+
	|	|			|		: <time: storyTimeBetween>
	|	|			|	StartNewStory()
	|	|			|		|
	+-----------------------+
		|			|		
		|			: <time: chapterTimeBetween>
		|		[StartNewChapter()]
		|			|		
		+-----------+		
							
							












*/