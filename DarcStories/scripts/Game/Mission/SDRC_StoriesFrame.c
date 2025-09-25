//------------------------------------------------------------------------------------------------
/*!
This is the mission story framework file. 
*/

const string DC_MISSIONCONFIG_FILE_STORIES = "dc_storiesConfig.json";

//------------------------------------------------------------------------------------------------
enum SDRC_EStoriesFrameState
{
	ERROR = -1,
	NONE = 0,			//Unknown state. Nothing should be run at this state.
	INIT,				//Story frame is being init
	ACTIVE,
}

//------------------------------------------------------------------------------------------------
class SDRC_StoriesFrame
{
	private static SDRC_StoriesFrame s_Instance;		
	private static int m_RequestIdCounter = 1000;
	
	private SDRC_EStoriesFrameState m_State;
	private ref SDRC_StoriesFrameJsonApi m_DC_StoriesFrameJsonApi = new SDRC_StoriesFrameJsonApi(DC_MISSIONCONFIG_FILE_STORIES);
	ref SDRC_StoriesFrameConfig m_Config;
	private int m_StoryIdx;
	private ref array<ref SDRC_StoryFSM> m_StoriesRunning = {};
//	private string m_sWorldName;
	
	//------------------------------------------------------------------------------------------------
	void SDRC_StoriesFrame()
	{
		SDRC_Log.Add("[SDRC_StoriesFrame] Starting SDRC_StoriesFrame", LogLevel.NORMAL);
		s_Instance = this;
				
//		m_sWorldName = SDRC_Misc.GetWorldName(true);

		//Load configuration from file
		m_DC_StoriesFrameJsonApi.CreateStories();
		bool success = m_DC_StoriesFrameJsonApi.Load();
		
		if (!success)
		{
			SDRC_Log.Add("[SDRC_StoriesFrame] Error loading " + DC_MISSIONCONFIG_FILE_STORIES + ". SDRC_StoriesFrame not started.", LogLevel.ERROR);
			m_State = SDRC_EStoriesFrameState.ERROR;
			return;
		}
		
		m_Config = m_DC_StoriesFrameJsonApi.conf;
		m_StoryIdx = 0;
		
		//Fix seconds to ms
		SDRC_Log.Add("[SDRC_StoriesFrame] Waiting for " + m_Config.storiesStartDelay + " seconds before starting stories.", LogLevel.NORMAL);
		m_Config.storiesStartDelay = m_Config.storiesStartDelay * 1000;		//sec to ms
		
		//Start the mission framework.
		m_State = SDRC_EStoriesFrameState.INIT;
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
		if (m_State == SDRC_EStoriesFrameState.ERROR)
		{
			SDRC_Log.Add("[SDRC_StoriesFrame] We're in an error state. To be fixed...", LogLevel.ERROR);
		}
		
		if (m_State == SDRC_EStoriesFrameState.INIT)
		{
			string fileName = m_Config.storiesList[m_StoryIdx];
			
			ref SDRC_StoryFSM story = new SDRC_StoryFSM(fileName);
			if (story.GetState() != SDRC_EStoryState.ERROR)
			{
				m_StoriesRunning.Insert(story);
				m_State = SDRC_EStoriesFrameState.ACTIVE;
			}
			else
			{
				SDRC_Log.Add("[SDRC_StoriesFrame] Error starting story: " + fileName, LogLevel.ERROR);
				delete story;
				m_State = SDRC_EStoriesFrameState.ERROR;
			}
		}
		
		if (m_State == SDRC_EStoriesFrameState.ACTIVE)
		{
			SDRC_Log.Add("[SDRC_StoriesFrame] Running StoriesCycleManager", LogLevel.SPAM);
			StoriesDump();
		}
		
		GetGame().GetCallqueue().CallLater(StoriesCycleManager, m_Config.storiesFrameCycleTime*1000, false);					
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Provides a request id keeping it unique for each chapter.
	*/		
	int GetRequestId()
	{
		m_RequestIdCounter++;
		return m_RequestIdCounter;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Dumps the current mission details to log.
	*/	
	protected void StoriesDump()
	{
		int i = 0;
		int aiCount = 0;
		int cutLen = 32;
		
		if (m_StoriesRunning.Count() == 0)
		{
			return;
		}

		SDRC_Log.Add("[SDRC_StoriesDump] -- Stories -------------------------------------------------------------------", LogLevel.NORMAL);
		foreach (SDRC_StoryFSM story : m_StoriesRunning)
		{
			SDRC_Log.Add("[SDRC_StoriesDump] " + i + ": " + story.GetStoryTitle() + " : " + story.GetChapterTitle() + " : " + SCR_Enum.GetEnumName(SDRC_EStoryState, story.GetState()), LogLevel.NORMAL);
			i++;
		}		
		string lastLine = "[SDRC_StoriesDump] ---------------------------------------------------------------------------------";
		lastLine = lastLine.Substring(0, 100);
		SDRC_Log.Add(lastLine, LogLevel.NORMAL);
	}
}