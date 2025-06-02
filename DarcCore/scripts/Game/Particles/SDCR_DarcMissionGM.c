[EntityEditorProps(category: "GameScripted/Particles", description: "")]
class SDRC_DarcMissionGMClass : SCR_ReplicatedParticleEffectEntityClass
{
}

class SDRC_DarcMissionGM : SCR_ReplicatedParticleEffectEntity
{	
	private bool m_bAdded = false;

/*	override event protected bool RplLoad(ScriptBitReader reader)
	{
		SDRC_Log.Add("[SDRC_DarcMissionGM:RplLoad] Here we are!", LogLevel.DEBUG);		
		return super.RplLoad(reader);
	}	*/
	
	override event protected void EOnInit(IEntity owner)	
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		if (baseGameMode.IsMaster())
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
		
		super.EOnInit(owner);		
	}
	
/*	override event protected void EOnActivate(IEntity owner)
	{
		SDRC_Log.Add("[SDRC_DarcMissionGM:EOnActivate] Here we are!", LogLevel.DEBUG);
		super.EOnActivate(owner);		
	}*/
	
	void AddedToList()
	{
		m_bAdded = true;
	}
	
	bool IsAdded()
	{
		return m_bAdded;
	}
}
