//Helpers SDRC_GMHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_GMHelper
{		
	static private SCR_BaseGameMode m_BaseGameMode;
	static private SDRC_RplGMComp m_RplGMComp;
	
	//------------------------------------------------------------------------------------------------
	/*!	
	A common function to find default information
	*/	
	static private void FindGameModeAndComponent()
	{		
		m_BaseGameMode = null;
		m_RplGMComp = null;
		
		m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!m_BaseGameMode)
		{
			SDRC_Log.Add("[SDRC_GMHelper:FindGameModeAndComponent] SCR_BaseGameMode not found", LogLevel.ERROR);
			return;
		}
	
		m_RplGMComp = SDRC_RplGMComp.GetInstance();
		if (!m_RplGMComp)
		{
			SDRC_Log.Add("[SDRC_GMHelper:FindGameModeAndComponent] SDRC_RplGMComp not found", LogLevel.ERROR);
			return;
		}					
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Find the clicked marker on the map.
	\return -1 if no marker found
	*/
	static int GetSymbolIndex(float worldX, float worldY)
	{
		const int MARKER_SIZE_BB = 64;//(24 * 0.8);		//Marker 'bounding box size' when searhing for mouse hit		
		int symbolIdx = -1;
		
		SCR_MapEntity m_MapEntity = SCR_MapEntity.GetMapInstance();
		
		if (!m_MapEntity)
			return symbolIdx;
		
		float currentZoom = m_MapEntity.GetCurrentZoom();

		//SDRC_Log.Add("[SDRC_GMHelper:GetMarkerIndex] pos: " + worldX + " , " + worldY + " z:" + currentZoom, LogLevel.NORMAL);	
				
		vector pos = "0 0 0";
		pos[0] = worldX;
		pos[2] = worldY - (MARKER_SIZE_BB/2);///currentZoom;	//Move the point to check up a little
		
		float distanceCheck = (MARKER_SIZE_BB/2)/currentZoom;		
				
		SDRC_RplGMComp rplGMComp = SDRC_RplGMComp.GetInstance();
		
		if (rplGMComp)
		{
			int idx = 0;
			 
			foreach (SDRC_GMMapSymbol symbol : rplGMComp.m_Symbols)
			{
				//SDRC_Log.Add("[SDRC_GMHelper:GetMarkerIndex] Checking: " + pos + " vs " + symbol.vPos + " r=" + radius, LogLevel.NORMAL);
				
				//Find symbol: Marker
				if (symbol.symbolType == SDRC_EDrawSymbol.MARKER && symbol.visible)
				{
					float distance = vector.DistanceXZ(pos, symbol.vPos);
					if (SDRC_Misc.IsPosNearPos(pos, symbol.vPos, distanceCheck))
					{
						SDRC_Log.Add("[SDRC_GMHelper:GetCircleIndex] Found: " + idx, LogLevel.DEBUG);
						symbolIdx = idx;
						break;
					}								
				}
				
				//Find symbol: NonValidArea
				if (symbol.symbolType == SDRC_EDrawSymbol.NON_VALID_AREA)
				{
					if (SDRC_Misc.IsPosNearPos(pos, symbol.vPos, symbol.fRadius))
					{
						SDRC_Log.Add("[SDRC_GMHelper:GetCircleIndex] Found: " + idx, LogLevel.DEBUG);
						symbolIdx = idx;
						break;
					}								
				}
								
				idx++;
			}
			
			if (symbolIdx > -1)
			{
				SDRC_Log.Add("[SDRC_GMHelper:GetMarkerIndex] Found symbol - index: " + symbolIdx, LogLevel.SPAM);
			}
		}		
		
		return symbolIdx;
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
		
		//If missionFrame is not available yet, stop
 		if (!m_BaseGameMode.missionFrame)
		{
			return;
		}
		
		AddNonValidAreas();
		AddMarkers();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Fill the symbol table with NonValidAreas
	*/
	static private void AddNonValidAreas()
	{
		if ((m_BaseGameMode) && (m_RplGMComp))
		{
			bool visibleForGm = m_BaseGameMode.m_SDRC_Core.m_Config.showOnGMMapNonValidArea;
			foreach (int idx, SDRC_NonValidArea nonValidArea : m_BaseGameMode.missionFrame.m_ConfigNonValidArea.m_NonValidAreas)
			{
				m_RplGMComp.AddSymbolCircle(visibleForGm, nonValidArea.pos, nonValidArea.name, nonValidArea.radius, idx, ARGB(75, 255, 0, 0));
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Fill the symbol table with ACTIVE markers
	*/	
	static private void AddMarkers()
	{
		if ((m_BaseGameMode) && (m_RplGMComp))
		{
			bool visibleForGm = m_BaseGameMode.m_SDRC_Core.m_Config.showOnGMMapMissionMarker;
			foreach (SDRC_Mission mission : m_BaseGameMode.missionFrame.m_MissionList)
			{
				if (mission.GetState() == SDRC_EMissionState.ACTIVE)
				{
					//TBD: This should also fill GetMarkerType() information
					SDRC_EMissionIcon icon = mission.GetMarkerIcon();
					string markerType = mission.GetMarkerType();
/*					if (mission.GetMarkerType() == "")
					{
						icon = SDRC_EMissionIcon.GM_MISSION_X_MAP;
					}*/
					m_RplGMComp.AddSymbolMarker(visibleForGm, mission.GetPos(), mission.GetType(), icon, markerType, mission.GetActiveTime(), mission.GetId(), mission.GetTitle());
				}
			}
		}	
	}
}
