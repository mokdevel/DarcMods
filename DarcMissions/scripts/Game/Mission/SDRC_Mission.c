//------------------------------------------------------------------------------------------------
//Stages in the state machine
enum SDRC_EMissionState
{
	NONE,		//Unknown state. Nothing should be run at this state.
	INIT,		//The mission is being init. This automatically set when object is created. Mission should not run any code in this state!
	SPAWN,		//Things are spawned etc. This automatically set when INIT is success.
	ACTIVE,		//Normal state when mission is running.	
	END,		//Mission is ending. Things are cleaned, despawned etc.
	EXIT,		//State to inform the MissionFrame that the mission should be destroyed.
	FAILED		//Mission startup has failed, delete mission
};

enum SDRC_EMissionWinCondition
{
	DEFAULT = -1,
	NONE = 0,
	AI_KILL_ALL,
	AI_KILL_75,
	AI_KILL_50,
	AI_KILL_RANDOM,
	
	HVT_KILL_VIP = 10,
	HVT_DESTROY_ITEM = 20,
	
	FIND_IN_15 = 30,
	FIND_IN_30,
	FIND_IN_45,
	FIND_IN_60,
};

enum SDRC_EMissionSuccess
{
	UNKNOWN = 0,	//Success is uncertain. Normal state while mission is running.
	WIN,			//The mission was a success -> win
	LOSE,			//The mission was a success -> lose
	WIN_OR_LOSE,	//The missions state either win / lose - used for second wave
	DELETED,		//Mission was deleted prematurely. For example by GM.
}

enum SDRC_EMissionError
{
	NONE = 0,
	ERROR_LOADING_JSON,
	//Mission specific
	LOCATION_NOT_FOUND,
	WRONG_SUBIDX,
	STARTING_POINT_NOT_FOUND,
	ROAD_FOR_START_NOT_FOUND,
	COULD_NOT_SPAWN_VEHICLE,
	COULD_NOT_FIND_VEHICLE_TO_SPAWN,
	ROUTE_NOT_FOUND,
	SUITABLE_BUILDING_NOT_FOUND,
	UNABLE_TO_SPAWN_AI,
	NO_PLAYERS_NEARBY,

	//Mainly from checking if position is suitable for mission	
	POSITION_IN_WATER,
	POSITION_UNDER_MAP,
	PLAYER_TOO_CLOSE,
	PLAYER_TOO_FAR,
	MISSION_TOO_CLOSE,
	IN_NON_VALID_AREA,
}

const string SDRC_DEFAULT = "default";

//------------------------------------------------------------------------------------------------
class SDRC_MissionConfig : SDRC_Config
{
	SDRC_EMissionType missionType = SDRC_EMissionType.NONE;
	//Default information
	int version = 1;
	string author = "darc";
	string comment = "";
	int missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;		//How often the mission is run
	bool showMarker = true;
	bool showHint = true;
	bool showMessage = true;
	bool disableArsenal;										//Disable arsenal for vehicles so that no other items are found
	ref array<ref int> missionList = {};						//The list of mission suids.
	ref array<ref string> missionFiles = {};					//The list of mission files to load.

