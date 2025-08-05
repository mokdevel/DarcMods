//Helpers SDRC_GMHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_GMHelper
{		
	static private SCR_BaseGameMode m_BaseGameMode;
	static private SDRC_RplGMComp m_GmComponent;
	
	//------------------------------------------------------------------------------------------------
	static private void FindModeAndComponent()
	{		
		m_BaseGameMode = null;
		m_GmComponent = null;
		
		m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!m_BaseGameMode)
		{
			SDRC_Log.Add("[SDRC_GMHelper:FindModeAndComponent] SCR_BaseGameMode not found", LogLevel.ERROR);
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
			SDRC_Log.Add("[SDRC_GMHelper:FindModeAndComponent] SDRC_RplGMComp not found", LogLevel.ERROR);
			return;
		}					
	}
	
	//------------------------------------------------------------------------------------------------
	static void AddSymbols()
	{
		FindModeAndComponent();
		AddNonValidAreas();
		AddMarkers();
	}
	
	//------------------------------------------------------------------------------------------------
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
	static private void AddMarkers()
	{
		if ((m_BaseGameMode) && (m_GmComponent))
		{
			if (m_BaseGameMode.m_SDRC_Core.m_Config.showOnGMMapMissionMarker)
			{
				foreach (SDRC_Mission mission : m_BaseGameMode.missionFrame.m_MissionList)
				{
					m_GmComponent.AddSymbolMarker(mission.GetPos(), mission.GetType(), mission.GetMarker());
				}
			}
		}	
	}
}
