//SDRC_SCR_EditorManagerEntity_Missions.c

modded class SCR_EditorManagerEntity
{
	private bool m_bDataAvailable = false;
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)	
	{
		//Maybe find m_BaseGameMode and m_SDRC_RplLineDrawEntity here for cleaner code
		super.EOnInit(owner);
	}
	
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

		if (m_bDataAvailable)
		{
			return isOpened;
		}
				
		if (isOpened)
		{	
			SDRC_Log.Add("[SDRC_SCR_EditorManagerEntity_Missions:IsOpened] GM mode opened.", LogLevel.DEBUG);			
						
			if (!m_bDataAvailable)
			{
				SDRC_RplPlayerComp playerComponent = SDRC_RplPlayerComp.FindLocalInstance();
				if (playerComponent)
				{
					playerComponent.AskForInfo_MissionData();
				}

				m_bDataAvailable = true;
			}			
		}
				
		return isOpened;
	}	
}