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
	private bool m_StoryActive;			//true if a story has been started
	private bool m_ChapterActive;		//true if a chapter inside a story has started
	private int m_RequestId;
	
//	private string m_sWorldName;
	
	//------------------------------------------------------------------------------------------------
	void SDRC_StoriesFrame()
	{
		SDRC_Log.Add("[SDRC_StoriesFrame] Starting SDRC_StoriesFrame", LogLevel.NORMAL);
		s_Instance = this;
				
//		m_sWorldName = SDRC_Misc.GetWorldName(true);

		//Load configuration from file
		m_DC_StoriesFrameJsonApi.Load();
		m_Config = m_DC_StoriesFrameJsonApi.conf;
		
		m_DC_StoriesFrameJsonApi.LoadStories();

		//Reset
		m_StoryIdx = 0;
		m_StoryActive = false;
		m_ChapterActive = false;
		m_RequestId = -1;
				
		//Fix seconds to ms
		SDRC_Log.Add("[SDRC_StoriesFrame] Waiting for " + m_Config.storiesStartDelay + " seconds before starting stories.", LogLevel.NORMAL);
		m_Config.storiesStartDelay = m_Config.storiesStartDelay * 1000;		//sec to ms
		
		//Start the mission framework.
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
		if (!m_StoryActive)
		{	
			m_ChapterActive = false;
			m_RequestId = -1;
			m_Story = m_Config.stories[m_StoryIdx];
			m_Chapter = m_Story.chapters[m_Story.index];
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Starting story: " + m_StoryIdx + " , chapter: " + m_Chapter.general.title, LogLevel.NORMAL);
			
			m_StoryActive = true;
		}		
		
		if (m_ChapterActive)
		{	
			//Search if the mission is active
			SDRC_MissionStat stat = SDRC_MissionStats.GetStat(m_RequestId);
			
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
					if (stat.success == DC_EMissionSuccess.WIN)
					{
						SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Mission: " + stat.id + " : WIN");
					}
					
					if (stat.success == DC_EMissionSuccess.LOSE)
					{
						SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Mission: " + stat.id + " : FAILURE");
					}
				}
				
				if (stat.state == DC_EMissionState.FAILED)
				{
					SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Mission: " + stat.id + " failed to start.");
					//Let's try again
					m_ChapterActive = false;
				}
			}			
		}
		
		if (!m_ChapterActive)
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
				GetGame().GetCallqueue().CallLater(SetMissionParameters_Delayed, 2000, false, missionEntity);
				m_ChapterActive = true;
			}
			else
			{
				SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Could not spawn: " + resourceName, LogLevel.ERROR);
			}
		}
		
		GetGame().GetCallqueue().CallLater(StoriesCycleManager, m_Config.storiesFrameCycleTime*1000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SetMissionParameters_Delayed(IEntity missionEntity)
	{
		SDRC_DarcMissionGM ent = SDRC_DarcMissionGM.Cast(missionEntity);
		if (ent)
		{
			SDRC_DarcMissionEditableRequestComp requestComp = SDRC_DarcMissionEditableRequestComp.Cast(ent.FindComponent(SDRC_DarcMissionEditableRequestComp));
//			requestComp.SetMissionSubIdx(m_Chapter.subIdx);
			requestComp.general = m_Chapter.general;
//			requestComp.general.subIdx = m_Chapter.subIdx;
			m_RequestId = GetRequestId();
			requestComp.SetRequestId(m_RequestId);
			requestComp.SetMissionType(m_Chapter.missionType);
			requestComp.SetSubIdx(m_Chapter.subIdx);
			
			SDRC_RplStoryComp storyComp = SDRC_RplStoryComp.GetInstance();
			if (storyComp)
			{
				storyComp.UpdateChapter(m_Chapter.title, m_Chapter.text);
			}						
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