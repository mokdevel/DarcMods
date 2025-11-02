//------------------------------------------------------------------------------------------------
//Stages in the state machine
enum SDRC_EMissionState
{
	NONE,		//Unknown state. Nothing should be run at this state.
	INIT,		//The mission is being init. This automatically set when object is created.
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
	LOCATION_NOT_FOUND,
	WRONG_SUBIDX,
	ROAD_FOR_START_NOT_FOUND,
	COULD_NOT_SPAWN_VEHICLE,
	ROUTE_NOT_FOUND,
	SUITABLE_BUILDING_NOT_FOUND,

	//Mainly from checking if position is suitable for mission	
	POSITION_IN_WATER,
	POSITION_UNDER_MAP,
	PLAYER_TOO_CLOSE,
	MISSION_TOO_CLOSE,
	IN_NON_VALID_AREA,
}

const string SDRC_DEFAULT = "default";

//------------------------------------------------------------------------------------------------
class SDRC_MissionConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	int missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;	//How often the mission is run
	bool showMarker = true;
	bool showHint = true;
	bool showMessage = true;
	bool disableArsenal;										//Disable arsenal for vehicles so that no other items are found
	ref array<ref int> missionList = {};						//The list of mission suids.
	ref array<ref string> missionFiles = {};					//The list of mission files to load.
	#ifndef NEW_VERSION_WIP	
		ref array<ref SDRC_MissionConfigSecondWave> secondWave = {};	//TBD: This is not used anywhere. REMOVE!
	#endif
}

//------------------------------------------------------------------------------------------------
// GENERAL CONFIG
//------------------------------------------------------------------------------------------------
class SDRC_MissionConfigGeneral : Managed
{
	int subIdx;								//Unique index for the sub mission. 
	string comment;							//Generic comment to describe the mission. Not used in game.
	ref array<vector> pos = {};				//Positions for mission. "0 0 0" used for random location chosen from locationTypes. First is mission position, second is destination for missions that need it.
	float size;								//The size (radius) of the empty space needed to decide on a mission position.
	ref array<EMapDescriptorType> locationTypes = {};
	string posName;							//Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes 
	string title;							//Title for the hint shown for players
	string info;							//Details for the hint shown for players
	SDRC_EMissionWinCondition winCondition;	//Mission win condidition
	string winMessage;						//Message to show when mission is completed
	string loseMessage;						//Message to show when mission fails
	string faction;							//Faction for the mission. Setting as empty, works as the default to select from the enemyFactions
	string markerType;						//Marker type for the mission
	int markerIcon;							//Marker ID within markerType
	SDRC_EMissionDifficulty difficulty;		//Difficulty for specific mission
	int xp;									//Experience given	
	
	//------------------------------------------------------------------------------------------------
	void SDRC_MissionConfigGeneral()
	{
		SetDefaults();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults(int subIdx_ = -1, string comment_ = SDRC_DEFAULT, 
					 vector pos_ = "0 0 0", float size_ = 5,
					 array<EMapDescriptorType> locationTypes_ = null,
					 string posName_ = SDRC_DEFAULT, string title_ = SDRC_DEFAULT, string info_ = SDRC_DEFAULT, 
					 SDRC_EMissionWinCondition winCondition_ = SDRC_EMissionWinCondition.DEFAULT, 
					 string winMessage_ = SDRC_DEFAULT, string loseMessage_ = SDRC_DEFAULT, 
					 string faction_ = SDRC_DEFAULT, 
					 string markerType_ = SDRC_DEFAULT, int markerIcon_ = -1, 
					 SDRC_EMissionDifficulty difficulty_ = SDRC_EMissionDifficulty.NORMAL, 
					 int xp_ = 0)
	{
		array<vector> pos_array = {pos_, "0 0 0"};
		Set(subIdx_, comment_, pos_array, size_, locationTypes_, posName_, title_, info_, winCondition_, winMessage_, loseMessage_, faction_, markerType_, markerIcon_, difficulty_, xp_);
	}
	
	//------------------------------------------------------------------------------------------------
	void Set(int subIdx_, string comment_, array<vector> pos_, float size_, array<EMapDescriptorType> locationTypes_, string posName_, string title_, string info_, SDRC_EMissionWinCondition winCondition_, string winMessage_, string loseMessage_, string faction_, string markerType_, int markerIcon_, SDRC_EMissionDifficulty difficulty_, int xp_)
	{
		subIdx = subIdx_;
		comment = comment_;
		pos = pos_;
		size = size_;
		if (locationTypes_)
		{
			locationTypes = locationTypes_;
		}
		posName = posName_;
		title = title_;
		info = info_;
		winCondition = winCondition_;
		winMessage = winMessage_;
		loseMessage = loseMessage_;
		faction = faction_;
		markerType = markerType_;
		markerIcon = markerIcon_;
		difficulty = difficulty_;		
		xp = xp_;
	}	
}

//------------------------------------------------------------------------------------------------
// AI CONFIG
//------------------------------------------------------------------------------------------------
class SDRC_MissionConfigAi : Managed
{
	private ref array<int> count = {};
	ref array<string> types = {}; 			//The prefab names of AI groups or characters. The AI is randomly picked from this list.
	private int skill;						//Skill for AI (0-100). See SCR_AICombatComponent and EAISkill
	private float perception;
	ref array<int> waypointRange = {};		//min, max
	SDRC_EWaypointGenerationType waypointGenType;
	SDRC_EWaypointMoveType waypointMoveType;
	
