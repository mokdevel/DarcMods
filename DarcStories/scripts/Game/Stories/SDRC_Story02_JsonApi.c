//Helpers SDRC_StoryConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
class SDRC_Story02_JsonApi : SDRC_StoryJsonApi
{
	void SDRC_Story02_JsonApi(string fileName = "")
	{
		SetFileName("dc_storyConfig_02.json");
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		
		#ifdef SDRC_RELEASE
		#endif	

		#ifndef SDRC_RELEASE				
		#endif
		
		conf.Set(
			2, 1, 
			"Story 02",
			"Testing",
			{},
		);
		
		conf.chapters.Insert(Chapter01());
		conf.chapters.Insert(ChapterWin());
	};

	SDRC_Chapter Chapter01()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			//1, SDRC_EMissionType.SQUATTERS, 0, 
			1, {2, SDRC_ENextChapter.LOSE}, 
			SDRC_EMissionType.OCCUPATION, 3,
			SDRC_CHAPTER_TIME_DEFAULT,			
			"Chapter 1: Testing"
			,"Briefing"
			,"Win text: asdf"
			,"Lose text: asdf"
		);
		
		chapter.general.SetDefaults(comment_: "Chapter 1", title_: "Chapter 1: Testing", info_: "See map for more information", winMessage_: "See map for more information", loseMessage_: "See map for more information", difficulty_: SDRC_EMissionDifficulty.HARD);
	
		return chapter;
	};	

	SDRC_Chapter ChapterWin()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			2, {SDRC_ENextChapter.WIN, SDRC_ENextChapter.LOSE},
			SDRC_EMissionType.STASH, 0, 
			SDRC_CHAPTER_TIME_DEFAULT,			
			"Epilogue: Loot"
			,"Yeah, there is loot."
			,"Loot was collected."
			,"Loot was not collected"
		);
	
		chapter.general.SetDefaults(comment_: "Epilogue", title_: "Epilogue: Loot", info_: "See map for more information", winMessage_: "See map for more information", loseMessage_: "See map for more information");
		
		return chapter;
	};			
}