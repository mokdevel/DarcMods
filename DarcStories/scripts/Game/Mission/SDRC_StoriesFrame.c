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
		SetState(DC_EStoryState.INIT);
				
//		m_sWorldName = SDRC_Misc.GetWorldName(true);

		//Load configuration from file
		m_DC_StoriesFrameJsonApi.Load();
		m_Config = m_DC_StoriesFrameJsonApi.conf;
		
		m_DC_StoriesFrameJsonApi.CreateStories();

		//Reset
		m_StoryIdx = 1;
		m_RequestId = -1;

		//Fix seconds to ms
		SDRC_Log.Add("[SDRC_StoriesFrame] Waiting for " + m_Config.storiesStartDelay + " seconds before starting stories.", LogLevel.NORMAL);
		m_Config.storiesStartDelay = m_Config.storiesStartDelay * 1000;		//sec to ms
		
		//Start the mission framework.
		SetState(DC_EStoryState.STORY_WAITING);
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
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Waiting for story to begin.", LogLevel.DEBUG);							
			SetState(DC_EStoryState.STORY_START);
		}
		
		if (m_State == DC_EStoryState.STORY_START)
		{	
			m_RequestId = -1;
			
			string fileName = m_Config.storiesList[m_StoryIdx];
			m_DC_StoriesFrameJsonApi.LoadStory(fileName);
			
			if (!m_Config.story)
			{
				SetState(DC_EStoryState.ERROR, DC_EStoryError.STORY_LOAD_FAILED, "Load failed: " + fileName);
			}
			else
			{
				m_Story = m_Config.story;
	
				SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Starting story: " + m_StoryIdx + " : " + m_Story.title, LogLevel.NORMAL);
	
				//TBD: Error checking
				SDRC_StoriesHelper.CheckStory(m_Story);
							
				SetState(DC_EStoryState.CHAPTER_START);
			}
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
				SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Spawning new story chapter: " + m_Story.chapterId + " , chapter: " + m_Chapter.general.title, LogLevel.NORMAL);
				
				string resourceName = SDRC_MissionEnumHelper.GetMissionPrefab(m_Chapter.missionType);
				if (resourceName == "")
				{
					SetState(DC_EStoryState.ERROR, DC_EStoryError.INVALID_RESOURCE_NAME, "Incorrect chapter missiontype: " + m_Chapter.missionType);
				}
				missionEntity = SDRC_SpawnHelper.SpawnItem(pos, resourceName, 0, -1);
				if (missionEntity)
				{		
					GetGame().GetCallqueue().CallLater(SetChapterParameters_Delayed, 2000, false, missionEntity);
					SetState(DC_EStoryState.CHAPTER_INIT);
				}
				else
				{
					SetState(DC_EStoryState.ERROR, DC_EStoryError.SPAWN_FAILED, "Could not spawn: " + resourceName);
				}
			}
			else
			{
				SetState(DC_EStoryState.ERROR, DC_EStoryError.CHAPTER_WRONG_ID, "Wrong chapterId (" + cidx + ") in story : " + m_Story.id);
			}
		}
		
		if (m_State == DC_EStoryState.CHAPTER_WAITING)
		{	
			if (stat)
			{
				if (stat.state == DC_EMissionState.FAILED)
				{
					SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Chapter failed to start. Mission: " + stat.id + ". Retrying.", LogLevel.WARNING);
					//Let's try again
					SetState(DC_EStoryState.CHAPTER_START);
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
					
					SetState(DC_EStoryState.CHAPTER_ACTIVE);
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
					int currentTime = (System.GetTickCount() / 1000);					
					int timeLeft = m_Chapter.chapterEndTime - currentTime;
					if (timeLeft < 0)
					{
						timeLeft = 0;
					}
					float timeLeftPercent = (timeLeft/m_Chapter.activeTime)*100;
					storyComp.UpdateTime((int)timeLeftPercent);
					storyComp.UpdateChapter(m_Chapter.title, m_Chapter.text, SCR_Enum.GetEnumName(DC_EMissionSuccess, m_Chapter.success));
					
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
				
				SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] id: " + stat.id + " - " + SCR_Enum.GetEnumName(DC_EMissionState, stat.state) + " - " + SCR_Enum.GetEnumName(DC_EMissionSuccess, stat.success), LogLevel.DEBUG);
				
				//While mission is ACTIVE, check if SUCCESS has changed. If yes, the chapter is done.
				if (stat.state == DC_EMissionState.ACTIVE)
				{
					if (stat)
					{	
						if (stat.success != DC_EMissionSuccess.UNKNOWN)
						{
							SetState(DC_EStoryState.CHAPTER_DONE);
						}
					}
				}
				
				//If mission reaches END or EXIT STATE, the chapter is done
				if (stat.state == DC_EMissionState.END || stat.state == DC_EMissionState.EXIT)
				{
					SetState(DC_EStoryState.CHAPTER_DONE);
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
					//TBD: If mission was deleted, go the LOSE state. Not sure if this is the best way to handle this.
					//SetState(DC_EStoryState.ERROR, DC_EStoryError.MISSION_DELETED, "Mission: " + stat.id + " : DELETED");
					SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Mission: " + stat.id + " : DELETED. Setting mission state to LOSE.", LogLevel.ERROR);
					m_Chapter.success = DC_EMissionSuccess.LOSE;
				}
				
				if (m_Chapter.success == DC_EMissionSuccess.WIN)
				{					
					SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Mission: " + stat.id + " : WIN", LogLevel.NORMAL);
					if (storyComp)
					{
						storyComp.UpdateChapter(m_Chapter.title, m_Chapter.textWin, SCR_Enum.GetEnumName(DC_EMissionSuccess, m_Chapter.success));
					}
					SetState(DC_EStoryState.CHAPTER_OVER);	//NOTE: SetState(CHAPTER_OVER) will do a delayed StartNewChapter()
				}
				
				if (m_Chapter.success == DC_EMissionSuccess.LOSE)
				{
					SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Mission: " + stat.id + " : FAILURE", LogLevel.NORMAL);
					if (storyComp)
					{
						storyComp.UpdateChapter(m_Chapter.title, m_Chapter.textLose, SCR_Enum.GetEnumName(DC_EMissionSuccess, m_Chapter.success));
					}
					SetState(DC_EStoryState.CHAPTER_OVER);	//NOTE: SetState(CHAPTER_OVER) will do a delayed StartNewChapter()
				}
			}
		}
				
		if (m_State == DC_EStoryState.CHAPTER_OVER)
		{
			//NOTE: SetState(CHAPTER_OVER) has enabled a delayed StartNewChapter()
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Chapter over.", LogLevel.DEBUG);				
		}
		
		if (m_State == DC_EStoryState.STORY_END)
		{
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Story over.", LogLevel.DEBUG);				
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
			
			SetState(DC_EStoryState.CHAPTER_WAITING);
		}	
		else
		{
			SDRC_Log.Add("[SDRC_StoriesFrame:SetChapterParameters_Delayed] Mission entity not found.", LogLevel.ERROR);
			SetState(DC_EStoryState.ERROR);
		}	
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Starts a new chapter
	*/		
	protected void StartNewChapter()
	{
		if (m_Chapter.success == DC_EMissionSuccess.WIN)
		{
			m_Story.chapterId = m_Chapter.nextChapter[0];			
		}
		
		if (m_Chapter.success == DC_EMissionSuccess.LOSE)
		{
			m_Story.chapterId = m_Chapter.nextChapter[1];
		}		
		
		if (m_Story.chapterId == DC_ENextChapter.WIN || m_Story.chapterId == DC_ENextChapter.LOSE)
		{
			GetGame().GetCallqueue().CallLater(StartNewStory, m_Config.storyTimeBetween, false);
			SetState(DC_EStoryState.STORY_END);
		}
		else
		{
			SetState(DC_EStoryState.CHAPTER_START);
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	protected void StartNewStory()
	{
		SetState(DC_EStoryState.STORY_WAITING);
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
	protected void SetState(DC_EStoryState state, DC_EStoryError errorReason = DC_EStoryError.NONE, string errorInfo = "")	
	{	
		m_State = state;

		SDRC_Log.Add("[SDRC_StoriesFrame:SetState] Chapter set to state: " + SCR_Enum.GetEnumName(DC_EStoryState, state), LogLevel.DEBUG);
		
		//If chapter is over, 		
		if (m_State == DC_EStoryState.CHAPTER_OVER)
		{
			GetGame().GetCallqueue().CallLater(StartNewChapter, m_Config.chapterTimeBetween*1000, false);
		}
		
		if (m_State == DC_EStoryState.ERROR)
		{
			if (errorReason != DC_EStoryError.NONE)
			{
				SDRC_Log.Add("[SDRC_StoriesFrame:SetState] ERROR: " + SCR_Enum.GetEnumName(DC_EStoryError, errorReason) + " " + errorInfo, LogLevel.ERROR);						
			}
		}
	}
}