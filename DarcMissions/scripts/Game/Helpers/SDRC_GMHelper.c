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
					if (mission.GetState() == SDRC_EMissionState.ACTIVE)
					{
						m_GmComponent.AddSymbolMarker(mission.GetPos(), mission.GetType(), mission.GetMarker(), mission.GetActiveTime(), mission.GetId(), mission.GetTitle());
					}
				}
			}
		}	
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Get details of the clicked marker on the map.
	\return null
	*/
	static SDRC_GMMapSymbol GetMarkerDetails(float worldX, float worldY)
	{
		FindGameModeAndComponent();
		
		int idx = -1;
		
		if ((m_BaseGameMode) && (m_GmComponent))
		{		
			idx = GetMarkerIndex(worldX, worldY);
		}
		else
		{
			return null;	
		}
			
		if (idx < 0)
		{
			return null;
		}
						
		return m_GmComponent.GetSymbolMarker(idx);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Find the clicked marker on the map.
	\return -1 if no marker found
	*/
	static int GetMarkerIndex(float worldX, float worldY)
	{
		int MARKER_SIZE_BB = (24 * 0.8);		//Marker 'bounding box size' when searhing for mouse hit		
		int symbolIdx = -1;

		SCR_MapEntity m_MapEntity = SCR_MapEntity.GetMapInstance();
		
		if (!m_MapEntity)
			return symbolIdx;
		
		float currentZoom = m_MapEntity.GetCurrentZoom();
		
		vector pos = "0 0 0";
		pos[0] = worldX;
		pos[2] = worldY - (MARKER_SIZE_BB/2)/currentZoom;	//Move the point to check up a little
		
		float distanceCheck = MARKER_SIZE_BB/currentZoom;		
				
		SDRC_RplGMComp gmComponent = SDRC_RplGMComp.GetInstance();
		if (gmComponent)
		{
			int idx = 0;
			
			foreach(SDRC_GMMapSymbol symbol : gmComponent.m_Symbols)
			{
				if (symbol.type == SDRC_EDrawSymbol.MARKER && symbol.visible)
				{
					float distance = vector.DistanceXZ(pos, symbol.pos);
					//SDRC_Log.Add("[SDRC_MapSystem:ShowMarkerInfo] Checking: " + cursorPos + " vs " + symbol.pos + " d=" + distance + " (" + distanceCheck + ")", LogLevel.NORMAL);
					if (SDRC_Misc.IsPosNearPos(pos, symbol.pos, distanceCheck))
					{
//						SDRC_Log.Add("[SDRC_MapSystem:ShowMarkerInfo] Found.", LogLevel.NORMAL);
						symbolIdx = idx;
						break;
					}								
				}
				idx++;
			}
			
			if (symbolIdx > -1)
			{
				SDRC_Log.Add("[SDRC_MapSystem:ShowMarkerInfo] Found marker - index: " + symbolIdx, LogLevel.SPAM);
			}
		}		
		
		return symbolIdx;
	}			
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Delete a mission with specific mission ID (DCMxxxx).
	*/	
	static void DeleteMission(string id, SDRC_EMissionSuccess success = SDRC_EMissionSuccess.DELETED)
	{
		FindGameModeAndComponent();
		int idx = m_BaseGameMode.missionFrame.FindMissionWithId(id);
		if (idx != -1)
		{
			m_BaseGameMode.missionFrame.m_MissionList[idx].SetState(SDRC_EMissionState.END);
			m_BaseGameMode.missionFrame.m_MissionList[idx].SetSuccess(success);
			SDRC_Log.Add("[SDRC_GMHelper:DeleteMission] Ending mission: " + id + " - " + m_BaseGameMode.missionFrame.m_MissionList[idx].GetTitle(), LogLevel.DEBUG);				
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Delete a mission with specific mission ID (DCMxxxx).
	*/	
	static void EndMission(string id, SDRC_EMissionSuccess success)
	{
		FindGameModeAndComponent();
		int idx = m_BaseGameMode.missionFrame.FindMissionWithId(id);
		if (idx != -1)
		{
			if (success == SDRC_EMissionSuccess.LOSE)
			{
				m_BaseGameMode.missionFrame.m_MissionList[idx].DoLose();
			}
			if (success == SDRC_EMissionSuccess.WIN)
			{
				m_BaseGameMode.missionFrame.m_MissionList[idx].DoWin();
			}
			SDRC_Log.Add("[SDRC_GMHelper:DeleteMission] Ending mission: " + id + " - (" + SCR_Enum.GetEnumName(SDRC_EMissionSuccess, success) + ")" + m_BaseGameMode.missionFrame.m_MissionList[idx].GetTitle(), LogLevel.DEBUG);				
		}
	}	
}
