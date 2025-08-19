[EntityEditorProps(category: "GameScripted/Particles", description: "")]
class SDRC_DarcMissionGMClass : GenericEntityClass
{
}

class SDRC_DarcMissionGM : GenericEntity
{	
	private bool m_bAdded = false;

	override event protected void EOnInit(IEntity owner)	
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		
		if (baseGameMode)		
		{
			//When the entity is created, EOnInit happens both on server and client. 
			//On server we store it in a list for spawning.
			//On client, we do nothing.
			if (baseGameMode.IsMaster() && baseGameMode.missionFrame)
			{
				SDRC_Log.Add("[SDRC_DarcMissionGM:EOnInit] Server call!", LogLevel.DEBUG);
				
				SDRC_DarcMissionGM ent = SDRC_DarcMissionGM.Cast(owner);
				if (!ent.IsAdded())
				{
					ent.AddedToList();
					
					ref SDRC_MissionRequested mission = new SDRC_MissionRequested();
					
					mission.entityID = owner.GetID();
					mission.pos = owner.GetOrigin();
					baseGameMode.missionFrame.m_missionsRequested.Insert(mission);
					
					ResourceName res = owner.GetPrefabData().GetPrefabName();
					SDRC_Log.Add("[SDRC_DarcMissionGM:EOnInit] Found: " + res + " at " + owner.GetOrigin(), LogLevel.DEBUG);				
				}
			}
			else
			{
				SDRC_Log.Add("[SDRC_DarcMissionGM:EOnInit] Client call!", LogLevel.DEBUG);
			}
		}
				
		super.EOnInit(owner);		
	}
	
	void AddedToList()
	{
		m_bAdded = true;
	}
	
	bool IsAdded()
	{
		return m_bAdded;
	}
}