	void Set(array<int> count_, array<string> types_, int skill_, float perception_, array<int> waypointRange_, SDRC_EWaypointGenerationType waypointGenType_, SDRC_EWaypointMoveType waypointMoveType_)
	{
		count = count_;
		types = types_;
		skill = skill_;
		perception = perception_;		
		waypointRange = waypointRange_;
		waypointGenType = waypointGenType_;
		waypointMoveType = waypointMoveType_;
	}
	
	//------------------------------------------------------------------------------------------------
	/*! Return the count of AIs for the mission 
	
	NOTE: mission and missionFrame difficulty affects the outcome	
	*/	
	int GetCount(SDRC_EMissionDifficulty difficulty = SDRC_EMissionDifficulty.NORMAL)
	{
		int cnt = Math.RandomInt(count[0], count[1]);
		
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (m_BaseGameMode)
		{
			float coef = m_BaseGameMode.missionFrame.m_Config.missionDifficulty.aiCountCoef[difficulty];
			
			int low = count[0] * coef;
			if (low < count[0])
			{
				low = count[0];
			}
			int high = count[1] * coef;
			if (high < count[1])
			{
				high = count[1];
			}
			cnt = Math.RandomInt(low, high);
		}
		
		return cnt;
	}
	
	//------------------------------------------------------------------------------------------------
	/*! Return a proper skill value that matches EAISkill. 
	
	NOTE: mission and missionFrame difficulty affects the outcome	
	*/	
	int GetSkill(SDRC_EMissionDifficulty difficulty = SDRC_EMissionDifficulty.NORMAL)
	{
		int sk = skill;
		
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (m_BaseGameMode)
		{
			float coef = m_BaseGameMode.missionFrame.m_Config.missionDifficulty.aiSkillCoef[difficulty];
			sk = skill * coef;
		}		
		
		if (sk < 10) 				{sk = EAISkill.NOOB;}			
		if (sk >= 20 && sk < 50)	{sk = EAISkill.ROOKIE;}
		if (sk >= 50 && sk < 70)	{sk = EAISkill.REGULAR;}
		if (sk >= 70 && sk < 80)	{sk = EAISkill.VETERAN;}
		if (sk >= 80 && sk < 100)	{sk = EAISkill.EXPERT;}
		if (sk > 100)				{sk = EAISkill.CYLON;}
		
		return sk;
	}
	
