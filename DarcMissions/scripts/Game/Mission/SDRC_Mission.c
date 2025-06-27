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
	CHOPPER,	//Mission not ready or working
	
	REQUESTED = 100
};

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
	KILL_ALL_AI	
};

//------------------------------------------------------------------------------------------------
class SDRC_MissionConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	int missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;	//How often the mission is run
	bool showMarker = true;
	bool showHint = true;
	int xp = 0;
	DC_EMissionWinCondition winCondition = DC_EMissionWinCondition.KILL_ALL_AI;
	string winMessage = "";
	string loseMessage = "";
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
    private string m_PosName;
    private string m_Title;
    private string m_Info;
    private bool m_ShowHint;
	//Internals
	private int m_StartTime;					//Seconds when mission started
	private int m_EndTime;						//Seconds when mission shall end.
	private int m_ActiveTime;					//Seconds of how long the mission should be active
	private string m_sFaction;					//Faction for the mission		
	private bool m_bShowMarker;					//If the icon is to be shown
	private DC_EMissionIcon m_sIcon;			//The icon to show
	//Internals without getters
	private int m_ActiveDistance;				//The distance to a player to keep the mission active. This is set to default, but could be changed by the mission.
	private int m_ActiveTimeToEnd;				//The time to keep mission active once all AIs are dead.
	private bool m_bMissionIsEnding;			//Once all AIs are dead, we're getting close to end the mission.
	private bool m_bRequested;					//The missions spawn was requested by a an external party (like GM)
	
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
		m_PosName = "";
		m_Title = "";
		m_Info = "";
		m_ShowHint = true;
		SetFaction(SDRC_EnemyHelper.SelectEnemyFaction()); 		//m_sFaction 
		//Internals
		m_StartTime = (System.GetTickCount() / 1000); 			//The time in seconds when the mission was started.
		SetActiveTime(SDRC_MISSION_CYCLE_TIME_DEFAULT*20);		//Sets m_EndTick. NOTE: This is properly set in MissionFrame to use the config value. This is just some default.
		m_ActiveDistance = 0;									//Set a default zero
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

	void SetPos(vector pos)
	{
		m_Pos = pos;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetPosName()
	{
		return m_PosName;
	}

	void SetPosName(string posname)
	{
		m_PosName = posname;
	}

	//------------------------------------------------------------------------------------------------
	string GetTitle()
	{
		return m_Title;
	}
	
	void SetTitle(string title)
	{
		m_Title = title;
	}

	//------------------------------------------------------------------------------------------------
	string GetInfo()
	{		
		return m_Info;	//GetPosName() + " at " + GetPos();
	}

	void SetInfo(string info)
	{		
		m_Info = info;
	}

	//------------------------------------------------------------------------------------------------
	void SetActiveDistance(int distance)	
	{
		if (m_ActiveDistance > 0)
		{
			//It has been set by the mission already
		}
		else		
		{
			//Use provided distance
			m_ActiveDistance = distance;
		}
	}		
	
	//------------------------------------------------------------------------------------------------
	void SetActiveTimeToEnd(int seconds)	
	{
		m_ActiveTimeToEnd = seconds;
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
	bool IsActive()
	{
		//Are all AIs dead
		if (SDRC_AIHelper.AreAllGroupsDead(m_Groups) && m_State == DC_EMissionState.ACTIVE && !m_bMissionIsEnding)
		{
			//Mission is soon to be ending
			m_bMissionIsEnding = true;
			//Set ActiveTimeToEnd to be the final active time
			SetActiveTime(m_ActiveTimeToEnd);
			ResetActiveTime();
		}
		
		//Are there players still nearby
		if (SDRC_PlayerHelper.PlayerGetClosestToPos(m_Pos, 0, m_ActiveDistance))
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
		
		//Well, we should not be active anymore
		SDRC_Log.Add("[SDRC_Mission:IsActive] Mission " + GetId() + " : " + GetTitle() + " has ended.", LogLevel.NORMAL);				
		return false;
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
	void SetFaction(string faction)
	{
		m_sFaction = faction;
		SDRC_Log.Add("[SDRC_Mission:SetFaction] " + faction, LogLevel.SPAM);		
	}
			
	//------------------------------------------------------------------------------------------------
	string GetFaction()
	{
		return m_sFaction;
	}
			
	//------------------------------------------------------------------------------------------------
	void SetMarker(bool showMarker, DC_EMissionIcon icon)
	{
		m_sIcon = icon;
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
	
	//------------------------------------------------------------------------------------------------
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
	//\return bool status it the mission spawn was requested by a an external party (like GM)	
	//NOTE: m_bRequested is set in SDRC_Mission constructor
	bool IsRequested()
	{
		return m_bRequested;
	}	

	//------------------------------------------------------------------------------------------------
	void AddToEntityList(IEntity entity)
	{
		m_EntityList.Insert(entity);
	}
	
	IEntity GetFromEntityList(int index)
	{
		return m_EntityList[index];	
	}		

	//------------------------------------------------------------------------------------------------
	void AddToGroupsList(SCR_AIGroup group)
	{
		m_Groups.Insert(group);
	}		
}