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
		
				GetGame().GetCallqueue().CallLater(EOnInitDelayed, 15000, false, owner);
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
				SDRC_Log.Add("[SDRC_DarcMissionGM:EOnInitDelayed] Server call!", LogLevel.SPAM);
				
				SDRC_DarcMissionGM ent = SDRC_DarcMissionGM.Cast(owner);
				if (!ent.IsAdded())
				{					
					ResourceName res = owner.GetPrefabData().GetPrefabName();
					SDRC_Log.Add("[SDRC_DarcMissionGM:EOnInitDelayed] Found: " + SDRC_Misc.GetSimpleEntityName(res) + " at " + owner.GetOrigin(), LogLevel.DEBUG);					
					
					//Handle NonValidArea requests
					SCR_EffectsModuleAreaMeshComponent effComp = SCR_EffectsModuleAreaMeshComponent.Cast(ent.FindComponent(SCR_EffectsModuleAreaMeshComponent));
					if (effComp)
					{
						SDRC_Log.Add("[SDRC_DarcMissionGM:EOnInitDelayed] This is a NonValidArea request.", LogLevel.DEBUG);
						
						SDRC_NonValidArea nva = new SDRC_NonValidArea();
						float width;
						float length;
						effComp.GetDimensions2D(width, length);
						
						nva.worldName = "";
						nva.pos = owner.GetOrigin();
						nva.radius = width;
						nva.name = SDRC_Locations.CreateName(nva.pos);
						//baseGameMode.missionFrame.m_aNonValidAreas.Insert(nva);
						baseGameMode.missionFrame.m_ConfigNonValidArea.m_NonValidAreas.Insert(nva);
						
						SDRC_SpawnHelper.DespawnItem(owner);						
					}
					else //Handle missions requests
					{
						//Add to the requested mission to list for spawning
						ref SDRC_MissionRequested mission = new SDRC_MissionRequested();
						mission.entityID = owner.GetID();
											
						baseGameMode.missionFrame.m_missionsRequested.Insert(mission);
						
						ent.AddedToList();					
					}
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
