//SDRC_RPLGMComp_NonValidArea.c

//------------------------------------------------------------------------------------------------
/*!
RPL component to provide GM map information to clients.

NOTE: This has been added to {}}Prefabs/Characters/DarcMissions/DefaultPlayerController.et
*/

//------------------------------------------------------------------------------------------------
modded class SDRC_RplGMComp
{
	//------------------------------------------------------------------------------------------------	
	// NonValidArea stuff
	//------------------------------------------------------------------------------------------------	

	//------------------------------------------------------------------------------------------------
	/*!	
	Request for NonValidArea size change
	*/
 	void DoNonValidAreaSizeChange(int playerID, string nvaId, float size)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:DoNonValidAreaIncrease] Change of " + size + " asked for NonValidArea " + nvaId + ". Requested by " + playerID, LogLevel.DEBUG);	
		NonValidAreaSizeChange(nvaId, size);
		SyncMapSymbols(playerID);
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Request for NonValidArea save to file
	*/
 	void DoNonValidAreaSave(int playerID)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:DoNonValidAreaSave] Asked for save. Requested by " + playerID, LogLevel.DEBUG);	
		SaveNonValidAreaData();
	}
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Request for NonValidArea deletion
	*/
 	void DoDeleteNonValidArea(int playerID, string nvaId)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:DoDeleteNonValidArea] Deletion of NonValidArea " + nvaId + " requested by " + playerID, LogLevel.DEBUG);	
		DeleteNonValidArea(nvaId);
		SyncMapSymbols(playerID);
	}	
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Delete a NonValidArea.
	*/	
	void DeleteNonValidArea(string id, SDRC_EMissionSuccess success = SDRC_EMissionSuccess.DELETED)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!baseGameMode)
		{
			SDRC_Log.Add("[SDRC_RplGMComp:DeleteNonValidArea] SCR_BaseGameMode not found.", LogLevel.ERROR);
		}
		
		int idx = id.ToInt();
		
		if (idx < baseGameMode.missionFrame.m_ConfigNonValidArea.m_NonValidAreas.Count())
		{
			SDRC_Log.Add("[SDRC_RplGMComp:DeleteNonValidArea] Deleting NonValidArea: " + idx + " - " + baseGameMode.missionFrame.m_ConfigNonValidArea.m_NonValidAreas[idx].name, LogLevel.DEBUG);
			baseGameMode.missionFrame.m_ConfigNonValidArea.m_NonValidAreas.RemoveOrdered(idx);
		}		
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Adjust the size of a NonValidArea
	*/	
	void NonValidAreaSizeChange(string id, float size)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!baseGameMode)
		{
			SDRC_Log.Add("[SDRC_RplGMComp:NonValidAreaSizeChange] SCR_BaseGameMode not found.", LogLevel.ERROR);
		}
		
		int idx = id.ToInt();
		
		SDRC_Log.Add("[SDRC_RplGMComp:NonValidAreaIncrease] Changing area " + idx + " with " + size, LogLevel.DEBUG);		
		baseGameMode.missionFrame.m_ConfigNonValidArea.m_NonValidAreas[idx].ChangeRadius(size);
	}	
			
	//------------------------------------------------------------------------------------------------
	/*!	
	Save the NonValidArea json
	*/	
	void SaveNonValidAreaData()
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!baseGameMode)
		{
			SDRC_Log.Add("[SDRC_RplGMComp:SaveNonValidAreaData] SCR_BaseGameMode not found.", LogLevel.ERROR);
		}
		
		SDRC_Log.Add("[SDRC_RplGMComp:SaveNonValidAreaData] Saving...", LogLevel.DEBUG);		
		baseGameMode.missionFrame.m_NonValidAreaJsonApi.Save(baseGameMode.missionFrame.m_ConfigNonValidArea, SDRC_Config.Cast(baseGameMode.missionFrame.m_ConfigNonValidArea));
	}		
}