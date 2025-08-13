//------------------------------------------------------------------------------------------------
enum DC_EMissionType
{
	NONE = 0,
	HUNTER,
	OCCUPATION,
	CONVOY,
	CRASHSITE,
	PATROL,
	SQUATTER,
	ROADBLOCK,
	HVTVIP,
	HVTITEM,
	CHOPPER,	//Mission not ready or working
	
	REQUESTED = 100
};

//Stages in the state machine
enum DC_EMissionState
{
	NONE,		//Unknown state. Nothing should be run at this state.
	INIT,		//The mission is being init. Things are spawned etc.
	ACTIVE,		//Normal state when mission is running.	
	END,		//Mission is ending. Things are cleaned, despawned etc.
	EXIT,		//State to inform the MissionFrame that the mission should be destroyed.
	FAILED		//Mission startup has failed, delete mission
};

enum DC_EMissionWinCondition
{
	NONE,
	AI_KILL_ALL,
	AI_KILL_75,
	AI_KILL_50,
	AI_KILL_RANDOM,
	
	HVT_VIP,
};

enum DC_EMissionSuccess
{
	UNKNOWN,
	WIN,
	LOSE
}

//------------------------------------------------------------------------------------------------
class SDRC_MissionConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	int missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;	//How often the mission is run
	string markerType = "DARC_MISSION";
	int markerIdx = -1;						//marker ID
	bool showMarker = true;
	bool showHint = true;
	bool showMessage = true;
}

class SDRC_MissionConfigGeneral : Managed
{
	string comment;							//Generic comment to describe the mission. Not used in game.
	ref array<vector> pos = {};				//Positions for mission. "0 0 0" used for random location chosen from locationTypes. First is mission position, second is destination for missions that need it.
	string posName;							//Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes 
	string title;							//Title for the hint shown for players
	string info;							//Details for the hint shown for players
	DC_EMissionWinCondition winCondition;	//Mission win condidition
	string winMessage;						//Message to show when mission is completed
	string loseMessage;						//Message to show when mission fails
	int xp;									//Experience given	
	
