[EntityEditorProps(category: "GameScripted/Particles", description: "")]
class SDCR_ReplicatedParticleEffectEntityClass : SCR_ReplicatedParticleEffectEntityClass
{
}

class SDCR_ReplicatedParticleEffectEntity : SCR_ReplicatedParticleEffectEntity
{
	override event protected void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);
		
		Print("WHOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOA!");
		
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		baseGameMode.missionFrame.SendHint();
	}
}
