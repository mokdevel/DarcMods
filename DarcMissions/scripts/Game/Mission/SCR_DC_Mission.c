
//------------------------------------------------------------------------------------------------
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
    private string m_Id;
    private vector m_POS;
    private string m_PosName;
    private string m_Title;
    private string m_Info;
	private string m_MarkerId;
//	MapDescriptorComponent m_MapDescriptorComponent = new MapDescriptorComponent;
				
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission()
	{
		m_State = DC_MissionState.INIT;
		m_Id = DC_ID_PREFIX + string.ToString(System.GetTickCount());
		m_POS = "0 0 0";
		m_PosName = "";
		m_Title = "";
		m_Info = "";
		m_MarkerId = "";
//		m_MapDescriptorComponent.IsActive();		
	}

	//------------------------------------------------------------------------------------------------
	void MissionRun()	//You should override this in your mission
	{
		if (m_State == DC_MissionState.INIT)
		{
			//Add init code
			m_State = DC_MissionState.ACTIVE;	
		}
		
		if (m_State == DC_MissionState.END)
		{
			//Add code clean up code
			MissionEnd();
			m_State = DC_MissionState.EXIT;	
		}
		
		if (m_State == DC_MissionState.ACTIVE)
		{
			//Add code
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, 3000);		
	}

	//------------------------------------------------------------------------------------------------
	void MissionEnd()	//You should override this in your mission
	{
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
	string GetId()
	{
		return m_Id;
	}	
			
	//------------------------------------------------------------------------------------------------
	vector GetPos()
	{
		return m_POS;
	}

	void SetPos(vector pos)
	{
		m_POS = pos;
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
	string GetMarkerId()
	{
		return m_MarkerId;
	}

	void SetMarkerId(string markerId)
	{
		m_MarkerId = markerId;
	}	
}