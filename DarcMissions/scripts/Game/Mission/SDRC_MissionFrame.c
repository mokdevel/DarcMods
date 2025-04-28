//------------------------------------------------------------------------------------------------
/*!
This is the mission main framework file. 

Add this to your StartGameTrigger or use SDRC_GameCoreBase.c

	//------------------------------------------------------------------------------------------------
	// user script
	private bool m_GameHasStarted = false;
	private ref SDRC_MissionFrame MissionFrame;

	override void EOnActivate(IEntity owner)
	{
		if (!m_GameHasStarted)
		{
			m_GameHasStarted = true;		
			MissionFrame = new SDRC_MissionFrame(true);
			MissionFrame.MissionFrameStart();			
		}
	}
	//------------------------------------------------------------------------------------------------
*/

const string DC_ID_PREFIX = "DCM_";				//The prefix used for marker and missions Id's.

//------------------------------------------------------------------------------------------------
class SDRC_MissionFrame
{
	ref array<ref SDRC_Mission> m_MissionList = new array<ref SDRC_Mission>;
	ref SDRC_MissionFrameJsonApi m_DC_MissionFrameJsonApi = new SDRC_MissionFrameJsonApi();
	ref SDRC_MissionFrameConfig m_Config;
	
	ref SDRC_NonValidAreaJsonApi m_DC_NonValidAreaJsonApi = new SDRC_NonValidAreaJsonApi();
	ref SDRC_NonValidAreaConfig m_NonValidAreaConfig;
	ref array<ref SDRC_NonValidArea> m_aNonValidAreas = {};
	
	private string m_sWorldName;
	private int m_iLastMissionSpawnTime;
	private int m_iStaticFailCount = 0;				//Counter for failed static missions. If staticFailLimit is reached, we'll spawn a dynamic one.
	private bool m_bFirstMissionSpawned = false;		//The first mission is to be dynamic
		
	//------------------------------------------------------------------------------------------------
	void SDRC_MissionFrame()
	{
		SDRC_Log.Add("[SDRC_MissionFrame] Starting SDRC_MissionFrame", LogLevel.NORMAL);
		m_sWorldName = SDRC_Misc.GetWorldName(true);

		//Load configuration from file		
		m_DC_MissionFrameJsonApi.Load();
		m_Config = m_DC_MissionFrameJsonApi.conf;
		
		SDRC_Log.Add("[SDRC_MissionFrame] Conf destination: /profile/" + SDRC_Conf.CONF_DIRECTORY + "/" + m_Config.missionProfile, LogLevel.NORMAL);

		//Check if a request to create new logs has been made		
		if (m_Config.recreateConfigs)
		{
			SDRC_Log.Add("[SDRC_MissionFrame] ---------------- Creating default configs -------------------", LogLevel.WARNING);
			SDRC_Log.Add("[SDRC_MissionFrame] - Changing recreateConfigs to false and saving the config.  -", LogLevel.WARNING);
			m_Config.recreateConfigs = false;
			m_DC_MissionFrameJsonApi.Save("");
			SDRC_Log.Add("[SDRC_MissionFrame] - Creating configs. Existing ones will not be over written. -", LogLevel.WARNING);
			SDRC_Conf.missionProfile = m_Config.missionProfile;
			CreateAllConfigs();
			SDRC_Log.Add("[SDRC_MissionFrame] --------------------- Configs created. ----------------------", LogLevel.WARNING
			);
		}
		
		//Set mission profile directory. This needs to be after a possible MissionFrame config save.
		SDRC_Conf.missionProfile = m_Config.missionProfile;		
		SDRC_Log.Add("[SDRC_MissionFrame] Worldname: " + m_sWorldName, LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_MissionFrame] Worldsize: " + SDRC_Misc.GetWorldSize(), LogLevel.DEBUG);

		//Load non valid area configuration from file		
		m_DC_NonValidAreaJsonApi.Load();
		m_NonValidAreaConfig = m_DC_NonValidAreaJsonApi.conf;
				
		//Pick nonValidAreas for the current world
		foreach (SDRC_NonValidArea nonValidArea : m_NonValidAreaConfig.nonValidAreas)
		{
			if (nonValidArea.worldName == m_sWorldName || nonValidArea.worldName == "")
			{
				m_aNonValidAreas.Insert(nonValidArea);
				SDRC_DebugHelper.AddDebugPos(nonValidArea.pos, Color.BLACK, nonValidArea.radius);
			}
		}
		SDRC_Log.Add("[SDRC_MissionFrame] Number of nonValidAreas defined: " + m_aNonValidAreas.Count(), LogLevel.NORMAL);		

		//Set some defaults
		m_iStaticFailCount = 0;
		m_iLastMissionSpawnTime = (System.GetTickCount() / 1000) - m_Config.missionDelayBetweeen;	//Fix the timer so that first mission immediately spawns
		m_bFirstMissionSpawned = SDRC_Conf.FIRST_MISSION_HAS_SPAWNED;
				
		//Fix seconds to ms
		SDRC_Log.Add("[SDRC_MissionFrame] Waiting for " + m_Config.missionStartDelay + " seconds before spawning missions.", LogLevel.NORMAL);
		m_Config.missionStartDelay = m_Config.missionStartDelay * 1000;		//sec to ms
		
		#ifndef SDRC_RELEASE
//			SDRC_MapMarkerHelper.CreateMapMarker("1000 0 3000", DC_EMissionIcon.REDCROSS_SMALL, "DMC_B", "");
//			SDRC_MapMarkerHelper.CreateMapMarker("800 0 3500", DC_EMissionIcon.REDCROSS, "DMC_B", "");
//			SDRC_MapMarkerHelper.CreateMapMarker("1500 0 3200", DC_EMissionIcon.MISSION, "DMC_B", "");
		#endif	
		
//		GetGame().GetCallqueue().CallLater(SendHint, 6000, true);
		
		MissionFrameStart();
	}
	
