//------------------------------------------------------------------------------------------------
/*!
This is the hint entity class.

This is the entity with the RPL component attached and uses SDRC_RplHintComp for the communication.
The entity is not streamed so everyone gets the hint regardless on where they are on the map.
*/

//------------------------------------------------------------------------------------------------
class SDRC_RplGMEntityClass: GenericEntityClass {}
 
class SDRC_RplGMEntity : GenericEntity
{
	protected static SDRC_RplGMEntity s_Instance;	
    PlayerManager m_PlayerManager;

	override void EOnActivate(IEntity owner)
	{
		SetEventMask(EntityEvent.FRAME | EntityEvent.POSTFRAME);
        SetFlags(EntityFlags.ACTIVE, true);
		s_Instance = this;
		SDRC_Log.Add("[SDRC_RplGMEntity] Running", LogLevel.DEBUG);
		
        m_PlayerManager = GetGame().GetPlayerManager();
        if(SCR_PlayerController.GetLocalControlledEntity() != owner)
		{
			SDRC_Log.Add("[SDRC_RplGMEntity] GetPlayerManager failed", LogLevel.DEBUG);
            return;
		}
        GetGame().GetCallqueue().CallLater(DelayedInit,1000,false,owner);
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to class
	*/
	static SDRC_RplGMEntity GetInstance()
	{
		return s_Instance;		
	}	
	
    //------------------------------------------------------------------------------------------------
    void DelayedInit(IEntity owner)
    {
		SDRC_Log.Add("[SDRC_RplGMEntity] DelayedInit", LogLevel.DEBUG);
		
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
		SDRC_Log.Add("[SDRC_RplGMEntity] GM opened", LogLevel.DEBUG);
		
//        int PlayerID = m_PlayerManager.GetPlayerIdFromControlledEntity(GetOwner());
//        int PlayerID = SCR_PossessingManagerComponent.GetPlayerIdFromControlledEntity(GetOwner());
//        Rpc(Ask_Authority_PrintStuff, PlayerID);
    } 
	
    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void Ask_Authority_PrintStuff(int PlayerID)
    {
        SDRC_Log.Add("[SDRC_RplGMEntity] GM Mode by: " + m_PlayerManager.GetPlayerName(PlayerID), LogLevel.DEBUG);        
    }	
};