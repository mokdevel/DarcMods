//SDRC_RPLGMComp_Symbols.c

//------------------------------------------------------------------------------------------------
/*!
RPL component to provide GM map information to clients.

NOTE: This has been added to {}}Prefabs/Characters/DarcMissions/DefaultPlayerController.et
*/

//------------------------------------------------------------------------------------------------
enum SDRC_EDrawSymbol
{
	NONE,
	NON_VALID_AREA,
	MARKER
}
 
//------------------------------------------------------------------------------------------------
// Generic symbol class
// NOTE: Depending on SDRC_EDrawSymbol type, parameters are used for various topics
class SDRC_GMMapSymbol : Managed
{	
	//Default information
	bool visible = true;			//If true, will be shown on the GM map
	SDRC_EDrawSymbol symbolType;
	vector vPos;
	int iTimeLeft;					//Seconds
	float fRadius;					
	string sId;						//
	int iIntval;					//Integer value for color or icon or ..
	string sStrval;
	int iType;						//Generic type int - used for SDRC_EMissionType
}

//------------------------------------------------------------------------------------------------
modded class SDRC_RplGMComp
{
	static ref array<ref SDRC_GMMapSymbol> m_Symbols = {};
	
	[RplProp(onRplName: "OnUpdateTimeLeft")]
	bool m_ShowMissionTimeLeft = false;
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Clear symbols
	*/
	void Clear_Symbols()
	{
		m_Symbols.Clear();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Add a circle on symbol list
	*/
 	void AddSymbolCircle(bool visible, vector pos, string name, float radius, int id, int color)
    {
		pos[1] = 0;			//Set to zero plane
		
		SDRC_GMMapSymbol symbol = new SDRC_GMMapSymbol();
		symbol.visible = visible;
		symbol.symbolType = SDRC_EDrawSymbol.NON_VALID_AREA;
		symbol.vPos = pos;
		symbol.iTimeLeft = -1;
		symbol.fRadius = radius;						//Radius of the circle
		symbol.sId = id.ToString();						//Index
		symbol.iIntval = color;							//Color for the circle
		symbol.sStrval = name;							//Name of the area
		symbol.iType = -1;								//Unused
		m_Symbols.Insert(symbol);
    }
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Add a marker on symbol list
	*/
 	void AddSymbolMarker(bool visible, vector pos, SDRC_EMissionType missionType, SDRC_EMissionIcon icon, string markerTypeString, int timeLeft, string id, string strval)
    {
		pos[1] = 0;			//Set to zero plane
		
		SDRC_GMMapSymbol symbol = new SDRC_GMMapSymbol();
		symbol.visible = visible;
		symbol.symbolType = SDRC_EDrawSymbol.MARKER;
		symbol.vPos = pos;
		symbol.iTimeLeft = timeLeft;
		symbol.fRadius = 0;								//Not used
		symbol.sId = id;								//Id for the marker
		symbol.iIntval = icon;							//SDRC_EMissionIcon
		symbol.sStrval = strval;						//Title of marker
		symbol.iType = missionType;						//MissionType
		m_Symbols.Insert(symbol);
    }
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Get marker symbol information
	*/
	SDRC_GMMapSymbol GetSymbolMarker(int idx)
	{
		if (m_Symbols.IsEmpty())
		{
			return null;
		}
				
		if (idx < 0 || idx > m_Symbols.Count())
		{
			return null;
		}
		
		return m_Symbols[idx];
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Sync all map symbols to all players
	*/
 	override void Sync_Symbols(int playerID)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:SyncMapSymbols] Starting..", LogLevel.SPAM);	
		//Clear symbols on server
		Clear_Symbols();
		//Clear symbols on client
		Rpc(RpcDo_Clear_Symbols);
		
		SDRC_GMHelper.AddSymbols();
		
		foreach (SDRC_GMMapSymbol symbol : m_Symbols)
		{
			SDRC_Log.Add("[SDRC_RplGMComp:SyncMapSymbols] Syncing: " + symbol.vPos, LogLevel.SPAM);	
	        Rpc(RpcDo_Sync_Symbol, symbol.symbolType, symbol.vPos, symbol.iTimeLeft, symbol.fRadius, symbol.sId, symbol.iIntval, symbol.sStrval, symbol.iType); 	// broadcast to clients			
		}		
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Clear symbols 
	*/
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_Clear_Symbols()
    {
		SDRC_Log.Add("[SDRC_RplGMComp:RpcDo_Clear_Symbols] Clearing.. ", LogLevel.SPAM);
		Clear_Symbols();
    }
		
	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Syncronize symbols to clients
	*/
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_Sync_Symbol(SDRC_EDrawSymbol symbolType, vector pos, int timeLeft, float radius, string id, int intval, string strval, int type)
    {
		SDRC_Log.Add("[SDRC_RplGMComp:RpcDo_Sync_Symbol] Adding " + SCR_Enum.GetEnumName(SDRC_EDrawSymbol, symbolType) + " at pos: " + pos, LogLevel.SPAM);
		
		SDRC_GMMapSymbol symbol = new SDRC_GMMapSymbol();
		symbol.symbolType = symbolType;
		symbol.vPos = pos;
		symbol.iTimeLeft = timeLeft;
		symbol.fRadius = radius;
		symbol.sId = id;
		symbol.iIntval = intval;
		symbol.sStrval = strval;
		symbol.iType = type;
		m_Symbols.Insert(symbol);
    }
	
	//------------------------------------------------------------------------------------------------	
	// Time left stuff
	//------------------------------------------------------------------------------------------------	
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Update time left attribute
	*/
	protected void OnUpdateTimeLeft()
	{
		SDRC_Log.Add("[SDRC_RplStoryComp:OnUpdateTimeLeft] TimeLeft updated.", LogLevel.NORMAL);
	}	
	
	//------------------------------------------------------------------------------------------------
	void UpdateTimeLeft(bool timeLeft)
	{
		RpcDo_UpdateTimeLeft(timeLeft);
		Rpc(RpcDo_UpdateTimeLeft, timeLeft);
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Time left attribute update
	*/
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_UpdateTimeLeft(bool timeLeft)
	{
		m_ShowMissionTimeLeft = timeLeft;

		Replication.BumpMe();
	}		
	
	//------------------------------------------------------------------------------------------------	
	// Mission stuff
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Request for a mission deletion.
	*/
 	void DoDeleteMission(int playerID, string missionId)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:DoDeleteMission] Deletion of " + missionId + " requested by " + playerID, LogLevel.SPAM);	
		DeleteMission(missionId);
//		SyncMapSymbols(playerID);
	}
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Request for a mission ending. Used by DarcStories
	*/
 	void DoEndMission(int playerID, string missionId)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:DoEndMission] Ending of " + missionId + " requested by " + playerID, LogLevel.SPAM);	
		EndMission(missionId, SDRC_EMissionSuccess.LOSE);
