//------------------------------------------------------------------------------------------------
//Stages in the state machine
enum DC_EMissionState
{
	NONE,		//Unknown state. Nothing should be run at this state.
	INIT,		//The mission is being init. This automatically set when object is created.
	SPAWN,		//Things are spawned etc. This automatically set when INIT is success.
	ACTIVE,		//Normal state when mission is running.	
	END,		//Mission is ending. Things are cleaned, despawned etc.
	EXIT,		//State to inform the MissionFrame that the mission should be destroyed.
	FAILED		//Mission startup has failed, delete mission
};

enum DC_EMissionWinCondition
{
	DEFAULT = -1,
	NONE = 0,
	AI_KILL_ALL,
	AI_KILL_75,
	AI_KILL_50,
	AI_KILL_RANDOM,
	
	HVT_KILL_VIP = 10,
	HVT_DESTROY_ITEM = 20,
};

enum DC_EMissionSuccess
{
	UNKNOWN,
	WIN,
	LOSE
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
	ref array<ref int> missionList = {};	//The list of mission suids.
}

//------------------------------------------------------------------------------------------------
class SDRC_MissionConfigGeneral : Managed
{
	int subIdx;								//Unique index for the sub mission. 
	string comment;							//Generic comment to describe the mission. Not used in game.
	ref array<vector> pos = {};				//Positions for mission. "0 0 0" used for random location chosen from locationTypes. First is mission position, second is destination for missions that need it.
	string posName;							//Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes 
	string title;							//Title for the hint shown for players
	string info;							//Details for the hint shown for players
	DC_EMissionWinCondition winCondition;	//Mission win condidition
	string winMessage;						//Message to show when mission is completed
	string loseMessage;						//Message to show when mission fails
	string faction;							//Faction for the mission. Setting as empty, works as the default to select from the enemyFactions
	string markerType;						//Marker type for the mission
	int markerIcon;							//Marker ID within markerType
	int xp;									//Experience given	
	
	//------------------------------------------------------------------------------------------------
	void SDRC_MissionConfigGeneral()
	{
		SetDefaults();
	}
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults(int subIdx_ = -1, string comment_ = SDRC_DEFAULT, 
					 vector pos_ = "0 0 0", 
					 string posName_ = SDRC_DEFAULT, string title_ = SDRC_DEFAULT, string info_ = SDRC_DEFAULT, 
					 DC_EMissionWinCondition winCondition_ = DC_EMissionWinCondition.DEFAULT, 
					 string winMessage_ = SDRC_DEFAULT, string loseMessage_ = SDRC_DEFAULT, 
					 string faction_ = SDRC_DEFAULT, 
					 string markerType_ = SDRC_DEFAULT, int markerIcon_ = -1, 
					 int xp_ = 0)
	{
		array<vector> pos_array = {pos_, "0 0 0"};
		Set(subIdx_, comment_, pos_array, posName_, title_, info_, winCondition_, winMessage_, loseMessage_, faction_, markerType_, markerIcon_, xp_);
	}
	
