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

//------------------------------------------------------------------------------------------------
const string DC_ID_PREFIX = "DCM_";				//The prefix used for marker and missions Id's.
const string DC_MISSIONCONFIG_FILE = "dc_missionConfig.json";
const string DC_MISSIONCONFIG_FILE_NONVALIDAREA = "dc_nonValidArea.json";

//------------------------------------------------------------------------------------------------
class SDRC_MissionFrame
{
	protected static SDRC_MissionFrame s_Instance;		
	ref array<ref SDRC_Mission> m_MissionList = new array<ref SDRC_Mission>;
	ref SDRC_MissionFrameJsonApi m_DC_MissionFrameJsonApi = new SDRC_MissionFrameJsonApi(DC_MISSIONCONFIG_FILE);
	ref SDRC_MissionFrameConfig m_Config;
	
	ref SDRC_NonValidAreaJsonApi m_DC_NonValidAreaJsonApi = new SDRC_NonValidAreaJsonApi(DC_MISSIONCONFIG_FILE_NONVALIDAREA);
	ref SDRC_NonValidAreaConfig m_NonValidAreaConfig;
	ref array<ref SDRC_NonValidArea> m_aNonValidAreas = {};
	
	private string m_sWorldName;
	private int m_iMissionCountDynamicMax;				//Max amount of dynamic missions
	private int m_iMissionCountStaticMax;				//Max amount of static missions
	private int m_iLastMissionSpawnTime;
	private int m_iStaticTryCount = 0;					//Counter for tries on static missions - both failed and succesful. If m_iStaticTryLimit is reached, we stop trying to spawn static missions
	private int m_iStaticTryLimit = 0;					
		
	ref array<ref SDRC_MissionRequested> m_missionsRequested = {};
	
	static int justAnumber = 15;	//TBD: Just for testing - remove
	
