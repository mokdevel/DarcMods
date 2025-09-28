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
	
	const bool SDRC_DUMP_LINES = true;
	
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

		array<ref SDRC_StoryMapLine> storyMap = {};
	
		//Add the indexes
		foreach(SDRC_Chapter chapter : story.chapters)
		{
			while (storyMap.Count() < chapter.id + 1)
			{
				ref SDRC_StoryMapLine sml = new SDRC_StoryMapLine();
				storyMap.Insert(sml);
			}
			
			storyMap[chapter.id].line.Insert(chapter.id);
			storyMap[chapter.id].line.InsertAt(SDRC_ENextChapter.NONE, 0);			
		}

		//Handle the LOSE spots
		foreach(SDRC_Chapter chapter : story.chapters)
		{			
			//Add the LOSE index on the right side. This could be -1 for final end, or a next chapter
			storyMap[chapter.id].line.Insert(chapter.nextChapter[1]);
		}

		//Start state is on the left. This is the beginning of the story
		storyMap[1].line.RemoveOrdered(0);

		dumpStoryMapLine(storyMap);
				
		int i = 0;
		
		//Move the WIN states to left side on the next line
		i = 1;		
		foreach(SDRC_Chapter chapter : story.chapters)
		{
			int winIdx = chapter.nextChapter[0];
			if (storyMap[winIdx].line[0] == SDRC_ENextChapter.NONE)
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

		dumpStoryMapLine(storyMap);
				
		//Check that the last chapter has a win condition.
		int lastIdx = story.chapters.Count() - 1;
		if (story.chapters[lastIdx].nextChapter[0] == SDRC_ENextChapter.WIN)
		{
			ref SDRC_StoryMapLine sml = new SDRC_StoryMapLine();
			sml.line.Insert(SDRC_ENextChapter.WIN);
			storyMap.Insert(sml);			
		}
		else
		{
			SDRC_Log.Add("[SDRC_StoriesHelper:CheckStory] Story has no WIN chapter!", LogLevel.ERROR);
		}

		dumpStoryMapLine(storyMap);
		
		//Check that second column has WIN condition below.
		i = 0;		
		while (i < story.chapters.Count() - 1)
		{
			int idx = storyMap[i + 1].line[1];
			if (idx > 0)
			{
				if ( (storyMap[idx + 1].line[1] == SDRC_ENextChapter.LOSE)	//Position to be empty with a LOSE marker
				  && (idx != storyMap[i + 2].line[0])						//WIN condition not needed if the LOSE condition is the same as next line
				   )
				{
					//Put the win condition on the second column
					int winToAdd = story.chapters[idx - 1].nextChapter[0];
					bool isWinBelow = false;

					//Check that the win condition is not already there					
					int j = 0;
					for (j = i + 1; j < story.chapters.Count(); j++)
					{
						if (storyMap[j].line[1] == winToAdd)
						{
							isWinBelow = true;
							break;
						}
					}
					
					if (!isWinBelow)
					{
						storyMap[idx + 1].line.InsertAt(winToAdd, 1);
					}
					else
					{
						storyMap[idx + 1].line.InsertAt(SDRC_ENextChapter.NONE, 1);
					}
				}
			}			
			dumpStoryMapLine(storyMap);
			i++;			
		}

		dumpStoryMapLine(storyMap);	
//		drawStoryMapLine(storyMap);
		
		//Check the win connections
		i = 0;
		foreach(SDRC_Chapter chapter : story.chapters)
		{		
			if (storyMap[i + 1].line[1] == SDRC_ENextChapter.LOSE)
			{								
				//Check if there is a win condition below
				bool isWinBelow = false;				
				int j = 0;
				for (j = i + 1; j < story.chapters.Count(); j++)
				{
					if ( (storyMap[j].line[1] != SDRC_ENextChapter.LOSE) && (storyMap[j].line[1] == chapter.nextChapter[0]) )
					{
						isWinBelow = true;
						break;
					}
				}
				
				//Check if there are double wins 		
				if ( (storyMap[i + 2].line[0] == chapter.nextChapter[0]) && (chapter.nextChapter[0] == SDRC_ENextChapter.WIN) )
				{
					storyMap[i + 1].line.InsertAt(storyMap[i + 1].line[0], 1);
				}
				else //Add a vertical connection..
				{	
					if (isWinBelow) //..only if there is a WIN below
					{
						storyMap[i + 1].line.InsertAt(SDRC_ENextChapter.NONE, 1);
					}
				}
			}
			
			i++;

			dumpStoryMapLine(storyMap);
			
			//Stop before the last one
			if (i >= story.chapters.Count())
			{
				break;
			}
		}		
		
		dumpStoryMapLine(storyMap);
		
		drawStoryMapLine(storyMap);
	}
	
	//------------------------------------------------------------------------------------------------
	// Dump the strings
	static void dumpStoryMapLine(array<ref SDRC_StoryMapLine> storyMap)
	{
		#ifdef SDRC_DUMP_LINES
			SDRC_Log.Add("[SDRC_StoriesHelper:dumpStoryMapLine] ------------------------------------", LogLevel.NORMAL);
			foreach(SDRC_StoryMapLine sml : storyMap)
			{
					SDRC_Log.Add("[SDRC_StoriesHelper:dumpStoryMapLine] " + sml.line, LogLevel.NORMAL);
			}
		#endif
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
				if (char == SDRC_ENextChapter.LOSE)
				{
					line = line + "[L]";					
				}
				else if (char == SDRC_ENextChapter.WIN)
				{
					line = line + "[WIN]";
				}
				else if (char != 0)
				{
					line = line + "[" + char.ToString(2) + "]";
				}
				if (char == SDRC_ENextChapter.NONE)
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