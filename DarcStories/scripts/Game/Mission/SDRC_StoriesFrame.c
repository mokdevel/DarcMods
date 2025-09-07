//------------------------------------------------------------------------------------------------
/*!
This is the mission story framework file. 
*/

//------------------------------------------------------------------------------------------------
//const string DCS_ID_PREFIX = "DCS_";				//The prefix used for stories marker and missions Id's.

class storyMapLine : Managed
{
	ref array<int> line = {};
}

//------------------------------------------------------------------------------------------------
class SDRC_StoriesFrame
{
	private static SDRC_StoriesFrame s_Instance;		
	private static int m_RequestIdCounter = 1000;
	
//	ref array<ref SDRC_Mission> m_MissionList = new array<ref SDRC_Mission>;
	private ref SDRC_StoriesFrameJsonApi m_DC_StoriesFrameJsonApi = new SDRC_StoriesFrameJsonApi();
	private ref SDRC_StoriesFrameConfig m_Config;
	private ref SDRC_Story m_Story;
	ref SDRC_Chapter m_Chapter = new SDRC_Chapter();
	
	private int m_StoryIdx;
	private DC_EStoryState m_State;
	private int m_RequestId;
	
//	private bool m_StoryActive;			//true if a story has been started
//	private bool m_ChapterActive;		//true if a chapter inside a story has started
	
//	private string m_sWorldName;
	
	//------------------------------------------------------------------------------------------------
	void SDRC_StoriesFrame()
	{
		SDRC_Log.Add("[SDRC_StoriesFrame] Starting SDRC_StoriesFrame", LogLevel.NORMAL);
		s_Instance = this;
		m_State = DC_EStoryState.INIT;
				
//		m_sWorldName = SDRC_Misc.GetWorldName(true);

		//Load configuration from file
		m_DC_StoriesFrameJsonApi.Load();
		m_Config = m_DC_StoriesFrameJsonApi.conf;
		
		m_DC_StoriesFrameJsonApi.LoadStories();

		//Reset
		m_StoryIdx = 1;
		m_RequestId = -1;

		CheckStory();
						
		//Fix seconds to ms
		SDRC_Log.Add("[SDRC_StoriesFrame] Waiting for " + m_Config.storiesStartDelay + " seconds before starting stories.", LogLevel.NORMAL);
		m_Config.storiesStartDelay = m_Config.storiesStartDelay * 1000;		//sec to ms
		
		//Start the mission framework.
		m_State = DC_EStoryState.STORY_START;
		GetGame().GetCallqueue().CallLater(StoriesCycleManager, m_Config.storiesStartDelay, false);
	}
	
	//------------------------------------------------------------------------------------------------
	void CheckStory()
	{
		m_Story = m_Config.stories[m_StoryIdx];
		foreach(SDRC_Chapter chapter : m_Story.chapters)
		{
			SDRC_Log.Add("[SDRC_StoriesFrame:CheckStory] [" + chapter.nextChapter[1] + "]-[" + chapter.id + "]-[" + chapter.nextChapter[0] + "]", LogLevel.NORMAL);
		}

		ref array<ref storyMapLine> storyMap = {};
		
		//Add the indexes
		foreach(SDRC_Chapter chapter : m_Story.chapters)
		{
			while (storyMap.Count() < chapter.id + 1)
			{
				ref storyMapLine sml = new storyMapLine();
				storyMap.Insert(sml);
			}
			
			storyMap[chapter.id].line.Insert(chapter.id);
			storyMap[chapter.id].line.InsertAt(DC_ENextChapter.NONE, 0);			
		}

		//Handle the lose spots
		foreach(SDRC_Chapter chapter : m_Story.chapters)
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
		foreach(SDRC_Chapter chapter : m_Story.chapters)
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
			if (i >= m_Story.chapters.Count())
			{
				break;
			}
		}		

		dumpStoryMapLine(storyMap);
		
		//Check if there are double wins 
		
		//Handle the final win 
		int lastIdx = m_Story.chapters.Count() - 1;
		if (m_Story.chapters[lastIdx].nextChapter[0] == DC_ENextChapter.WIN)
		{
			ref storyMapLine sml = new storyMapLine();
			sml.line.Insert(DC_ENextChapter.WIN);
			storyMap.Insert(sml);			
		}
		else
		{
			SDRC_Log.Add("[SDRC_StoriesFrame:CheckStory] Story has no WIN chapter!", LogLevel.ERROR);
		}
		
