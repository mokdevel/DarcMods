//Helpers SDRC_StoriesHelper

//------------------------------------------------------------------------------------------------
/*!
Helper functions for DarcStories
*/

//------------------------------------------------------------------------------------------------
class SDRC_StoryMapLine : Managed
{
	ref array<int> line = {};
}

//------------------------------------------------------------------------------------------------
sealed class SDRC_StoriesHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Create a visual representation of the story arch
	*/	
	static void CheckStory(SDRC_Story story)
	{
		/*foreach(SDRC_Chapter chapter : story.chapters)
		{
			SDRC_Log.Add("[SDRC_StoriesHelper:CheckStory] [" + chapter.nextChapter[1] + "]-[" + chapter.id + "]-[" + chapter.nextChapter[0] + "]", LogLevel.NORMAL);
		}*/

		ref array<ref SDRC_StoryMapLine> storyMap = {};
		
		//Add the indexes
		foreach(SDRC_Chapter chapter : story.chapters)
		{
			while (storyMap.Count() < chapter.id + 1)
			{
				ref SDRC_StoryMapLine sml = new SDRC_StoryMapLine();
				storyMap.Insert(sml);
			}
			
			storyMap[chapter.id].line.Insert(chapter.id);
			storyMap[chapter.id].line.InsertAt(DC_ENextChapter.NONE, 0);			
		}

		//Handle the lose spots
		foreach(SDRC_Chapter chapter : story.chapters)
		{			
			//Add the lose index on the right side
			storyMap[chapter.id].line.Insert(chapter.nextChapter[1]);
		}

		int i = 0;
		
		//Start state is on the left
		storyMap[1].line.RemoveOrdered(0);

		//dumpStoryMapLine(storyMap);
				
		//Move the win states to left side
		i = 1;		
		foreach(SDRC_Chapter chapter : story.chapters)
		{
			int winIdx = chapter.nextChapter[0];
			if (storyMap[winIdx].line[0] == DC_ENextChapter.NONE)
			{			
				if (storyMap[i].line[1] != i)
				{
					storyMap[winIdx].line.RemoveOrdered(0);
				}
			}
			i++;			
			
			//Stop before the last one			
			if (i >= story.chapters.Count())
			{
				break;
			}
		}		

		//dumpStoryMapLine(storyMap);
		
		//Handle the final win 
		int lastIdx = story.chapters.Count() - 1;
		if (story.chapters[lastIdx].nextChapter[0] == DC_ENextChapter.WIN)
		{
			ref SDRC_StoryMapLine sml = new SDRC_StoryMapLine();
			sml.line.Insert(DC_ENextChapter.WIN);
			storyMap.Insert(sml);			
		}
		else
		{
			SDRC_Log.Add("[SDRC_StoriesHelper:CheckStory] Story has no WIN chapter!", LogLevel.ERROR);
		}
		
//		dumpStoryMapLine(storyMap);
//		drawStoryMapLine(storyMap);
		
		//Check the win connections
		i = 1;
		foreach(SDRC_Chapter chapter : story.chapters)
		{
			if (storyMap[i].line[1] == DC_ENextChapter.LOSE)
			{
				//Check if there are double wins 		
				if ( (storyMap[i + 1].line[0] == chapter.nextChapter[0]) && (chapter.nextChapter[0] == DC_ENextChapter.WIN) )
				{
					storyMap[i].line.InsertAt(storyMap[i].line[0], 1);
				}
				else //Add a vertical connection
				{				
					storyMap[i].line.InsertAt(DC_ENextChapter.NONE, 1);
				}
			}
			
			i++;

			//Stop before the last one			
			if (i > story.chapters.Count())
			{
				break;
			}
		}		
		
		//dumpStoryMapLine(storyMap);
		
		drawStoryMapLine(storyMap);
	}
	
	//------------------------------------------------------------------------------------------------
	// Dump the strings
	static void dumpStoryMapLine(array<ref SDRC_StoryMapLine> storyMap)
	{
		SDRC_Log.Add("[SDRC_StoriesHelper:dumpStoryMapLine] ------------------------------------", LogLevel.NORMAL);
		foreach(SDRC_StoryMapLine sml : storyMap)
		{
				SDRC_Log.Add("[SDRC_StoriesHelper:dumpStoryMapLine] " + sml.line, LogLevel.NORMAL);
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	// Draw the visual lines
	static void drawStoryMapLine(array<ref SDRC_StoryMapLine> storyMap)
	{
		SDRC_Log.Add("[SDRC_StoriesHelper:drawStoryMapLine] Story chapters visualised: ", LogLevel.NORMAL);
		
		array<string>lines = {};
		
		foreach(SDRC_StoryMapLine sml : storyMap)
		{
			string line = "";
			foreach (int char : sml.line)
			{
				if (char == DC_ENextChapter.LOSE)
				{
					line = line + "[L]";					
				}
				else if (char == DC_ENextChapter.WIN)
				{
					line = line + "[W]";
				}
				else if (char != 0)
				{
					line = line + "[" + char.ToString(2) + "]";
				}
				if (char == DC_ENextChapter.NONE)
				{
					line = line + " |  ";
				}
			}
			lines.Insert(line);
		}		
		
		foreach(string line : lines)
		{
			if (line.Contains("[L]"))
			{
				line.Replace("[L]", "");				
				line = line + "----------------------";
				line = line.Substring(0,20);
				line = line + "[L]";
			}
			
			SDRC_Log.Add("[SDRC_StoriesHelper:drawStoryMapLine] " + line, LogLevel.NORMAL);
		}		
	}
}