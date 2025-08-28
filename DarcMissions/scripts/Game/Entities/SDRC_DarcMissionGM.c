[EntityEditorProps(category: "GameScripted/Particles", description: "")]
class SDRC_DarcMissionGMClass : GenericEntityClass
{
}

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for missions. 
*/
class SDRC_DarcMissionGM : GenericEntity
{	
	private bool m_bAdded = false;

	//------------------------------------------------------------------------------------------------
	override event protected void EOnInit(IEntity owner)	
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		
		if (baseGameMode)		
		{
			//When the entity is created, EOnInitDelayed happens both on server and client. 
			//On server we store it in a list for spawning.
			//On client, we do nothing.
			if (baseGameMode.IsMaster() && baseGameMode.missionFrame)
			{
				SDRC_Log.Add("[SDRC_DarcMissionGM:EOnInit] Entity created.", LogLevel.DEBUG);
		
				GetGame().GetCallqueue().CallLater(EOnInitDelayed, 5000, false, owner);
			}
		}
			
		super.EOnInit(owner);		
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Delayed 'EOnInit' to give some time before mission starts
	*/
	protected void EOnInitDelayed(IEntity owner)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		
		if (baseGameMode)		
		{
			//When the entity is created, EOnInitDelayed happens on server. On server we store it in a list for spawning.
			//Client should never run this.
			if (baseGameMode.IsMaster() && baseGameMode.missionFrame)
			{
				SDRC_Log.Add("[SDRC_DarcMissionGM:EOnInitDelayed] Server call!", LogLevel.DEBUG);
				
				SDRC_DarcMissionGM ent = SDRC_DarcMissionGM.Cast(owner);
				if (!ent.IsAdded())
				{
					//Add to the requested mission to list for spawning
					ref SDRC_MissionRequested mission = new SDRC_MissionRequested();
					mission.entityID = owner.GetID();
										
					baseGameMode.missionFrame.m_missionsRequested.Insert(mission);
					ent.AddedToList();
					
					ResourceName res = owner.GetPrefabData().GetPrefabName();
					SDRC_Log.Add("[SDRC_DarcMissionGM:EOnInitDelayed] Found: " + SDRC_Misc.GetSimpleEntityName(res) + " at " + owner.GetOrigin(), LogLevel.DEBUG);				
				}
			}
			else
			{
				SDRC_Log.Add("[SDRC_DarcMissionGM:EOnInitDelayed] Client call! This should never happen.", LogLevel.DEBUG);
			}
		}	
	}
	
	//------------------------------------------------------------------------------------------------
	void AddedToList()
	{
		m_bAdded = true;
	}
	
	bool IsAdded()
	{
		return m_bAdded;
	}
}
