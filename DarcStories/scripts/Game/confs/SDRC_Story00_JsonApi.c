//Helpers SDRC_StoryConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
class SDRC_Story00_JsonApi : SDRC_StoryJsonApi
{
	void SDRC_Story00_JsonApi(string fileName = "")
	{
		SetFileName("dc_storyConfig_00.json");
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
			0, 1, 
			"Story 00",
			"The Hit",
			{},
		);
		
		conf.chapters.Insert(Chapter01());
		conf.chapters.Insert(Chapter02());
		conf.chapters.Insert(Chapter03());
		conf.chapters.Insert(ChapterWin());
	};

	SDRC_Chapter Chapter01()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			//1, SDRC_EMissionType.SQUATTERS, 0, 
			1, {2, 2}, 
			SDRC_EMissionType.OCCUPATION, 3,
			SDRC_CHAPTER_TIME_DEFAULT,			
			"Chapter 1: Burning Metal"
			,"The team deployed at dawn. A vehicle crash on a rural road had drawn intelligence attention — reports suggested the wreck contained documents tied to a high-value individual. The destination, dotted with small cities and dense forests, offered both cover and danger. \n\nLanding near the twisted wreck, the squad swept the site. The squad had only minutes before hostile patrols noticed them."
			,"Amid burnt metal and shattered glass, a briefcase survived the flames. Inside, fragments of coordinates and coded notes hinted at movements and safehouses. The team exfiltrated with the documents intact, slipping back into the forest before addition lmilitias closed in. The intel raised more questions than answers, but it was enough to point them deeper into the island’s shadows. \n The trail to the target was not lost—just hidden further, deeper, and more dangerous than before."
			,"Pinned down by enemy patrols, the team was forced to retreat, leaving the documents behind. Flames consumed the wreck as they withdrew, erasing vital clues. The trail to the target was not lost—just hidden further, deeper, and more dangerous than before. \n\nWith the documents lost in the flames, command redirected the squad. Reports indicated that another cache of intel had been smuggled to a militia roadblock deeper inland. The squad advanced along cracked asphalt, the humid air thick with the scent of pine and salt."
		);
		
		chapter.general.SetDefaults(comment_: "Chapter 1", title_: "Chapter 1: Burning Metal", info_: "See map for more information", winMessage_: "See map for more information", loseMessage_: "See map for more information");
	
		return chapter;
	};	
		
	SDRC_Chapter Chapter02()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			2, {3, SDRC_ENextChapter.LOSE},
			SDRC_EMissionType.ROADBLOCK, 1,
			SDRC_CHAPTER_TIME_DEFAULT,			
			"Chapter 2: The Roadblock"
			,"The roadblock was crude but fortified—sandbags, steel sheets, and armed guards scanning every passing vehicle. If the squad could breach it, the missing link might be recovered."
			,"After a precise assault, the roadblock fell. Hidden with the commander, the squad uncovered a satchel of coded manifests. Among the pages: a repeated reference to a “Yellow Sector” and clandestine supply runs—threads leading back to the target."
			,"The attack collapsed under heavy resistance. The squad was forced to withdraw into the forest, leaving the roadblock intact. The intel remained out of reach, and the trail grew colder, leaving the target’s whereabouts shrouded in silence. \n\n<END>"
		);
		
		chapter.general.SetDefaults(comment_: "Chapter 2", title_: "Chapter 2: The Roadblock", info_: "See map for more information", winMessage_: "See map for more information", loseMessage_: "See map for more information");
	
		return chapter;
	};	
	
	SDRC_Chapter Chapter03()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			3, {4, SDRC_ENextChapter.LOSE},
			SDRC_EMissionType.HVTVIP, 3, 
			SDRC_CHAPTER_TIME_DEFAULT,			
			"Chapter 3: The Hit"
			,"With the salvaged documents secure, the squad moved inland. The coded coordinates pointed towards %l. Recon drones showed faint activity. It seemed to be a staging post, possibly linked to the target’s network. \n\nThe squad prepared: setting observation posts, marking entry routes, and coordinating fire support. Every step forward brought them closer to the unseen figure that loomed over the mission."
			,"The team infiltrated quietly, seizing encrypted files and maps before the enemy realized their presence. The evidence confirmed patterns of movement—the target’s trail was narrowing, close enough to prepare for elimination. \n\n<END>"
			,"A misstep triggered alarms. Gunfire lit the station, forcing a hasty retreat. In the chaos, the enemy managed to destroy part of their own intel. The squad escaped, but with gaps in the puzzle—the path to the target now unclear. \n\n<END>"
		);
	
		chapter.general.SetDefaults(comment_: "Chapter 3", title_: "Chapter 3: The Hit", info_: "See map for more information", winMessage_: "See map for more information", loseMessage_: "See map for more information");
		
		return chapter;
	};	

	SDRC_Chapter ChapterWin()
	{
		ref SDRC_Chapter chapter = new SDRC_Chapter();
		chapter.Set(
			4, {SDRC_ENextChapter.WIN, SDRC_ENextChapter.LOSE},
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