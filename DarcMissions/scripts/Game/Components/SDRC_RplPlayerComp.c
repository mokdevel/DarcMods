//SDRC_RplPlayerComp.c

//------------------------------------------------------------------------------------------------
/*!
RPL component to provide GM map information to clients.

NOTE: This has been added to {}}Prefabs/Characters/DarcMissions/DefaultPlayerController.et
*/
[ComponentEditorProps(category: "GameScripted/DarcMissions", description: "DarcMissions Player Helper Component")]
class SDRC_RplPlayerCompClass : ScriptComponentClass { }
SDRC_RplPlayerCompClass g_RplPlayerCompClass;

//------------------------------------------------------------------------------------------------
class SDRC_RplPlayerComp : ScriptComponent
{
	private static SDRC_RplPlayerComp s_Instance;	
		 
    override void OnPostInit(IEntity owner)
    {
		s_Instance = this;				
    }
 
	//------------------------------------------------------------------------------------------------
	//! Get instance owned by local player
	static SDRC_RplPlayerComp FindLocalInstance()
	{
		PlayerController playerController = GetGame().GetPlayerController();
		
		if (!playerController)
		{
			return null;
		}
		
		return SDRC_RplPlayerComp.Cast(playerController.FindComponent(SDRC_RplPlayerComp));
	}	
	
	//------------------------------------------------------------------------------------------------
	//!	Return instance to component
	static SDRC_RplPlayerComp GetInstance()
	{
		return s_Instance;
	}

	//------------------------------------------------------------------------------------------------
 	void AskForInfo()
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();		
		Rpc(RpcAsk_GiveMeInfo, playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Client requests for information
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_GiveMeInfo(int playerId)
    {
		SDRC_Log.Add("[SDRC_RplPlayerComp:RpcAsk_GiveMeInfo] Asked by: " + playerId, LogLevel.DEBUG);	

		SDRC_RplGMComp gmComp = SDRC_RplGMComp.FindInstance();
		if (gmComp)
		{
			gmComp.SyncMapSymbols(playerId);
		}
    }
	
	//------------------------------------------------------------------------------------------------	
	// Mission stuff
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
 	void AskForMissionDeletion(string missionId)
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();		
		Rpc(RpcAsk_DeleteMission, playerId, missionId);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Client requests for mission deletion
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_DeleteMission(int playerID, string missionId)
    {
		SDRC_Log.Add("[SDRC_RplPlayerComp:RpcAsk_DeleteMission] Mission " + missionId + " deletion asked by: " + playerID, LogLevel.DEBUG);	

		SDRC_RplGMComp gmComp = SDRC_RplGMComp.FindInstance();
		if (gmComp)
		{
			gmComp.DoDeleteMission(playerID, missionId);
		}
    }

	//------------------------------------------------------------------------------------------------	
	// NonValidArea stuff
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
 	void AskForNonValidAreaDeletion(string id)
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();		
		Rpc(RpcAsk_DeleteNonValidArea, playerId, id);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Client requests for mission deletion
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_DeleteNonValidArea(int playerID, string id)
    {
		SDRC_Log.Add("[SDRC_RplPlayerComp:RpcAsk_DeleteNonValidArea] NonValidArea " + id + " deletion asked by: " + playerID, LogLevel.DEBUG);	

		SDRC_RplGMComp gmComp = SDRC_RplGMComp.FindInstance();
		if (gmComp)
		{
			gmComp.DoDeleteNonValidArea(playerID, id);
		}
    }	
		
	//------------------------------------------------------------------------------------------------
 	void AskForNonValidAreaSizeChange(string circleId, float size)
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();		
		Rpc(RpcAsk_NonValidAreaSizeChange, playerId, circleId, size);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Client requests for NVA size change
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_NonValidAreaSizeChange(int playerID, string circleId, float size)
    {
		SDRC_Log.Add("[SDRC_RplPlayerComp:RpcAsk_NonValidAreaSizeChange] Asked by: " + playerID, LogLevel.DEBUG);	

		SDRC_RplGMComp gmComp = SDRC_RplGMComp.FindInstance();
		if (gmComp)
		{
			gmComp.DoNonValidAreaSizeChange(playerID, circleId, size);
		}
    }
	
	//------------------------------------------------------------------------------------------------
 	void AskForNonValidAreaSave()
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();		
		Rpc(RpcAsk_NonValidAreaSave, playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Client requests for NVA saving to file
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_NonValidAreaSave(int playerID)
    {
		SDRC_Log.Add("[SDRC_RplPlayerComp:RpcAsk_DeleteNonValidArea] Asked by: " + playerID, LogLevel.DEBUG);	

		SDRC_RplGMComp gmComp = SDRC_RplGMComp.FindInstance();
		if (gmComp)
		{
			gmComp.DoNonValidAreaSave(playerID);
		}
    }	
}