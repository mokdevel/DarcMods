//Helpers SDRC_StoryConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
class SDRC_Story01_JsonApi : SDRC_StoryJsonApi
{
	void SDRC_Story01_JsonApi(string fileName = "")
	{
		SetStoryFileName("dc_storyConfig_01.json");
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
			1, 1,
			"Story 01",
			"Takedown",
			{},
		);
		
		conf.chapters.Insert(Chapter01());
		conf.chapters.Insert(Chapter02());
		conf.chapters.Insert(Chapter03());
		conf.chapters.Insert(Chapter04());
		conf.chapters.Insert(Chapter05());
		conf.chapters.Insert(Chapter06());
		conf.chapters.Insert(Chapter07());
	};

	SDRC_Chapter Chapter01()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			//1, DC_EMissionType.SQUATTERS, 0, 
			1, {2, 3},
			DC_EMissionType.OCCUPATION, 3,
			SDRC_CHAPTER_TIME_DEFAULT,			
			""
			,""
			,""
			,""
		);
		
		chapter.general.SetDefaults(comment_: "Chapter 1", title_: "Chapter 1", info_: "See map for more information", winMessage_: "See map for more information", loseMessage_: "See map for more information");
	
		return chapter;
	};	
		
	SDRC_Chapter Chapter02()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			2, {5, DC_ENextChapter.LOSE},
			DC_EMissionType.ROADBLOCK, 1,
			SDRC_CHAPTER_TIME_DEFAULT,
			""
			,""
			,""
			,""
		);
		
		chapter.general.SetDefaults(comment_: "Chapter 2", title_: "Chapter 2", info_: "See map for more information", winMessage_: "See map for more information", loseMessage_: "See map for more information");
	
		return chapter;
	};	
	
	SDRC_Chapter Chapter03()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			3, {4, DC_ENextChapter.LOSE},
			DC_EMissionType.HVTVIP, 3, 
			SDRC_CHAPTER_TIME_DEFAULT,			
			""
			,""
			,""
			,""
		);
	
		chapter.general.SetDefaults(comment_: "Chapter 3", title_: "Chapter 3", info_: "See map for more information", winMessage_: "See map for more information", loseMessage_: "See map for more information");
		
		return chapter;
	};	
	
	SDRC_Chapter Chapter04()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			4, {6, 5},
			DC_EMissionType.HVTVIP, 3, 
			SDRC_CHAPTER_TIME_DEFAULT,			
			""
			,""
			,""
			,""
		);
	
		chapter.general.SetDefaults(comment_: "Chapter 4", title_: "Chapter 4", info_: "See map for more information", winMessage_: "See map for more information", loseMessage_: "See map for more information");
		
		return chapter;
	};		

	SDRC_Chapter Chapter05()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			5, {7, DC_ENextChapter.LOSE},
			DC_EMissionType.HVTVIP, 3, 
			SDRC_CHAPTER_TIME_DEFAULT,			
			""
			,""
			,""
			,""
		);
	
		chapter.general.SetDefaults(comment_: "Chapter 5", title_: "Chapter 5", info_: "See map for more information", winMessage_: "See map for more information", loseMessage_: "See map for more information");
		
		return chapter;
	};

	SDRC_Chapter Chapter06()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			6, {7, DC_ENextChapter.LOSE},
			DC_EMissionType.HVTVIP, 3, 
			SDRC_CHAPTER_TIME_DEFAULT,			
			""
			,""
			,""
			,""
		);
	
		chapter.general.SetDefaults(comment_: "Chapter 6", title_: "Chapter 6", info_: "See map for more information", winMessage_: "See map for more information", loseMessage_: "See map for more information");
		
		return chapter;
	};
	
	SDRC_Chapter Chapter07()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			7, {10, DC_ENextChapter.LOSE},
			DC_EMissionType.HVTVIP, 3, 
			SDRC_CHAPTER_TIME_DEFAULT,			
			""
			,""
			,""
			,""
		);
	
		chapter.general.SetDefaults(comment_: "Chapter 7", title_: "Chapter 7", info_: "See map for more information", winMessage_: "See map for more information", loseMessage_: "See map for more information");
		
		return chapter;
	};
	
	SDRC_Chapter ChapterWin()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			10, {DC_ENextChapter.WIN, DC_ENextChapter.WIN},
			DC_EMissionType.STASH, 3, 
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