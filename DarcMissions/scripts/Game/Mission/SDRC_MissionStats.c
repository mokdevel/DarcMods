//------------------------------------------------------------------------------------------------
class SDRC_MissionStat : Managed
{
	string id;
	int requestId;
	DC_EMissionState state;
	DC_EMissionSuccess success;
		
	void Set(string id_, int requestId_, DC_EMissionState state_, DC_EMissionSuccess success_)
	{
		id = id_;
		requestId = requestId_;
		state = state_;
		success = success_;
	}	
}

//------------------------------------------------------------------------------------------------
class SDRC_MissionStats
{
	static private ref array<ref SDRC_MissionStat> m_MissionStatList = {};
	
	//------------------------------------------------------------------------------------------------	
	static void Add(string id, int requestId, DC_EMissionState state, DC_EMissionSuccess success)
	{
		ref SDRC_MissionStat stat = new SDRC_MissionStat();
		stat.Set(id, requestId, state, success);
		m_MissionStatList.Insert(stat);		
	}
	
	//------------------------------------------------------------------------------------------------
	static void UpdateState(string id, DC_EMissionState state)
	{
		int idx = FindIndex(id);
		if (idx > -1)
		{
			m_MissionStatList[idx].state = state;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static void UpdateSuccess(string id, DC_EMissionSuccess success)
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