	void SendHint()
	{
		SDRC_HintHelper.ShowHint("Testing", "Yeah", 2);					
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	End the mission framework. Clean up the running missions.
	*/	
	void ~SDRC_MissionFrame()
	{
		//Clean and delete missions
		while (m_MissionList.Count() > 0)
		{
			SDRC_Mission mission = m_MissionList[0];
			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Deleting mission: " + mission.GetId() + " : " + mission.GetTitle(), LogLevel.DEBUG);
			SDRC_DebugHelper.DeleteDebugPos(mission.GetId());
			//delete mission;	//This gives an error...
			m_MissionList.RemoveOrdered(0);			
		}
		SDRC_Log.Add("[~SDRC_MissionFrame] Stopping SDRC_MissionFrame", LogLevel.NORMAL);
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
	Mission life cycle manager.
	*/	
	protected void MissionCycleManager()
	{			
		//Check if more missions are to be spawned		
		if ( (m_MissionList.Count() < m_Config.missionCount) && (isMissionDelayPassed()) && SDRC_PlayerHelper.PlayerCount() > 0)
		{
			private ref SDRC_Mission tmpDC_Mission = null;

			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Spawning new mission", LogLevel.NORMAL);
			
			DC_EMissionType missionType = null;
			
			//Select a new mission to spawn. 
			//Static missions are prioritized so check that the list spawned. 
			if (CountStaticMissions() < m_Config.missionTypeArrayStatic.Count() && m_iStaticFailCount < m_Config.staticFailLimit && m_bFirstMissionSpawned)
			{
				missionType = m_Config.missionTypeArrayStatic.GetRandomElement();
				tmpDC_Mission = MissionCreate(missionType);
				if (tmpDC_Mission)
				{
					tmpDC_Mission.SetStatic(true);
					tmpDC_Mission.SetActiveTime(m_Config.missionActiveTimeStatic);
					tmpDC_Mission.ResetActiveTime();
				}
			}
			else	//Select a dynamic mission to spawn
			{
				missionType = m_Config.missionTypeArrayDynamic.GetRandomElement();				
				tmpDC_Mission = MissionCreate(missionType);
				if (tmpDC_Mission)
				{
					tmpDC_Mission.SetActiveTime(m_Config.missionActiveTime);
					tmpDC_Mission.ResetActiveTime();
					m_bFirstMissionSpawned = true;
				}				
			}

			if (!tmpDC_Mission)
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] MissionCreate failed.", LogLevel.WARNING);
			}
						
			//Mission is ready to be run. Finalize the last details
			if (tmpDC_Mission)
			{
				//Add to list
				m_MissionList.Insert(tmpDC_Mission);
				
				//If there was an error starting the mission, it has been prepared for deletion.
				if (tmpDC_Mission.GetState() != DC_EMissionState.FAILED)
				{		
					//Mission startup was success. Reset fail counter for static missions.	
					m_iStaticFailCount = 0;
					//Set the defaul active distance
					tmpDC_Mission.SetActiveDistance(m_Config.missionActiveDistance);
					tmpDC_Mission.SetActiveTimeToEnd(m_Config.missionActiveTimeToEnd);
					//Set mission to start to run
					m_MissionList[m_MissionList.Count() - 1].MissionRun();
				
					SDRC_Log.Add(string.Format("[SDRC_MissionFrame:MissionCycleManager] Spawning mission %1 (%2) %3", tmpDC_Mission.GetTitle(), tmpDC_Mission.GetPos(), tmpDC_Mission.GetPosName()), LogLevel.NORMAL);

					if (m_Config.missionHintTime > 0 && tmpDC_Mission.IsShowHint())
					{
						SDRC_HintHelper.ShowHint("Mission: " + tmpDC_Mission.GetTitle(), tmpDC_Mission.GetInfo(), m_Config.missionHintTime);					
					}
					
					SDRC_DebugHelper.AddDebugPos(tmpDC_Mission.GetPos(), Color.YELLOW, 10, tmpDC_Mission.GetId());
					
					//Set the time when the mission has started. Activates the delay.
					m_iLastMissionSpawnTime = (System.GetTickCount() / 1000);
				}
			}
		}
		else
		{
			//SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] " + m_MissionList.Count() + " active missions. Waiting for mission delay and/or players to join.", LogLevel.DEBUG);
		}

		//Check if missions are 
		//- to be despawned
		//- active
		//- not-active and to be ended
				
		int i = 0;				
		SDRC_Mission mission;
		
		while (i < m_MissionList.Count())
		{
			mission = m_MissionList[i];
						
			if (mission.GetState() == DC_EMissionState.FAILED)
			{
				if (mission.IsStatic())
				{
					m_iStaticFailCount++;
				}
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Mission start failed: " + mission.GetId() + " (" + SCR_Enum.GetEnumName(DC_EMissionType, mission.GetType()) + "). Static fail count: " + m_iStaticFailCount, LogLevel.WARNING);
			}
			
			if (mission.GetState() == DC_EMissionState.EXIT || mission.GetState() == DC_EMissionState.FAILED)
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Deleting mission: " + mission.GetId(), LogLevel.NORMAL);
				SDRC_DebugHelper.DeleteDebugPos(mission.GetId());
				m_MissionList.Remove(i);
				delete mission;
			}
			else
			{
				//This check is to be done in the missions and not by MissionFrame
/*				if (!mission.IsActive())
				{
					SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Mission not active anymore: " + mission.GetId() + " : " + mission.GetTitle(), LogLevel.DEBUG);
					mission.SetState(DC_EMissionState.END);
				}*/
				
				i++;	//Next mission to check
			}			
		}		
			
		SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Active missions: " + m_MissionList.Count() + "/" + m_Config.missionCount + ". Delay for next mission: " + getMissionDelayWait() + " seconds.", LogLevel.NORMAL);
		MissionDump();

		//Check if no players available
		if (SDRC_PlayerHelper.PlayerCount() == 0)
		{
			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Waiting for players..", LogLevel.NORMAL);
			if (m_MissionList.Count() == 0 && m_bFirstMissionSpawned)
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Resetting m_bFirstMissionSpawned", LogLevel.DEBUG);
				m_bFirstMissionSpawned = false;
			}
		}

		#ifndef SDRC_RELEASE				
			if (SDRC_Conf.SHOW_VALID_MISSION_AREAS)
			{
				SDRC_MissionHelper.DeleteDebugTestMissionPos();
				SDRC_MissionHelper.DebugTestMissionPos();
			}
		#endif
				
		GetGame().GetCallqueue().CallLater(MissionCycleManager, m_Config.missionFrameCycleTime*1000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Creates the mission object
	*/		
	protected SDRC_Mission MissionCreate(DC_EMissionType missionType)
	{
		SDRC_Mission tmpDC_Mission = null;
		
		switch (missionType)
		{
			case DC_EMissionType.NONE:
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Mission of type NONE ignored.", LogLevel.DEBUG);
				break;
			}
			case DC_EMissionType.HUNTER:
			{
				tmpDC_Mission = new SDRC_Mission_Hunter();
				break;
			}
			case DC_EMissionType.OCCUPATION:
			{
				tmpDC_Mission = new SDRC_Mission_Occupation();
				break;
			}
			case DC_EMissionType.CONVOY:
			{
				tmpDC_Mission = new SDRC_Mission_Convoy();
				break;
			}
			case DC_EMissionType.CRASHSITE:
			{
				tmpDC_Mission = new SDRC_Mission_Crashsite();
				break;
			}
			case DC_EMissionType.PATROL:
			{
				tmpDC_Mission = new SDRC_Mission_Patrol();
				break;
			}
			case DC_EMissionType.SQUATTER:
			{
				tmpDC_Mission = new SDRC_Mission_Squatter();
				break;
			}			
/*			case DC_EMissionType.CHOPPER:
			{
				tmpDC_Mission = new SDRC_Mission_Chopper();
				break;
			}*/
			default:
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Incorrect mission type: " + missionType, LogLevel.ERROR);
		}	
		
		return tmpDC_Mission;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Checks if the delay between missions has passed.
	*/	
	protected bool isMissionDelayPassed()
	{
		if ( getMissionDelayWait() > 0)
		{
//			SDRC_Log.Add("[SDRC_MissionFrame:isMissionDelayPassed] Waiting for delay: " + delayTime + ">" + systemTime, LogLevel.DEBUG);
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
		int delayTime = m_iLastMissionSpawnTime + m_Config.missionDelayBetweeen;
		int systemTime = (System.GetTickCount() / 1000);
		
		return delayTime - systemTime;
	}	
		
	//------------------------------------------------------------------------------------------------
	/*!
	Creates config files. To be run at first run of the mod. Will not overwrite existing confs.
	*/	
	void CreateAllConfigs()
	{		
		//Create a default nonValidArea config
		SDRC_NonValidAreaJsonApi nonValidAreaJsonApi = new SDRC_NonValidAreaJsonApi();
		nonValidAreaJsonApi.Load();
		delete nonValidAreaJsonApi;		
		
		//Create a default mission configs
		SDRC_ConvoyJsonApi convoyJsonApi = new SDRC_ConvoyJsonApi();	
		convoyJsonApi.Load();
		delete convoyJsonApi;
		
		SDRC_CrashsiteJsonApi crashsiteJsonApi = new SDRC_CrashsiteJsonApi();	
		crashsiteJsonApi.Load();		
		delete crashsiteJsonApi;
		
		SDRC_HunterJsonApi hunterJsonApi = new SDRC_HunterJsonApi();				
		hunterJsonApi.Load();								
		delete hunterJsonApi;
		
		SDRC_OccupationJsonApi occupationJsonApi = new SDRC_OccupationJsonApi();	
		occupationJsonApi.Load();		
		delete occupationJsonApi;
		
		SDRC_PatrolJsonApi patrolJsonApi = new SDRC_PatrolJsonApi();	
		patrolJsonApi.Load();
		delete patrolJsonApi;
		
		SDRC_SquatterJsonApi squatterJsonApi = new SDRC_SquatterJsonApi();	
		squatterJsonApi.Load();
		delete squatterJsonApi;		
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Dumps the current mission details to log.
	*/	
	protected void MissionDump()
	{
		int i = 0;
		int aiCount = 0;
		int cutLen = 25;
		
		if (m_MissionList.Count() == 0)
		{
			return;
		}

		SDRC_Log.Add("[SDRC_MissionDump] -- Missions -------------------------------------------------------------------", LogLevel.NORMAL);
		foreach (SDRC_Mission mission : m_MissionList)
		{
			string staticString = "dynamic";
			if (mission.IsStatic())
			{
				staticString = "static";
			}
			string missionType = SCR_Enum.GetEnumName(DC_EMissionType, mission.GetType());
			string missionTitle = mission.GetTitle();
			if (missionTitle.Length() > cutLen)
			{
				missionTitle = mission.GetTitle().Substring(0, cutLen) + "..";
			}
			string missionState = SCR_Enum.GetEnumName(DC_EMissionState,  mission.GetState());
			
			SDRC_Log.Add("[SDRC_MissionDump] " + i + ": " + mission.GetId() + " (" + missionType + ", " + staticString + ", " + missionState + ") - " + missionTitle + " - " + "Time left: " + mission.GetActiveTime(), LogLevel.NORMAL);
			aiCount = aiCount + mission.GetAICount();
			i++;
		}		
		string lastLine = "[SDRC_MissionDump] -- AI count: " + aiCount + " -------------------------------------------------------------------------";
		lastLine = lastLine.Substring(0, 100);
		SDRC_Log.Add(lastLine, LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Counts the amount of active static missions
	*/	
	protected int CountStaticMissions()	
	{
		int i = 0;
		foreach (SDRC_Mission mission : m_MissionList)
		{
			if (mission.IsStatic())
			{
				i++;
			}
		}
		
		return i;
	}
}