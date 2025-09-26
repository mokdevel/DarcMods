
//------------------------------------------------------------------------------------------------
class SDRC_MissionStat : Managed
{
	string id;						//The mission id assigned by DarcMissions
	int requestId;					//The request id defined by an external requestor
	SDRC_EMissionType type;			
	SDRC_EMissionState state;
	SDRC_EMissionSuccess success;
		
	void Set(string id_, int requestId_, SDRC_EMissionType type_, SDRC_EMissionState state_, SDRC_EMissionSuccess success_)
	{
		id = id_;
		requestId = requestId_;
		type = type_;
		state = state_;
		success = success_;
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_MissionStats
{
	static private ref array<ref SDRC_MissionStat> m_MissionStatList = {};
	
	//------------------------------------------------------------------------------------------------	
	static void Dump()
	{
		foreach (int i, SDRC_MissionStat stat : m_MissionStatList)
		{
			SDRC_Log.Add("[SDRC_MissionStat:Dump] " + i + ": " + stat.id + " - " + SCR_Enum.GetEnumName(SDRC_EMissionType, stat.type) + " - " + SCR_Enum.GetEnumName(SDRC_EMissionState, stat.state) + " - " + SCR_Enum.GetEnumName(SDRC_EMissionSuccess, stat.success), LogLevel.NORMAL);
		}
	}	
	
	//------------------------------------------------------------------------------------------------	
	static void Add(string id, int requestId, SDRC_EMissionType type, SDRC_EMissionState state, SDRC_EMissionSuccess success)
	{
		ref SDRC_MissionStat stat = new SDRC_MissionStat();
		stat.Set(id, requestId, type, state, success);
		m_MissionStatList.Insert(stat);		
	}
	
	//------------------------------------------------------------------------------------------------
	static void UpdateState(string id, SDRC_EMissionState state)
	{
		int idx = FindIndex(id);
		if (idx > -1)
		{
			m_MissionStatList[idx].state = state;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static void UpdateSuccess(string id, SDRC_EMissionSuccess success)
	{
		int idx = FindIndex(id);
		if (idx > -1)
		{
			m_MissionStatList[idx].success = success;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find index to right item in m_MissionStatList. 
	
	NOTE: Overloaded function. You can ask with id (string) for a mission, or with the requestId (int)
	*/		

	static private int FindIndex(string id)
	{
		int i = -1;
		
		foreach(SDRC_MissionStat stat : m_MissionStatList)
		{
			i++;
			
			if (stat.id == id)
			{
				return i;
			}			
		}
		
		return -1;
	}

	//------------------------------------------------------------------------------------------------	
	static private int FindIndex(int requestId)
	{
		int i = -1;
		
		foreach(SDRC_MissionStat stat : m_MissionStatList)
		{
			i++;
			
			if (stat.requestId == requestId)
			{
				return i;
			}			
		}
		
		return -1;
	}
		
	//------------------------------------------------------------------------------------------------
	static SDRC_MissionStat GetStat(int requestId)
	{
		int idx = FindIndex(requestId);
		if (idx > -1)
		{
			return m_MissionStatList[idx];
		}

		return null;
	}
		
	//------------------------------------------------------------------------------------------------
	static string GetId(int requestId)
	{
		int i = -1;
		
		foreach(SDRC_MissionStat stat : m_MissionStatList)
		{
			i++;
			
			if (stat.requestId == requestId)
			{
				return stat.id;
			}			
		}
		
		return "";
	}
	
}

//------------------------------------------------------------------------------------------------
// Remote console 
//------------------------------------------------------------------------------------------------

/*
SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
if (m_BaseGameMode)
{
	SDRC_MissionStats.Dump();
}
*/