	void Set(string comment_, array<vector> pos_, string posName_, string title_, string info_, DC_EMissionWinCondition winCondition_, string winMessage_, string loseMessage_, int xp_)
	{
		comment = comment_;
		pos = pos_;
		posName = posName_;
		title = title_;
		info = info_;
		winCondition = winCondition_;
		winMessage = winMessage_;
		loseMessage = loseMessage_;
		xp = xp_;
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_Mission
{
	//Common for all missions
	private DC_EMissionState m_State;
	private DC_EMissionType m_Type;
	private bool m_Static;						//Defines if the mission is dynamic or static. Dynamic is default. 
    private string m_Id;
    private vector m_Pos;
    private vector m_PosDestination;
    private string m_sPosName;
    private string m_sTitle;
    private string m_sInfo;
    private bool m_ShowHint;
    private bool m_ShowMessage;
	//Internals
	private bool m_bRequested;					//The missions spawn was requested by a an external party (like GM)
	private int m_StartTime;					//Seconds when mission started
	private int m_EndTime;						//Seconds when mission shall end.
	private int m_ActiveTime;					//Seconds of how long the mission should be active
	private string m_sFaction;					//Faction for the mission		
	private bool m_bShowMarker;					//If the icon is to be shown
	private DC_EMissionIcon m_sIcon;			//The icon to show
	private string m_sMarkerType;				//Markertype defined by SCR_EMapMarkerType
    private string m_sWinMessage;
    private string m_sLoseMessage;
	private DC_EMissionWinCondition m_WinCondition;
	private DC_EMissionSuccess m_Success;
	//Internals without getters
	private int m_iActiveDistance;				//The distance to a player to keep the mission active. This is set to default, but could be changed by the mission.
	private int m_iActiveTimeToEnd;				//The time to keep mission active once all AIs are dead.
	private bool m_bMissionIsEnding;			//Once all AIs are dead, we're getting close to end the mission.
	//Win condition related
	private int m_iAICountOriginal;				//The amount of AI at the beginning on the mission - at the time it was set active
	private int m_iAIKillPercentageRandom;		//The random amount of AIs to kill (30%-100%)	
	protected ref array<IEntity> m_EntityList = {};		//Entities (e.g., tents) spawned
	protected ref array<SCR_AIGroup> m_Groups = {};		//Groups spawned
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission(vector pos = "0 0 0")
	{
		m_State = DC_EMissionState.INIT;
		m_Type = DC_EMissionType.NONE;
		m_Static = false;
		m_Id = DC_ID_PREFIX + string.ToString(System.GetTickCount());
		m_Pos = pos;
		m_sPosName = "";
		m_sTitle = "";
		m_sInfo = "";
		m_ShowHint = true;
		m_ShowMessage = true;
		m_sWinMessage = "";
		m_sLoseMessage = "";
		m_WinCondition = DC_EMissionWinCondition.AI_KILL_ALL;
		m_Success = DC_EMissionSuccess.UNKNOWN;
		m_sMarkerType = "DARC_MISSION";
		SetFaction(SDRC_EnemyHelper.SelectEnemyFaction()); 		//m_sFaction 
		//Internals
		m_StartTime = (System.GetTickCount() / 1000); 			//The time in seconds when the mission was started.
		SetActiveTime(SDRC_MISSION_CYCLE_TIME_DEFAULT*20);		//Sets m_EndTick. NOTE: This is properly set in MissionFrame to use the config value. This is just some default.
		m_iActiveDistance = 0;									//Set a default zero
		m_bMissionIsEnding = false;
		if (pos != "0 0 0")										//Requested is set here
		{
			m_bRequested = true;
		}
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
		
		if (m_State == DC_EMissionState.INIT)
		{
		}
		
		if (m_State == DC_EMissionState.END)
		{
		}
		
		if (m_State == DC_EMissionState.ACTIVE)
		{
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
		SDRC_Log.Add("[SDRC_Mission:MissionEnd] Deleting entities", LogLevel.DEBUG);
		foreach (IEntity entity : m_EntityList)
		{
			if (entity)
			{
				SDRC_Log.Add("[SDRC_Mission:MissionEnd] Despawning: " + entity.GetPrefabData().GetPrefabName(), LogLevel.DEBUG);
				SDRC_SpawnHelper.DespawnItem(entity);
			}
		}		
		
		//Remove AI
		SDRC_Log.Add("[SDRC_Mission:MissionEnd] Deleting AI groups", LogLevel.DEBUG);
		foreach (SCR_AIGroup group : m_Groups)
		{
			if (group)
			{
				SDRC_AIHelper.GroupDelete(group);
			}
		}		

		//Remove marker from map
		SDRC_MapMarkerHelper.DeleteMarker(GetId());
		
		SDRC_Log.Add("[SDRC_Mission:MissionEnd] Mission " + GetId() + " : " + GetTitle() + " cleared for deletion.", LogLevel.NORMAL);		
	}

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
	DC_EMissionState IsStatic()
	{
		return m_Static;
	}

	void SetStatic(bool static_)
	{
		m_Static = static_;
	}		
				
	//------------------------------------------------------------------------------------------------
	string GetId()
	{
		return m_Id;
	}	
			
	//------------------------------------------------------------------------------------------------
	vector GetPos()
	{
		return m_Pos;
	}

	void SetPos(vector pos, vector destination = "0 0 0")
	{
		m_Pos = pos;
		m_PosDestination = destination;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetPosName()
	{
		return m_sPosName;
	}

	void SetPosName(string posname)
	{
		m_sPosName = posname;
	}

	//------------------------------------------------------------------------------------------------
	void SetShowHint(bool showHint)
	{
		m_ShowHint = showHint;
	}
	
	bool IsShowHint()
	{
		return m_ShowHint;
	}

	//------------------------------------------------------------------------------------------------
	void SetHint(bool show, string title, string info)
	{
		SetShowHint(show);
		SetTitle(title);
		SetInfo(info);
	}
	
	string GetTitle()
	{
		return m_sTitle;
	}
	
	void SetTitle(string title)
	{
		m_sTitle = FixString(title);
	}

	string GetInfo()
	{		
		return m_sInfo;	//GetPosName() + " at " + GetPos();
	}

	void SetInfo(string info)
	{		
		m_sInfo = FixString(info);
	}
	
	//------------------------------------------------------------------------------------------------	
	void SetWinCondition(DC_EMissionWinCondition winCondition)
	{
		m_WinCondition = winCondition;
	}
	
	DC_EMissionWinCondition GetWinCondition()
	{
		return m_WinCondition;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetShowMessage(bool showMessage)
	{
		m_ShowMessage = showMessage;
	}
	
	bool IsShowMessage()
	{
		return m_ShowMessage;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetMessages(bool show, string winMessage, string LoseMessage)
	{
		SetShowMessage(show);
		SetWinMessage(winMessage);
		SetLoseMessage(LoseMessage);
	}
	
	string GetWinMessage()
	{
		return m_sWinMessage;
	}
	
	void SetWinMessage(string message)
	{
		m_sWinMessage = FixString(message);
	}
	
	string GetLoseMessage()
	{
		return m_sLoseMessage;
	}
	
	void SetLoseMessage(string message)
	{
		m_sLoseMessage = FixString(message);
	}
	
	//------------------------------------------------------------------------------------------------
	DC_EMissionSuccess GetSuccess()
	{
		return m_Success;
	}
	
	void SetSuccess(DC_EMissionSuccess success)
	{
		m_Success = success;
	}

	//------------------------------------------------------------------------------------------------
	string GetFaction()
	{
		return m_sFaction;
	}
			
	void SetFaction(string faction)
	{
		m_sFaction = faction;
		SDRC_Log.Add("[SDRC_Mission:SetFaction] " + faction, LogLevel.SPAM);		
	}
			
	//------------------------------------------------------------------------------------------------
	DC_EMissionIcon GetMarker()
	{
		return m_sIcon;
	}

	void SetMarker(bool showMarker, DC_EMissionIcon icon, string markerType = "DARC_MISSION")
	{
		m_sIcon = icon;
		m_sMarkerType = markerType;
		m_bShowMarker = showMarker;
	}
	
	void ShowMarker()
	{
		//If static mission, check if we are to show a marker
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (IsStatic() && !baseGameMode.missionFrame.m_Config.showStaticMissionMarker)
		{
			return;
		}
		
		if (m_bShowMarker)
		{
			SDRC_MapMarkerHelper.CreateMapMarker(GetPos(), m_sIcon, GetId(), GetTitle());
		}
	}
	
	void MoveMarker()
	{
		SDRC_MapMarkerHelper.DeleteMarker(GetId());
		SetMarker(m_bShowMarker, GetMarker());
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
	\param checkOnlyWinCondition If set to true, ignore any timing related checks.
	*/
	bool IsActive(bool checkOnlyWinCondition = false)
	{
		bool isWin = false;
		
		//Check the different winning conditions
		if (m_State == DC_EMissionState.ACTIVE && !m_bMissionIsEnding && m_iAICountOriginal > 0)
		{			
			float aiKillPercentage = (1 - GetAICount()/m_iAICountOriginal);
			
			switch (m_WinCondition)
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
			
/*			if (SDRC_AIHelper.AreAllGroupsDead(m_Groups))
			{
				isWin = true;
			}*/
			
			if (isWin)
			{
				SDRC_Log.Add("[SDRC_Mission:IsActive] Mission WIN: " + GetId() + " : " + GetTitle(), LogLevel.DEBUG);
			}
				
			//If we did win the mission, set the message and prepare for ending.
			if (isWin)
			{
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
			if (SDRC_PlayerHelper.PlayerGetClosestToPos(m_Pos, 0, m_iActiveDistance))
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
			DoLose();
		}

		SDRC_Log.Add("[SDRC_Mission:IsActive] END - Mission " + GetId() + " : " + GetTitle() + " has ended.", LogLevel.DEBUG);				
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
		if (m_PosDestination != "0 0 0")
		{
			destinationName = SDRC_Locations.CreateName(m_PosDestination, "any");			
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
		SDRC_Log.Add("[SDRC_Mission:GetAICountDelayed] Mission " + GetId() + " spawned " + m_iAICountOriginal + " AIs.", LogLevel.SPAM);
	}
}