//Mission SCR_DC_Mission_Hunter.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns one (or more) hunter groups that search players. Original idea and parts of the 
code are from HunterKiller mod by Rabid Squirrel. This mod worked as a great example when taking the 
first steps in AR modding.

See: https://reforger.armaplatform.com/workshop/597324ECFC025225-HunterKiller 
Note: The original mod is discontinued.
*/

//------------------------------------------------------------------------------------------------
class SCR_DC_Mission_Hunter : SCR_DC_Mission
{
	private ref SCR_DC_HunterJsonApi m_HunterJsonApi = new SCR_DC_HunterJsonApi();				
	private ref SCR_DC_HunterConfig m_Config;
	private const int DC_LOCATION_SEACRH_ITERATIONS = 30;	//How many different spots to try for a mission before giving up	
	
	private ref array<IEntity> m_Locations = {};
	private ref array<SCR_AIGroup> m_Groups = {};
	int m_GroupsSpawned = 0;	//The amount of groups spawned. Between spawns, a group may be killed so the total of m_Groups is not reliable to know the count.
	
	//------------------------------------------------------------------------------------------------
	//Constructor
	void SCR_DC_Mission_Hunter()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Hunter] Constructor", LogLevel.DEBUG);

		//Set some defaults				
		SCR_DC_Mission();

		//Load config	
		m_HunterJsonApi.Load();
		m_Config = m_HunterJsonApi.conf;
		
		//Find position
		bool positionFound = false;
		vector pos;

		for (int i = 0; i < DC_LOCATION_SEACRH_ITERATIONS; i++)
		{
			pos = SCR_DC_Misc.GetRandomWorldPos();
			
			//Find a position close to any player
			if (SCR_DC_PlayerHelper.IsAnyPlayerCloseToPos(pos, m_Config.maxDistanceToPlayer, m_Config.minDistanceToPlayer))
			{
				positionFound = true;
			
				SCR_DC_Log.Add("[SCR_DC_Mission_Hunter] Location for spawn " + pos, LogLevel.DEBUG);
				break;
			}
			else
			{						
				SCR_DC_Log.Add("[SCR_DC_Mission_Hunter] Invalid mission position. Try " + (i + 1) + "/" + DC_LOCATION_SEACRH_ITERATIONS, LogLevel.SPAM);
			}
		}
		
		if (positionFound)
		{	
			SetTitle("Hunters");
			SetInfo("They are coming for you...");
			SetPos(pos);
			SetPosName("");
			SetMarkerId(SCR_DC_MapMarkersUI.AddMarker(DC_ID_PREFIX, GetPos(), GetTitle()));
	
			SetState(DC_MissionState.INIT);
		}
		else
		{				
			//No suitable location found.
			SCR_DC_Log.Add("[SCR_DC_Mission_Hunter] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;
		}	
	}

	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		if (GetState() == DC_MissionState.INIT)
		{
			MissionSpawn();
			SetState(DC_MissionState.ACTIVE);
		}
		
		if (GetState() == DC_MissionState.END)
		{
			MissionEnd();
			SetState(DC_MissionState.EXIT);
		}
		
		if (GetState() == DC_MissionState.ACTIVE)
		{
			if (m_GroupsSpawned < m_Config.groupsToSpawn)
			{
				SCR_DC_Log.Add("[SCR_DC_Mission_Hunter:MissionRun] Waiting for all groups to spawn. " + m_GroupsSpawned + "/" + m_Config.groupsToSpawn + " ready.", LogLevel.DEBUG);
			}
			else
			{
				if (SCR_DC_AIHelper.AreAllGroupsDead(m_Groups))
				{
					SCR_DC_Log.Add("[SCR_DC_Mission_Hunter:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
					SetState(DC_MissionState.END);
				}
			}
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionLifeCycleTime*1000);
	}

	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{
		//Remove marker from map
		SCR_DC_MapMarkersUI.DeleteMarker(GetMarkerId());
		//TBD: Delete all AI groups. This case is needed in case the mission is stopped by an external force (e.g., admin).
		SCR_DC_Log.Add("[SCR_DC_Mission_Hunter:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);
	}	
			
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		SCR_DC_Log.Add(("[SCR_DC_Mission_Hunter:MissionSpawn] Spawning " + m_Config.groupsToSpawn + " groups"), LogLevel.NORMAL);
		
		for (int i = 1; i <= m_Config.groupsToSpawn; i++)
		{
			SCR_DC_Log.Add(("[SCR_DC_Mission_Hunter:MissionSpawn] Initiating spawn for group " + i + " of " + m_Config.groupsToSpawn), LogLevel.NORMAL);
			
			GetGame().GetCallqueue().CallLater(SpawnGroup, (m_Config.groupSpawnDelay + i*1000), false);
		}
		
		SCR_DC_Log.Add("[SCR_DC_Mission_Hunter:MissionSpawn] INIT ready. Changing to ACTIVE state", LogLevel.NORMAL);		
	}

	//------------------------------------------------------------------------------------------------
	protected void GroupLifeCycle(SCR_AIGroup group)
	{
		if (group)
		{
			//Check if there are any nearby AI
			IEntity closestPlayer = SCR_DC_PlayerHelper.PlayerGetClosestToPos(group.GetLeaderEntity().GetOrigin(), 0, m_Config.maxDistanceToPlayer);
		
			if (closestPlayer != null)
			{
				if (group.GetAgentsCount() > 0)
				{
					SCR_DC_Log.Add("[SCR_DC_Mission_Hunter:GroupLifeCycle] Creating waypoint for group: " + group.GetID(), LogLevel.SPAM);
					
					SCR_DC_AIHelper.RemoveWaypoints(group);
					AIWaypoint wp = GetWaypoint(group);
					group.AddWaypoint(wp);
					GetGame().GetCallqueue().CallLater(GroupLifeCycle, m_Config.lifeCycleTime, false, group);
					return;
				}
			}
			else
			{
				// If there aren't any players close end the group
				SCR_DC_Log.Add("[SCR_DC_Mission_Hunter:GroupLifeCycle] No players nearby, deleting group group: " + group.GetID(), LogLevel.NORMAL);
				SCR_DC_AIHelper.GroupDelete(group);
			}
		}
	}	
		
	//------------------------------------------------------------------------------------------------	
	protected void SpawnGroup()
	{
		vector spawnLocation = GetSpawnPointForAI();
		
		if (spawnLocation)
		{
			string groupToSpawn = m_Config.groupTypes.GetRandomElement();
			SCR_AIGroup group = SCR_DC_AIHelper.SpawnGroup(groupToSpawn, spawnLocation);
			
			if (group)
			{
				m_Groups.Insert(group);
				m_GroupsSpawned++;
				SCR_DC_Log.Add("[SCR_DC_Mission_Hunter:SpawnHunterGroup] Group spawned to " + spawnLocation, LogLevel.NORMAL);				
			}
			else
			{
				SCR_DC_Log.Add("[SCR_DC_Mission_Hunter:SpawnHunterGroup] Unable to spawn group!", LogLevel.ERROR);
				return;
			}
			
			// Manage the life cycle for the spawned group
			GetGame().GetCallqueue().CallLater(GroupLifeCycle, m_Config.lifeCycleTime, false, group);
		}
		else
		{
			SCR_DC_Log.Add("[SCR_DC_Mission_Hunter:SpawnHunterGroup] Unable to find spawn point for group! Retrying...", LogLevel.WARNING);
			
			// Try again later
			GetGame().GetCallqueue().CallLater(SpawnGroup, (m_Config.lifeCycleTime*2), false);
		}
	}

	//------------------------------------------------------------------------------------------------	
	/*!
	Find a spawn position for hunters that is close to mission position.
	*/
	protected vector GetSpawnPointForAI()
	{
		vector pos = GetPos();
		vector posFixed = SCR_DC_SpawnHelper.FindEmptyPos(pos, 100, 8);
		
		if (pos != posFixed)
		{						
			SCR_DC_Log.Add("[SCR_DC_Mission_Hunter:GetSpawnPointForAI] Pos: " + pos, LogLevel.SPAM);
		}
		else
			SCR_DC_Log.Add("[SCR_DC_Mission_Hunter:GetSpawnPointForAI] Could not find an empty pos.", LogLevel.ERROR);
		
		return pos;
	}
		
	//------------------------------------------------------------------------------------------------
	protected AIWaypoint GetWaypoint(SCR_AIGroup group)
	{
		IEntity closestPlayer = SCR_DC_PlayerHelper.PlayerGetClosestToPos(group.GetLeaderEntity().GetOrigin());
		
		if (closestPlayer != null)
		{
			AIWaypoint waypoint = SCR_DC_AIHelper.CreateWaypointEntity(DC_EWaypointMoveType.MOVE);
//			waypoint.SetOrigin(closestPlayer.GetOrigin());
			waypoint.SetOrigin(SCR_DC_Misc.RandomizePos(closestPlayer.GetOrigin(), m_Config.rndDistanceToPlayer));
			return waypoint;
		}
		else
		{
			SCR_DC_Log.Add("[SCR_DC_Mission_Hunter:GetWaypoint] Unable to find player for waypoint creation!", LogLevel.ERROR);
			SCR_DC_AIHelper.GroupDelete(group);
		}
		
		return null;
	}	
}

