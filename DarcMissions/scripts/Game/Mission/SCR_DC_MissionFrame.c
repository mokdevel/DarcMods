//------------------------------------------------------------------------------------------------
/*!
This is the mission main framework file. 

Add this to your StartGameTrigger or use SCR_DC_GameCoreBase.c

	//------------------------------------------------------------------------------------------------
	// user script
	private bool m_GameHasStarted = false;
	private ref SCR_DC_MissionFrame MissionFrame;

	override void EOnActivate(IEntity owner)
	{
		if (!m_GameHasStarted)
		{
			m_GameHasStarted = true;		
			MissionFrame = new SCR_DC_MissionFrame(true);
			MissionFrame.MissionFrameStart();			
		}
	}
	//------------------------------------------------------------------------------------------------
*/

enum DC_EMissionType
{
	NONE,
	HUNTER,
	OCCUPATION,
	BANDITCAMP		//TBD
};

const string DC_ID_PREFIX = "DCM_";				//The prefix used for marker and missions Id's.
const int DC_MISSION_ACTIVE_DISTANCE = 300;		//The distance to a player to keep the mission active 
const int DC_MISSION_ACTIVE_TIME = 3;			//Minutes to keep the mission active

//------------------------------------------------------------------------------------------------
class SCR_DC_MissionFrame
{
	ref array<ref SCR_DC_Mission> m_MissionList = new array<ref SCR_DC_Mission>();
	ref SCR_DC_MissionFrameJsonApi m_DC_MissionFrameJsonApi = new SCR_DC_MissionFrameJsonApi();
	ref SCR_DC_MissionFrameConfig m_Config;
	
	//------------------------------------------------------------------------------------------------
	void SCR_DC_MissionFrame()
	{
		SCR_DC_Log.Add("[SCR_DC_MissionFrame] Starting SCR_DC_MissionFrame", LogLevel.NORMAL);
//		SCR_DC_Log.Add("[SCR_DC_MissionFrame] Worldname" + worldName, LogLevel.NORMAL);

		//TBD: For some reason the SCR_DC_Core.RELEASE seems not to be defined when we come here. It is safer to delete the conf files manually to create them.
		
		//Load configuration from file		
		m_DC_MissionFrameJsonApi.Load();
		
		SCR_DC_Log.SetLogLevel(m_Config.logLevel);
		if (!SCR_DC_Core.RELEASE)
		{
			SCR_DC_Log.SetLogLevel(DC_LogLevel.DEBUG);						//Remove in production
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Start the mission framework.
	*/	
	void MissionFrameStart()
	{
		GetGame().GetCallqueue().CallLater(MissionLifeCycleManager, m_Config.missionLifeCycleTime * 2, false);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	End the mission framework. Clean up the running missions.
	*/	
	void MissionFrameEnd()
	{
		//TBD: Clean and delete missions
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Mission life cycle manager.
	*/	
	protected void MissionLifeCycleManager()
	{			
		SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionLifeCycleManager] Number of active missions: " + m_MissionList.Count() + "/" + m_Config.missionCount, LogLevel.NORMAL);
				
		//Check if more missions are to be spawned
		if (m_MissionList.Count() < m_Config.missionCount)
		{
			private ref SCR_DC_Mission tmpDC_Mission = null;

			SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionLifeCycleManager] Starting new missions", LogLevel.DEBUG);
			
			DC_EMissionType missionType = m_Config.missionTypeArray.GetRandomElement();
			
			switch (missionType)
			{
				case DC_EMissionType.NONE:
				{
					SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionLifeCycleManager] Mission of type NONE ignored.", LogLevel.DEBUG);
					break;
				}
				case DC_EMissionType.HUNTER:
				{
					tmpDC_Mission = new SCR_DC_Mission_Hunter();
					break;
				}
				case DC_EMissionType.OCCUPATION:
				{
//					tmpDC_Mission = new SCR_DC_Mission_Occupation();
					break;
				}
				case DC_EMissionType.BANDITCAMP:	//TBD
				{
//					tmpDC_Mission = new SCR_DC_Mission_xxxx();
					break;
				}
				default:
					SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionLifeCycleManager] Incorrect mission type.", LogLevel.DEBUG);
			}
			
			if (tmpDC_Mission)
			{
				//Add to list
				m_MissionList.Insert(tmpDC_Mission);
				//Set mission to start to run
				m_MissionList[m_MissionList.Count() - 1].MissionRun();
				
				//If there was an error starting the mission, it has been prepared for deletion (state = EXIT).
				if (tmpDC_Mission.GetState() != DC_MissionState.EXIT)
				{			
					SCR_DC_Log.Add(string.Format("[SCR_DC_MissionFrame:MissionLifeCycleManager] Spawning mission %1 (%2) %3", tmpDC_Mission.GetTitle(), tmpDC_Mission.GetPos(), tmpDC_Mission.GetPosName()), LogLevel.NORMAL);
					
//					SCR_DC_MapMarkersUI.AddMarkerHint("Mission: " + GetTitle(), GetInfo());
					SCR_DC_MapMarkersUI.AddMarkerHint("Mission: " + tmpDC_Mission.GetTitle(), tmpDC_Mission.GetInfo(), tmpDC_Mission.GetId());
					
					
					//SCR_HintManagerComponent hintComponent = SCR_HintManagerComponent.GetInstance();
					//hintComponent.ShowCustomHint(tmpDC_Mission.GetInfo(), "Mission: " + tmpDC_Mission.GetTitle(), 10);
					
					SCR_DC_DebugHelper.AddDebugPos(tmpDC_Mission.GetPos(), Color.YELLOW, 10)
				}
			}
		}
		else
		{
			SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionLifeCycleManager] Maximum amount of missions spawned: " + m_Config.missionCount, LogLevel.SPAM);
		}

		//Check if missions are 
		//- to be despawned
		//- active
		//- not-active and to be ended
				
		int i = 0;				
		SCR_DC_Mission mission;	
		
		while (i < m_MissionList.Count())
		{
			mission = m_MissionList[i];
						
			if (mission.GetState() == DC_MissionState.EXIT)
			{
				SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionLifeCycleManager] Deleting mission: " + mission.GetId() + " : " + mission.GetTitle(), LogLevel.DEBUG);
				m_MissionList.Remove(i);
				delete mission;
			}
			else
			{
				if (!mission.IsActive())
				{
					SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionLifeCycleManager] Mission not active anymore: " + mission.GetId() + " : " + mission.GetTitle(), LogLevel.DEBUG);
					mission.SetState(DC_MissionState.END);
				}			
				
				i++;	//Next mission to check
			}			
		}		
		
		GetGame().GetCallqueue().CallLater(MissionLifeCycleManager, m_Config.missionLifeCycleTime, false);
	}	
}