	//------------------------------------------------------------------------------------------------
	void SDRC_MissionFrame()
	{
		SDRC_Log.Add("[SDRC_MissionFrame] Starting SDRC_MissionFrame", LogLevel.NORMAL);
		s_Instance = this;
				
		m_sWorldName = SDRC_Misc.GetWorldName(true);

		//Load configuration from file		
		m_DC_MissionFrameJsonApi.Load();
		m_Config = m_DC_MissionFrameJsonApi.conf;
		
		//Load non valid area configuration from file		
		m_DC_NonValidAreaJsonApi.Load();
		m_DC_NonValidAreaJsonApi.Populate(m_aNonValidAreas);

		//TBD: Difficulty details
		//SDRC_Log.Add("[SDRC_MissionFrame] Difficulty: " + SCR_Enum.GetEnumName(SDRC_EMissionDifficulty, m_Config.difficulty), LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_MissionFrame] Difficulty AI count multiplier: " + m_Config.difficultyAiCountCoefMul, LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_MissionFrame] Difficulty AI skill multiplier: " + m_Config.difficultyAiSkillCoefMul, LogLevel.NORMAL);
		SDRC_Log.Add("[SDRC_MissionFrame] Difficulty AI perception multiplier: " + m_Config.difficultyAiPerceptionCoefMul, LogLevel.NORMAL);
		
		SDRC_RplGMComp gmComp = SDRC_RplGMComp.FindInstance();
		if (gmComp)
		{
			gmComp.UpdateTimeLeft(m_Config.showMissionTimeLeft);
		}
		
		//Checking the enemies found
		SDRC_Log.Add("[SDRC_MissionFrame] Enemy factions: " + m_Config.enemyFactions, LogLevel.NORMAL);
		SDRC_EnemyHelper.SetEnemyFactions(m_Config.enemyFactions);

		//Count amount of dynamic and static missions
		m_iMissionCountDynamicMax = SDRC_MissionHelper.GetMissionCountForWorld(m_Config.missionDynamic.count, m_Config.missionDynamic.countMul);
		SDRC_Log.Add("[SDRC_MissionFrame] Max dynamic mission count: " + m_iMissionCountDynamicMax, LogLevel.NORMAL);		

		m_iMissionCountStaticMax = SDRC_MissionHelper.GetMissionCountForWorld(m_Config.missionStatic.count, m_Config.missionStatic.countMul);
		SDRC_Log.Add("[SDRC_MissionFrame] Max static mission count: " + m_iMissionCountStaticMax, LogLevel.NORMAL);		
				
		//Set some defaults
		m_iStaticTryCount = 0;
		m_iStaticTryLimit = m_iMissionCountStaticMax * 2;
		m_iLastMissionSpawnTime = (System.GetTickCount() / 1000) - m_Config.missionStatic.delayBetween;		//Fix the timer so that first mission immediately spawns		
		
		//Check if a request to create new configs has been made		
		if (m_Config.recreateConfigs)
		{
			SDRC_Log.Add("[SDRC_MissionFrame] ---------------- Creating default configs -------------------", LogLevel.WARNING);
			SDRC_Log.Add("[SDRC_MissionFrame] - Changing recreateConfigs to false and saving the config.  -", LogLevel.WARNING);
			m_Config.recreateConfigs = false;
			m_DC_MissionFrameJsonApi.Save();
			SDRC_Log.Add("[SDRC_MissionFrame] - Creating configs. Existing ones will not be over written. -", LogLevel.WARNING);
			SDRC_MissionEnumHelper.CreateAllConfigs();
			SDRC_Log.Add("[SDRC_MissionFrame] --------------------- Configs created. ----------------------", LogLevel.WARNING
			);
		}
				
		//Fix seconds to ms
		SDRC_Log.Add("[SDRC_MissionFrame] Waiting for " + m_Config.missionStartDelay + " seconds before spawning missions.", LogLevel.NORMAL);
		m_Config.missionStartDelay = m_Config.missionStartDelay * 1000;		//sec to ms
		
		#ifndef SDRC_RELEASE
			//SDRC_DevHelper.SDRC_DevDump();
		#endif	
		
		/*
		//Freedom Fighters part
		JWK_TerritoryControlManagerComponent territoryControlManagerComponent;		
		territoryControlManagerComponent = JWK_CompT<JWK_TerritoryControlManagerComponent>.FindIn(GetGame().GetGameMode());
		if (territoryControlManagerComponent)
		{
			if (territoryControlManagerComponent.IsInitialized()) 
			{
				JWK_EFactionRole role = territoryControlManagerComponent.GetControllingRoleAt("0 0 0");
			}
		}
		*/
		
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
		while (m_MissionList.Count() > 0)
		{
			SDRC_Mission mission = m_MissionList[0];
			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Deleting mission: " + mission.GetId() + " : " + mission.GetTitle(), LogLevel.DEBUG);
			SDRC_DebugHelper.DeleteDebugPos(mission.GetId());
			//delete mission;	//This gives an error...
			mission.MissionEnd();
			m_MissionList.RemoveOrdered(0);			
		}
		SDRC_Log.Add("[~SDRC_MissionFrame] Stopping SDRC_MissionFrame", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Mission life cycle manager.
	*/	
	protected void MissionCycleManager()
	{		
		ref SDRC_Mission tmpDC_Mission = null;
		SDRC_EMissionType missionType = null;
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
				missionType = m_Config.missionStatic.missionTypeArray.GetRandomElement();
			}
			tmpDC_Mission = MissionCreate(missionType);
			if (tmpDC_Mission)
			{
				tmpDC_Mission.SetStatic(true);
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
						missionType = m_Config.missionDynamic.missionTypeArray.GetRandomElement();
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
				//Show marker
				//tmpDC_Mission.ShowMarker();	//TBD: Moved marker creation to ACTIVE state. If all good, remove this line.
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
			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Dynamic missions: " + (m_MissionList.Count() - staticCount) + "/" + m_iMissionCountDynamicMax + " - Static missions: " + staticCount + "/" + m_iMissionCountStaticMax + ". Next mission in " + getMissionDelayWait() + " seconds.", LogLevel.NORMAL);
			MissionDump();
			GetGame().GetCallqueue().CallLater(MissionCycleManager, m_Config.missionFrameCycleTime*1000, false);
		}				
		else
		{
			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Static missions: " + staticCount + "/" + m_iMissionCountStaticMax + " - try/limit: " + m_iStaticTryCount + "/" + m_iStaticTryLimit, LogLevel.NORMAL);
			MissionDump();
			GetGame().GetCallqueue().CallLater(MissionCycleManager, m_Config.missionStatic.delayBetween*1000, false);
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Creates the mission object
	*/		
	protected SDRC_Mission MissionCreate(SDRC_EMissionType missionType)
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
				tmpDC_Mission = new SDRC_Mission_Hunter(missionType, missionRequest);
				break;
			}
			case SDRC_EMissionType.OCCUPATION:
			{
				tmpDC_Mission = new SDRC_Mission_Occupation(missionType, missionRequest);
				break;
			}
			case SDRC_EMissionType.CONVOY:
			{
				tmpDC_Mission = new SDRC_Mission_Convoy(missionType, missionRequest);
				break;
			}
			case SDRC_EMissionType.CRASHSITE:
			{
				tmpDC_Mission = new SDRC_Mission_Crashsite(missionType, missionRequest);
				break;
			}
			case SDRC_EMissionType.PATROL:
			{
				tmpDC_Mission = new SDRC_Mission_Patrol(missionType, missionRequest);
				break;
			}
			case SDRC_EMissionType.SQUATTERS:
			{
				tmpDC_Mission = new SDRC_Mission_Squatter(missionType, missionRequest);
				break;
			}
			case SDRC_EMissionType.ROADBLOCK:
			{
				tmpDC_Mission = new SDRC_Mission_Roadblock(missionType, missionRequest);
				break;
			}			
			case SDRC_EMissionType.HVTVIP:
			{
				tmpDC_Mission = new SDRC_Mission_HvtVip(missionType, missionRequest);
				break;
			}			
			case SDRC_EMissionType.HVTITEM:
			{
				tmpDC_Mission = new SDRC_Mission_HvtItem(missionType, missionRequest);
				break;
			}			
			case SDRC_EMissionType.STASH:
			{
				tmpDC_Mission = new SDRC_Mission_Stash(missionType, missionRequest);
				break;
			}			
/*			case SDRC_EMissionType.CHOPPER:
			{
				tmpDC_Mission = new SDRC_Mission_Chopper(missionType, missionRequest);
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
		int cutLen = 32;
		
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
			if (mission.IsRequested())
			{
				staticString = staticString + "(req)";
			}
			string missionType = SCR_Enum.GetEnumName(SDRC_EMissionType, mission.GetType());
			string missionTitle = mission.GetTitle();
			if (missionTitle.Length() > cutLen)
			{
				missionTitle = mission.GetTitle().Substring(0, cutLen) + "..";
			}
			string missionState = SCR_Enum.GetEnumName(SDRC_EMissionState,  mission.GetState());
			
			SDRC_Log.Add("[SDRC_MissionDump] " + i + ": " + mission.GetId() + " (" + missionType + ", " + staticString + ", " + missionState + ") - " + missionTitle + " - " + "Time left: " + mission.GetActiveTime(), LogLevel.NORMAL);
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
}