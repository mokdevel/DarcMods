//------------------------------------------------------------------------------------------------
/*!
This is the mission main framework file. 

Add this to your StartGameTrigger or use SDRC_GameCoreBase.c

	//------------------------------------------------------------------------------------------------
	// user script
	private bool m_GameHasStarted = false;
	private ref SDRC_StoriesFrame MissionFrame;

	override void EOnActivate(IEntity owner)
	{
		if (!m_GameHasStarted)
		{
			m_GameHasStarted = true;		
			MissionFrame = new SDRC_StoriesFrame(true);
			MissionFrame.MissionFrameStart();			
		}
	}
	//------------------------------------------------------------------------------------------------
*/

//------------------------------------------------------------------------------------------------
const string DCS_ID_PREFIX = "DCS_";				//The prefix used for stories marker and missions Id's.

//------------------------------------------------------------------------------------------------
class SDRC_StoriesFrame
{
	protected static SDRC_StoriesFrame s_Instance;		
//	ref array<ref SDRC_Mission> m_MissionList = new array<ref SDRC_Mission>;
	ref SDRC_StoriesFrameJsonApi m_DC_StoriesFrameJsonApi = new SDRC_StoriesFrameJsonApi();
	ref SDRC_StoriesFrameConfig m_Config;
	ref SDRC_Story m_Story;
	ref SDRC_Chapter m_Chapter;
	
	int m_StoryIdx;
	
	private string m_sWorldName;
	
//	private int m_iMissionCountDynamicMax;				//Max amount of dynamic missions
//	private int m_iMissionCountStaticMax;				//Max amount of static missions
	
	//------------------------------------------------------------------------------------------------
	void SDRC_StoriesFrame()
	{
		SDRC_Log.Add("[SDRC_StoriesFrame] Starting SDRC_StoriesFrame", LogLevel.NORMAL);
		s_Instance = this;
				
		m_sWorldName = SDRC_Misc.GetWorldName(true);

		//Load configuration from file
		m_DC_StoriesFrameJsonApi.Load();
		m_Config = m_DC_StoriesFrameJsonApi.conf;
		
		m_DC_StoriesFrameJsonApi.LoadStories();
		
		m_StoryIdx = 0;
		m_Story = m_Config.stories[m_StoryIdx];
		m_Chapter = m_Story.chapters[m_Story.index];
		SDRC_Log.Add("[SDRC_StoriesFrame] Starting story: " + m_StoryIdx + " , chapter: " + m_Chapter.general.title, LogLevel.NORMAL);
		
		//Fix seconds to ms
		SDRC_Log.Add("[SDRC_StoriesFrame] Waiting for " + m_Config.storiesStartDelay + " seconds before spawning stories.", LogLevel.NORMAL);
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
		}
		else
		{
			SDRC_Log.Add("[SDRC_StoriesFrame:StoriesCycleManager] Could not spawn: " + resourceName, LogLevel.ERROR);
		}

//		GetGame().GetCallqueue().CallLater(MissionCycleManager, m_Config.storiesFrameCycleTime*1000, false);
	}
	
	protected void SetMissionParameters_Delayed(IEntity missionEntity)
	{
		SDRC_DarcMissionGM ent = SDRC_DarcMissionGM.Cast(missionEntity);
		if (ent)
		{
			SDRC_DarcMissionRequestComp requestComp = SDRC_DarcMissionRequestComp.Cast(ent.FindComponent(SDRC_DarcMissionRequestComp));
			requestComp.SetMissionSubIdx(m_Chapter.subIdx);
			requestComp.general = m_Chapter.general;
			requestComp.general.suid = 1000 + m_StoryIdx * 100 + m_Story.index;
		}		
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