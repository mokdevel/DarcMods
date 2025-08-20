//Mission SDRC_Mission_Hunter.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns one (or more) hunter groups that search players. Original idea and parts of the 
code are from HunterKiller mod by Rabid Squirrel. This mod worked as a great example when taking the 
first steps in AR modding.

See: https://reforger.armaplatform.com/workshop/597324ECFC025225-HunterKiller 
Note: The original HunterKiller mod is discontinued.
*/

//TBD: It should be possible to define multiple different hunter types.

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Hunter : SDRC_Mission
{
	private ref SDRC_HunterJsonApi m_HunterJsonApi = new SDRC_HunterJsonApi();				
	private ref SDRC_HunterConfig m_Config;	
	private ref SDRC_Hunter m_DC_Hunter;		//Hunter configuration in use
	
	private const int DC_LOCATION_SEACRH_ITERATIONS = 10;	//How many different spots to try for a mission before giving up	
	private const int DC_GROUP_SPAWN_DELAY = 2000;			//Delay between AI group spawns
	
	private ref array<IEntity> m_Locations = {};
	private int m_iGroupsToSpawn	= 0;	//Amount of groups to spawn
	private int m_iGroupsSpawned = 0;	//The amount of groups spawned. Between spawns, a group may be killed so the total of m_Groups is not reliable to know the count.
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Hunter(vector pos = "0 0 0")
	{
		SDRC_Log.Add("[SDRC_Mission_Hunter] Constructor", LogLevel.SPAM);

		//Set some defaults				
		SetType(DC_EMissionType.HUNTER);
		
		//Load config	
		m_HunterJsonApi.Load();
		m_Config = m_HunterJsonApi.conf;
		
		//Pick a configuration for mission
		int idx = SDRC_MissionHelper.SelectMissionIndex(m_Config.hunterList);
		if (idx == -1)
		{
			SDRC_Log.Add("[SDRC_Mission_Hunter] No hunters defined.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}
		m_DC_Hunter = m_Config.hunters[idx];
		
		m_iGroupsToSpawn = Math.RandomInt(m_DC_Hunter.groupCount[0], m_DC_Hunter.groupCount[1]);
		
		//Find position
		bool positionFound = false;

		if (!IsRequested())
		{
			for (int i = 0; i < DC_LOCATION_SEACRH_ITERATIONS; i++)
			{
				pos = SDRC_Misc.GetRandomWorldPos();
							
				if (SDRC_MissionHelper.IsValidMissionPos(pos))
				{			
					//Find a position close to any player
					if (SDRC_PlayerHelper.IsAnyPlayerCloseToPos(pos, m_Config.maxDistanceToPlayer, m_Config.minDistanceToPlayer))
					{
						positionFound = true;
					
						SDRC_Log.Add("[SDRC_Mission_Hunter] Location for spawn " + pos, LogLevel.DEBUG);
						break;
					}
					else
					{						
						SDRC_Log.Add("[SDRC_Mission_Hunter] Invalid mission position. Try " + (i + 1) + "/" + DC_LOCATION_SEACRH_ITERATIONS, LogLevel.SPAM);
					}
				}
			}
		}
		else
		{
			//Requested positions are blindly accepted
			positionFound = true;
		}
		
		if (!positionFound)	//No suitable location found.
		{				
			SDRC_Log.Add("[SDRC_Mission_Hunter] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}	
				
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, m_DC_Hunter.general.posName));
		SetMarker(m_Config.showMarker, m_Config.markerIdx, m_Config.markerType);
		SetHint(m_Config.showHint, m_DC_Hunter.general.title, m_DC_Hunter.general.info);		
		SetMessages(m_Config.showMessage, m_DC_Hunter.general.winMessage, m_DC_Hunter.general.loseMessage);		
		SetWinCondition(m_DC_Hunter.general.winCondition);
	}

	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == DC_EMissionState.SPAWN)
		{
			MissionSpawn();
			SetState(DC_EMissionState.ACTIVE);
		}
		
		if (GetState() == DC_EMissionState.END)
		{
			MissionEnd();
			SetState(DC_EMissionState.EXIT);
		}
		
		if (GetState() == DC_EMissionState.ACTIVE)
		{
			if (m_iGroupsSpawned < m_iGroupsToSpawn)
			{
				SDRC_Log.Add("[SDRC_Mission_Hunter:MissionRun] Waiting for all groups to spawn. " + m_iGroupsSpawned + "/" + m_iGroupsToSpawn + " ready.", LogLevel.DEBUG);
			}
			else
			{
				if (!IsActive(true))
				{
					SetState(DC_EMissionState.END);
				}
			}
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionCycleTime*1000);
	}

	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{
		super.MissionEnd();
	}	
			
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		SDRC_Log.Add(("[SDRC_Mission_Hunter:MissionSpawn] Spawning " + m_iGroupsToSpawn + " groups"), LogLevel.NORMAL);
		
		for (int i = 1; i <= m_iGroupsToSpawn; i++)
		{
			SDRC_Log.Add(("[SDRC_Mission_Hunter:MissionSpawn] Initiating spawn for group " + i + " of " + m_iGroupsToSpawn), LogLevel.DEBUG);
			
			GetGame().GetCallqueue().CallLater(SpawnGroup, (DC_GROUP_SPAWN_DELAY + i*1000), false);
		}
		
		SDRC_Log.Add("[SDRC_Mission_Hunter:MissionSpawn] INIT ready. Changing to ACTIVE state", LogLevel.DEBUG);		
	}

	//------------------------------------------------------------------------------------------------
	protected void GroupLifeCycle(SCR_AIGroup group)
	{
		if (group)
		{
			//Check if there are any nearby AI
			IEntity closestPlayer = SDRC_PlayerHelper.PlayerGetClosestToPos(group.GetLeaderEntity().GetOrigin(), 0, m_Config.maxDistanceToPlayer);
		
			if (closestPlayer != null)
			{
				if (group.GetAgentsCount() > 0)
				{
					SDRC_Log.Add("[SDRC_Mission_Hunter:GroupLifeCycle] Creating waypoint for group: " + group.GetID(), LogLevel.SPAM);
					
					SDRC_WPHelper.RemoveWaypoints(group);
					AIWaypoint wp = GetWaypoint(group);
					group.AddWaypoint(wp);
					GetGame().GetCallqueue().CallLater(GroupLifeCycle, m_Config.missionCycleTime*1000, false, group);
					return;
				}
			}
			else
			{
				// If there aren't any players close, delete the group
				SDRC_Log.Add("[SDRC_Mission_Hunter:GroupLifeCycle] No players nearby, deleting group: " + group.GetID(), LogLevel.NORMAL);
				SDRC_AIHelper.GroupDelete(group);
			}
		}
	}	
		
	//------------------------------------------------------------------------------------------------	
	protected void SpawnGroup()
	{
		vector spawnLocation = GetSpawnPointForAI();
		
		if (spawnLocation)
		{
			string groupToSpawn = m_DC_Hunter.groupTypes.GetRandomElement();
			SCR_AIGroup group = SDRC_AIHelper.SpawnGroup(groupToSpawn, spawnLocation, GetFaction());
			
			if (group)
			{
				SDRC_AIHelper.SetAIGroupSkill(group, m_DC_Hunter.aiSkill, m_DC_Hunter.aiPerception);					
				m_Groups.Insert(group);
				m_iGroupsSpawned++;
				SDRC_Log.Add("[SDRC_Mission_Hunter:SpawnHunterGroup] Group spawned to " + spawnLocation, LogLevel.NORMAL);				
			}
			else
			{
				SDRC_Log.Add("[SDRC_Mission_Hunter:SpawnHunterGroup] Unable to spawn group!", LogLevel.ERROR);
				return;
			}
			
			// Manage the life cycle for the spawned group
			GetGame().GetCallqueue().CallLater(GroupLifeCycle, m_Config.missionCycleTime*1000, false, group);
		}
		else
		{
			SDRC_Log.Add("[SDRC_Mission_Hunter:SpawnHunterGroup] Unable to find spawn point for group! Retrying...", LogLevel.WARNING);
			
			// Try again later
			GetGame().GetCallqueue().CallLater(SpawnGroup, DC_GROUP_SPAWN_DELAY, false);
		}
	}

	//------------------------------------------------------------------------------------------------	
	/*!
	Find a spawn position for hunters that is close to mission position.
	*/
	protected vector GetSpawnPointForAI()
	{
		vector pos = GetPos();
		
		if (SDRC_SpawnHelper.FindEmptyPos(pos, 100, 8))
		{						
			SDRC_Log.Add("[SDRC_Mission_Hunter:GetSpawnPointForAI] Pos: " + pos, LogLevel.SPAM);
		}
		else
			SDRC_Log.Add("[SDRC_Mission_Hunter:GetSpawnPointForAI] Could not find an empty pos.", LogLevel.ERROR);
		
		return pos;
	}
		
	//------------------------------------------------------------------------------------------------
	protected AIWaypoint GetWaypoint(SCR_AIGroup group)
	{
		IEntity closestPlayer = SDRC_PlayerHelper.PlayerGetClosestToPos(group.GetLeaderEntity().GetOrigin());
		
		if (closestPlayer != null)
		{
			AIWaypoint waypoint = SDRC_WPHelper.CreateWaypointEntity(DC_EWaypointMoveType.MOVE);
			waypoint.SetOrigin(SDRC_Misc.RandomizePos(closestPlayer.GetOrigin(), m_Config.rndDistanceToPlayer));
			return waypoint;
		}
		else
		{
			SDRC_Log.Add("[SDRC_Mission_Hunter:GetWaypoint] Unable to find player for waypoint creation!", LogLevel.ERROR);
			SDRC_AIHelper.GroupDelete(group);
		}
		
		return null;
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_HunterConfig : SDRC_MissionConfig
{
	int minDistanceToPlayer;				//Hunter group minimum distance to player for spawn
	int maxDistanceToPlayer;				//...max distance to despawn
	int rndDistanceToPlayer;				//The error on the location where AI thinks you are. (0..rndDistanceToPlayer)  
	ref array<ref int> hunterList = {};				//The indexes of hunters.
	ref array<ref SDRC_Hunter> hunters = {};		//List of hunters
}

class SDRC_Hunter : Managed
{
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref array<int> groupCount = {1, 2};
	ref array<string> groupTypes = {}; 		//The prefab names of AI groups or characters. The AI is randomly picked from this list.
	int aiSkill;							//Skill for AI (0-100). See SCR_AICombatComponent and EAISkill
	float aiPerception;		
	
	void Set(array<int> groupCount_, array<string> groupTypes_, int AISkill_, float aiPerception_)
	{
		groupCount = groupCount_;
		groupTypes = groupTypes_;
		aiSkill = AISkill_;
		aiPerception = aiPerception_;				
	}
				
}

//------------------------------------------------------------------------------------------------
class SDRC_HunterJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Hunter.json";
	ref SDRC_HunterConfig conf = new SDRC_HunterConfig();
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_MISSIONCONFIG_FILE);
		
		if (!loadContext)
		{
			SetDefaults();
			Save("");
			return;
		}
		
		loadContext.ReadValue("", conf);
	}	

	//------------------------------------------------------------------------------------------------
	void Save(string data)
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen(DC_MISSIONCONFIG_FILE);
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}	
		
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		//Default
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT * 3;		//The cycle with Hunter mission can be really slow
		conf.markerIdx = DC_EMissionIcon.GM_MISSION_X_MAP;
		conf.showMarker = false;
		//Mission specific
		conf.minDistanceToPlayer = 200;
		conf.maxDistanceToPlayer = 800;
		conf.rndDistanceToPlayer = 60;
		conf.hunterList = {0,0,0,1};
		
		//----------------------------------------------------
		conf.hunters.Insert(Hunter0());				
		conf.hunters.Insert(Hunter1());				
	}
	
	//----------------------------------------------------
	SDRC_Hunter Hunter0()
	{		
		ref SDRC_Hunter hunter = new SDRC_Hunter();
		hunter.general.Set(
			"index 0: general mission",
			{"0 0 0"},
			"any",
			"Hunters",
			"They are coming for you... Last time they were seen close to %l.",
			DC_EMissionWinCondition.AI_KILL_ALL,
			"You outsmarted the hunters.",
			"Hunters lost track of you.", 
			0
		);
		hunter.Set(
			{1, 4},
			{
				"G_SMALL"
			},
			30, 0.7
		);
		return hunter;
	}	
	//----------------------------------------------------
	SDRC_Hunter Hunter1()
	{		
		ref SDRC_Hunter hunter = new SDRC_Hunter();
		hunter.general.Set(
			"index 1: general mission",
			{"0 0 0"},
			"any",
			"Hunters",
			"Sharpshooters are hunting you. Last known location was near %l.",
			DC_EMissionWinCondition.AI_KILL_ALL,
			"Sharpshooters were not that sharp.",
			"Sharpshooters lost track of you.", 
			0
		);
		hunter.Set(
			{1, 1},
			{
				"G_SNIPER"
			},
			40, 0.8
		);
		return hunter;
	}		
}