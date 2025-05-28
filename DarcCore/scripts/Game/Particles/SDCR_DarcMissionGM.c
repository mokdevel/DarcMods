[EntityEditorProps(category: "GameScripted/Particles", description: "")]
class SDCR_DarcMissionGMClass : SCR_ReplicatedParticleEffectEntityClass
{
}

class SDCR_DarcMissionGM : SCR_ReplicatedParticleEffectEntity
{	
	private bool m_bAdded = false;
	
	override event protected void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);		
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
