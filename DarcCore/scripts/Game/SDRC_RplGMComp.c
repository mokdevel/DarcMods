//SDRC_RplGMComp.c

//[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
class SDRC_RplGMCompClass : ScriptComponentClass{}
SDRC_RplGMCompClass g_RplGMCompClass;

class SDRC_RplGMComp : ScriptComponent
{
	private static SDRC_RplGMComp s_Instance;	
	
    PlayerManager m_PlayerManager;
    //------------------------------------------------------------------------------------------------
    override void OnPostInit(IEntity owner)
    {
		SDRC_Log.Add("[SDRC_RplGMComp] OnPostInit", LogLevel.DEBUG);
        SetEventMask(owner, EntityEvent.INIT);

		s_Instance = this;
    }
	
    //------------------------------------------------------------------------------------------------
    override void EOnInit(IEntity owner)
    {
        m_PlayerManager = GetGame().GetPlayerManager();
		
        if(SCR_PlayerController.GetLocalControlledEntity() != owner)
		{
			SDRC_Log.Add("[SDRC_RplGMComp] GetPlayerManager failed", LogLevel.DEBUG);
            return;
		}		
		
        GetGame().GetCallqueue().CallLater(DelayedInit,1000,false,owner);
    }
	
    //------------------------------------------------------------------------------------------------
    void DelayedInit(IEntity owner)
    {
		SDRC_Log.Add("[SDRC_RplGMComp] DelayedInit", LogLevel.DEBUG);
		
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
		SDRC_Log.Add("[SDRC_RplGMComp] GM opened", LogLevel.DEBUG);
		
		
		
        int PlayerID = m_PlayerManager.GetPlayerIdFromControlledEntity(GetOwner());
        Rpc(Ask_Authority_PrintStuff, PlayerID);
    }    
	
    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void Ask_Authority_PrintStuff(int PlayerID)
    {
        SDRC_Log.Add("[SDRC_RplGMComp] GM Mode by: " + m_PlayerManager.GetPlayerName(PlayerID), LogLevel.DEBUG);        
    }
}