	//------------------------------------------------------------------------------------------------
	/*!
	Override to create external mission files 
	*/		
	void CreateMissionFiles()
	{
		SDRC_Log.Add("[SDRC_MissionConfig:CreateMissionFiles] Creating...", LogLevel.SPAM);
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Override to load mission files. Remember to call super to add the unique suids to a list
	*/		
	void LoadMissionFiles(int ver)
	{
		array<int> suids = {};
		
		foreach (int id : missionList)		
		{
			if (!suids.Contains(id))
			{
				suids.Insert(id);
			}
		}
		
		SDRC_Log.Add("[SDRC_MissionConfig:LoadMissionFiles] Found suids: " + missionList.Count() + " : " + suids, LogLevel.SPAM);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Return the biggest index used in missionList.
	*/		
	int FindBiggestIndex()
	{
		int bigIndex = -1;
		foreach (int index : missionList)
		{
			if (index > bigIndex)
			{
				bigIndex = index
			}
		}
		
		return bigIndex;		
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	TBD: LoadMissionFiles2 should be in this class, but currently I don't know how to handle all 
	     different missions classes in one single function.
	*/		
/*	
	void LoadMissionFiles2(array<Managed> subMissions)
	{
//		typename t = SDRC_StashConfig;
		auto conf = new SDRC_StashConfig();
		
		//Load mission files
		foreach (string missionFile : missionFiles)
		{
			SDRC_JsonApi2 jsonApi = new SDRC_JsonApi2(missionFile);
			
			if (jsonApi.Load(conf, SDRC_MissionConfig.Cast(conf), false))
			{
//				foreach (SDRC_Camp subMission : conf.subMissions)
				foreach (auto subMission : conf.subMissions)
				{
					subMissions.Insert(subMission);
				}
				foreach (int idx : conf.missionList)
				{
					missionList.Insert(idx);
				}
			}
		}
	}		*/
}

//------------------------------------------------------------------------------------------------
// MISSION CONFIG
//------------------------------------------------------------------------------------------------
class SDRC_Mission : Managed
{
	static int m_MissionIDCounter = 1;			//Static counter for mission ID
	
	//Common for all missions
    private string m_sId;
	private SDRC_EMissionState m_State;
	private SDRC_EMissionType m_Type;
	private bool m_bStatic;						//Defines if the mission is dynamic or static. Dynamic is default. 
    private bool m_bShowHint;
    private bool m_bShowMessage;
	private bool m_bShowMarker;					//If the icon is to be shown
	
	//Common for all sub missions
	private ref SDRC_MissionConfigGeneral m_General = new SDRC_MissionConfigGeneral();
	private string m_sFaction;					//The faction in use for the mission
	private SDRC_EDifficulty m_sDifficulty;		//The difficulty chosen for the mission
	//Internals
	private bool m_bRequested;					//The missions spawn was requested by a an external party (like GM)
	private int m_iRequestId;					//An ID set by the requestor. Default -1 which means no special ID set.
	private SDRC_EMissionSuccess m_Success;
	//Internals without getters
	private int m_iStartTime;					//Seconds when mission started
	private int m_iEndTime;						//Seconds when mission shall end.
	private int m_iActiveTime;					//Seconds of how long the mission should be active
	private int m_iActiveDistance;				//The distance to a player to keep the mission active. This is set to default, but could be changed by the mission. Set to -1 to disable the check.
	private int m_iActiveTimeToEnd;				//The time to keep mission active once all AIs are dead.
	private bool m_bMissionIsEnding;			//Once all AIs are dead, we're getting close to end the mission.
	//Win condition related
	private int m_iAICountOriginal;				//The amount of AI at the beginning on the mission - at the time it was set active
	private int m_iAIKillPercentageRandom;		//The random amount of AIs to kill (30%-100%)	
	protected ref array<IEntity> m_EntityList = {};		//Entities (e.g., tents) spawned
	protected ref array<SCR_AIGroup> m_Groups = {};		//Groups spawned
	//QRF object
	private ref SDRC_MissionConfigQrf m_QrfConf = new SDRC_MissionConfigQrf();
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission(SDRC_EMissionType missionType, SDRC_MissionRequested request, bool staticMission = false)
	{
		m_sId = DC_ID_PREFIX + SCR_StringHelper.PadLeft(string.ToString(m_MissionIDCounter), 4, "0");
		m_MissionIDCounter++;
		m_State = SDRC_EMissionState.INIT;
		m_Type = missionType;	//SDRC_EMissionType.NONE;
		m_bStatic = staticMission;
		m_bShowHint = true;
		m_bShowMessage = true;
		m_bShowMarker = true;
		m_iRequestId = -1;
		
		//NOTE: m_General default values have been set in the constructor
//		m_General.winCondition = SDRC_EMissionWinCondition.AI_KILL_ALL;
//		m_sMarkerType = "DARC_MISSION";

		if (!request)
		{
			//Will pick any mission from the list
			m_General.subIdx = -1;		
		}
		else
		{
			m_bRequested = true;
			//Set the requested values
			m_iRequestId = request.requestId;
			m_General = request.general;
		}
		
		//Internals
		m_Success = SDRC_EMissionSuccess.UNKNOWN;
		m_iStartTime = (System.GetTickCount() / 1000); 	//The time in seconds when the mission was started.
		SetActiveTime(0);								//Sets m_EndTick. NOTE: This is properly set in MissionFrame to use the config value. This is just some default.
		m_iActiveDistance = 0;							//Set a default zero
		m_iActiveTimeToEnd = 0;							//Set a default zero
		m_bMissionIsEnding = false;
		//Win condition related	
		m_iAICountOriginal = -1;						//Defaults set, updated once mission goes ACTIVE
		m_iAIKillPercentageRandom = 99;
		
		SDRC_MissionStats.Add(m_sId, m_iRequestId, m_Type, m_State, m_Success);
	}

	//------------------------------------------------------------------------------------------------
	/*! This is called when INIT is successful and mission is ready to start. We delay the start a 
		few seconds to make sure everything has settled properly.
	*/
	void MissionStart()	
	{
		SetState(SDRC_EMissionState.SPAWN);
		SDRC_Log.Add("[SDRC_Mission:MissionStart] " +  GetId() + " : State changed to SPAWN", LogLevel.DEBUG);
		GetGame().GetCallqueue().CallLater(MissionRun, SDRC_Conf.MISSION_RUN_DELAY, false);	
	}	
	
	//------------------------------------------------------------------------------------------------
	/*! You should override this in your mission but remember to super.MissionRun()
		override void MissionRun()
		{			
			super.MissionEnd();	
			
			.. and add the functionality for the states. See SDRC_Mission_Template
		}
	*/
	void MissionRun()	
	{
		//SDRC_Log.Add("[SDRC_Mission:MissionRun] Super on: " + GetId(), LogLevel.DEBUG);
		
		if (m_State == SDRC_EMissionState.SPAWN)
		{
		}
		
		if (m_State == SDRC_EMissionState.END)
		{
		}
		
		if (m_State == SDRC_EMissionState.ACTIVE)
		{			
			//Check if a player is close to a mission vehicle. If yes, remove it from entities list so that it's not deleted at the end of mission.
			int i = 0;
			
			while (i < m_EntityList.Count())
			{
				IEntity entity = m_EntityList[i];
				Vehicle vehicle = Vehicle.Cast(entity);
				
				if (vehicle)
				{
					SDRC_Log.Add("[SDRC_Mission:MissionRun] Vehicle: " + entity.GetPrefabData().GetPrefabName(), LogLevel.SPAM);
					
					if (SDRC_PlayerHelper.IsAnyPlayerCloseToPos(entity.GetOrigin(), 30))
					{
						SDRC_Log.Add("[SDRC_Mission:MissionRun] Setting vehicle as persistent: " + entity.GetPrefabData().GetPrefabName(), LogLevel.DEBUG);
						SDRC_SpawnHelper.SetPersistence(entity, true);
						//SDRC_SpawnHelper.SetPersistenceDelayed(entity, true);	//TBD: Could immediately call this, by compat mods hae SetPersistence
						m_EntityList.Remove(i);
						i--;
					}
				}
				
				i++;
			}			
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
		override void MissionEnd()
		{			
			super.MissionEnd();	
		}
	*/
	void MissionEnd()	//You should calls this at the end of your mission
	{
		//Remove spawned items
		SDRC_Log.Add("[SDRC_Mission:MissionEnd] " +  GetId() + " : Deleting entities", LogLevel.DEBUG);
		foreach (IEntity entity : m_EntityList)
		{
			if (entity)
			{
				SDRC_Log.Add("[SDRC_Mission:MissionEnd] " +  GetId() + " : Despawning: " + entity.GetPrefabData().GetPrefabName(), LogLevel.DEBUG);
				SDRC_SpawnHelper.DespawnItem(entity);
			}
		}		
		
		//Remove AI
		SDRC_Log.Add("[SDRC_Mission:MissionEnd] " +  GetId() + " : Deleting AI groups", LogLevel.DEBUG);
		foreach (SCR_AIGroup group : m_Groups)
		{
			if (group)
			{
				SDRC_AIHelper.GroupDelete(group);
			}
		}		

		//Remove marker from map
		SDRC_MapMarkerHelper.DeleteMarker(GetId());
		
		SDRC_Log.Add("[SDRC_Mission:MissionEnd] " + GetId() + " : " + GetTitle() + " - Cleared for deletion.", LogLevel.NORMAL);		
	}

	
	//------------------------------------------------------------------------------------------------	
	/*!
	Fill in information from general for sub mission to run. Only used for REQUESTED missions.
	
	In the case we have changed a value from default in the request, we use it for the mission.
	See also constructor for additional variables set at creation.
	*/		
	void HandleRequestGeneralVariables(inout SDRC_MissionConfigGeneral general, SDRC_MissionRequested request)
	{
		if (IsRequested())
		{
			if (request.general.pos.IsEmpty())
			{
				SDRC_Log.Add("[SDRC_Mission:HandleRequestGeneralVariables] pos is empty. This should never happen.", LogLevel.ERROR);		
				return;
			}
			
			if (request.general.pos.Count() > 0 && general.pos.Count() > 0 )
			{
				if (request.general.pos[0] != "0 0 0")
				{
					general.pos[0] = request.general.pos[0];
				}
			}
						
			if (request.general.pos.Count() > 1 && general.pos.Count() > 1 )
			{
				if (request.general.pos[1] != "0 0 0")
				{
					general.pos[1] = request.general.pos[1];
				}
			}
			
			if (request.general.posName != SDRC_DEFAULT)
			{
				general.posName = request.general.posName;
			}
			
			if (request.general.messages.Count() > 0)
			{
				general.messages.Insert(request.general.messages.GetRandomElement());
			}
/*			else
			{
				SDRC_MissionMessage defaultMessages = new SDRC_MissionMessage();
				//Fill in some default texts...
				defaultMessages.Set("default", "default", "default", "defatult");
				general.messages.Insert(defaultMessages);
			}*/
			
			if (request.general.winCondition != SDRC_EMissionWinCondition.DEFAULT)
			{
				general.winCondition = request.general.winCondition;
			}

			general.faction.Clear();
			if (!request.general.faction.IsEmpty())
			{
				//Use the factions requested
				general.faction = request.general.faction;
			}
			
			if (request.general.markerType != SDRC_DEFAULT)
			{
				general.markerType = request.general.markerType;
			}
			
			if (request.general.markerIcon != -1)
			{
				general.markerIcon = request.general.markerIcon;
			}
			
			if (request.general.xp != 0)
			{
				general.xp = request.general.xp;
			}
			
			general.difficulty = request.general.difficulty;
		}
		
		//TBD: Should this do a dump of mission settings?
		
		//Select and print out the faction for mission
		string faction = "";
		if (general.faction.Count() > 0)
		{
			faction = general.faction.GetRandomElement();
		}
		faction = SDRC_EnemyHelper.SelectEnemyFaction(faction);
		SetFaction(faction);

		SDRC_EDifficulty difficulty	= SDRC_EDifficulty.RANDOM;
		if (general.difficulty.Count() > 0)
		{
			difficulty = general.difficulty.GetRandomElement();
		}
		SetDifficulty(difficulty);		
	}	

	//------------------------------------------------------------------------------------------------
	void UpdateGeneral(SDRC_MissionConfigGeneral general)
	{
		SetMarker(general.markerIcon, general.markerType);
		SetMessages(general.messages.GetRandomElement());
		SetWinCondition(general.winCondition);
		SetXP(general.xp);
	}
	
	//------------------------------------------------------------------------------------------------
	// Getters/Setters for members and other related functions 
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	SDRC_EMissionState GetState()
	{
		return m_State;
	}

	void SetState(SDRC_EMissionState state, SDRC_EMissionError errorReason = SDRC_EMissionError.NONE, string errorInfo = "")
	{
		m_State = state;
		
		if (state == SDRC_EMissionState.END)
		{
			//Things to set when mission is to end
			
			//TBD: The time should be set as a short one when mission ends.
			//m_Config.missionCycleTime = SDRC_Conf.MISSION_END_TIME;
		}		
		
		if (state == SDRC_EMissionState.ACTIVE)
		{			
			//Things to set when mission goes to active state
			GetGame().GetCallqueue().CallLater(GetAICountActiveDelayed, 10000);		//Do the counting after a while. AIs needs to be spawned.
			m_iAIKillPercentageRandom = SDRC_Misc.RandomInt(30, 99);
			ShowMarker();
		}
		
		if (state == SDRC_EMissionState.FAILED)
		{
			if (errorReason != SDRC_EMissionError.NONE)
			{
				SDRC_Log.Add("[SDRC_Mission:SetState] ERROR: " + GetId() + " : " + SCR_Enum.GetEnumName(SDRC_EMissionError, errorReason) + " " + errorInfo + " (" + SCR_Enum.GetEnumName(SDRC_EMissionType, m_Type) + ")", LogLevel.ERROR);						
			}
		}
		
		SDRC_MissionStats.UpdateState(GetId(), state);
	}

	//------------------------------------------------------------------------------------------------
	SDRC_EMissionType GetType()
	{
		return m_Type;
	}

/*	void SetType(SDRC_EMissionType type)	//TBD: This setter not needed in theory
	{
		m_Type = type;
	}*/
	
	//------------------------------------------------------------------------------------------------
	SDRC_EMissionState GetSubIdx()
	{
		return m_General.subIdx;
	}

	void SetSubIdx(int subIdx)
	{
		m_General.subIdx = subIdx;
	}	
	
	//------------------------------------------------------------------------------------------------
	bool IsStatic()
	{
		return m_bStatic;
	}

	void SetStatic(bool static_)
	{
		m_bStatic = static_;
	}		
				
	//------------------------------------------------------------------------------------------------
	string GetId()
	{
		return m_sId;
	}	
			
	//------------------------------------------------------------------------------------------------
	vector GetPos()
	{
		return m_General.pos[0];
	}

	void SetPos(vector pos, vector destination = "0 0 0")
	{
		m_General.pos[0] = pos;
		m_General.pos[1] = destination;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetPosName()
	{
		return m_General.posName;
	}

	void SetPosName(string posname)
	{
		m_General.posName = posname;
	}

	//------------------------------------------------------------------------------------------------	
	void SetWinCondition(SDRC_EMissionWinCondition winCondition)
	{
		m_General.winCondition = winCondition;

		switch (m_General.winCondition)
		{
			case SDRC_EMissionWinCondition.FIND_IN_15:
			{
				InitActiveTime(15*60);
				break;
			}
			case SDRC_EMissionWinCondition.FIND_IN_30:
			{
				InitActiveTime(30*60);
				break;
			}
			case SDRC_EMissionWinCondition.FIND_IN_45:
			{
				InitActiveTime(45*60);
				break;
			}
			case SDRC_EMissionWinCondition.FIND_IN_60:
			{
				InitActiveTime(60*60);
				break;
			}
		}				
	}
	
	SDRC_EMissionWinCondition GetWinCondition()
	{
		return m_General.winCondition;
	}
	
	//------------------------------------------------------------------------------------------------
	// Message related
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	void SetShowMessage(bool showMessage)
	{
		m_bShowMessage = showMessage;
	}
	
	bool IsShowMessage()
	{
		return m_bShowMessage;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetMessages(SDRC_MissionMessage message)
	{
		if (!message)
		{
			m_General.messages.Insert(new SDRC_MissionMessage());
			SetTitle("Title missing");
			SetInfo("Info missing");
			SetWinMessage("Win message missing");
			SetLoseMessage("Lose message missing");
		}
		else
		{		
			m_General.messages.Insert(message);
			SetTitle(message.title);
			SetInfo(message.info);
			SetWinMessage(message.win);
			SetLoseMessage(message.lose);
		}
	}
	
	string GetTitle()
	{
		return m_General.messages[0].title;
	}
	
	void SetTitle(string title)
	{
		m_General.messages[0].title = FixString(title);
	}

	string GetInfo()
	{		
		return m_General.messages[0].info;	//GetPosName() + " at " + GetPos();
	}

	void SetInfo(string info)
	{		
		m_General.messages[0].info = FixString(info);
	}
	
	string GetWinMessage()
	{
		return m_General.messages[0].win;
	}
	
	void SetWinMessage(string message)
	{
		m_General.messages[0].win = FixString(message);
	}
	
	string GetLoseMessage()
	{
		return m_General.messages[0].lose;
	}
	
	void SetLoseMessage(string message)
	{
		m_General.messages[0].lose = FixString(message);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetShowHint(bool showHint)
	{
		m_bShowHint = showHint;
	}
	
	bool IsShowHint()
	{
		return m_bShowHint;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetVisibility(bool showMarker, bool showHint, bool showMessage)
	{
		SetShowMarker(showMarker);
		SetShowHint(showHint);
		SetShowMessage(showMessage);
	}
	
	//------------------------------------------------------------------------------------------------
	SDRC_EMissionSuccess GetSuccess()
	{
		return m_Success;
	}
	
	void SetSuccess(SDRC_EMissionSuccess success)
	{
		m_Success = success;
		
		SDRC_MissionStats.UpdateSuccess(GetId(), success);
	}

	//------------------------------------------------------------------------------------------------
	string GetFaction()
	{
		return m_sFaction;
	}
			
	void SetFaction(string faction)
	{
		m_sFaction = faction;
		SDRC_Log.Add("[SDRC_Mission:SetFaction] " +  GetId() + " : " + faction, LogLevel.DEBUG);		
	}

	//------------------------------------------------------------------------------------------------
	SDRC_EDifficulty GetDifficulty()
	{
		return m_sDifficulty;
	}
			
	void SetDifficulty(SDRC_EDifficulty difficulty)
	{
		if (difficulty == SDRC_EDifficulty.RANDOM)
		{
			SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
			difficulty = baseGameMode.missionFrame.m_Config.missionDifficultyList.GetRandomElement();
			if ( (difficulty < SDRC_EDifficulty.EASY) || (difficulty > SDRC_EDifficulty.HARD) )
			{
				SDRC_Log.Add("[SDRC_Mission:SetDifficulty] Incorrect value found in missionDifficultyList. Please check dc_missionConfig.json . Reverting to NORMAL difficulty.", LogLevel.WARNING);		
				difficulty = SDRC_EDifficulty.NORMAL;
			}
		}
		
		m_sDifficulty = difficulty;
		
		SDRC_Log.Add("[SDRC_Mission:SetDifficulty] " +  GetId() + " : " + SCR_Enum.GetEnumName(SDRC_EDifficulty, difficulty) + " (" + difficulty + ")", LogLevel.DEBUG);		
	}
		
	//------------------------------------------------------------------------------------------------
	int GetXP()
	{
		return m_General.xp;
	}

	void SetXP(int value)
	{
		m_General.xp = value;
	}	
			
	//------------------------------------------------------------------------------------------------
	SDRC_EMissionIcon GetMarkerIcon()
	{
		return m_General.markerIcon;
	}

	string GetMarkerType()
	{
		return m_General.markerType;
	}
	
	void SetMarker(SDRC_EMissionIcon icon, string markerType = "DARC_MISSION")
	{
		m_General.markerIcon = icon;
		m_General.markerType = markerType;
	}

	void SetShowMarker(bool showMarker)
	{
		m_bShowMarker = showMarker;
	}	
		
	bool IsShowMarker()
	{
		return m_bShowMarker;
	}	
	
	void ShowMarker()
	{
		//If static mission, check if we are to show a marker
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (IsStatic() && !baseGameMode.missionFrame.m_Config.showStaticMissionMarker)
		{
			return;
		}
		
		//If dynamic mission, check if we are to show a marker		
		if (IsShowMarker() && baseGameMode.missionFrame.m_Config.showDynamicMissionMarker)
		{
			string markerType = GetMarkerType();
			
			if (baseGameMode.missionFrame.m_Config.showMissionDifficulty)
			{
				int difficultyIcon = GetDifficulty();	//NOTE: This returns a value 0-4 which represents SDRC_EMissionIconDifficulty
				SDRC_MapMarkerHelper.CreateMapMarker(GetPos(), GetMarkerIcon(), GetId(), "", markerTypeString: markerType);
				SDRC_MapMarkerHelper.CreateMapMarker(GetPos(), difficultyIcon, GetId(), GetTitle(), markerTypeString: "DARC_MISSION_DIFFICULTY");
			}
			else
			{
				SDRC_MapMarkerHelper.CreateMapMarker(GetPos(), GetMarkerIcon(), GetId(), GetTitle(), markerTypeString: markerType);
			}
		}
	}
	
	void MoveMarker()
	{
		SDRC_MapMarkerHelper.DeleteMarker(GetId());
		SetMarker(GetMarkerIcon(), GetMarkerType());
		ShowMarker();
	}
	
	//------------------------------------------------------------------------------------------------
	//\return bool status it the mission spawn was requested by a an external party (like GM)	
	//NOTE: m_bRequested is set in SDRC_Mission constructor
	bool IsRequested()
	{
		return m_bRequested;
	}	
	
	//------------------------------------------------------------------------------------------------
	// Getters/Setters for information when no access to m_EntityList nor m_Groups
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	IEntity GetFromEntityList(int index)
	{
		if (m_EntityList.IsEmpty())
		{
			return null;
		}
		if (index > m_EntityList.Count())
		{
			return null;
		}
		
		return m_EntityList[index];	
	}		

	void AddToEntityList(IEntity entity)
	{
		m_EntityList.Insert(entity);
	}
	
	//------------------------------------------------------------------------------------------------
	void AddToGroupsList(SCR_AIGroup group)
	{
		m_Groups.Insert(group);
	}
	
	int GetGroupsCount()
	{
		return m_Groups.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	// ACTIVE state related things
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if mission is to be active.
	*/
	bool IsActive()
	{
		bool isWin = false;
		int currentTime = (System.GetTickCount() / 1000);
		
		//Are there players still nearby, reset the timer
		if (m_iActiveDistance > -1)
		{
			if (SDRC_PlayerHelper.GetPlayerClosestToPos(m_General.pos[0], 0, m_iActiveDistance))
			{
				ResetActiveTime();
			}
		}
				
		//Check the different winning conditions
		if ( (m_State == SDRC_EMissionState.ACTIVE) && (!m_bMissionIsEnding) && (m_iAICountOriginal >= 0) )
		{			
			float aiKillPercentage;
			
			if (m_iAICountOriginal == 0)	//This happens when AIs are killed before the delayed counting happens. For example, GM has killed the AIs 
			{
				aiKillPercentage = 1.0;
			}
			else
			{
				aiKillPercentage = (1 - GetAICountActive()/m_iAICountOriginal);
			}
			
			switch (m_General.winCondition)
			{
				case SDRC_EMissionWinCondition.AI_KILL_ALL:
				{
					if (aiKillPercentage > 0.95)
						isWin = true;					
					break;
				}
				case SDRC_EMissionWinCondition.AI_KILL_75:
				{
					if (aiKillPercentage > 0.74)
						isWin = true;					
					break;
				}
				case SDRC_EMissionWinCondition.AI_KILL_50:
				{
					if (aiKillPercentage > 0.49)
						isWin = true;					
					break;
				}
				case SDRC_EMissionWinCondition.AI_KILL_RANDOM:
				{
					if (aiKillPercentage > m_iAIKillPercentageRandom)
						isWin = true;					
					break;
				}
				//case SDRC_EMissionWinCondition.HVT_KILL_VIP:			NOTE: Handles internally in HvtVip mission
				//case SDRC_EMissionWinCondition.HVT_DESTROY_ITEM:		NOTE: Handles internally in HvtItem mission
				case SDRC_EMissionWinCondition.FIND_IN_15:
				case SDRC_EMissionWinCondition.FIND_IN_30:
				case SDRC_EMissionWinCondition.FIND_IN_45:
				case SDRC_EMissionWinCondition.FIND_IN_60:
				{
					if (SDRC_PlayerHelper.GetPlayerClosestToPos(m_General.pos[0], 0, m_iActiveDistance))
					{
						isWin = true;
						break;
					}					
				}
			}
			
			//If we did win the mission, set the message and prepare for ending.
			if (isWin)
			{
				SDRC_Log.Add("[SDRC_Mission:IsActive] " + GetId() + " : Mission WIN: " + GetTitle(), LogLevel.DEBUG);
				DoWin();
			}
		}
		
		//If we're in some other success mode than UNKNOWN, start to modify the active time and distance to end.		
		//This way, the time gets shorter and distance smaller.
		if (GetSuccess() != SDRC_EMissionSuccess.UNKNOWN)
		{
			SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
			if (baseGameMode)
			{
				m_iActiveTime = m_iActiveTime * baseGameMode.missionFrame.m_Config.missionActiveTimeToEndMul;
				m_iActiveDistance = m_iActiveDistance * baseGameMode.missionFrame.m_Config.missionActiveDistanceMul;
			}
		}
		
		if (currentTime < m_iEndTime)
		{
			return true;
		}

		//Well, if got here, we should not be active anymore
		
		//If we won, don't show a lose message
		if (GetSuccess() != SDRC_EMissionSuccess.WIN)
		{
			SDRC_Log.Add("[SDRC_Mission:IsActive] " + GetId() + " : Mission LOSE: " + GetTitle(), LogLevel.DEBUG);
			DoLose();
		}

		SDRC_Log.Add("[SDRC_Mission:IsActive] " +  GetId() + " : END - Mission : " + GetTitle() + " has ended.", LogLevel.DEBUG);				
		return false;
	}			
	
	//------------------------------------------------------------------------------------------------
	/*!
	Mission has been won. Do the final stuff.
	*/
	void DoWin()
	{
		//Mission is soon to be ending
		SetSuccess(SDRC_EMissionSuccess.WIN);
		m_bMissionIsEnding = true;
		
		if (IsShowHint() && IsShowMessage())			
		{
			SDRC_MissionHintHelper.Show("Success: " + GetTitle(), GetWinMessage(), SDRC_EMissionIcon.ICON_WIN_ROUND);
		}
			
		if (m_bShowMarker)
		{
			SDRC_MapMarkerHelper.DeleteMarker(GetId());
			SetMarker(SDRC_EMissionIcon.GM_MISSION_WIN_MAP, m_General.markerType);
			ShowMarker();
		}
		
		//Set ActiveTimeToEnd to be the final active time
		SetActiveTime(m_iActiveTimeToEnd);
		ResetActiveTime();
		
		DoQrf();
		GiveReward();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Mission was lost. Do the final stuff.
	*/
	void DoLose()
	{
		SetSuccess(SDRC_EMissionSuccess.LOSE);
		m_bMissionIsEnding = true;
		
		if (IsShowHint() && IsShowMessage())
		{
			SDRC_MissionHintHelper.Show("Failure: " + GetTitle(), GetLoseMessage(), SDRC_EMissionIcon.ICON_LOSE_ROUND);
		}
		
		if (m_bShowMarker)
		{
			SDRC_MapMarkerHelper.DeleteMarker(GetId());
			SetMarker(SDRC_EMissionIcon.GM_MISSION_LOSE_MAP, m_General.markerType);
			ShowMarker();
		}
		
		DoQrf();
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Give XP or what not to players
	*/
	void GiveReward()
	{
		//TBD: Currently does nothing
	}
		
	//------------------------------------------------------------------------------------------------
	void SetActiveDistance(int distance)	
	{
		if (m_iActiveDistance > 0)
		{
			//It has been set by the mission already
		}
		else		
		{
			//Use provided distance
			m_iActiveDistance = distance;
		}
	}		
	
	//------------------------------------------------------------------------------------------------
	void SetActiveTimeToEnd(int seconds)	
	{
		if (m_iActiveTimeToEnd > 0)
		{
			//It has been set by the mission already
		}
		else		
		{
			//Use provided distance
			m_iActiveTimeToEnd = seconds;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// InitActiveTime set the initial time and should only be used at mission init
	//
	//NOTE: Call ResetActiveTime(); after you've set this
	void InitActiveTime(int seconds)	
	{
		if (m_iActiveTime > 0)
		{
			//It has been set by the mission already
		}
		else		
		{
			//Use provided distance
			m_iActiveTime = seconds;
		}
	}		

	//NOTE: Call ResetActiveTime(); after you've set this
	void SetActiveTime(int seconds)	
	{
		m_iActiveTime = seconds;
	}
	
	int GetActiveTime()
	{
		int currentTime = (System.GetTickCount() / 1000);
		return m_iEndTime - currentTime;
	}		
		
	//------------------------------------------------------------------------------------------------
	void ResetActiveTime()	
	{
		int currentTime = (System.GetTickCount() / 1000);		
		m_iEndTime = currentTime + m_iActiveTime;
	}
	
	//------------------------------------------------------------------------------------------------
	// Handle groups in areas where they should not be in
	//------------------------------------------------------------------------------------------------
	
	
	
	//------------------------------------------------------------------------------------------------
	// Quick Reaction Forces (QRF) functionality
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	void SetQrfConf(SDRC_MissionConfigQrf qrfConf)
	{
		m_QrfConf = qrfConf;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if QRF needs to be initialized.
	*/
	private void DoQrf()
	{
		//SDRC_Log.Add("[SDRC_Mission:DoQrf] QRF functionality not enabled yet. : idx:" + subIdx + " : " + qrf.comment, LogLevel.WARNING);
		
		//If no QRF defned, return
		if (!m_QrfConf)
		{
			SDRC_Log.Add("[SDRC_Mission:DoQrf] " + GetId() + " : No QRF sent as none defined.", LogLevel.DEBUG);
			return;
		}
		
		if (m_QrfConf.subIdx.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_Mission:DoQrf] " + GetId() + " : No QRF sent as subIdx is empty.", LogLevel.DEBUG);
			return;
		}
		
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!m_BaseGameMode)
		{
			SDRC_Log.Add("[SDRC_Mission:DoQrf] " + GetId() + " : SCR_BaseGameMode not found.", LogLevel.ERROR);			
			return;
		}		
		
		if ( (m_QrfConf.activation == GetSuccess()) || (m_QrfConf.activation == SDRC_EMissionSuccess.WIN_OR_LOSE) )
		{
			#ifdef SDRC_RELEASE //Development version will always do a QRF
			if (SDRC_Misc.RandomFloat(0, 1) > (m_QrfConf.chance * m_BaseGameMode.missionFrame.m_Config.missionDifficulty.qrfChance[GetDifficulty()]) )
			{
				SDRC_Log.Add("[SDRC_Mission:DoQrf] " + GetId() + " : QRF not to be sent.", LogLevel.DEBUG);
				return;
			}
			#endif
			
			SDRC_Log.Add("[SDRC_Mission:DoQrf] " + GetId() + " : Trying to send QRF.", LogLevel.DEBUG);			
			
			int subIdx = m_QrfConf.subIdx.GetRandomElement();
			SDRC_Qrf qrf = m_BaseGameMode.missionFrame.m_ConfigQrf.GetQrf(subIdx);
			
			if (!qrf)
			{
				return;
			}
			
			SDRC_Log.Add("[SDRC_Mission:DoQrf] " + GetId() + " : Selected idx: " + subIdx + " : " + qrf.comment, LogLevel.NORMAL);
			
			//Find spawn position
			vector spawnPos = SDRC_MissionPosHelper.FindMissionPosAroundPos(GetPos(), SDRC_Misc.RandomInt(qrf.distance[0], qrf.distance[1]));
			
			if (spawnPos == vector.Zero)
			{
				SDRC_Log.Add("[SDRC_Mission:DoQrf] " + GetId() + " : Could not find a position for QRF. Skipping.", LogLevel.ERROR);
				return;
			}
			
			SDRC_Log.Add("[SDRC_Mission:DoQrf] " + GetId() + " : Spawning QRF.", LogLevel.DEBUG);
			
			SDRC_DebugHelper.AddDebugPos(spawnPos, color: Color.YELLOW, id: "qrf");
			
			if (qrf.vehicle == "")
			{
				int groupCount = SDRC_MissionHelper.SpawnAiFromClassAi(qrf.ai, this, spawnPos, GetPos());
				SDRC_Log.Add("[SDRC_Mission:DoQrf] " + GetId() + " : Spawned " + groupCount + " groups to QRF.", LogLevel.DEBUG);
			}
			else
			{
				int groupCount = SDRC_MissionHelper.SpawnAiFromClassAiInVehicle(qrf.vehicle, qrf.ai, this, spawnPos, GetPos());
				SDRC_Log.Add("[SDRC_Mission:DoQrf] " + GetId() + " : Spawned " + groupCount + " groups to QRF.", LogLevel.DEBUG);
			}
		}		
	}	
		
	//------------------------------------------------------------------------------------------------
	// Misc functions
	//------------------------------------------------------------------------------------------------
		
	//------------------------------------------------------------------------------------------------
	//Fix the string with proper information - like location, destination, ..	
	private string FixString(string info)
	{
		string destinationName = "";
		if (m_General.pos[1] != "0 0 0")
		{
			destinationName = SDRC_Locations.CreateName(m_General.pos[1], "any");			
		}
		
		info = SDRC_MissionHelper.CreateInfo(info, GetPosName(), destinationName, GetDifficulty());
		return info;
	}

	//------------------------------------------------------------------------------------------------
	// AI related things
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	//Count total amount of AI from all groups
	int GetAICountActive()
	{
		int count = 0;
		
		foreach (SCR_AIGroup group: m_Groups)
		{		
			if (group)
			{
				count = count + group.GetAgentsCount();
			}
		}
		
		return count;
	}	
	
	//------------------------------------------------------------------------------------------------	
	//Function called when setting state to ACTIVE. AIs needs to be spawned before the counting works properly.
	private void GetAICountActiveDelayed()
	{
		m_iAICountOriginal = GetAICountActive();
		SDRC_Log.Add("[SDRC_Mission:GetAICountDelayed] " + GetId() + " : Spawned " + m_iAICountOriginal + " AIs.", LogLevel.SPAM);
	}
}