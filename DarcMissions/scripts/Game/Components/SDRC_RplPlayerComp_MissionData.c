//SDRC_RplPlayerComp.c

//------------------------------------------------------------------------------------------------
/*!
RPL component to provide GM map information to clients.

NOTE: This has been added to {}}Prefabs/Characters/DarcMissions/DefaultPlayerController.et
*/

//------------------------------------------------------------------------------------------------
modded class SDRC_RplPlayerComp
{
	//This data is asked from server and filled in via RPL
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
	// Edit mode stuff
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
 	void SetEditing_MissionData(bool editing)
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();		
		//Rpc(RpcAsk_SetEditing_MissionData, playerId, editing);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Syncronize mission data to clients
	*/
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_Clear_MissionData()
	{
		m_MissionData.Clear();
	}
	
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_Sync_MissionData(SDRC_EMissionType type, int maxIndex)
    {
		SDRC_Log.Add("[SDRC_RplGMComp:RpcDo_Sync_MissionData] Adding " + SCR_Enum.GetEnumName(SDRC_EMissionType, type), LogLevel.NORMAL);

	    SDRC_GMMissionData missionData = new SDRC_GMMissionData();
        missionData.type = type;
        missionData.maxIndex = maxIndex;
        //NOTE: missionData.names will be filled in next stage
        m_MissionData.Insert(missionData);
    }	
	
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_Sync_MissionDataName(SDRC_EMissionType type, string name)
    {
		SDRC_Log.Add("[SDRC_RplGMComp:RpcDo_Sync_MissionDataName] Adding: " + name, LogLevel.NORMAL);
		m_MissionData[type].names.Insert(name);
    }	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Syncronize symbols to clients
	*/
    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    void RpcDo_Ready_MissionData()
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
		Rpc(RpcDo_Clear_MissionData);
		
		if (rplGMComp.IsMissionDataReady_S())
		{		
			foreach (SDRC_GMMissionData missionData : rplGMComp.m_MissionData_S)
			{
				SDRC_Log.Add("[SDRC_RplGMComp:Sync_MissionData] Syncing: " + SCR_Enum.GetEnumName(SDRC_EMissionType, missionData.type), LogLevel.NORMAL);	
		        Rpc(RpcDo_Sync_MissionData, missionData.type, missionData.maxIndex); 	// send to client	
				foreach(string name : missionData.names)
				{
		        	Rpc(RpcDo_Sync_MissionDataName, missionData.type, name); 			// send to client
				}
			}
			Rpc(RpcDo_Ready_MissionData); 	// send to client
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