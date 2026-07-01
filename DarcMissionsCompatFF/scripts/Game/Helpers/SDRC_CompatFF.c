//Helpers SDRC_Compat

//------------------------------------------------------------------------------------------------
/*!
Compatibility for Freedom Fighters
*/

//------------------------------------------------------------------------------------------------
modded class SDRC_Compat
{	
	const string DC_COMPATCONFIG_FILE = "dc_compatFFConfig.json";
	const int DC_COMPATCONFIG_FILE_JSONVER = 2;
	
	const int DC_COMPAT_WAIT_FOR_PLAYERS_TIME = 15;
	const int DC_COMPAT_CLEAN_WAIT_TIME = 15;
	
	private static ref SDRC_JsonApi2 m_JsonApi = null;	
	static ref SDRC_CompatFFConfig m_Config = new SDRC_CompatFFConfig();
	
	static ref array<string> m_FF_cleanUpList = {};
	
	//------------------------------------------------------------------------------------------------
	/*!
	Add you init code here.
	*/
	override static bool Init()
	{
		SDRC_Log.Add("[SDRC_CompatFF] Initializing compatibility: Freedom Fighters", LogLevel.NORMAL);

		super.Init();

		m_JsonApi = new SDRC_JsonApi2(DC_COMPATCONFIG_FILE);		

		//Load config
		if (!m_JsonApi.Load(m_Config, SDRC_CompatFFConfig.Cast(m_Config), DC_COMPATCONFIG_FILE_JSONVER))
		{
			SDRC_Log.Add("[SDRC_CompatFF] Could not initialize compatibility: Freedom Fighters", LogLevel.ERROR);
			return false;
		}				
		
		// as early as possible
//		EPF_PersistenceManager.GetInstance().GetOnStateChangeEvent().Insert(MyCallback);
		
		GetGame().GetCallqueue().CallLater(WaitForPlayers, DC_COMPAT_CLEAN_WAIT_TIME*1000, false);
//		GetGame().GetCallqueue().CallLater(Clear, DC_COMPAT_CLEAN_WAIT_TIME*1000, false);

		if (m_Config.setEnemyFactionAutomatically)
		{
			GetGame().GetCallqueue().CallLater(SetEnemyFactionAutomatically, 5000, true);
		}
				
		return true;
	}
	
	static void MyCallback(EPF_PersistenceManager persistenceManager)
	{
	    if (persistenceManager.GetState() != EPF_EPersistenceManagerState.ACTIVE) 
		{
			return;
		}
		
		EPF_PersistenceManager.GetInstance().GetOnStateChangeEvent().Remove(MyCallback);
	    persistenceManager.GetOnStateChangeEvent().Remove(MyCallback);
	    
		Clear();
	}		
	
	
	//------------------------------------------------------------------------------------------------
	static void WaitForPlayers()
	{
		if (SDRC_PlayerHelper.PlayerCount() == 0)
		{
			GetGame().GetCallqueue().CallLater(WaitForPlayers, DC_COMPAT_WAIT_FOR_PLAYERS_TIME*1000, false);
			return;
		}
		
		GetGame().GetCallqueue().CallLater(Clear, DC_COMPAT_CLEAN_WAIT_TIME*1000, false);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Set enemy faction to be the same for missions as is defined for FF.
	*/		
	static void SetEnemyFactionAutomatically()
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (baseGameMode)
		{						
			if (baseGameMode.GetState() == SCR_EGameModeState.PREGAME)
			{
				SDRC_Log.Add("[SDRC_CompatFF] Waiting.. in PREGAME.", LogLevel.SPAM);
				return;
			}
			
			JWK_FactionManager factionmanager = JWK_FactionManager.Cast(GetGame().GetFactionManager());
			
			if (factionmanager)
			{
				JWK_Faction enemyFaction = JWK_Faction.GetByRole(JWK_EFactionRole.ENEMY);
				string faction = enemyFaction.GetKey();
				
				bool doChange = false;
				
				if (baseGameMode.missionFrame.m_Config.enemyFactions.Count() > 1)
				{
					doChange = true;
				}
				else
				{
					if (baseGameMode.missionFrame.m_Config.enemyFactions[0] != faction)
					{
						doChange = true;
					}
				}
				
				if (doChange)
				{
					baseGameMode.missionFrame.m_Config.enemyFactions.Clear();
					baseGameMode.missionFrame.m_Config.enemyFactions.Insert(faction);
					SDRC_Log.Add("[SDRC_CompatFF] Changing faction to: " + faction + ". This will override any enemyFactions setting in DarcMissions.", LogLevel.WARNING);
				}
			}
			else
			{
				SDRC_Log.Add("[SDRC_CompatFF] No factionmanager.", LogLevel.SPAM);
			}
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Count reward
	*/		
	static int GetRewardValue(int rewardValue = 0, float rewardMul = 1.0)
	{
		array<int> playerIds = {};
		GetGame().GetPlayerManager().GetPlayers(playerIds);
		if (playerIds.IsEmpty()) 
		{
			return 0;
		}

		int divider = playerIds.Count();
		if (rewardValue == 0)
		{
			rewardValue = m_Config.rewardDefault;
		}
				
		if (m_Config.rewardPerUser)
		{
			divider = 1;
		}

		int perPlayerReward = rewardValue / divider;
		
		return perPlayerReward * rewardMul;		
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Clear old stuff from map
	*/		
	static void Clear()
	{		
		SDRC_Log.Add("[SDRC_CompatFF] Clearing old mission stuff from map.", LogLevel.NORMAL);
				
		//Clean choppers from map
		SDRC_List cleanUpList = new SDRC_List();
		cleanUpList.id = "CompatFF_Helicopters";
		cleanUpList.modDir.Insert("Prefabs");
		cleanUpList.include.Insert("_flying");
		cleanUpList.include.Insert("Loot");
		cleanUpList.include.Insert("Hvt");
		SDRC_Resources.GetList(m_FF_cleanUpList, "$DarcMissions:", cleanUpList);
		
		m_FF_cleanUpList.Debug();
		
		float mapSize = SDRC_Misc.GetWorldSize();
		vector pos = "0 0 0";
		pos[0] = mapSize/2;
		pos[2] = mapSize/2;
		GetGame().GetWorld().QueryEntitiesBySphere(pos, mapSize, FindEntitiesCallback, null, EQueryEntitiesFlags.ALL);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Call back filter for finding the burning choppers
	*/		
	static bool FindEntitiesCallback(IEntity entity)
	{
		if (entity)
		{
			if (entity.ClassName() == "Vehicle")
			{
				if (entity.GetPrefabData())
				{
					//string resName = entity.GetPrefabData().GetPrefabName();
					//SDRC_Log.Add("[SDRC_CompatFF:FindEntitiesCallback] Found " + resName, LogLevel.DEBUG);
					
					if (SCR_StringHelper.ContainsAny(entity.GetPrefabData().GetPrefabName(), m_FF_cleanUpList) )
					{
						SDRC_Log.Add("[SDRC_CompatFF:FindEntitiesCallback] Removing " + entity, LogLevel.DEBUG);
						SDRC_SpawnHelper.DespawnItem(entity);
						return true;	//Just continue...
					}
				}
			}
		}
		
		return true;
	}	
}

//------------------------------------------------------------------------------------------------
modded class SDRC_Mission
{
	override void GiveReward()
	{
		array<int> playerIds = {};
		GetGame().GetPlayerManager().GetPlayers(playerIds);
		if (playerIds.IsEmpty()) 
		{
			return;
		}

		float rewardMul = 1.0;		
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (baseGameMode)
		{
			rewardMul = baseGameMode.missionFrame.m_Config.missionDifficulty.rewardCoef[GetDifficulty()];
		}
		
		int perPlayerReward = SDRC_Compat.GetRewardValue(GetXP(), rewardMul);

		SDRC_Log.Add("[SDRC_CompatFF:GiveReward] Giving " + perPlayerReward + " money to each player.", LogLevel.DEBUG);
		
		foreach (int playerId : playerIds) {
    		JWK.GetPlayerProfile(playerId).AddMoney_S(perPlayerReward);
    
    		JWK.GetPlayerController(playerId).ShowNotification(
        		ENotification.JWK_QUEST_RECEIVED_MONEY,
        		JWK.FormatMoney(perPlayerReward)
    		);
		}
		super.GiveReward();
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_CompatFFConfig : SDRC_Config
{
	//Default information
	string author = "darc";
	//Mission specific
	string comment;
	//Specific
	int hideOutSafeZoneDistance = 300;
	float spawnRateForGreenZones = 0.25; 
	bool setEnemyFactionAutomatically = true;	//Automatically set enemy faction from FF. (WIP)
	bool rewardPerUser = false;					//Shall reward be set per user or for a group
	int rewardDefault = 500;					//Default reward unless specific reward has been set in a mission. (WIP)
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_CompatFFConfig data = SDRC_CompatFFConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{	
		super.SetDefaults();
	}			
}