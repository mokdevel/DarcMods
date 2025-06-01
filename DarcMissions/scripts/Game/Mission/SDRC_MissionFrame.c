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

class SDRC_MissionRequested : Managed
{
	EntityID entityID;
	vector pos;				//Just for debugging purposes. Entity may have been moved so we read the pos before spawning mission.
	//DC_EMissionType type;
}

//------------------------------------------------------------------------------------------------
class SDRC_MissionFrame
{
	protected static SDRC_MissionFrame s_Instance;		
	ref array<ref SDRC_Mission> m_MissionList = new array<ref SDRC_Mission>;
	ref SDRC_MissionFrameJsonApi m_DC_MissionFrameJsonApi = new SDRC_MissionFrameJsonApi();
	ref SDRC_MissionFrameConfig m_Config;
	
	ref SDRC_NonValidAreaJsonApi m_DC_NonValidAreaJsonApi = new SDRC_NonValidAreaJsonApi();
	ref SDRC_NonValidAreaConfig m_NonValidAreaConfig;
	ref array<ref SDRC_NonValidArea> m_aNonValidAreas = {};
	
	private string m_sWorldName;
	private int m_iMissionCount;
	private int m_iMissionCountStatic;					//Amount of static missions
	private int m_iLastMissionSpawnTime;
	private int m_iStaticTryCount = 0;					//Counter for tries on static missions - both failed and succesful. If m_iStaticTryLimit is reached, we stop trying to spawn static missions
	private int m_iStaticTryLimit = 0;					
		
	ref array<ref SDRC_MissionRequested> m_missionsRequested = {};
	
	//------------------------------------------------------------------------------------------------
	void SDRC_MissionFrame()
	{
		SDRC_Log.Add("[SDRC_MissionFrame] Starting SDRC_MissionFrame", LogLevel.NORMAL);
		s_Instance = this;
		
		m_sWorldName = SDRC_Misc.GetWorldName(true);

		//Load configuration from file		
		m_DC_MissionFrameJsonApi.Load();
		m_Config = m_DC_MissionFrameJsonApi.conf;
		
		//Check if a request to create new logs has been made		
		if (m_Config.recreateConfigs)
		{
			SDRC_Log.Add("[SDRC_MissionFrame] ---------------- Creating default configs -------------------", LogLevel.WARNING);
			SDRC_Log.Add("[SDRC_MissionFrame] - Changing recreateConfigs to false and saving the config.  -", LogLevel.WARNING);
			m_Config.recreateConfigs = false;
			m_DC_MissionFrameJsonApi.Save("");
			SDRC_Log.Add("[SDRC_MissionFrame] - Creating configs. Existing ones will not be over written. -", LogLevel.WARNING);
			CreateAllConfigs();
			SDRC_Log.Add("[SDRC_MissionFrame] --------------------- Configs created. ----------------------", LogLevel.WARNING
			);
		}
		
		//Load non valid area configuration from file		
		m_DC_NonValidAreaJsonApi.Load();
		m_DC_NonValidAreaJsonApi.Populate(m_aNonValidAreas);

		//Checking the enemies found
		SDRC_Log.Add("[SDRC_MissionFrame] Enemy factions: " + m_Config.enemyFactions, LogLevel.NORMAL);
		SDRC_EnemyHelper.SetEnemyFactions(m_Config.enemyFactions);

		//Count amount of dynamic and static missions
		m_iMissionCount = GetMissionCount(m_Config.missionDynamic.count, m_Config.missionDynamic.countMul);
		SDRC_Log.Add("[SDRC_MissionFrame] Max dynamic mission count: " + m_iMissionCount, LogLevel.NORMAL);		

		m_iMissionCountStatic = GetMissionCount(m_Config.missionStatic.count, m_Config.missionStatic.countMul);
		SDRC_Log.Add("[SDRC_MissionFrame] Max static mission count: " + m_iMissionCountStatic, LogLevel.NORMAL);		
				
		//Set some defaults
		m_iStaticTryCount = 0;
		m_iStaticTryLimit = m_iMissionCountStatic + 5;	//We may fail 5 times
		m_iLastMissionSpawnTime = (System.GetTickCount() / 1000) - m_Config.missionDelayBetweeen;	//Fix the timer so that first mission immediately spawns
				
		//Fix seconds to ms
		SDRC_Log.Add("[SDRC_MissionFrame] Waiting for " + m_Config.missionStartDelay + " seconds before spawning missions.", LogLevel.NORMAL);
		m_Config.missionStartDelay = m_Config.missionStartDelay * 1000;		//sec to ms
		
		#ifndef SDRC_RELEASE
			SDRC_MapMarkerHelper.CreateMapMarker("1000 0 3000", DC_EMissionIcon.N_FENCE, "DMC_B", "Here is a text");
			SDRC_MapMarkerHelper.CreateMapMarker("1200 0 3500", DC_EMissionIcon.N_HOUSE, "DMC_B", "Darc_SK");
			SDRC_MapMarkerHelper.CreateMapMarker("1500 0 3200", DC_EMissionIcon.N_HELI, "DMC_B", "This is a description for a mission");
		
/*			for (int i = 0;i < 150; i++)
			{
				vector pos = SDRC_Misc.GetRandomWorldPos();
				pos = SDRC_SpawnHelper.FindEmptyPos(pos, 300, 30);			
			}*/
		
			//vector pos = SDRC_SpawnHelper.FindEmptyPos("1990 0 2330", 300, 60);			
		
		/*
			vector pos;
			pos[0] = SDRC_Misc.GetWorldSize() / 2;
			pos[1] = 0;
			pos[2] = pos[0];
			pos = SDRC_SpawnHelper.FindEmptyPos(pos, 300, 5000);			*/
				
		#endif	
		
		//GetGame().GetCallqueue().CallLater(SendHint, 15000, true);
		GetGame().GetCallqueue().CallLater(FindGMSpawnedMissions, 5000, true);
		
		//Start the mission framework.
		GetGame().GetCallqueue().CallLater(MissionCycleManager, m_Config.missionStartDelay, false);
	}
	
