//------------------------------------------------------------------------------------------------
enum DC_EMissionType
{
	NONE,
	HUNTER,
	OCCUPATION,
	CONVOY,
	CRASHSITE,
	PATROL,
	SQUATTER,
	CHOPPER		//Mission not ready or working
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

//------------------------------------------------------------------------------------------------
class SDRC_MissionConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	int missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;	//How often the mission is run
	bool showMarker = true;
	bool showHint = true;
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
	//Internals without getters
	private int m_ActiveDistance;				//The distance to a player to keep the mission active. This is set to default, but could be changed by the mission.
	private int m_ActiveTimeToEnd;				//The time to keep mission active once all AIs are dead.
	private bool m_bMissionIsEnding;			//Once all AIs are dead, we're getting close to end the mission.
	
	protected ref array<IEntity> m_EntityList = {};		//Entities (e.g., tents) spawned
	protected ref array<SCR_AIGroup> m_Groups = {};		//Groups spawned
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission()
	{
		m_State = DC_EMissionState.INIT;
		m_Type = DC_EMissionType.NONE;
		m_Static = false;
		m_Id = DC_ID_PREFIX + string.ToString(System.GetTickCount());
		m_Pos = "0 0 0";
		m_PosName = "";
		m_Title = "";
		m_Info = "";
		m_StartTime = (System.GetTickCount() / 1000); 			//The time in seconds when the mission was started.
		SetActiveTime(SDRC_MISSION_CYCLE_TIME_DEFAULT*20);		//Sets m_EndTick. NOTE: This is properly set in MissionFrame to use the config value. This is just some default.
		m_ActiveDistance = 0;									//Set a default zero
		m_bMissionIsEnding = false;
	}

	//------------------------------------------------------------------------------------------------
	void MissionRun()	//You should override this in your mission
	{
		if (m_State == DC_EMissionState.INIT)
		{
			//Add init code
			SetState(DC_EMissionState.ACTIVE);
		}
		
		if (m_State == DC_EMissionState.END)
		{
			//Add code clean up code
			MissionEnd();
			SetState(DC_EMissionState.EXIT);
		}
		
		if (m_State == DC_EMissionState.ACTIVE)
		{
			//Add code for runtime
			
			//Eventually when mission is to ended do this:
			//SetState(DC_EMissionState.END);
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, 3000);		
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

	//------------------------------------------------------------------------------------------------
	int GetActiveTime()
	{
		int currentTime = (System.GetTickCount() / 1000);
		return m_EndTime - currentTime;
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
		return false;
	}	
	
	//------------------------------------------------------------------------------------------------
	void ResetActiveTime()	
	{
		int currentTime = (System.GetTickCount() / 1000);		
		m_EndTime = currentTime + m_ActiveTime;
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
	void SetMarker(bool setMarker, DC_EMissionIcon icon)
	{
		if (setMarker)
		{
			SDRC_MapMarkerHelper.CreateMapMarker(GetPos(), icon, GetId(), GetTitle());
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
}