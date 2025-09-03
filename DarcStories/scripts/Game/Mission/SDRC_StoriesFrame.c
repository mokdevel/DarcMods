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
		m_State = DC_EStoryState.STORY_WAITING;
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
			m_RequestId = -1;
			m_Story = m_Config.stories[m_StoryIdx];
			m_Chapter = m_Story.chapters[m_Story.index];
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Starting story: " + m_StoryIdx + " , chapter: " + m_Chapter.general.title, LogLevel.NORMAL);
			
			m_State = DC_EStoryState.CHAPTER_START;
		}		
		
		if (m_State == DC_EStoryState.CHAPTER_START)
		{		
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Spawning new story mission", LogLevel.NORMAL);
			IEntity missionEntity;
			vector pos = "0 0 0";
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
				
				if (stat.success == DC_EMissionSuccess.WIN)
				{					
					SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Mission: " + stat.id + " : WIN", LogLevel.NORMAL);
					if (storyComp)
					{
						storyComp.UpdateChapter(m_Chapter.title, m_Chapter.textWin);
					}
					m_State = DC_EStoryState.CHAPTER_OVER;
				}
				
				if (stat.success == DC_EMissionSuccess.LOSE)
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
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Chapter over. Go to next chapter or an ending. To be fixed...", LogLevel.WARNING);				
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