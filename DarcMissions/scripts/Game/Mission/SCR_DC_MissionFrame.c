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
	OCCUPATION
};

const string DC_ID_PREFIX = "DCM_";				//The prefix used for marker and missions Id's.

//------------------------------------------------------------------------------------------------
class SCR_DC_MissionFrame
{
	ref array<ref SCR_DC_Mission> m_MissionList = new array<ref SCR_DC_Mission>();
	ref SCR_DC_MissionFrameJsonApi m_DC_MissionFrameJsonApi = new SCR_DC_MissionFrameJsonApi();
	ref SCR_DC_MissionFrameConfig m_Config;
	ref array<ref SCR_DC_NonValidArea> m_NonValidAreas = {};
	string m_WorldName;
	
	protected SCR_MapMarkerManagerComponent m_mapMarkerManager;
	
	//------------------------------------------------------------------------------------------------
	void SCR_DC_MissionFrame()
	{
		SCR_DC_Log.Add("[SCR_DC_MissionFrame] Starting SCR_DC_MissionFrame", LogLevel.NORMAL);
		m_WorldName = SCR_DC_Misc.GetWorldName();
		SCR_DC_Log.Add("[SCR_DC_MissionFrame] Worldname: " + m_WorldName, LogLevel.NORMAL);

		//Load configuration from file		
		m_DC_MissionFrameJsonApi.Load();
		m_Config = m_DC_MissionFrameJsonApi.conf;

		//Set loglevel
		SCR_DC_Log.SetLogLevel(m_Config.logLevel);
		if (!SCR_DC_Core.RELEASE)
		{
			SCR_DC_Log.SetLogLevel(DC_LogLevel.DEBUG);	//Debug enabled when not release
		}
		
		//Set debug visibility
		SCR_DC_DebugHelper.Configure(m_Config.debugShowWaypoints, m_Config.debugShowMarks);
		
		//Fix seconds to ms
		m_Config.missionStartDelay = m_Config.missionStartDelay * 1000;
		
		//Pick nonValidAreas for the current world
		foreach(SCR_DC_NonValidArea nonValidArea : m_Config.nonValidAreas)
		{
			if(nonValidArea.worldName == m_WorldName)
			{
				m_NonValidAreas.Insert(nonValidArea);
				SCR_DC_DebugHelper.AddDebugPos(nonValidArea.pos, Color.BLACK, nonValidArea.radius);
			}
		}
		SCR_DC_Log.Add("[SCR_DC_MissionFrame] Number of nonValidAreas defined: " + m_NonValidAreas.Count(), LogLevel.DEBUG);		
		
		m_mapMarkerManager = SCR_MapMarkerManagerComponent.GetInstance();
		
		CreateMapMarker("1000 0 1000");		
	}

	
	//------------------------------------------------------------------------------------------------
	/*!
	DEBUGging things with CreateMapMarker
	*/	
	void CreateMapMarker(vector pos, int color = Color.RED)
	{
		SCR_MapMarkerBase markerst = new SCR_MapMarkerBase();
		markerst.SetType(SCR_EMapMarkerType.PLACED_MILITARY);
		markerst.SetCustomText("Marker");
		markerst.SetWorldPos(pos[0], pos[2]);
		markerst.SetColorEntry(color);
		
		m_mapMarkerManager.InsertStaticMarker(markerst, false, true);
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Start the mission framework.
	*/	
	void MissionFrameStart()
	{
		GetGame().GetCallqueue().CallLater(MissionLifeCycleManager, m_Config.missionStartDelay, false);
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
					tmpDC_Mission = new SCR_DC_Mission_Occupation();
					break;
				}
//				case DC_EMissionType.CAMP:
//				{
//					tmpDC_Mission = new SCR_DC_Mission_Camp();
//					break;
//				}
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
					
					SCR_DC_MapMarkersUI.AddMarkerHint("Mission: " + tmpDC_Mission.GetTitle(), tmpDC_Mission.GetInfo(), tmpDC_Mission.GetId());		
	
					SCR_DC_DebugHelper.AddDebugPos(tmpDC_Mission.GetPos(), Color.YELLOW, 10);
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
				SCR_DC_DebugHelper.DeleteDebugPos(mission.GetPos());				
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
		
		if (m_Config.logLevel < LogLevel.NORMAL)
		{
			MissionStatusDump();
		}
		
		GetGame().GetCallqueue().CallLater(MissionLifeCycleManager, m_Config.missionFrameLifeCycleTime*1000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Dumps the current mission details to log.
	*/	
	protected void MissionStatusDump()
	{
		int i = 0;
		SCR_DC_Log.Add("[SCR_DC_MissionStatusDump] -------------------------------------------------------------------------", LogLevel.DEBUG);
		foreach(SCR_DC_Mission mission : m_MissionList)
		{
			SCR_DC_Log.Add("[SCR_DC_MissionStatusDump] Mission: " + i + ": " + mission.GetId() + " - " + mission.GetTitle() + " - " + "Time left: " + mission.GetActiveTime() + " (" + SCR_Enum.GetEnumName(DC_MissionState,  mission.GetState()) + ")", LogLevel.DEBUG);
			i++;
		}		
		SCR_DC_Log.Add("[SCR_DC_MissionStatusDump] -------------------------------------------------------------------------", LogLevel.DEBUG);
	}

}