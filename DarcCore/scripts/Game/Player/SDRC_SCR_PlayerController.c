/*
modded class SCR_PlayerController : PlayerController
{
	
    PlayerManager m_PlayerManager;
	
    override void OnInit(IEntity owner)
    {
		super.OnInit(owner);
		
		SDRC_Log.Add("[SDRC_SCR_PlayerController] OnInit", LogLevel.DEBUG);
        SetEventMask(EntityEvent.INIT);
    }
	
}
*/

/*	override void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);
		
		SetEventMask(EntityEvent.FRAME | EntityEvent.POSTFRAME);
        SetFlags(EntityFlags.ACTIVE, true);
		SDRC_Log.Add("[SDRC_SCR_PlayerController] EOnActivate", LogLevel.DEBUG);
		
        m_PlayerManager = GetGame().GetPlayerManager();
	}*/