//------------------------------------------------------------------------------------------------
class SCR_DC_HunterConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	int missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_DEFAULT;		//How often the mission is run	
	
	//Mission specific
	int groupsToSpawn;					//Number of groups to spawn
	int groupSpawnDelay;				//Delay between group spawns 
	int lifeCycleTime;					//Loop time for giving hunters a new player location
	int minDistanceToPlayer;			//Hunter group minimum distance to player for spawn
	int maxDistanceToPlayer;			//...max distance
	int rndDistanceToPlayer;			//The error on the location where AI thinks you are. (0..rndDistanceToPlayer)  
	
	ref array<string> groupTypes = {}; 				//Types of AI groups
}

//------------------------------------------------------------------------------------------------
class SCR_DC_HunterJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Hunter.json";
	ref SCR_DC_HunterConfig conf = new SCR_DC_HunterConfig();
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_MISSIONCONFIG_FILE);
		
		if(!loadContext)
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
		//Mission specific
		conf.missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_DEFAULT;		
		conf.groupsToSpawn = 2;
		conf.groupSpawnDelay = 5000;
		conf.lifeCycleTime = 5000;
		conf.minDistanceToPlayer = 250;
		conf.maxDistanceToPlayer = 1000;
		conf.rndDistanceToPlayer = 60;
		conf.groupTypes = 
		{
			"{ADB43E67E3766CE7}Prefabs/Characters/Factions/OPFOR/USSR_Army/Spetsnaz/Character_USSR_SF_Sharpshooter.et",
			"{976AC400219898FA}Prefabs/Characters/Factions/OPFOR/USSR_Army/Character_USSR_Sharpshooter.et",
			"{8E29E7581DE832CC}Prefabs/Groups/OPFOR/KLMK/Group_USSR_MedicalSection_KLMK.et",
		};
	}	
}