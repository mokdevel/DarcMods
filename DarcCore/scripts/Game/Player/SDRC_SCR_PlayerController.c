modded class SCR_PlayerController : PlayerController
{
	
#ifndef SDRC_RELEASE
    PlayerManager m_PlayerManager;
	
    override void OnInit(IEntity owner)
    {
		super.OnInit(owner);
		
		SDRC_Log.Add("[SDRC_SCR_PlayerController] OnInit", LogLevel.DEBUG);
        SetEventMask(EntityEvent.INIT);
    }
	
	override void EOnActivate(IEntity owner)
	{
		super.EOnActivate(owner);
		
		SetEventMask(EntityEvent.FRAME | EntityEvent.POSTFRAME);
        SetFlags(EntityFlags.ACTIVE, true);
		SDRC_Log.Add("[SDRC_SCR_PlayerController] EOnActivate", LogLevel.DEBUG);
		
        m_PlayerManager = GetGame().GetPlayerManager();
/*        if(SCR_PlayerController.GetLocalControlledEntity() != owner)
		{
			SDRC_Log.Add("[SDRC_SCR_PlayerController] GetPlayerManager failed", LogLevel.DEBUG);
            return;
		}*/
        GetGame().GetCallqueue().CallLater(DelayedInit,1000,false,owner);
	}
	
    //------------------------------------------------------------------------------------------------
    void DelayedInit(IEntity owner)
    {
		SDRC_Log.Add("[SDRC_SCR_PlayerController] DelayedInit", LogLevel.DEBUG);
		
        SCR_EditorManagerCore core = SCR_EditorManagerCore.Cast(SCR_EditorManagerCore.GetInstance(SCR_EditorManagerCore));
        if (!core)
            return;

        SCR_EditorManagerEntity editorManager = core.GetEditorManager();
        if (!editorManager)
            return;        
        
        editorManager.GetOnOpened().Insert(FunctionToInvoke);
    }
	
    //------------------------------------------------------------------------------------------------
    void FunctionToInvoke()
    {
		SDRC_Log.Add("[SDRC_SCR_PlayerController] GM opened", LogLevel.DEBUG);
		
        int PlayerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(GetControlledEntity());
        Rpc(Ask_Authority_PrintStuff, PlayerID);
    } 

    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void Ask_Authority_PrintStuff(int PlayerID)
    {
        SDRC_Log.Add("[SDRC_SCR_PlayerController] GM Mode by: " + m_PlayerManager.GetPlayerName(PlayerID), LogLevel.DEBUG);        
    }
		
#endif	
}