//		SyncMapSymbols(playerID);
	}
	
	//------------------------------------------------------------------------------------------------	
	// Mission stuff
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Delete a mission with specific mission ID (DCMxxxx).
	*/	
	void DeleteMission(string id, SDRC_EMissionSuccess success = SDRC_EMissionSuccess.DELETED)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!baseGameMode)
		{
			SDRC_Log.Add("[SDRC_RplGMComp:DeleteMission] SCR_BaseGameMode not found.", LogLevel.ERROR);
		}
		
		int idx = baseGameMode.missionFrame.FindMissionWithId(id);
		if (idx != -1)
		{
			baseGameMode.missionFrame.m_MissionList[idx].SetState(SDRC_EMissionState.END);
			baseGameMode.missionFrame.m_MissionList[idx].SetSuccess(success);
			SDRC_Log.Add("[SDRC_RplGMComp:DeleteMission] Ending mission: " + id + " - " + baseGameMode.missionFrame.m_MissionList[idx].GetTitle(), LogLevel.DEBUG);				
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Delete a mission with specific mission ID (DCMxxxx).
	*/	
	void EndMission(string id, SDRC_EMissionSuccess success)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!baseGameMode)
		{
			SDRC_Log.Add("[SDRC_RplGMComp:EndMission] SCR_BaseGameMode not found.", LogLevel.ERROR);
		}
		
		int idx = baseGameMode.missionFrame.FindMissionWithId(id);
		if (idx != -1)
		{
			if (success == SDRC_EMissionSuccess.LOSE)
			{
				baseGameMode.missionFrame.m_MissionList[idx].DoLose();
			}
			if (success == SDRC_EMissionSuccess.WIN)
			{
				baseGameMode.missionFrame.m_MissionList[idx].DoWin();
			}
			SDRC_Log.Add("[SDRC_RplGMComp:DeleteMission] Ending mission: " + id + " - (" + SCR_Enum.GetEnumName(SDRC_EMissionSuccess, success) + ")" + baseGameMode.missionFrame.m_MissionList[idx].GetTitle(), LogLevel.DEBUG);				
		}
	}		
}