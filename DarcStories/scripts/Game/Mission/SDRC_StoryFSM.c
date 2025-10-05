//------------------------------------------------------------------------------------------------
/*!
This is the mission story framework file. 
*/

//------------------------------------------------------------------------------------------------
class SDRC_StoryFSM
{
	private static SDRC_StoryFSM s_Instance;		
	private SDRC_StoriesFrame m_StoriesFrame;
	
	private int m_RequestId;
	private string m_StoryFileName;
	private ref SDRC_Story m_Story;
	private SDRC_EStoryState m_State;
	private ref SDRC_Chapter m_Chapter = new SDRC_Chapter();
	private int m_ChapterWaitStart = 0;
	
	//------------------------------------------------------------------------------------------------
	void SDRC_StoryFSM(string fileName)
	{
		SDRC_Log.Add("[SDRC_StoryFSM] Starting SDRC_StoryFSM", LogLevel.NORMAL);
		s_Instance = this;
		SetState(SDRC_EStoryState.INIT);
		m_StoryFileName = fileName;
				
		m_StoriesFrame = SDRC_StoriesFrame.GetInstance();
		
		if (!m_StoriesFrame)
		{
			SetState(SDRC_EStoryState.ERROR, SDRC_EStoryError.STORYFRAME_NOT_FOUND, "Story not started: " + m_StoryFileName);
			return;
		}

		m_RequestId = -1;	//m_StoriesFrame.GetRequestId();
				
		//Start the mission framework.
		SetState(SDRC_EStoryState.STORY_WAITING);
		GetGame().GetCallqueue().CallLater(StoryFSM, 2000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	static SDRC_StoryFSM GetInstance()
	{
		return s_Instance;		
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	End the mission framework. Clean up the running missions.
	*/	
	void ~SDRC_StoryFSM()
	{
		//Do clean up, save, etc ...
		SDRC_Log.Add("[~SDRC_StoryFSM] Stopping SDRC_StoryFSM", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Mission life cycle manager.
	*/
	protected void StoryFSM()
	{	
		if (m_State == SDRC_EStoryState.ERROR)
		{
			SDRC_Log.Add("[SDRC_StoryFSM:StoryFSM] We're in an error state. To be fixed...", LogLevel.ERROR);
		}
		
		if (m_State == SDRC_EStoryState.STORY_WAITING)
		{	
			SDRC_Log.Add("[SDRC_StoryFSM:StoryFSM] Waiting for story to begin.", LogLevel.DEBUG);							
			SetState(SDRC_EStoryState.STORY_START);
		}
		
		if (m_State == SDRC_EStoryState.STORY_START)
		{	
			LoadStory(m_StoryFileName);
			
			if (!m_Story)
			{
				SetState(SDRC_EStoryState.ERROR, SDRC_EStoryError.STORY_LOAD_FAILED, "Load failed: " + m_StoryFileName);
			}
			else
			{
				SDRC_Log.Add("[SDRC_StoryFSM:StoryFSM] Starting story: " + m_Story.title, LogLevel.NORMAL);
	
				//TBD: Error checking
				SDRC_StoriesHelper.CheckStory(m_Story);
							
				SetState(SDRC_EStoryState.CHAPTER_START);
			}
		}		
		
		if (m_State == SDRC_EStoryState.CHAPTER_START)
		{		
			IEntity missionEntity;
			vector pos = "0 0 0";

			int cidx = m_Story.GetIndex(m_Story.chapterId);
			
			if ( (cidx >= 0) && (m_Story.chapters.Count() > cidx) )
			{
				m_Chapter = m_Story.chapters[cidx];
				m_Chapter.success == SDRC_EMissionSuccess.UNKNOWN;
				m_RequestId = m_StoriesFrame.GetRequestId();
				
				//TBD: Error checking
				SDRC_Log.Add("[SDRC_StoryFSM:StoryFSM] Spawning new story chapter: " + m_Story.chapterId + " , chapter: " + m_Chapter.general.title, LogLevel.NORMAL);
				
				string resourceName = SDRC_MissionEnumHelper.GetMissionPrefab(m_Chapter.missionType);
				if (resourceName == "")
				{
					SetState(SDRC_EStoryState.ERROR, SDRC_EStoryError.INVALID_RESOURCE_NAME, "Incorrect chapter missiontype: " + m_Chapter.missionType);
				}
				missionEntity = SDRC_SpawnHelper.SpawnItem(pos, resourceName, 0, -1);
				if (missionEntity)
				{		
					GetGame().GetCallqueue().CallLater(SetChapterParameters_Delayed, 2000, false, missionEntity);
					SetState(SDRC_EStoryState.CHAPTER_INIT);
				}
				else
				{
					SetState(SDRC_EStoryState.ERROR, SDRC_EStoryError.SPAWN_FAILED, "Could not spawn: " + resourceName);
				}
			}
			else
			{
				SetState(SDRC_EStoryState.ERROR, SDRC_EStoryError.CHAPTER_WRONG_ID, "Wrong chapterId (" + cidx + ") in story : " + m_Story.id);
			}
		}
		
		//Search if the mission is active
		SDRC_MissionStat stat = SDRC_MissionStats.GetStat(m_RequestId);
		
		if (m_State == SDRC_EStoryState.CHAPTER_WAITING)
		{	
			if (stat)
			{
				if (stat.state == SDRC_EMissionState.FAILED)
				{
					SDRC_Log.Add("[SDRC_StoryFSM:StoryFSM] Chapter failed to start. Mission: " + stat.id + ". Retrying.", LogLevel.WARNING);
					//Let's try again
					SetState(SDRC_EStoryState.CHAPTER_START);
				}
				else	//All good, start the chapter
				{
					int currentTime = (System.GetTickCount() / 1000);					
					m_Chapter.chapterEndTime = m_Chapter.activeTime + currentTime;
					
					SDRC_RplStoryComp storyComp = SDRC_RplStoryComp.GetInstance();
					if (storyComp)
					{
						storyComp.UpdateBrief(m_Story.title);
					}
					
					SetState(SDRC_EStoryState.CHAPTER_ACTIVE);
				}				
			}
		}
		
		if (m_State == SDRC_EStoryState.CHAPTER_ACTIVE)
		{	
			if (stat)
			{
				//Send the RPL information to players about mission timing etc. 
				SDRC_RplStoryComp storyComp = SDRC_RplStoryComp.GetInstance();
				if (storyComp)
				{
					int currentTime = (System.GetTickCount() / 1000);					
					int timeLeft = m_Chapter.chapterEndTime - currentTime;
					if (timeLeft < 0)
					{
						timeLeft = 0;
					}
					float timeLeftPercent = (timeLeft/m_Chapter.activeTime)*100;
					storyComp.UpdateTime((int)timeLeftPercent);
					storyComp.UpdateChapter(m_Chapter.title, m_Chapter.text, SCR_Enum.GetEnumName(SDRC_EMissionSuccess, m_Chapter.success));
					
					//End the missions					
					if (timeLeft == 0)
					{
						SDRC_RplGMComp gmComp = SDRC_RplGMComp.FindInstance();
						if (gmComp)
						{
							gmComp.DoEndMission(-1, stat.id);
						}
					}
				}
				
				SDRC_Log.Add("[SDRC_StoryFSM:StoryFSM] id: " + stat.id + " - " + SCR_Enum.GetEnumName(SDRC_EMissionState, stat.state) + " - " + SCR_Enum.GetEnumName(SDRC_EMissionSuccess, stat.success), LogLevel.SPAM);
				
				//While mission is ACTIVE, check if SUCCESS has changed. If yes, the chapter is done.
				if (stat.state == SDRC_EMissionState.ACTIVE)
				{
					if (stat)
					{	
						if (stat.success != SDRC_EMissionSuccess.UNKNOWN)
						{
							SetState(SDRC_EStoryState.CHAPTER_DONE);
						}
					}
				}
				
				//If mission reaches END or EXIT STATE, the chapter is done
				if (stat.state == SDRC_EMissionState.END || stat.state == SDRC_EMissionState.EXIT)
				{
					SetState(SDRC_EStoryState.CHAPTER_DONE);
				}
			}			
		}

		if (m_State == SDRC_EStoryState.CHAPTER_DONE)
		{
			SDRC_Log.Add("[SDRC_StoryFSM:StoryFSM] Chapter done.", LogLevel.SPAM);
			
			if (stat)
			{	
				SDRC_RplStoryComp storyComp = SDRC_RplStoryComp.GetInstance();
				m_Chapter.success = stat.success;
				
				if (m_Chapter.success == SDRC_EMissionSuccess.DELETED)
				{					
					//TBD: If mission was deleted, go the LOSE state. Not sure if this is the best way to handle this.
					//SetState(SDRC_EStoryState.ERROR, SDRC_EStoryError.MISSION_DELETED, "Mission: " + stat.id + " : DELETED");
					SDRC_Log.Add("[SDRC_StoryFSM:StoryFSM] Mission: " + stat.id + " : DELETED. Setting mission state to LOSE.", LogLevel.ERROR);
					m_Chapter.success = SDRC_EMissionSuccess.LOSE;
				}
				
				if (m_Chapter.success == SDRC_EMissionSuccess.WIN)
				{					
					SDRC_Log.Add("[SDRC_StoryFSM:StoryFSM] Mission: " + stat.id + " : WIN", LogLevel.NORMAL);
					if (storyComp)
					{
						storyComp.UpdateChapter(m_Chapter.title, m_Chapter.textWin, SCR_Enum.GetEnumName(SDRC_EMissionSuccess, m_Chapter.success));
					}
					SetState(SDRC_EStoryState.CHAPTER_OVER);	//NOTE: SetState(CHAPTER_OVER) will do a delayed StartNewChapter()
				}
				
				if (m_Chapter.success == SDRC_EMissionSuccess.LOSE)
				{
					SDRC_Log.Add("[SDRC_StoryFSM:StoryFSM] Mission: " + stat.id + " : FAILURE", LogLevel.NORMAL);
					if (storyComp)
					{
						storyComp.UpdateChapter(m_Chapter.title, m_Chapter.textLose, SCR_Enum.GetEnumName(SDRC_EMissionSuccess, m_Chapter.success));
					}
					SetState(SDRC_EStoryState.CHAPTER_OVER);	//NOTE: SetState(CHAPTER_OVER) will do a delayed StartNewChapter()
				}
			}
		}
				
		if (m_State == SDRC_EStoryState.CHAPTER_OVER)
		{
			//NOTE: SetState(CHAPTER_OVER) has enabled a delayed StartNewChapter()
			SDRC_Log.Add("[SDRC_StoryFSM:StoryFSM] Chapter over. Next in " + ((m_ChapterWaitStart + m_StoriesFrame.m_Config.chapterTimeBetween) - (System.GetTickCount() / 1000)) + " seconds.", LogLevel.DEBUG);
		}
		
		if (m_State == SDRC_EStoryState.STORY_END)
		{
			SDRC_Log.Add("[SDRC_StoryFSM:StoryFSM] Story over.", LogLevel.DEBUG);
		}
		
		GetGame().GetCallqueue().CallLater(StoryFSM, m_StoriesFrame.m_Config.storyCycleTime*1000, false);
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
			requestComp.SetRequestId(m_RequestId);
			requestComp.SetMissionType(m_Chapter.missionType);
			requestComp.SetSubIdx(m_Chapter.subIdx);
			
			SetState(SDRC_EStoryState.CHAPTER_WAITING);
		}	
		else
		{
			SDRC_Log.Add("[SDRC_StoryFSM:SetChapterParameters_Delayed] Mission entity not found.", LogLevel.ERROR);
			SetState(SDRC_EStoryState.ERROR);
		}	
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Starts a new chapter
	*/		
	protected void StartNewChapter()
	{
		if (m_Chapter.success == SDRC_EMissionSuccess.WIN)
		{
			m_Story.chapterId = m_Chapter.nextChapter[0];
		}
		
		if (m_Chapter.success == SDRC_EMissionSuccess.LOSE)
		{
			m_Story.chapterId = m_Chapter.nextChapter[1];
		}		
		
		if (m_Story.chapterId == SDRC_ENextChapter.WIN || m_Story.chapterId == SDRC_ENextChapter.LOSE)
		{
			GetGame().GetCallqueue().CallLater(StartNewStory, m_StoriesFrame.m_Config.storyTimeBetween, false);
			SetState(SDRC_EStoryState.STORY_END);
		}
		else
		{
			SetState(SDRC_EStoryState.CHAPTER_START);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void StartNewStory()
	{
		SetState(SDRC_EStoryState.STORY_WAITING);
	}
	
	//------------------------------------------------------------------------------------------------	
	SDRC_EStoryState GetState()
	{
		return m_State;
	}
	
	protected void SetState(SDRC_EStoryState state, SDRC_EStoryError errorReason = SDRC_EStoryError.NONE, string errorInfo = "")	
	{	
		m_State = state;

		SDRC_Log.Add("[SDRC_StoryFSM:SetState] Chapter set to state: " + SCR_Enum.GetEnumName(SDRC_EStoryState, state), LogLevel.DEBUG);
		
		//If chapter is over, 		
		if (m_State == SDRC_EStoryState.CHAPTER_OVER)
		{
			m_ChapterWaitStart = (System.GetTickCount() / 1000);
			GetGame().GetCallqueue().CallLater(StartNewChapter, m_StoriesFrame.m_Config.chapterTimeBetween*1000, false);
		}
		
		if (m_State == SDRC_EStoryState.ERROR)
		{
			if (errorReason != SDRC_EStoryError.NONE)
			{
				SDRC_Log.Add("[SDRC_StoryFSM:SetState] ERROR: " + SCR_Enum.GetEnumName(SDRC_EStoryError, errorReason) + " " + errorInfo, LogLevel.ERROR);						
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	string GetStoryTitle()
	{
		if (m_Story)
		{
			return m_Story.title;
		}
		return "";
	}
	
	string GetChapterTitle()
	{
		if (m_Story)
		{
			return m_Chapter.title;
		}
		return "";
	}
	
	//------------------------------------------------------------------------------------------------
	void LoadStory(string fileName)
	{			
		SDRC_StoryJsonApi story_JsonApi = new SDRC_StoryJsonApi(fileName);		
		story_JsonApi.Load(false);
		m_Story = story_JsonApi.conf;		
	}	
}