	//------------------------------------------------------------------------------------------------
	static SDRC_MissionFrame GetInstance()
	{
		return s_Instance;		
	}	
	
	//------------------------------------------------------------------------------------------------
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
	Mission life cycle manager.
	*/	
	protected void MissionCycleManager()
	{			
		ref SDRC_Mission tmpDC_Mission = null;
		DC_EMissionType missionType = null;
		bool staticMissionSpawned = false;
		
		//Check if static missions have been spawned
		if (CountStaticMissions() < m_iMissionCountStatic && m_iStaticTryCount < m_iStaticTryLimit)
		{
			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Spawning new static mission", LogLevel.NORMAL);
			
			missionType = m_Config.missionStatic.missionTypeArray.GetRandomElement();
			tmpDC_Mission = MissionCreate(missionType);
			if (tmpDC_Mission)
			{
				tmpDC_Mission.SetStatic(true);
				tmpDC_Mission.SetActiveTime(m_Config.missionStatic.activeTime);
				tmpDC_Mission.ResetActiveTime();
			}
			else
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] MissionCreate failed (static): " + SCR_Enum.GetEnumName(DC_EMissionType, missionType), LogLevel.WARNING);
			}
			
			staticMissionSpawned = true;	//Static missions to be spawned faster at startup
			m_iStaticTryCount++;			//We increase this even if the mission start failed for static
		}
		else
		{
			//Check if more dynamic or GM missions are to be spawned
			if ( ( (m_MissionList.Count() < m_iMissionCount) && (isMissionDelayPassed()) && SDRC_PlayerHelper.PlayerCount() > 0 ) || (!m_missionsRequested.IsEmpty()) )
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Spawning new dynamic mission", LogLevel.NORMAL);
				
				//Select a new mission to spawn.
				
				//GM has not requested any missions to spawn, go for a regular dynamic one
				if (m_missionsRequested.IsEmpty())
				{
					missionType = m_Config.missionDynamic.missionTypeArray.GetRandomElement();
				}
				else	//Spawn a GM requested mission
				{
					missionType = DC_EMissionType.REQUESTED;
				}
				
				//Do the spawning
				tmpDC_Mission = MissionCreate(missionType);
				if (tmpDC_Mission)
				{
					tmpDC_Mission.SetActiveTime(m_Config.missionDynamic.activeTime);
					tmpDC_Mission.ResetActiveTime();
				}
				else
				{
					SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] MissionCreate failed (dynamic): " + SCR_Enum.GetEnumName(DC_EMissionType, missionType), LogLevel.WARNING);
				}
			}
		}

		//Mission is ready to be run. Finalize the last details
		if (tmpDC_Mission)
		{
			//Add to list
			m_MissionList.Insert(tmpDC_Mission);
			
			//If there was an error starting the mission, it has been prepared for deletion.
			if (tmpDC_Mission.GetState() != DC_EMissionState.FAILED)
			{		
				//Set the defaul active distance
				tmpDC_Mission.SetActiveDistance(m_Config.missionActiveDistance);
				tmpDC_Mission.SetActiveTimeToEnd(m_Config.missionActiveTimeToEnd);
				//Set mission to start to run
				m_MissionList[m_MissionList.Count() - 1].MissionRun();
			
				SDRC_Log.Add(string.Format("[SDRC_MissionFrame:MissionCycleManager] Spawning mission %1 (%2) %3", tmpDC_Mission.GetTitle(), tmpDC_Mission.GetPos(), tmpDC_Mission.GetPosName()), LogLevel.NORMAL);

				if (!staticMissionSpawned)
				{
					if (m_Config.missionHintTime > 0 && tmpDC_Mission.IsShowHint())
					{
						SDRC_HintHelper.ShowHint("Mission: " + tmpDC_Mission.GetTitle(), tmpDC_Mission.GetInfo(), m_Config.missionHintTime);					
					}
				}
								
				SDRC_DebugHelper.AddDebugPos(tmpDC_Mission.GetPos(), ARGB(20, 255, 255, 0), 10, tmpDC_Mission.GetId());
				
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
						
			if (mission.GetState() == DC_EMissionState.FAILED)
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Mission start failed: " + mission.GetId() + " (" + SCR_Enum.GetEnumName(DC_EMissionType, mission.GetType()) + ")", LogLevel.WARNING);
			}

			if (mission.GetState() == DC_EMissionState.EXIT || mission.GetState() == DC_EMissionState.FAILED)
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
			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Dynamic missions: " + (m_MissionList.Count() - staticCount) + "/" + m_iMissionCount + " - Static missions: " + staticCount + "/" + m_iMissionCountStatic + ". Next mission in " + getMissionDelayWait() + " seconds.", LogLevel.NORMAL);
			MissionDump();
			GetGame().GetCallqueue().CallLater(MissionCycleManager, m_Config.missionFrameCycleTime*1000, false);
		}				
		else
		{
			SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Static missions: " + staticCount + "/" + m_iMissionCountStatic + " - try/limit: " + m_iStaticTryCount + "/" + m_iStaticTryLimit, LogLevel.NORMAL);
			MissionDump();
			GetGame().GetCallqueue().CallLater(MissionCycleManager, 5000, false);
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Creates the mission object
	*/		
	protected SDRC_Mission MissionCreate(DC_EMissionType missionType)
	{
		SDRC_Mission tmpDC_Mission = null;
		vector pos = "0 0 0";
		
		if (missionType == DC_EMissionType.REQUESTED)
		{					
			CleanMissionsRequestedArray();
	
			//Is the list empty?		
			if (m_missionsRequested.IsEmpty())
			{
				return null;
			}
			
			SDRC_MissionRequested missionRequest = m_missionsRequested[0];
			
			IEntity missionEntity = GetGame().GetWorld().FindEntityByID(missionRequest.entityID);
			SDRC_DarcMissionRequestComp requestComp = SDRC_DarcMissionRequestComp.Cast(missionEntity.FindComponent(SDRC_DarcMissionRequestComp));
			missionType = requestComp.GetMissionType();
			pos = missionEntity.GetOrigin();
			SDRC_SpawnHelper.DespawnItem(missionEntity);
		}
		
		SDRC_Log.Add("[SDRC_MissionFrame:MissionCreate] Starting mission of type: " + SCR_Enum.GetEnumName(DC_EMissionType, missionType), LogLevel.DEBUG);
		
		switch (missionType)
		{
			case DC_EMissionType.NONE:
			{
				SDRC_Log.Add("[SDRC_MissionFrame:MissionCycleManager] Mission of type NONE ignored.", LogLevel.DEBUG);
				break;
			}
			case DC_EMissionType.HUNTER:
			{
				tmpDC_Mission = new SDRC_Mission_Hunter(pos);
				break;
			}
			case DC_EMissionType.OCCUPATION:
			{
				tmpDC_Mission = new SDRC_Mission_Occupation(pos);
				break;
			}
			case DC_EMissionType.CONVOY:
			{
				tmpDC_Mission = new SDRC_Mission_Convoy(pos);
				break;
			}
			case DC_EMissionType.CRASHSITE:
			{
				tmpDC_Mission = new SDRC_Mission_Crashsite(pos);
				break;
			}
			case DC_EMissionType.PATROL:
			{
				tmpDC_Mission = new SDRC_Mission_Patrol(pos);
				break;
			}
			case DC_EMissionType.SQUATTER:
			{
				tmpDC_Mission = new SDRC_Mission_Squatter(pos);
				break;
			}			
/*			case DC_EMissionType.CHOPPER:
			{
				tmpDC_Mission = new SDRC_Mission_Chopper(pos);
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
	
	//------------------------------------------------------------------------------------------------
	/*!
	Counts the amount of missions for the map
	\param count
	\param mul
	*/	
	protected int GetMissionCount(int count, float mul)	
	{
		if (count == -1)
		{			
			count = (SDRC_Misc.GetWorldSize() * mul) / 1000;
			SDRC_Log.Add("[SDRC_MissionFrame:GetMissionCount] Count = (Worldsize) " + SDRC_Misc.GetWorldSize() + " * " + mul + " / 1000 = " + count, LogLevel.DEBUG);			
		}
		
		return count;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Search the map for GM spawned missions
	*/	
	void FindGMSpawnedMissions()
	{
		int size = SDRC_Misc.GetWorldSize();
		vector pos = "0 0 0";
		pos[0] = size / 2;
		pos[2] = size / 2;
		GetGame().GetWorld().QueryEntitiesBySphere(pos, size / 2, FindGMSpawnedMissionsCallback, null, EQueryEntitiesFlags.STATIC);
		
		//dumpMissionRequested();
		CleanMissionsRequestedArray();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Callback for the search of GM spawned missions
	*/		
	bool FindGMSpawnedMissionsCallback(IEntity entity)
	{
		if (entity.ClassName() == "SDCR_DarcMissionGM")
		{
			SDCR_DarcMissionGM ent = SDCR_DarcMissionGM.Cast(entity);
			if (!ent.IsAdded())
			{
				ent.AddedToList();
				
				ref SDRC_MissionRequested mission = new SDRC_MissionRequested();
				
				mission.entityID = entity.GetID();
				mission.pos = entity.GetOrigin();
				m_missionsRequested.Insert(mission);
				
				ResourceName res = entity.GetPrefabData().GetPrefabName();
				SDRC_Log.Add("[SDRC_MissionFrame:FindGMSpawnedMissions] Found: " + res + " at " + entity.GetOrigin(), LogLevel.DEBUG);
			}
		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------	
	/*!
	Remove deleted missions from the list
	*/		
	void CleanMissionsRequestedArray()
	{
		int i = 0;
		
		while (i < m_missionsRequested.Count())
		{
			IEntity entity = GetGame().GetWorld().FindEntityByID(m_missionsRequested[i].entityID);
			if (!entity)
			{
				m_missionsRequested.Remove(i);
			}
			else
			{
				i++;
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Dump information of GM spawned missions
	*/			
	void dumpMissionRequested()
	{
		foreach (SDRC_MissionRequested mission : m_missionsRequested)
		{
			IEntity entity = GetGame().GetWorld().FindEntityByID(mission.entityID);
			//SDCR_ReplicatedParticleEffectEntity ent
			
			if (entity)
			{
				SDRC_Log.Add("[SDRC_MissionFrame:dumpGMSpawnedMissions] " + entity + " at " + mission.pos, LogLevel.DEBUG);
			}
			else
			{
				SDRC_Log.Add("[SDRC_MissionFrame:dumpGMSpawnedMissions] Deleted at: " + mission.pos, LogLevel.DEBUG);
			}
			
		}		
	}	
}