	//------------------------------------------------------------------------------------------------
	void Set(int subIdx_, string comment_, array<vector> pos_, string posName_, string title_, string info_, DC_EMissionWinCondition winCondition_, string winMessage_, string loseMessage_, string faction_, string markerType_, int markerIcon_, int xp_)
	{
		subIdx = subIdx_;
		comment = comment_;
		pos = pos_;
		posName = posName_;
		title = title_;
		info = info_;
		winCondition = winCondition_;
		winMessage = winMessage_;
		loseMessage = loseMessage_;
		faction = faction_;		//SDRC_EnemyHelper.SelectEnemyFaction(faction_);
		markerType = markerType_;
		markerIcon = markerIcon_;
		xp = xp_;
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_Mission
{
	static int m_MissionIDCounter = 1;			//Static counter for mission ID
	
	//Common for all missions
    private string m_sId;
	private DC_EMissionState m_State;
	private DC_EMissionType m_Type;
	private bool m_bStatic;						//Defines if the mission is dynamic or static. Dynamic is default. 
    private bool m_bShowHint;
    private bool m_bShowMessage;
	private bool m_bShowMarker;					//If the icon is to be shown
	
	//Common for all sub missions
	private ref SDRC_MissionConfigGeneral m_General = new SDRC_MissionConfigGeneral();

	//Internals
	private bool m_bRequested;					//The missions spawn was requested by a an external party (like GM)
	private int m_iRequestId;					//An ID set by the requestor. Default -1 which means no special ID set.
	private DC_EMissionSuccess m_Success;
	//Internals without getters
	private int m_StartTime;					//Seconds when mission started
	private int m_EndTime;						//Seconds when mission shall end.
	private int m_ActiveTime;					//Seconds of how long the mission should be active
	private int m_iActiveDistance;				//The distance to a player to keep the mission active. This is set to default, but could be changed by the mission.
	private int m_iActiveTimeToEnd;				//The time to keep mission active once all AIs are dead.
	private bool m_bMissionIsEnding;			//Once all AIs are dead, we're getting close to end the mission.
	//Win condition related
	private int m_iAICountOriginal;				//The amount of AI at the beginning on the mission - at the time it was set active
	private int m_iAIKillPercentageRandom;		//The random amount of AIs to kill (30%-100%)	
	protected ref array<IEntity> m_EntityList = {};		//Entities (e.g., tents) spawned
	protected ref array<SCR_AIGroup> m_Groups = {};		//Groups spawned
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission(SDRC_MissionRequested request)
	{
		m_sId = DC_ID_PREFIX + SCR_StringHelper.PadLeft(string.ToString(m_MissionIDCounter), 4, "0");
		m_MissionIDCounter++;
		m_State = DC_EMissionState.INIT;
		m_Type = DC_EMissionType.NONE;
		m_bStatic = false;
		m_bShowHint = true;
		m_bShowMessage = true;
		m_bShowMarker = true;
		m_iRequestId = -1;
		
		//NOTE: m_General default values have been set in the constructor
//		m_General.winCondition = DC_EMissionWinCondition.AI_KILL_ALL;
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
		m_Success = DC_EMissionSuccess.UNKNOWN;
		m_StartTime = (System.GetTickCount() / 1000); 			//The time in seconds when the mission was started.
		SetActiveTime(SDRC_MISSION_CYCLE_TIME_DEFAULT*20);		//Sets m_EndTick. NOTE: This is properly set in MissionFrame to use the config value. This is just some default.
		m_iActiveDistance = 0;									//Set a default zero
		m_bMissionIsEnding = false;
		
		SDRC_MissionStats.Add(m_sId, m_iRequestId, m_State, m_Success);
	}

	//------------------------------------------------------------------------------------------------
	/*! This is called when INIT is successful and mission is ready to start. We delay the start a 
		few seconds to make sure everything has settled properly.
	*/
	void MissionStart()	
	{
		SetState(DC_EMissionState.SPAWN);
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
		
		if (m_State == DC_EMissionState.SPAWN)
		{
		}
		
		if (m_State == DC_EMissionState.END)
		{
		}
		
		if (m_State == DC_EMissionState.ACTIVE)
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
			
			if (request.general.winCondition != DC_EMissionWinCondition.DEFAULT)
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
				general.faction = SDRC_EnemyHelper.SelectEnemyFaction("");	//Pick a random 
			}
			else //Pick the requested one if defined or random if left empty
			{
				general.faction = SDRC_EnemyHelper.SelectEnemyFaction(request.general.faction);
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
		//Just to print out the faction for mission
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
	DC_EMissionState GetState()
	{
		return m_State;
	}

	void SetState(DC_EMissionState state)
	{
		m_State = state;
		
		if (state == DC_EMissionState.END)
		{
			//Things to set when mission is to end
			
			//TBD: The time should be set as a short one when mission ends.
			//m_Config.missionCycleTime = SDRC_Conf.MISSION_END_TIME;
		}		
		
		if (state == DC_EMissionState.ACTIVE)
		{			
			//Things to set when mission goes to active state
			GetGame().GetCallqueue().CallLater(GetAICountDelayed, 10000);		//Do the counting after a while. AIs needs to be spawned.
			m_iAIKillPercentageRandom = Math.RandomInt(30, 99);
		}
		
		SDRC_MissionStats.UpdateState(GetId(), state);
	}

	//------------------------------------------------------------------------------------------------
	DC_EMissionState GetType()
	{
		return m_Type;
	}

	void SetType(DC_EMissionType type)
	{
		m_Type = type;
	}

	//------------------------------------------------------------------------------------------------
	DC_EMissionState GetSubIdx()
	{
		return m_General.subIdx;
	}

	void SetSubIdx(int subIdx)
	{
		m_General.subIdx = subIdx;
	}	
	
	//------------------------------------------------------------------------------------------------
	DC_EMissionState IsStatic()
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
	void SetWinCondition(DC_EMissionWinCondition winCondition)
	{
		m_General.winCondition = winCondition;
	}
	
	DC_EMissionWinCondition GetWinCondition()
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
	DC_EMissionSuccess GetSuccess()
	{
		return m_Success;
	}
	
	void SetSuccess(DC_EMissionSuccess success)
	{
		m_Success = success;
		
		SDRC_MissionStats.UpdateState(GetId(), success);
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
	DC_EMissionIcon GetMarker()
	{
		return m_General.markerIcon;
	}

	void SetMarker(DC_EMissionIcon icon, string markerType = "DARC_MISSION")
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
			SDRC_MapMarkerHelper.CreateMapMarker(GetPos(), GetMarker(), GetId(), GetTitle());
		}
	}
	
	void MoveMarker()
	{
		SDRC_MapMarkerHelper.DeleteMarker(GetId());
		SetMarker(GetMarker());
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

	//------------------------------------------------------------------------------------------------
	// ACTIVE state related things
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if mission is to be active.
	\param checkOnlyWinCondition If set to true, ignore any timing related checks. Currently only Hunter mission needs this.
	*/
	bool IsActive(bool checkOnlyWinCondition = false)
	{
		bool isWin = false;
		
		//Check the different winning conditions
		if (m_State == DC_EMissionState.ACTIVE && !m_bMissionIsEnding && m_iAICountOriginal > 0)
		{			
			float aiKillPercentage = (1 - GetAICount()/m_iAICountOriginal);
			
			switch (m_General.winCondition)
			{
				case DC_EMissionWinCondition.AI_KILL_ALL:
				{
					if (aiKillPercentage > 0.95)
						isWin = true;					
					break;
				}
				case DC_EMissionWinCondition.AI_KILL_75:
				{
					if (aiKillPercentage > 0.74)
						isWin = true;					
					break;
				}
				case DC_EMissionWinCondition.AI_KILL_50:
				{
					if (aiKillPercentage > 0.49)
						isWin = true;					
					break;
				}
				case DC_EMissionWinCondition.AI_KILL_RANDOM:
				{
					if (aiKillPercentage > m_iAIKillPercentageRandom)
						isWin = true;					
					break;
				}
			}
			
			//If we did win the mission, set the message and prepare for ending.
			if (isWin)
			{
				SDRC_Log.Add("[SDRC_Mission:IsActive] " + GetId() + " : Mission WIN: " + GetTitle(), LogLevel.DEBUG);
				DoWin();
			}
		}

		//If we did not win and only check for winCondition, we are still active. 
		if (checkOnlyWinCondition && !isWin)
		{
			ResetActiveTime();
			return true;
		}
		
		if (!checkOnlyWinCondition)
		{
			//Are there players still nearby
			if (SDRC_PlayerHelper.PlayerGetClosestToPos(m_General.pos[0], 0, m_iActiveDistance))
			{
				ResetActiveTime();
				return true;
			}
			
			//Has the active time passed
			int currentTime = (System.GetTickCount() / 1000);
			if (currentTime < m_EndTime)
			{
				return true;
			}
		}
				
		//Well, if get here, we should not be active anymore
		
		//If we won, don't show a lose message
		if (GetSuccess() != DC_EMissionSuccess.WIN)
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
		SetSuccess(DC_EMissionSuccess.WIN);
		m_bMissionIsEnding = true;
		
		if (IsShowHint() && IsShowMessage())			
		{
			SDRC_MissionHintHelper.Show("Success: " + GetTitle(), GetWinMessage(), DC_EMissionIcon.ICON_WIN_ROUND);
		}
			
		if (m_bShowMarker)
		{
			SDRC_MapMarkerHelper.DeleteMarker(GetId());
			SetMarker(DC_EMissionIcon.GM_MISSION_WIN_MAP, m_General.markerType);
			ShowMarker();
		}
		
		//Set ActiveTimeToEnd to be the final active time
		SetActiveTime(m_iActiveTimeToEnd);
		ResetActiveTime();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Mission was lost. Do the final stuff.
	*/
	void DoLose()
	{
		SetSuccess(DC_EMissionSuccess.LOSE);
		m_bMissionIsEnding = true;
		
		if (IsShowHint() && IsShowMessage())
		{
			SDRC_MissionHintHelper.Show("Failure: " + GetTitle(), GetLoseMessage(), DC_EMissionIcon.ICON_LOSE_ROUND);
		}
		
		if (m_bShowMarker)
		{
			SDRC_MapMarkerHelper.DeleteMarker(GetId());
			SetMarker(DC_EMissionIcon.GM_MISSION_LOSE_MAP, m_General.markerType);
			ShowMarker();
		}		
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
		m_iActiveTimeToEnd = seconds;
	}
	
	//------------------------------------------------------------------------------------------------
	//NOTE: Call ResetActiveTime(); after you've set this
	void SetActiveTime(int seconds)	
	{
		m_ActiveTime = seconds;
	}		

	int GetActiveTime()
	{
		int currentTime = (System.GetTickCount() / 1000);
		return m_EndTime - currentTime;
	}		
		
	//------------------------------------------------------------------------------------------------
	void ResetActiveTime()	
	{
		int currentTime = (System.GetTickCount() / 1000);		
		m_EndTime = currentTime + m_ActiveTime;
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
	//Count total amount of AI from all groups
	int GetAICount()
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
	private void GetAICountDelayed()
	{
		m_iAICountOriginal = GetAICount();
		SDRC_Log.Add("[SDRC_Mission:GetAICountDelayed] " + GetId() + " : Spawned " + m_iAICountOriginal + " AIs.", LogLevel.SPAM);
	}
}