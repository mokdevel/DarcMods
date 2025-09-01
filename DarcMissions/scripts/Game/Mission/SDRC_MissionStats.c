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

class SDRC_MissionStats
{
	static private ref array<ref SDRC_MissionStat> m_MissionList = {};
	
	static void Add(string id, int requestId, DC_EMissionState state, DC_EMissionSuccess success)
	{
		ref SDRC_MissionStat stat = new SDRC_MissionStat();
		stat.Set(id, requestId, state, success);
		m_MissionList.Insert(stat);		
	}
	
	static void UpdateState(string id, DC_EMissionState state)
	{
		int idx = FindIndex(id);
		if (idx > -1)
		{
			m_MissionList[idx].state = state;
		}
	}
	
	static void UpdateSuccess(string id, DC_EMissionSuccess success)
	{
		int idx = FindIndex(id);
		if (idx > -1)
		{
			m_MissionList[idx].success = success;
		}
	}
	
	static int FindIndex(string id)
	{
		int i = -1;
		
		foreach(SDRC_MissionStat mission : m_MissionList)
		{
			i++;
			
			if (mission.id == id)
			{
				return i;
			}			
		}
		
		return -1;
	}
	
	static string GetId(int requestId)
	{
		int i = -1;
		
		foreach(SDRC_MissionStat mission : m_MissionList)
		{
			i++;
			
			if (mission.requestId == requestId)
			{
				return mission.id;
			}			
		}
		
		return "";
	}
	
}
