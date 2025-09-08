//------------------------------------------------------------------------------------------------
/*!
This is the mission story framework file. 
*/

//------------------------------------------------------------------------------------------------
//const string DCS_ID_PREFIX = "DCS_";				//The prefix used for stories marker and missions Id's.

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
		m_StoryIdx = 0;
		m_RequestId = -1;

		//Fix seconds to ms
		SDRC_Log.Add("[SDRC_StoriesFrame] Waiting for " + m_Config.storiesStartDelay + " seconds before starting stories.", LogLevel.NORMAL);
		m_Config.storiesStartDelay = m_Config.storiesStartDelay * 1000;		//sec to ms
		
		//Start the mission framework.
		m_State = DC_EStoryState.STORY_START;
		GetGame().GetCallqueue().CallLater(StoriesCycleManager, m_Config.storiesStartDelay, false);
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

			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Starting story: " + m_StoryIdx + " : " + m_Story.title, LogLevel.NORMAL);

			//TBD: Error checking
			SDRC_StoriesHelper.CheckStory(m_Story);
						
			m_State = DC_EStoryState.CHAPTER_START;
		}		
		
		if (m_State == DC_EStoryState.CHAPTER_START)
		{		
			IEntity missionEntity;
			vector pos = "0 0 0";

			int cidx = m_Story.GetIndex(m_Story.chapterId);
			
			if ( (cidx >= 0) && (m_Story.chapters.Count() > cidx) )
			{
				m_Chapter = m_Story.chapters[cidx];
				//TBD: Error checking
				SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Spawning new story chapter: " + cidx + " , chapter: " + m_Chapter.general.title, LogLevel.NORMAL);
				
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
			else
			{
				SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Wrong chapterId (" + cidx + ") in story : " + m_Story.id, LogLevel.ERROR);
				m_State = DC_EStoryState.ERROR;
			}
		}
		
		if (m_State == DC_EStoryState.CHAPTER_PREACTIVE)
		{	
			if (stat)
			{
				if (stat.state == DC_EMissionState.FAILED)
				{
					SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Chapter failed to start. Mission: " + stat.id + ". Retrying.", LogLevel.WARNING);
					//Let's try again
					m_State = DC_EStoryState.CHAPTER_START;
				}
				else
				{
					SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Chapter set ACTIVE.", LogLevel.DEBUG);
					m_State = DC_EStoryState.CHAPTER_ACTIVE;
				}				
			}
		}
		
		if (m_State == DC_EStoryState.CHAPTER_ACTIVE)
		{	
			if (stat)
			{
				//Just for RPL testing.
				//TBD: The real functionality is not there yet.
				SDRC_RplStoryComp storyComp = SDRC_RplStoryComp.GetInstance();
				if (storyComp)
				{
					storyComp.UpdateTime(100);
				}
				
				SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] id: " + stat.id + " - " + SCR_Enum.GetEnumName(DC_EMissionState, stat.state) + " - " + SCR_Enum.GetEnumName(DC_EMissionSuccess, stat.success), LogLevel.DEBUG);
				
				//While mission is ACTIVE, check if SUCCESS has changed. If yes, the chapter is done.
				if (stat.state == DC_EMissionState.ACTIVE)
				{
					if (stat)
					{	
						if (stat.success != DC_EMissionSuccess.UNKNOWN)
						{
							m_State = DC_EStoryState.CHAPTER_DONE;
						}
					}
				}
				
				//If mission reaches END or EXIT STATE, the chapter is done
				if (stat.state == DC_EMissionState.END || stat.state == DC_EMissionState.EXIT)
				{
					m_State = DC_EStoryState.CHAPTER_DONE;
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
				
				if (m_Chapter.success == DC_EMissionSuccess.DELETED)
				{					
					SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Mission: " + stat.id + " : DELETED", LogLevel.WARNING);
					m_State = DC_EStoryState.ERROR;	//TBD: For now we go to error state
				}
				
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
			
			SDRC_Log.Add("[SDRC_StoriesFrame:SetChapterParameters_Delayed] Chapter set PREACTIVE.", LogLevel.DEBUG);
			m_State = DC_EStoryState.CHAPTER_PREACTIVE;
		}	
		else
		{
			SDRC_Log.Add("[SDRC_StoriesFrame:SetChapterParameters_Delayed] Mission entity not found.", LogLevel.DEBUG);
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
		int oldIndex = m_Story.chapterId;
		
		if (m_Chapter.success == DC_EMissionSuccess.WIN)
		{
			m_Story.chapterId = m_Chapter.nextChapter[0];
		}
		
		if (m_Chapter.success == DC_EMissionSuccess.LOSE)
		{
			m_Story.chapterId = m_Chapter.nextChapter[1];
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