//------------------------------------------------------------------------------------------------
/*!
This is the mission main framework file. 
*/

//------------------------------------------------------------------------------------------------

const string DC_ID_PREFIX = "DCM_";				//The prefix used for marker and missions Id's.

const string DC_MISSIONCONFIG_FILE_FRAME = "dc_missionConfig.json";
const int 	 DC_MISSIONCONFIG_FILE_FRAME_JSONVER = 4;

const string DC_MISSIONCONFIG_FILE_NONVALIDAREA = "dc_nonValidArea.json";
const int 	 DC_MISSIONCONFIG_FILE_NONVALIDAREA_JSONVER = 2;

const string DC_MISSIONCONFIG_FILE_QRF = "dc_qrf.json";
const int DC_MISSIONCONFIG_FILE_QRF_JSONVER = 4;

//------------------------------------------------------------------------------------------------
class SDRC_MissionFrame
{
	protected static SDRC_MissionFrame s_Instance;		
	ref array<ref SDRC_Mission> m_MissionList = new array<ref SDRC_Mission>;
	
	private ref SDRC_JsonApi2 m_DC_MissionFrameJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_FRAME);	
	ref SDRC_MissionFrameConfig m_Config = new SDRC_MissionFrameConfig();	
	
	ref SDRC_JsonApi2 m_NonValidAreaJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_NONVALIDAREA);	
	ref SDRC_NonValidAreaConfig m_ConfigNonValidArea = new SDRC_NonValidAreaConfig();	

	ref SDRC_JsonApi2 m_QrfJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_QRF);	
	ref SDRC_QrfConfig m_ConfigQrf = new SDRC_QrfConfig();	
	