	//------------------------------------------------------------------------------------------------
	/*! Return a proper perception value. 
	
	NOTE: mission and missionFrame difficulty affects the outcome	
	TBD: I'm a little unsure how this in total affects, but I presume the higher is better.
	*/
	float GetPerception(SDRC_EMissionDifficulty difficulty = SDRC_EMissionDifficulty.NORMAL)
	{
		float perc = perception;
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (m_BaseGameMode)
		{
			float coef = m_BaseGameMode.missionFrame.m_Config.missionDifficulty.aiPerceptionCoef[difficulty];
			perc = perception * coef;
		}		
		return perception;
	}			
}

//------------------------------------------------------------------------------------------------
// SECOND WAVE
//------------------------------------------------------------------------------------------------

#ifndef NEW_VERSION_WIP
class SDRC_MissionConfigSecondWave : Managed
{
	ref array<int> subIdx = {};									//subIdx from which to choose
	SDRC_EMissionSuccess activation = SDRC_EMissionSuccess.WIN;	//Which success activates the second wave
	ref array<int> delay = {};									//(seconds) Delay min-max before spawning second wave
	ref array<int> distance = {};								//min-max distance for the enemy spawn
	string info;												//Details for the hint shown for players
	SDRC_EMissionDifficulty difficulty;							//Difficulty for specific mission
	int xp;														//Experience given	
}
#endif

#ifdef NEW_VERSION_WIP
class SDRC_MissionConfigSecondWave : Managed
{
	ref array<int> subIdx = {};									//subIdx from which to choose
	SDRC_EMissionSuccess activation = SDRC_EMissionSuccess.WIN;	//Which success activates the second wave
	float chance;
	ref array<int> delay = {};									//(seconds) Delay min-max before spawning second wave
	string info;												//Details for the hint shown for players
	SDRC_EMissionDifficulty difficulty;							//Difficulty for specific mission
	int xp;														//Experience given

	void Set(array<int> subIdx_, SDRC_EMissionSuccess activation_, float chance_, array<int> delay_, string info_, SDRC_EMissionDifficulty difficulty_, int xp_)	
	{
		subIdx = subIdx_;
		activation = activation_;
		chance = chance_;
		delay = delay_;
		info = info_;
		difficulty = difficulty_;
		xp = xp_;
	}	
}
#endif

//------------------------------------------------------------------------------------------------
// MISSION CONFIG
//------------------------------------------------------------------------------------------------
class SDRC_Mission
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

	//Internals
	private bool m_bRequested;					//The missions spawn was requested by a an external party (like GM)
	private int m_iRequestId;					//An ID set by the requestor. Default -1 which means no special ID set.
	private SDRC_EMissionSuccess m_Success;
	//Internals without getters
	private int m_iStartTime;					//Seconds when mission started
	private int m_iEndTime;						//Seconds when mission shall end.
	private int m_iActiveTime;					//Seconds of how long the mission should be active
	private int m_iActiveDistance;				//The distance to a player to keep the mission active. This is set to default, but could be changed by the mission.
	private int m_iActiveTimeToEnd;				//The time to keep mission active once all AIs are dead.
	private bool m_bMissionIsEnding;			//Once all AIs are dead, we're getting close to end the mission.
	//Win condition related
	private int m_iAICountOriginal;				//The amount of AI at the beginning on the mission - at the time it was set active
	private int m_iAIKillPercentageRandom;		//The random amount of AIs to kill (30%-100%)	
	protected ref array<IEntity> m_EntityList = {};		//Entities (e.g., tents) spawned
	protected ref array<SCR_AIGroup> m_Groups = {};		//Groups spawned
	//Second wave object
	private ref SDRC_MissionConfigSecondWave m_SecondWaveConf = new SDRC_MissionConfigSecondWave();
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		m_sId = DC_ID_PREFIX + SCR_StringHelper.PadLeft(string.ToString(m_MissionIDCounter), 4, "0");
		m_MissionIDCounter++;
		m_State = SDRC_EMissionState.INIT;
		m_Type = missionType;	//SDRC_EMissionType.NONE;
		m_bStatic = false;
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
					
					if (SDRC_PlayerHelper.IsAnyPlayerCloseToPos(entity.GetOrigin(), 5))
					{
						SDRC_Log.Add("[SDRC_Mission:MissionRun] Vehicle set as persistent: " + entity.GetPrefabData().GetPrefabName(), LogLevel.DEBUG);
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
			
			if (request.general.title != SDRC_DEFAULT)
			{
				general.title = request.general.title;
			}

			if (request.general.info != SDRC_DEFAULT)
			{
				general.info = request.general.info;
			}
			
			if (request.general.winCondition != SDRC_EMissionWinCondition.DEFAULT)
			{
				general.winCondition = request.general.winCondition;
			}
			
			if (request.general.winMessage != SDRC_DEFAULT)
			{
				general.winMessage = request.general.winMessage;
			}
			
			if (request.general.loseMessage != SDRC_DEFAULT)
			{
				general.loseMessage = request.general.loseMessage;
			}
			
			if (request.general.faction == SDRC_DEFAULT)
			{
				general.faction = "";	//SDRC_EnemyHelper.SelectEnemyFaction("");	//Pick a random 
			}
			else //Pick the requested one if defined or random if left empty
			{
				general.faction = request.general.faction;	//SDRC_EnemyHelper.SelectEnemyFaction(request.general.faction);
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
		}
		
		//TBD: Should this do a dump of mission settings?
		//Select and print out the faction for mission 
		general.faction = SDRC_EnemyHelper.SelectEnemyFaction(general.faction);		
		SetFaction(general.faction);		
	}	