		dumpStoryMapLine(storyMap);
		
		//Check the win connections
		i = 1;
		foreach(SDRC_Chapter chapter : m_Story.chapters)
		{
			if (storyMap[i].line[1] == DC_ENextChapter.LOSE)
			{
				if (storyMap[i + 1].line[0] == chapter.nextChapter[0])
				{
					storyMap[i].line.InsertAt(storyMap[i].line[0], 1);
				}
				else
				{				
					storyMap[i].line.InsertAt(DC_ENextChapter.NONE, 1);
				}
			}
			
			i++;

			//Stop before the last one			
			if (i > m_Story.chapters.Count())
			{
				break;
			}
			dumpStoryMapLine(storyMap);
		}		
		
		dumpStoryMapLine(storyMap);
		
		drawStoryMapLine(storyMap);
	}
	
	void dumpStoryMapLine(array<ref storyMapLine> storyMap)
	{
		SDRC_Log.Add("[SDRC_StoriesFrame:CheckStory] ------------------------------------", LogLevel.NORMAL);
		foreach(storyMapLine sml : storyMap)
		{
//			if (sml.line.Count() > 0)
//			{
				SDRC_Log.Add("[SDRC_StoriesFrame:CheckStory] " + sml.line, LogLevel.NORMAL);
//			}
		}		
	}

	void drawStoryMapLine(array<ref storyMapLine> storyMap)
	{
		array<string>lines = {};
		
		foreach(storyMapLine sml : storyMap)
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
			
			SDRC_Log.Add("[SDRC_StoriesFrame:CheckStory] " + line, LogLevel.NORMAL);
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	static SDRC_StoriesFrame GetInstance()
	{
		return s_Instance;		
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	End the mission framework. Clean up the running missions.
	*/	
	void ~SDRC_StoriesFrame()
	{
		//Do clean up, save, etc ...
		SDRC_Log.Add("[~SDRC_StoriesFrame] Stopping SDRC_StoriesFrame", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Mission life cycle manager.
	*/
	protected void StoriesCycleManager()
	{	
		//Search if the mission is active
		SDRC_MissionStat stat = SDRC_MissionStats.GetStat(m_RequestId);
		
		if (m_State == DC_EStoryState.ERROR)
		{
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] We're in an error state. To be fixed...", LogLevel.ERROR);
		}
		
		if (m_State == DC_EStoryState.STORY_WAITING)
		{	
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Waiting for next chapter.", LogLevel.DEBUG);							
			//NOTE: CHAPTER_START is set in StartNewChapter()
		}
		
		if (m_State == DC_EStoryState.STORY_START)
		{	
			m_RequestId = -1;
			m_Story = m_Config.stories[m_StoryIdx];
			//TBD: Error checking
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Starting story: " + m_StoryIdx + " , chapter: " + m_Chapter.general.title, LogLevel.NORMAL);
			
			m_State = DC_EStoryState.CHAPTER_START;
		}		
		
		if (m_State == DC_EStoryState.CHAPTER_START)
		{		
			IEntity missionEntity;
			vector pos = "0 0 0";
			
			m_Chapter = m_Story.chapters[m_Story.index];
			//TBD: Error checking
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Spawning new story chapter: " + m_Story.index + " , chapter: " + m_Chapter.general.title, LogLevel.NORMAL);
			
			string resourceName = SDRC_MissionEnumHelper.GetMissionPrefab(m_Chapter.missionType);
			if (resourceName == "")
			{
				SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Invalid resourcename.", LogLevel.ERROR);
			}
			missionEntity = SDRC_SpawnHelper.SpawnItem(pos, resourceName, 0, -1);
			if (missionEntity)
			{		
				GetGame().GetCallqueue().CallLater(SetChapterParameters_Delayed, 2000, false, missionEntity);
				m_State = DC_EStoryState.CHAPTER_INIT;
			}
			else
			{
				SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Could not spawn: " + resourceName, LogLevel.ERROR);
				m_State = DC_EStoryState.ERROR;
			}
		}
		
		if (m_State == DC_EStoryState.CHAPTER_ACTIVE)
		{	
			if (stat)
			{
				SDRC_RplStoryComp storyComp = SDRC_RplStoryComp.GetInstance();
				if (storyComp)
				{
					storyComp.UpdateTime(100);
				}
				
				SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] id: " + stat.id + " - " + SCR_Enum.GetEnumName(DC_EMissionState, stat.state) + " - " + SCR_Enum.GetEnumName(DC_EMissionSuccess, stat.success));
				
				if (stat.state == DC_EMissionState.ACTIVE || stat.state == DC_EMissionState.END || stat.state == DC_EMissionState.EXIT)
				{
					m_State = DC_EStoryState.CHAPTER_DONE;
				}
				
				if (stat.state == DC_EMissionState.FAILED)
				{
					SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Chapter failed to start. Mission: " + stat.id + ". Retrying.", LogLevel.WARNING);
					//Let's try again
					m_State = DC_EStoryState.CHAPTER_START;
				}
			}			
		}

		if (m_State == DC_EStoryState.CHAPTER_DONE)
		{
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Chapter done.", LogLevel.NORMAL);
			
			if (stat)
			{	
				SDRC_RplStoryComp storyComp = SDRC_RplStoryComp.GetInstance();
				m_Chapter.success = stat.success;
				
				if (m_Chapter.success == DC_EMissionSuccess.WIN)
				{					
					SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Mission: " + stat.id + " : WIN", LogLevel.NORMAL);
					if (storyComp)
					{
						storyComp.UpdateChapter(m_Chapter.title, m_Chapter.textWin);
					}
					m_State = DC_EStoryState.CHAPTER_OVER;
				}
				
				if (m_Chapter.success == DC_EMissionSuccess.LOSE)
				{
					SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Mission: " + stat.id + " : FAILURE", LogLevel.NORMAL);
					if (storyComp)
					{
						storyComp.UpdateChapter(m_Chapter.title, m_Chapter.textLose);
					}
					m_State = DC_EStoryState.CHAPTER_OVER;
				}
			}
		}
				
		if (m_State == DC_EStoryState.CHAPTER_OVER)
		{
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Chapter over.", LogLevel.DEBUG);				
			GetGame().GetCallqueue().CallLater(StartNewChapter, m_Config.chapterTimeBetween*1000, false);
		}
		
		GetGame().GetCallqueue().CallLater(StoriesCycleManager, m_Config.storiesFrameCycleTime*1000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Delayed parameter setting. This is to make sure the mission entity is properly initialized.
	
	This equals to CHAPTER_INIT state. Once all is fine, we go to CHAPTER_ACTIVE.
	*/
	protected void SetChapterParameters_Delayed(IEntity missionEntity)
	{
		SDRC_DarcMissionGM ent = SDRC_DarcMissionGM.Cast(missionEntity);
		if (ent)
		{
			SDRC_DarcMissionEditableRequestComp requestComp = SDRC_DarcMissionEditableRequestComp.Cast(ent.FindComponent(SDRC_DarcMissionEditableRequestComp));
			requestComp.general = m_Chapter.general;
			m_RequestId = GetRequestId();
			requestComp.SetRequestId(m_RequestId);
			requestComp.SetMissionType(m_Chapter.missionType);
			requestComp.SetSubIdx(m_Chapter.subIdx);
			
			SDRC_RplStoryComp storyComp = SDRC_RplStoryComp.GetInstance();
			if (storyComp)
			{
				storyComp.UpdateChapter(m_Chapter.title, m_Chapter.text);
			}
			
			SDRC_Log.Add("[SDRC_StoriesFrame:SetChapterParameters_Delayed] Chapter set ACTIVE.", LogLevel.DEBUG);			
			m_State = DC_EStoryState.CHAPTER_ACTIVE;
		}	
		else
		{
			m_State = DC_EStoryState.ERROR;
		}	
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Starts a new chapter
	*/		
	protected void StartNewChapter()
	{
		//TBD: Select the proper chapter
		int oldIndex = m_Story.index;
		
		if (m_Chapter.success == DC_EMissionSuccess.WIN)
		{
			m_Story.index = m_Chapter.nextChapter[0];
		}
		
		if (m_Chapter.success == DC_EMissionSuccess.LOSE)
		{
			m_Story.index = m_Chapter.nextChapter[1];
		}		
		
		m_State = DC_EStoryState.CHAPTER_START;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Provides a request id keeping it unique for each chapter.
	*/		
	protected int GetRequestId()
	{
		m_RequestIdCounter++;
		return m_RequestIdCounter;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Creates the mission object
	*/		
/*	protected SDRC_Mission StoriesCreate(DC_EMissionType missionType)
	{		
		SDRC_Log.Add("[SDRC_StoriesFrame:MissionCreate] Starting mission of type: " + SCR_Enum.GetEnumName(DC_EMissionType, missionType), LogLevel.DEBUG);
	}*/
}