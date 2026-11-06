//SDRC_RplPlayerComp.c

//------------------------------------------------------------------------------------------------
/*!
RPL component to provide GM map information to clients.

NOTE: This has been added to {}}Prefabs/Characters/DarcMissions/DefaultPlayerController.et
*/

//------------------------------------------------------------------------------------------------
modded class SDRC_RplPlayerComp
{
	static ref array<ref SDRC_GMMissionData> m_MissionData = {};	//Client table
	static bool m_bMissionDataReady = false;						//If MissionData is available for client
	
	//------------------------------------------------------------------------------------------------
	// Mission data stuff
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
		Sync_MissionData(playerId);
    }
	
	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Syncronize symbols to clients
	*/
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    protected void RpcDo_Sync_MissionData(SDRC_EMissionType type, int maxIndex, string name)
    {
		SDRC_Log.Add("[SDRC_RplGMComp:RpcDo_Sync_MissionData] Adding " + SCR_Enum.GetEnumName(SDRC_EMissionType, type), LogLevel.NORMAL);
		
		SDRC_GMMissionData missionData = new SDRC_GMMissionData();
		missionData.type = type;
		missionData.maxIndex = maxIndex;
		missionData.names.Insert(name);
		m_MissionData.Insert(missionData);
    }	

	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Syncronize symbols to clients
	*/
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    protected void RpcDo_Ready_MissionData()
    {
		SDRC_Log.Add("[SDRC_RplGMComp:RpcDo_Ready_MissionData] Done!", LogLevel.NORMAL);
		m_bMissionDataReady = true;
    }	
		
	//------------------------------------------------------------------------------------------------
	/*!	
	[Server] Sync mission data to all player
	*/
 	void Sync_MissionData(int playerID)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:Sync_MissionData] Starting..", LogLevel.NORMAL);	
		
		SDRC_RplGMComp rplGMComp = SDRC_RplGMComp.GetInstance();
		if (!rplGMComp)
		{
			SDRC_Log.Add("[SDRC_RplGMComp:Sync_MissionData] Could not find SDRC_RplGMComp.", LogLevel.ERROR);
			return;
		}
		
		//Clear mission data
		m_MissionData.Clear();
		
		if (rplGMComp.IsMissionDataReady_S())
		{		
			foreach (SDRC_GMMissionData missionData : rplGMComp.m_MissionData_S)
			{
				SDRC_Log.Add("[SDRC_RplGMComp:Sync_MissionData] Syncing: " + SCR_Enum.GetEnumName(SDRC_EMissionType, missionData.type), LogLevel.NORMAL);	
		        Rpc(RpcDo_Sync_MissionData, missionData.type, missionData.maxIndex, missionData.names[0]); 	// broadcast to clients	
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_RplGMComp:Sync_MissionData] Data not available yet.", LogLevel.NORMAL);	
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void MissionDataReady()
	{
		m_bMissionDataReady = true;
	}

	bool IsMissionDataReady()
	{
		return m_bMissionDataReady;
	}		
}