	//------------------------------------------------------------------------------------------------
	void UpdateGeneral(SDRC_MissionConfigGeneral general)
	{
		SetMarker(general.markerIcon, general.markerType);
		SetHint(general.title, general.info);
		SetMessages(general.winMessage, general.loseMessage);		
		SetWinCondition(general.winCondition);
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
			m_iAIKillPercentageRandom = Math.RandomInt(30, 99);
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
	SDRC_EMissionState IsStatic()
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
	void SetShowHint(bool showHint)
	{
		m_bShowHint = showHint;
	}
	
	bool IsShowHint()
	{
		return m_bShowHint;
	}

	//------------------------------------------------------------------------------------------------
	void SetHint(string title, string info)
	{
		SetTitle(title);
		SetInfo(info);
	}
	
	string GetTitle()
	{
		return m_General.title;
	}
	
	void SetTitle(string title)
	{
		m_General.title = FixString(title);
	}

	string GetInfo()
	{		
		return m_General.info;	//GetPosName() + " at " + GetPos();
	}

	void SetInfo(string info)
	{		
		m_General.info = FixString(info);
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
	void SetShowMessage(bool showMessage)
	{
		m_bShowMessage = showMessage;
	}
	
	bool IsShowMessage()
	{
		return m_bShowMessage;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetMessages(string winMessage, string loseMessage)
	{
		SetWinMessage(winMessage);
		SetLoseMessage(loseMessage);
	}
	
	string GetWinMessage()
	{
		return m_General.winMessage;
	}
	
	void SetWinMessage(string message)
	{
		m_General.winMessage = FixString(message);
	}
	
	string GetLoseMessage()
	{
		return m_General.loseMessage;
	}
	
	void SetLoseMessage(string message)
	{
		m_General.loseMessage = FixString(message);
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
		return m_General.faction;
	}
			
	void SetFaction(string faction)
	{
		m_General.faction = faction;
		SDRC_Log.Add("[SDRC_Mission:SetFaction] " +  GetId() + " : " + faction, LogLevel.DEBUG);		
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
		
		if (IsShowMarker())
		{
			string markerType = GetMarkerType();
			SDRC_MapMarkerHelper.CreateMapMarker(GetPos(), GetMarkerIcon(), GetId(), GetTitle(), markerTypeString: markerType);
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
	// Second Wave functionality
	//------------------------------------------------------------------------------------------------
	void SetSecondWaveConf(SDRC_MissionConfigSecondWave secondWaveConf)
	{
		m_SecondWaveConf = secondWaveConf;
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
		if (SDRC_PlayerHelper.GetPlayerClosestToPos(m_General.pos[0], 0, m_iActiveDistance))
		{
			ResetActiveTime();
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
		
		//If we're in some other success mode the UNKNOWN, start to modify the active time and distance to end.		
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
		
		DoSecondWave();
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
		
		DoSecondWave();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if second wave needs to be initialized.
	*/
	private void DoSecondWave()
	{
		if ( (m_SecondWaveConf.activation == GetSuccess()) || (m_SecondWaveConf.activation == SDRC_EMissionSuccess.WIN_OR_LOSE) )
		{
			#ifdef NEW_VERSION_WIP
			if (SDRC_Misc.RandomFloat(0, 1) < m_SecondWaveConf.chance)
			{
				SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
				if (m_BaseGameMode)
				{
					int subIdx = 0;
					SDRC_SecondWave wave = m_BaseGameMode.missionFrame.m_DC_SecondWaveJsonApi.GetWave(0);
					SDRC_Log.Add("[SDRC_Mission:DoSecondWave] !Just for debugging! : idx:" + subIdx + " : " + wave.comment, LogLevel.DEBUG);
				}
			}
			#endif
		}		
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
		
		info = SDRC_MissionHelper.CreateInfo(info, GetPosName(), destinationName);
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