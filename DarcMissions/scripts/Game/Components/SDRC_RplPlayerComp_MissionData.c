//SDRC_RplPlayerComp.c

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
 	void AskForInfo_MissionData()
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();		
		Rpc(RpcAsk_GiveMeInfo_MissionData, playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Client requests for information
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcAsk_GiveMeInfo_MissionData(int playerId)
    {
		SDRC_Log.Add("[SDRC_RplPlayerComp:RpcAsk_GiveMeInfo_MissionData] Asked by: " + playerId, LogLevel.NORMAL);	

		SDRC_RplGMComp gmComp = SDRC_RplGMComp.FindInstance();
		if (gmComp)
		{
			gmComp.Sync_MissionData(playerId);
		}
    }
}