//	ref SDRC_SecondWaveJsonApi m_DC_SecondWaveJsonApi = new SDRC_SecondWaveJsonApi(DC_MISSIONCONFIG_FILE_SECONDWAVE);
		
	private string m_sWorldName;
	private int m_iMissionCountDynamicMax;				//Max amount of dynamic missions
	private int m_iMissionCountStaticMax;				//Max amount of static missions
	private int m_iLastMissionSpawnTime;
	private int m_iStaticTryCount = 0;					//Counter for tries on static missions - both failed and succesful. If m_iStaticTryLimit is reached, we stop trying to spawn static missions
	private int m_iStaticTryLimit = 0;					
		
	ref array<ref SDRC_MissionRequested> m_missionsRequested = {};
	
	//------------------------------------------------------------------------------------------------
	void SDRC_MissionFrame()
	{
		//SDRC_Spline3D.TestSpline();		
		
		SDRC_Log.Add("[SDRC_MissionFrame] Starting SDRC_MissionFrame", LogLevel.NORMAL);
		s_Instance = this;
				
		m_sWorldName = SDRC_Misc.GetWorldName(true);

		//Load configuration from file		
		bool successFrame = m_DC_MissionFrameJsonApi.Load(m_Config, SDRC_Config.Cast(m_Config), DC_MISSIONCONFIG_FILE_FRAME_JSONVER);
		
		//Load NonValidArea configuration from file
		bool successNoNValid = m_NonValidAreaJsonApi.Load(m_ConfigNonValidArea, SDRC_Config.Cast(m_ConfigNonValidArea), DC_MISSIONCONFIG_FILE_NONVALIDAREA_JSONVER);

		//Load Qrf configuration from file
		bool successQrf = m_QrfJsonApi.Load(m_ConfigQrf, SDRC_Config.Cast(m_ConfigQrf), DC_MISSIONCONFIG_FILE_QRF_JSONVER, safeUpdate: true);
		
		if ( (!successFrame) || (!successNoNValid) || (!successQrf) )
		{
			ShowFailure();
			return;
		}		
				
		//Load qrfs functionality
		m_ConfigQrf.Populate();
		
		//Update the setting for showing mission time left				
		SDRC_RplGMComp gmComp = SDRC_RplGMComp.FindInstance();
		if (gmComp)
		{
			gmComp.UpdateTimeLeft(m_Config.showMissionTimeLeft);
		}
		
		SDRC_Log.Add("[SDRC_MissionFrame] -------- General information --------", LogLevel.NORMAL);
		//Checking the enemies found
		SDRC_Log.Add("[SDRC_MissionFrame] Enemy factions: " + m_Config.enemyFactions, LogLevel.NORMAL);
		SDRC_EnemyHelper.SetEnemyFactions(m_Config.enemyFactions);
		SDRC_Log.Add("[SDRC_MissionFrame] Fallback faction: " + SDRC_EnemyHelper.GetDefaultEnemyFaction(), LogLevel.NORMAL);

		//Count amount of dynamic and static missions
		m_iMissionCountDynamicMax = SDRC_MissionHelper.GetMissionCountForWorld(m_Config.missionDynamic.count, m_Config.missionDynamic.countMul);
		SDRC_Log.Add("[SDRC_MissionFrame] Max dynamic mission count: " + m_iMissionCountDynamicMax, LogLevel.NORMAL);		

		m_iMissionCountStaticMax = SDRC_MissionHelper.GetMissionCountForWorld(m_Config.missionStatic.count, m_Config.missionStatic.countMul);
		SDRC_Log.Add("[SDRC_MissionFrame] Max static mission count: " + m_iMissionCountStaticMax, LogLevel.NORMAL);		
		SDRC_Log.Add("[SDRC_MissionFrame] -------------------------------------", LogLevel.NORMAL);
				
		//Set some defaults
		m_iStaticTryCount = 0;
		m_iStaticTryLimit = m_iMissionCountStaticMax * 2;
		m_iLastMissionSpawnTime = (System.GetTickCount() / 1000) - m_Config.missionStatic.delayBetween;		//Fix the timer so that first mission immediately spawns		
		
		//Check if a request to create new configs has been made		
		SDRC_Log.Add("[SDRC_MissionFrame] ---------------- Creating missing configs -------------------", LogLevel.WARNING);
		SDRC_Log.Add("[SDRC_MissionFrame] - Creating configs. Existing ones will not be over written. -", LogLevel.WARNING);
		SDRC_MissionEnumHelper.CreateAllConfigs();
		SDRC_Log.Add("[SDRC_MissionFrame] --------------------- Configs created. ----------------------", LogLevel.WARNING);
		
		//Initialize compatibility for mods
		SDRC_Compat.Init();
		
		//Fix seconds to ms
		SDRC_Log.Add("[SDRC_MissionFrame] Waiting for " + m_Config.missionStartDelay + " seconds before spawning missions.", LogLevel.NORMAL);
		m_Config.missionStartDelay = m_Config.missionStartDelay * 1000;		//sec to ms
		
		#ifdef SDRC_Conf.SHOW_DEBUG
			SDRC_DevHelper.SDRC_DevDump();
		#endif	
		
		//Start the mission framework.
		GetGame().GetCallqueue().CallLater(MissionCycleManager, m_Config.missionStartDelay, false);
	}
	
	//------------------------------------------------------------------------------------------------
	static SDRC_MissionFrame GetInstance()
	{
		return s_Instance;		
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	End the mission framework. Clean up the running missions.
	*/	
	void ~SDRC_MissionFrame()
	{
		//Clean and delete missions
		/* TBD: Enable back
		while (m_MissionList.Count() > 0)
		{
			SDRC_Mission mission = m_MissionList[0];
			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Deleting mission: " + mission.GetId() + " : " + mission.GetTitle(), LogLevel.DEBUG);
			SDRC_DebugHelper.DeleteDebugPos(mission.GetId());
			//delete mission;	//This gives an error...
			mission.MissionEnd();
			m_MissionList.RemoveOrdered(0);			
		}
		*/
		SDRC_Log.Add("[~SDRC_MissionFrame] Stopping SDRC_MissionFrame", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Mission life cycle manager.
	*/	
	protected void MissionCycleManager()
	{		
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (baseGameMode)
		{						
			if (baseGameMode.GetState() == SCR_EGameModeState.PREGAME)
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Waiting.. in PREGAME.", LogLevel.DEBUG);
				GetGame().GetCallqueue().CallLater(MissionCycleManager, m_Config.missionFrameCycleTime*1000, false);
				return;
			}
		}
		
		#ifndef SDRC_RELEASE
			//SDRC_DevHelper.TestMissionPositions();
		#endif	
		
		ref SDRC_Mission tmpDC_Mission = null;
		SDRC_EMissionType missionType = SDRC_EMissionType.NONE;
		bool staticMissionSpawned = false;
		int staticMissionsCount = CountStaticMissions();
		
		//Check if maximum amount of static missions has been spawned, no more are to be spawned		
		if (staticMissionsCount >= m_iMissionCountStaticMax)
		{
			m_iStaticTryCount = m_iStaticTryLimit;
		}
		
		//Check if static missions have been spawned
		if (staticMissionsCount < m_iMissionCountStaticMax && m_iStaticTryCount < m_iStaticTryLimit)
		{
			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Spawning new static mission", LogLevel.NORMAL);
			
			if (m_Config.missionStatic.missionTypeArray.IsEmpty())
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Error in file dc_missionConfig.json : missionStatic/missionTypeArray is empty.", LogLevel.ERROR);
				missionType = SDRC_EMissionType.ERROR;
			}
			else
			{
				missionType = SDRC_MissionHelper.SelectMissionType(false);
				//missionType = m_Config.missionStatic.missionTypeArray.GetRandomElement();
			}
			tmpDC_Mission = MissionCreate(missionType, true);
			if (tmpDC_Mission)
			{
//				tmpDC_Mission.SetStatic(true);
				tmpDC_Mission.InitActiveTime(m_Config.missionStatic.activeTime);
				tmpDC_Mission.ResetActiveTime();
//				tmpDC_Mission.ShowMarker();
			}
			else
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] MissionCreate failed (static): " + SCR_Enum.GetEnumName(SDRC_EMissionType, missionType), LogLevel.WARNING);
			}
			
			staticMissionSpawned = true;	//Static missions to be spawned faster at startup
			m_iStaticTryCount++;			//We increase this even if the mission start failed for static
		}
		else
		{
			//Check if more dynamic or GM missions are to be spawned
			if ( ( ( (m_MissionList.Count() - staticMissionsCount) < m_iMissionCountDynamicMax) && (isMissionDelayPassed()) && SDRC_PlayerHelper.PlayerCount() > 0 ) || (!m_missionsRequested.IsEmpty()) )
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Spawning new dynamic mission", LogLevel.NORMAL);
				
				//Select a new mission to spawn.
				
				//GM has not requested any missions to spawn, go for a regular dynamic one
				if (m_missionsRequested.IsEmpty())
				{
					if (m_Config.missionDynamic.missionTypeArray.IsEmpty())
					{
						SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Error in file dc_missionConfig.json : missionDynamic/missionTypeArray is empty.", LogLevel.ERROR);
						missionType = SDRC_EMissionType.ERROR;						
					}
					else
					{
						missionType = SDRC_MissionHelper.SelectMissionType(true);
						//missionType = m_Config.missionDynamic.missionTypeArray.GetRandomElement();
					}
					//If regular mission is to be spawned, static missions have already been handled.
					m_iStaticTryCount = m_iStaticTryLimit;
				}
				else	//Spawn a GM requested mission
				{
					missionType = SDRC_EMissionType.REQUESTED;
				}
				
				//Do the spawning
				tmpDC_Mission = MissionCreate(missionType);				
				if (tmpDC_Mission)
				{
					if (tmpDC_Mission.GetState() != SDRC_EMissionState.FAILED)
					{
						tmpDC_Mission.InitActiveTime(m_Config.missionDynamic.activeTime);
						tmpDC_Mission.ResetActiveTime();
//						tmpDC_Mission.ShowMarker();
					}
				}
				else
				{
					SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] MissionCreate failed (dynamic): " + SCR_Enum.GetEnumName(SDRC_EMissionType, missionType), LogLevel.WARNING);
				}
			}
		}
		
		//Mission is ready to be run. Finalize the last details
		if (tmpDC_Mission)
		{
			//Add to list
			m_MissionList.Insert(tmpDC_Mission);
			
			//If there was an error starting the mission, it has been prepared for deletion.
			if (tmpDC_Mission.GetState() != SDRC_EMissionState.FAILED)
			{		
				//Set the default active distance and time to end
				tmpDC_Mission.SetActiveDistance(m_Config.missionActiveDistance);
				tmpDC_Mission.SetActiveTimeToEnd(m_Config.missionActiveTimeToEnd);
				//Set mission to start to run
				m_MissionList[m_MissionList.Count() - 1].MissionStart();
			
				SDRC_Log.Add(string.Format("[SDRC_MissionFrame:MissionCycleManager] Spawning mission %1 (%2) %3", tmpDC_Mission.GetTitle(), tmpDC_Mission.GetPos(), tmpDC_Mission.GetPosName()), LogLevel.NORMAL);

				if (!staticMissionSpawned)
				{
					if (tmpDC_Mission.IsShowHint())
					{
						SDRC_MissionHintHelper.Show("Mission: " + tmpDC_Mission.GetTitle(), tmpDC_Mission.GetInfo());
					}
				}
								
				SDRC_DebugHelper.AddDebugPos(tmpDC_Mission.GetPos(), ARGB(5, 128, 128, 0), 10, tmpDC_Mission.GetId());
				
				//Set the time when the mission has started. Activates the delay.
				m_iLastMissionSpawnTime = (System.GetTickCount() / 1000);
			}
		}
		else
		{
			//SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] " + m_MissionList.Count() + " active missions. Waiting for mission delay and/or players to join.", LogLevel.DEBUG);
		}

		//------------------------------------------------------------------------------------------------
		//Check if missions are 
		//- to be despawned
		//- active
		//- not-active and to be ended
				
		int i = 0;				
		SDRC_Mission mission;
		
		while (i < m_MissionList.Count())
		{
			mission = m_MissionList[i];
						
			if (mission.GetState() == SDRC_EMissionState.FAILED)
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Mission start failed: " + mission.GetId() + " (" + SCR_Enum.GetEnumName(SDRC_EMissionType, mission.GetType()) + ")", LogLevel.WARNING);
			}

			if (mission.GetState() == SDRC_EMissionState.EXIT || mission.GetState() == SDRC_EMissionState.FAILED)
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Deleting mission: " + mission.GetId(), LogLevel.NORMAL);
				SDRC_DebugHelper.DeleteDebugPos(mission.GetId());
				m_MissionList.Remove(i);
			}
			else
			{
				i++;	//Next mission to check
			}			
		}		
			
		//Check if no players available
		if (SDRC_PlayerHelper.PlayerCount() == 0)
		{
			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Waiting for players..", LogLevel.NORMAL);
		}

		#ifndef SDRC_RELEASE
			if (SDRC_Conf.SHOW_VALID_MISSION_AREAS)
			{
				SDRC_MissionHelper.DeleteDebugTestMissionPos();
				SDRC_MissionHelper.DebugTestMissionPos();
			}
		#endif

		int staticCount = CountStaticMissions();
		
		//If static mission spawned, we set a shorter time to spawn them quickly at startup
		if (!staticMissionSpawned)
		{
			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Dynamic: " + (m_MissionList.Count() - staticCount) + "/" + m_iMissionCountDynamicMax + " - Static: " + staticCount + "/" + m_iMissionCountStaticMax + ". Next mission: " + getMissionDelayWait() + " seconds.", LogLevel.NORMAL);
			MissionDump();
			GetGame().GetCallqueue().CallLater(MissionCycleManager, m_Config.missionFrameCycleTime*1000, false);
		}				
		else
		{
			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Static: " + staticCount + "/" + m_iMissionCountStaticMax + " - try/limit: " + m_iStaticTryCount + "/" + m_iStaticTryLimit, LogLevel.NORMAL);
			MissionDump();
			GetGame().GetCallqueue().CallLater(MissionCycleManager, m_Config.missionStatic.delayBetween*1000, false);
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Creates the mission object
	*/		
	protected SDRC_Mission MissionCreate(SDRC_EMissionType missionType, bool staticMission = false)
	{
		SDRC_Mission tmpDC_Mission = null;
		SDRC_MissionRequested missionRequest = null;
		
		if (missionType == SDRC_EMissionType.REQUESTED)
		{					
			//If mission was requested, fill information from the request
			missionRequest = SDRC_MissionRequestHelper.FillMissionRequest();
			missionType = missionRequest.missionType;
		}
		
		SDRC_Log.Add("[SDRC_MissionFrame:MissionCreate] Starting mission of type: " + SCR_Enum.GetEnumName(SDRC_EMissionType, missionType), LogLevel.DEBUG);
		
		switch (missionType)
		{
			case SDRC_EMissionType.ERROR:
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Incorrect missiontype. Check dc_missionConfig.json for errors.", LogLevel.ERROR);
				break;
			}
			case SDRC_EMissionType.NONE:
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Mission of type NONE ignored.", LogLevel.ERROR);
				break;
			}
			case SDRC_EMissionType.HUNTER:
			{
				tmpDC_Mission = new SDRC_Mission_Hunter(missionType, missionRequest, staticMission);
				break;
			}
			case SDRC_EMissionType.OCCUPATION:
			{
				tmpDC_Mission = new SDRC_Mission_Occupation(missionType, missionRequest, staticMission);
				break;
			}
			case SDRC_EMissionType.CONVOY:
			{
				tmpDC_Mission = new SDRC_Mission_Convoy(missionType, missionRequest, staticMission);
				break;
			}
			case SDRC_EMissionType.CRASHSITE:
			{
				tmpDC_Mission = new SDRC_Mission_Crashsite(missionType, missionRequest, staticMission);
				break;
			}
			case SDRC_EMissionType.PATROL:
			{
				tmpDC_Mission = new SDRC_Mission_Patrol(missionType, missionRequest, staticMission);
				break;
			}
			case SDRC_EMissionType.SQUATTERS:
			{
				tmpDC_Mission = new SDRC_Mission_Squatter(missionType, missionRequest, staticMission);
				break;
			}
			case SDRC_EMissionType.ROADBLOCK:
			{
				tmpDC_Mission = new SDRC_Mission_Roadblock(missionType, missionRequest, staticMission);
				break;
			}			
			case SDRC_EMissionType.HVTVIP:
			{
				tmpDC_Mission = new SDRC_Mission_HvtVip(missionType, missionRequest, staticMission);
				break;
			}			
			case SDRC_EMissionType.HVTITEM:
			{
				tmpDC_Mission = new SDRC_Mission_HvtItem(missionType, missionRequest, staticMission);
				break;
			}			
			case SDRC_EMissionType.STASH:
			{
				tmpDC_Mission = new SDRC_Mission_Stash(missionType, missionRequest, staticMission);
				break;
			}			
			case SDRC_EMissionType.CHOPPER:
			{
				tmpDC_Mission = new SDRC_Mission_Chopper(missionType, missionRequest, staticMission);
				break;
			}
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
		int delayTime = m_iLastMissionSpawnTime + m_Config.missionDynamic.delayBetween;
		int systemTime = (System.GetTickCount() / 1000);
		
		return delayTime - systemTime;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Dumps the current mission details to log.
	*/	
	protected void MissionDump()
	{
		int i = 0;
		int aiCount = 0;
		const int CUT_LENGTH = 32;
		
		if (m_MissionList.Count() == 0)
		{
			return;
		}

		SDRC_Log.Add("[SDRC_MissionDump] -- Missions -------------------------------------------------------------------", LogLevel.NORMAL);
		foreach (SDRC_Mission mission : m_MissionList)
		{
			string staticString = "dyn";
			if (mission.IsStatic())
			{
				staticString = "sta";
			}
			if (mission.IsRequested())
			{
				staticString = staticString + "(req)";
			}
			string missionType = SCR_Enum.GetEnumName(SDRC_EMissionType, mission.GetType());
			string missionTitle = mission.GetTitle();
			if (missionTitle.Length() > CUT_LENGTH)
			{
				missionTitle = mission.GetTitle().Substring(0, CUT_LENGTH) + "..";
			}
			string missionState = SCR_Enum.GetEnumName(SDRC_EMissionState,  mission.GetState());

			string missionDetails = "" + i + ":" + mission.GetId() + " (" + missionType + "/" + staticString + "/" + missionState + "/" + mission.GetDifficulty() + ")";
						
			SDRC_Log.Add("[SDRC_MissionDump] " + missionDetails + ": " + missionTitle + " - " + "Time: " + mission.GetActiveTime(), LogLevel.NORMAL);
			aiCount = aiCount + mission.GetAICountActive();
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

	//------------------------------------------------------------------------------------------------
	/*!
	Find mission index with mission id
	*/		
	int FindMissionWithId(string id)
	{
		int idx = -1;
		foreach (SDRC_Mission mission : m_MissionList)
		{
			if (mission.GetId() == id)
			{
				idx++;
				break;
			}
			idx++;
		}
		
		return idx;
	}			
	
	//------------------------------------------------------------------------------------------------
	/*!
	Show a failed startup
	*/		
	protected void ShowFailure()
	{		
		SDRC_Log.Add("[SDRC_MissionFrame] ****** Could not initialize DarcMissions. Check your logs. ******", LogLevel.ERROR);
		GetGame().GetCallqueue().CallLater(ShowFailure, 10000, false);
	}	
}