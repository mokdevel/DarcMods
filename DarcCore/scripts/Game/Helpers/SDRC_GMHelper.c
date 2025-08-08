//Helpers SDRC_GMHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_GMHelper
{		
	static private SCR_BaseGameMode m_BaseGameMode;
	static private SDRC_RplGMComp m_GmComponent;
	
	//------------------------------------------------------------------------------------------------
	/*!	
	A common function to find default information
	*/	
	static private void FindGameModeAndComponent()
	{		
		m_BaseGameMode = null;
		m_GmComponent = null;
		
		m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!m_BaseGameMode)
		{
			SDRC_Log.Add("[SDRC_GMHelper:FindGameModeAndComponent] SCR_BaseGameMode not found", LogLevel.ERROR);
			return;
		}
		
		//If missionFrame is not available yet, stop
 		if (!m_BaseGameMode.missionFrame)
		{
			return;
		}
	
		m_GmComponent = SDRC_RplGMComp.GetInstance();
		if (!m_GmComponent)
		{
			SDRC_Log.Add("[SDRC_GMHelper:FindGameModeAndComponent] SDRC_RplGMComp not found", LogLevel.ERROR);
			return;
		}					
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Fill the symbol table with various information to sync to players
	- NonValidAreas
	- Markers
	*/
	static void AddSymbols()
	{
		FindGameModeAndComponent();
		AddNonValidAreas();
		AddMarkers();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Fill the symbol table with NonValidAreas
	*/
	static private void AddNonValidAreas()
	{
		if ((m_BaseGameMode) && (m_GmComponent))
		{
			if (m_BaseGameMode.m_SDRC_Core.m_Config.showOnGMMapNonValidArea)
			{
				foreach (SDRC_NonValidArea nonValidArea : m_BaseGameMode.missionFrame.m_aNonValidAreas)
				{
					m_GmComponent.AddSymbolCircle(nonValidArea.pos, nonValidArea.radius, ARGB(75, 255, 0, 0));
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Fill the symbol table with ACTIVE markers
	*/	
	static private void AddMarkers()
	{
		if ((m_BaseGameMode) && (m_GmComponent))
		{
			if (m_BaseGameMode.m_SDRC_Core.m_Config.showOnGMMapMissionMarker)
			{
				foreach (SDRC_Mission mission : m_BaseGameMode.missionFrame.m_MissionList)
				{
					if (mission.GetState() == DC_EMissionState.ACTIVE)
					{
						m_GmComponent.AddSymbolMarker(mission.GetPos(), mission.GetType(), mission.GetMarker(), mission.GetId());
					}
				}
			}
		}	
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Delete a mission with specific mission ID (DCMxxxx).
	*/	
	static void DeleteMission(string id)
	{
		FindGameModeAndComponent();
		int idx = m_BaseGameMode.missionFrame.FindMissionWithId(id);
		if (idx != -1)
		{
			m_BaseGameMode.missionFrame.m_MissionList[idx].SetState(DC_EMissionState.END);
			SDRC_Log.Add("[SDRC_RplGMComp:DeleteMission] Ending mission: " + id + " - " + m_BaseGameMode.missionFrame.m_MissionList[idx].GetTitle(), LogLevel.DEBUG);				
		}
	}	
}
