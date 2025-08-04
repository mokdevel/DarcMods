//Helpers SDRC_GMHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_GMHelper
{		
	static private SCR_BaseGameMode m_BaseGameMode;
	static private SDRC_RplGMComp m_GmComponent;
	
	//------------------------------------------------------------------------------------------------
	static void Init()
	{
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!m_BaseGameMode)
		{
			SDRC_Log.Add("[SDRC_GMHelper:Init] SCR_BaseGameMode not found", LogLevel.ERROR);
		}		
	
		SDRC_RplGMComp m_GmComponent = SDRC_RplGMComp.GetInstance();
		if (!m_GmComponent)
		{
			SDRC_Log.Add("[SDRC_GMHelper:Init] SDRC_RplGMComp not found", LogLevel.ERROR);
		}					
	}
	
	//------------------------------------------------------------------------------------------------
	static void AddNonValidAreas()
	{
		Init();
		if (m_BaseGameMode && m_GmComponent)
		{
			//TBD: if (m_BaseGameMode.missionFrame ... showOnGMMap)			
			foreach (SDRC_NonValidArea nonValidArea : m_BaseGameMode.missionFrame.m_aNonValidAreas)
			{
				m_GmComponent.AddSymbolCircle(nonValidArea.pos, nonValidArea.radius, ARGB(75, 255, 0, 0));
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static void AddMarkers()
	{
		Init();
		if (m_BaseGameMode && m_GmComponent)
		{
			foreach (SDRC_Mission mission : m_BaseGameMode.missionFrame.m_MissionList)
			{
				m_GmComponent.AddSymbolMarker(mission.GetPos(), mission.GetType(), 0);
			}
		}	
	}
}
