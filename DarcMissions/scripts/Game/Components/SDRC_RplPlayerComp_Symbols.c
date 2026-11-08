//SDRC_RplPlayerComp_Symbols.c

//------------------------------------------------------------------------------------------------
/*!
RPL component to provide GM map information to clients.

NOTE: This has been added to {}}Prefabs/Characters/DarcMissions/DefaultPlayerController.et
*/
//------------------------------------------------------------------------------------------------
modded class SDRC_RplPlayerComp
{
	//------------------------------------------------------------------------------------------------
	// GM Map icon/symbol information stuff
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
 	void AskForInfo()
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();		
		Rpc(RpcAsk_GiveMeInfo_Symbols, playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Client requests for information
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_GiveMeInfo_Symbols(int playerId)
    {
		SDRC_Log.Add("[SDRC_RplPlayerComp:RpcAsk_GiveMeInfo_Symbols] Asked by: " + playerId, LogLevel.SPAM);	

		SDRC_RplGMComp gmComp = SDRC_RplGMComp.FindInstance();
		if (gmComp)
		{
			gmComp.Sync_Symbols(playerId);
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