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

const string DC_ID_PREFIX = "DCM_";				//The prefix used for marker and missions Id's.

//------------------------------------------------------------------------------------------------
class SCR_DC_MissionFrame
{
	ref array<ref SCR_DC_Mission> m_MissionList = new array<ref SCR_DC_Mission>;
	ref SCR_DC_MissionFrameJsonApi m_DC_MissionFrameJsonApi = new SCR_DC_MissionFrameJsonApi;
	ref SCR_DC_MissionFrameConfig m_Config;
	ref array<ref SCR_DC_NonValidArea> m_NonValidAreas = {};
	private string m_WorldName;
	private int m_LastMissionSpawnTime = 0;
		
	//------------------------------------------------------------------------------------------------
	void SCR_DC_MissionFrame()
	{
		SCR_DC_Log.Add("[SCR_DC_MissionFrame] Starting SCR_DC_MissionFrame", LogLevel.NORMAL);
		m_WorldName = SCR_DC_Misc.GetWorldName(true);

		//Load configuration from file		
		m_DC_MissionFrameJsonApi.Load();
		m_Config = m_DC_MissionFrameJsonApi.conf;

		//Set loglevel
		if (!SCR_DC_Conf.RELEASE)
		{
			SCR_DC_RoadHelper.TestRoad();
		}
		
		//Check if a request to create new logs has been made		
		if (m_Config.recreateConfigs)
		{
			SCR_DC_Log.Add("[SCR_DC_MissionFrame] !!!!!!!!!!!!!!.....Creating default configs.....!!!!!!!!!!!!!", LogLevel.WARNING);
			SCR_DC_Log.Add("[SCR_DC_MissionFrame] Existing ones will not be over written.", LogLevel.WARNING);
			CreateAllConfigs();
			SCR_DC_Log.Add("[SCR_DC_MissionFrame] Changing recreateConfigs to false and saving the config.", LogLevel.WARNING);
			m_Config.recreateConfigs = false;
			m_DC_MissionFrameJsonApi.Save("");
			SCR_DC_Log.Add("[SCR_DC_MissionFrame] !!!!!!!!!!!!!!!!!!!!! Configs created. !!!!!!!!!!!!!!!!!!!!!!", LogLevel.WARNING
			);
		}
		
		SCR_DC_Log.Add("[SCR_DC_MissionFrame] Worldname: " + m_WorldName, LogLevel.NORMAL);
		
		//Pick nonValidAreas for the current world
		foreach(SCR_DC_NonValidArea nonValidArea : m_Config.nonValidAreas)
		{
			if(nonValidArea.worldName == m_WorldName)
			{
				m_NonValidAreas.Insert(nonValidArea);
				SCR_DC_DebugHelper.AddDebugPos(nonValidArea.pos, Color.BLACK, nonValidArea.radius);
			}
		}
		SCR_DC_Log.Add("[SCR_DC_MissionFrame] Number of nonValidAreas defined: " + m_NonValidAreas.Count(), LogLevel.NORMAL);		
		
		//Fix seconds to ms
		SCR_DC_Log.Add("[SCR_DC_MissionFrame] Mission start delay: " + m_Config.missionStartDelay + " seconds.", LogLevel.NORMAL);
		m_Config.missionStartDelay = m_Config.missionStartDelay * 1000;
		
		#ifndef SCR_DC_RELEASE
			SCR_DC_MapMarkerHelper.CreateMapMarker("1000 0 3000", DC_EMissionIcon.REDCROSS_SMALL, "DMC_B", "");
			SCR_DC_MapMarkerHelper.CreateMapMarker("800 0 3500", DC_EMissionIcon.REDCROSS, "DMC_B", "");
//			SCR_DC_MapMarkerHelper.CreateMapMarker("1500 0 3200", DC_EMissionIcon.MISSION, "DMC_B", "");
		#endif
		
		MissionFrameStart();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Start the mission framework.
	*/	
	void MissionFrameStart()
	{
		GetGame().GetCallqueue().CallLater(MissionCycleManager, m_Config.missionStartDelay, false);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	End the mission framework. Clean up the running missions.
	*/	
	void MissionFrameEnd()
	{
		//Clean and delete missions
		foreach(SCR_DC_Mission mission: m_MissionList)
		{
			mission.MissionEnd();
			SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionCycleManager] Deleting mission: " + mission.GetId() + " : " + mission.GetTitle(), LogLevel.DEBUG);
			SCR_DC_DebugHelper.DeleteDebugPos(mission.GetId());
			delete mission;		
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Mission life cycle manager.
	*/	
	protected void MissionCycleManager()
	{			
		//Check if more missions are to be spawned		
		if ( (m_MissionList.Count() < m_Config.missionCount) && (isMissionDelayPassed()) )
		{
			private ref SCR_DC_Mission tmpDC_Mission = null;

			SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionCycleManager] Starting new missions", LogLevel.DEBUG);
			
			DC_EMissionType missionType = null;
			
			//Select a new mission to spawn. 
			//Static missions are prioritized so check that the list spawned.
			if (CountStaticMissions() < m_Config.missionTypeArrayStatic.Count())
			{
				missionType = m_Config.missionTypeArrayStatic.GetRandomElement();
				tmpDC_Mission = MissionCreate(missionType);
				if (tmpDC_Mission)
				{
					tmpDC_Mission.SetStatic(true);
					//Set the defaul active time. NOTE: Static missions are kept alive 
					tmpDC_Mission.SetActiveTime(m_Config.missionActiveTimeStatic);
				}
			}
			else	//Select a dynamic mission to spawn
			{
				missionType = m_Config.missionTypeArrayDynamic.GetRandomElement();				
				tmpDC_Mission = MissionCreate(missionType);
				if (tmpDC_Mission)
				{
					tmpDC_Mission.SetActiveTime(m_Config.missionActiveTime);
				}				
			}
			
			//Mission is ready to be run. Finalize the last details
			if (tmpDC_Mission)
			{
				//Set the defaul active distance
				tmpDC_Mission.SetActiveDistance(m_Config.missionActiveDistance);
				//Add to list
				m_MissionList.Insert(tmpDC_Mission);
				//Set mission to start to run
				m_MissionList[m_MissionList.Count() - 1].MissionRun();
				
				//If there was an error starting the mission, it has been prepared for deletion (state = EXIT).
				if (tmpDC_Mission.GetState() != DC_MissionState.EXIT)
				{			
					SCR_DC_Log.Add(string.Format("[SCR_DC_MissionFrame:MissionCycleManager] Spawning mission %1 (%2) %3", tmpDC_Mission.GetTitle(), tmpDC_Mission.GetPos(), tmpDC_Mission.GetPosName()), LogLevel.NORMAL);

					if (m_Config.missionHintTime > 0 && tmpDC_Mission.IsShowHint())
					{
						SCR_DC_HintHelper.ShowHint("Mission: " + tmpDC_Mission.GetTitle(), tmpDC_Mission.GetInfo(), m_Config.missionHintTime);					
					}
					
					SCR_DC_DebugHelper.AddDebugPos(tmpDC_Mission.GetPos(), Color.YELLOW, 10, tmpDC_Mission.GetId());
					
					//Set the time when the mission has started. Activates the delay.
					m_LastMissionSpawnTime = (System.GetTickCount() / 1000);
				}
			}
		}
		else
		{
			SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionCycleManager] Maximum amount of missions spawned: " + m_Config.missionCount, LogLevel.SPAM);
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
				SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionCycleManager] Deleting mission: " + mission.GetId() + " : " + mission.GetTitle(), LogLevel.DEBUG);
				SCR_DC_DebugHelper.DeleteDebugPos(mission.GetId());
				m_MissionList.Remove(i);
				delete mission;
			}
			else
			{
				if (!mission.IsActive())
				{
					SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionCycleManager] Mission not active anymore: " + mission.GetId() + " : " + mission.GetTitle(), LogLevel.DEBUG);
					mission.SetState(DC_MissionState.END);
				}			
				
				i++;	//Next mission to check
			}			
		}		
		
		MissionStatusDump();
		SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionCycleManager] Active missions: " + m_MissionList.Count() + "/" + m_Config.missionCount + ". Delay for next mission: " + getMissionDelayWait() + " seconds.", LogLevel.NORMAL);
		
		if (SCR_DC_Conf.SHOW_VALID_MISSION_AREAS)
		{
			SCR_DC_MissionHelper.DeleteDebugTestMissionPos();
			SCR_DC_MissionHelper.DebugTestMissionPos();
		}
				
		GetGame().GetCallqueue().CallLater(MissionCycleManager, m_Config.missionFrameCycleTime*1000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Creates the mission object
	*/		
	protected SCR_DC_Mission MissionCreate(DC_EMissionType missionType)
	{
		SCR_DC_Mission tmpDC_Mission = null;
		
		switch (missionType)
		{
			case DC_EMissionType.NONE:
			{
				SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionCycleManager] Mission of type NONE ignored.", LogLevel.DEBUG);
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
			case DC_EMissionType.CONVOY:
			{
				tmpDC_Mission = new SCR_DC_Mission_Convoy();
				break;
			}
			case DC_EMissionType.CRASHSITE:
			{
				tmpDC_Mission = new SCR_DC_Mission_Crashsite();
				break;
			}
			case DC_EMissionType.PATROL:
			{
				tmpDC_Mission = new SCR_DC_Mission_Patrol();
				break;
			}
			case DC_EMissionType.CHOPPER:
			{
				tmpDC_Mission = new SCR_DC_Mission_Chopper();
				break;
			}
			default:
				SCR_DC_Log.Add("[SCR_DC_MissionFrame:MissionCycleManager] Incorrect mission type: " + missionType, LogLevel.ERROR);
		}	
		
		return tmpDC_Mission;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Dumps the current mission details to log.
	*/	
	protected void MissionStatusDump()
	{
		int i = 0;
		int aiCount = 0;
		SCR_DC_Log.Add("[SCR_DC_MissionStatusDump] -------------------------------------------------------------------------", LogLevel.DEBUG);
		foreach(SCR_DC_Mission mission : m_MissionList)
		{
			SCR_DC_Log.Add("[SCR_DC_MissionStatusDump] Mission: " + i + ": " + mission.GetId() + " (" + SCR_Enum.GetEnumName(DC_EMissionType, mission.GetType()) + ", static: " + mission.IsStatic() + ") - " + mission.GetTitle() + " - " + "Time left: " + mission.GetActiveTime() + " (" + SCR_Enum.GetEnumName(DC_MissionState,  mission.GetState()) + ")", LogLevel.DEBUG);
			aiCount = aiCount + mission.GetAICount();
			i++;
		}		
		SCR_DC_Log.Add("[SCR_DC_MissionStatusDump] AI count: " + aiCount, LogLevel.DEBUG);
		SCR_DC_Log.Add("[SCR_DC_MissionStatusDump] -------------------------------------------------------------------------", LogLevel.DEBUG);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Counts the amount of active static missions
	*/	
	protected int CountStaticMissions()	
	{
		int i = 0;
		foreach(SCR_DC_Mission mission : m_MissionList)
		{
			if (mission.IsStatic())
			{
				i++;
			}
		}
		
		return i;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Checks if the delay between missions has passed.
	*/	
	protected bool isMissionDelayPassed()
	{
//		int delayTime = m_LastMissionSpawnTime + m_Config.missionDelayBetweeen;
//		int systemTime = (System.GetTickCount() / 1000);
		
		if ( getMissionDelayWait() > 0)
		{
//			SCR_DC_Log.Add("[SCR_DC_MissionFrame:isMissionDelayPassed] Waiting for delay: " + delayTime + ">" + systemTime, LogLevel.DEBUG);
			return false;
		}
		return true;
	}

	
	//------------------------------------------------------------------------------------------------
	/*!
	Checks if the delay between missions has passed.
	*/	
	protected int getMissionDelayWait()
	{
		int delayTime = m_LastMissionSpawnTime + m_Config.missionDelayBetweeen;
		int systemTime = (System.GetTickCount() / 1000);
		
		return delayTime - systemTime;
	}	
		
	//------------------------------------------------------------------------------------------------
	/*!
	Creates config files. To be run at first run of the mod. Will not overwrite existing confs.
	*/	
	void CreateAllConfigs()
	{		
		SCR_DC_CampJsonApi campJsonApi = new SCR_DC_CampJsonApi();	
		campJsonApi.Load();
		delete campJsonApi;
		
		SCR_DC_ConvoyJsonApi convoyJsonApi = new SCR_DC_ConvoyJsonApi();	
		convoyJsonApi.Load();
		delete convoyJsonApi;
		
		SCR_DC_CrashsiteJsonApi crashsiteJsonApi = new SCR_DC_CrashsiteJsonApi();	
		crashsiteJsonApi.Load();		
		delete crashsiteJsonApi;
		
		SCR_DC_HunterJsonApi hunterJsonApi = new SCR_DC_HunterJsonApi();				
		hunterJsonApi.Load();								
		delete hunterJsonApi;
		
		SCR_DC_OccupationJsonApi occupationJsonApi = new SCR_DC_OccupationJsonApi;	
		occupationJsonApi.Load();		
		delete occupationJsonApi;
		
		SCR_DC_PatrolJsonApi patrolJsonApi = new SCR_DC_PatrolJsonApi;	
		patrolJsonApi.Load();
		delete patrolJsonApi;
	}	
}