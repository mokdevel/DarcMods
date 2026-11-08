//SDRC_SCR_EditorManagerEntity_Missions.c

modded class SCR_EditorManagerEntity
{
	private bool m_bDataAvailable = false;
	private bool m_bDataNeedsRefresh = true;
	
/*	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)	
	{
		//Maybe find m_BaseGameMode and m_SDRC_RplLineDrawEntity here for cleaner code
		super.EOnInit(owner);
	}*/
	
	//------------------------------------------------------------------------------------------------
	override bool IsOpened()
	{
		bool isOpened = super.IsOpened();
		
		//If in ArmaVision, leave
		if (IsLimited())
		{
			//SDRC_Log.Add("[SDRC_SCR_EditorManagerEntity_Missions:IsOpened] Limited mode.", LogLevel.DEBUG);
			return isOpened;
		}

		//If closed do nothing.
		if (!isOpened)
		{
			//If closed and no data available, ask for refresh
			if (!m_bDataAvailable)
			{
				m_bDataNeedsRefresh = true;
			}
			
			return isOpened;
		}
		
		//If data available, leave
		if (m_bDataAvailable)
		{
			return isOpened;
		}
		
		if (isOpened)
		{	
			//SDRC_Log.Add("[SDRC_SCR_EditorManagerEntity_Missions:IsOpened] GM mode opened.", LogLevel.DEBUG);			
						
			if ( (!m_bDataAvailable) && (m_bDataNeedsRefresh) )
			{
				SDRC_RplPlayerComp playerComponent = SDRC_RplPlayerComp.FindLocalInstance();
				if (playerComponent)
				{
					playerComponent.AskForInfo_MissionData();
				}
				
				if (playerComponent.IsMissionDataReady())
				{
					m_bDataAvailable = true;
				}
				else
				{
					//Inform that you should reopen GM to receive data. m_bDataNeedsRefresh is set to true once you close the GM.
					m_bDataNeedsRefresh = false;
					SDRC_Log.Add("[SDRC_SCR_EditorManagerEntity_Missions:IsOpened] Mission data not synced. Try opening GM again.", LogLevel.WARNING);
				}
			}			
		}
				
		return isOpened;
	}	
}