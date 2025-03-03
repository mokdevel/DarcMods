//------------------------------------------------------------------------------------------------
enum DC_EMissionType
{
	NONE,
	HUNTER,
	OCCUPATION,
	CHOPPER,
	CONVOY,
	CRASHSITE,
	PATROL
};

enum DC_MissionState
{
	NONE,		//Unknown state. Nothing should be run at this state.
	INIT,		//The mission is being init. Things are spawned etc.
	ACTIVE,		//Normal state when mission is running.	
	END,		//Mission is ending. Things are cleaned, despawned etc.
	EXIT		//State to inform the MissionFrame that the mission should be destroyed.
};

//------------------------------------------------------------------------------------------------
class SCR_DC_Mission
{
	//Common for all missions
	private DC_MissionState m_State;
	private DC_EMissionType m_Type;
	private bool m_Static;		//Defines if the mission is dynamic or static. Dynamic is default. 
    private string m_Id;
    private vector m_Pos;
    private string m_PosName;
    private string m_Title;
    private string m_Info;
	//Internals
	private int m_StartTime;		//Seconds when mission started
	private int m_EndTime;			//Seconds when mission shall end.
	private int m_ActiveTime;		//Seconds of how long the mission should be active 
	//Internals without getters
	private int m_ActiveDistance;	//The distance to a player to keep the mission active 
	
	protected ref array<IEntity> m_EntityList = {};		//Entities (e.g., tents) spawned
	protected ref array<SCR_AIGroup> m_Groups = {};		//Groups spawned
	
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission()
	{
		m_State = DC_MissionState.INIT;
		m_Type = DC_EMissionType.NONE;
		m_Static = false;
		m_Id = DC_ID_PREFIX + string.ToString(System.GetTickCount());
		m_Pos = "0 0 0";
		m_PosName = "";
		m_Title = "";
		m_Info = "";
		m_StartTime = (System.GetTickCount() / 1000); 			//The time in seconds when the game was started.
		SetActiveTime(DC_MISSION_LIFECYCLE_TIME_DEFAULT*20);	//Sets m_EndTick. NOTE: This is properly set in MissionFrame to use the config value. This is just some default.
	}

	//------------------------------------------------------------------------------------------------
	void MissionRun()	//You should override this in your mission
	{
		if (m_State == DC_MissionState.INIT)
		{
			//Add init code
			SetState(DC_MissionState.ACTIVE);
		}
		
		if (m_State == DC_MissionState.END)
		{
			//Add code clean up code
			MissionEnd();
			SetState(DC_MissionState.EXIT);
		}
		
		if (m_State == DC_MissionState.ACTIVE)
		{
			//Add code for runtime
			
			//Eventually when mission is to ended do this:
			//SetState(DC_MissionState.END);
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
		SCR_DC_Log.Add("[SCR_DC_Mission:MissionEnd] Deleting entities", LogLevel.DEBUG);
		foreach(IEntity entity : m_EntityList)
		{
			if (entity)
			{
				SCR_DC_Log.Add("[SCR_DC_Mission:MissionEnd] Despawning: " + entity.GetPrefabData().GetPrefabName(), LogLevel.DEBUG);
				SCR_DC_SpawnHelper.DespawnItem(entity);
			}
		}		
		
		//Remove AI
		SCR_DC_Log.Add("[SCR_DC_Mission:MissionEnd] Deleting AI groups", LogLevel.DEBUG);
		foreach(SCR_AIGroup group : m_Groups)
		{
			SCR_DC_AIHelper.GroupDelete(group);			
		}		

		//Remove marker from map
		SCR_DC_MapMarkerHelper.DeleteMarker(GetId());				
	}

	//------------------------------------------------------------------------------------------------
	DC_MissionState GetState()
	{
		return m_State;
	}

	void SetState(DC_MissionState state)
	{
		m_State = state;
	}

	//------------------------------------------------------------------------------------------------
	DC_MissionState GetType()
	{
		return m_Type;
	}

	void SetType(DC_EMissionType type)
	{
		m_Type = type;
	}

	//------------------------------------------------------------------------------------------------
	DC_MissionState IsStatic()
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
	bool IsActive()
	{
		//Are there players still nearby
		if (SCR_DC_PlayerHelper.PlayerGetClosestToPos(m_Pos, 0, m_ActiveDistance))
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
	void SetActiveDistance(int distance)	
	{
		m_ActiveDistance = distance;
	}		
	
	//------------------------------------------------------------------------------------------------
	void SetActiveTime(int seconds)	
	{
		m_ActiveTime = seconds;
		//m_EndTime = m_StartTime + m_ActiveTime;
		ResetActiveTime();
	}		

	//------------------------------------------------------------------------------------------------
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
	void SetMarker(bool setMarker, DC_EMissionIcon icon)
	{
		if(setMarker)
		{
			SCR_DC_MapMarkerHelper.CreateMapMarker(GetPos(), icon, GetId(), GetTitle());
		}
	}		
}