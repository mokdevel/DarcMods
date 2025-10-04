//SDRC_RPLGMComp.c

//------------------------------------------------------------------------------------------------
/*!
RPL component to provide GM map information to clients.
*/
class SDRC_RplGMCompClass : ScriptComponentClass { }
SDRC_RplGMCompClass g_RplGMCompClass;

//------------------------------------------------------------------------------------------------
enum SDRC_EDrawSymbol
{
	NONE,
	CIRCLE,
	MARKER
}
 
//------------------------------------------------------------------------------------------------
class SDRC_GMMapSymbol : Managed
{
	//Default information
	bool visible = true;			//If true, will be shown on the GM map
	SDRC_EDrawSymbol symbolType;
	vector pos;
	int timeLeft;
	float radius;
	int intval;						//Integer value for color or icon or ..
	string id;
	string strval;
	int type;						//Generic type int - used for SDRC_EMissionType
}

//------------------------------------------------------------------------------------------------
class SDRC_RplGMComp : ScriptComponent
{
	private static SDRC_RplGMComp s_Instance;	
	static ref array<ref SDRC_GMMapSymbol> m_Symbols = {};
	 
    override void OnPostInit(IEntity owner)
    {
		s_Instance = this;				
    }
 
	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to component
	*/
	static SDRC_RplGMComp GetInstance()
	{
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get instance
	static SDRC_RplGMComp FindInstance()
	{
		SDRC_RplGMEntity gmEnt = SDRC_RplGMEntity.GetInstance();
		
		if (!gmEnt)
		{
			return null;
		}
		
		return SDRC_RplGMComp.Cast(gmEnt.FindComponent(SDRC_RplGMComp));
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to component
	*/
	static void ClearSymbols()
	{
		m_Symbols.Clear();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Add a circle on symbol list
	*/
 	void AddSymbolCircle(bool visible, vector pos, float radius, int color, string strval = "")
    {
		pos[1] = 0;			//Set to zero plane
		
		SDRC_GMMapSymbol symbol = new SDRC_GMMapSymbol();
		symbol.visible = visible;
		symbol.symbolType = SDRC_EDrawSymbol.CIRCLE;
		symbol.pos = pos;
		symbol.timeLeft = -1;
		symbol.radius = radius;
		symbol.intval = color;
		symbol.id = "";
		symbol.strval = "";
		symbol.type = -1;
		m_Symbols.Insert(symbol);
    }
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Add a marker on symbol list
	*/
 	void AddSymbolMarker(bool visible, vector pos, SDRC_EMissionType missionType, SDRC_EMissionIcon icon, int timeLeft, string id, string strval)
    {
		pos[1] = 0;			//Set to zero plane
		
		SDRC_GMMapSymbol symbol = new SDRC_GMMapSymbol();
		symbol.visible = visible;
		symbol.symbolType = SDRC_EDrawSymbol.MARKER;
		symbol.pos = pos;
		symbol.timeLeft = timeLeft;
		symbol.radius = 0;
		symbol.intval = icon;
		symbol.id = id;
		symbol.strval = strval;
		symbol.type = missionType;
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
	Request for a mission deletion
	*/
 	void DoDeleteMission(int playerID, string missionId)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:DeleteMission] Deletion of " + missionId + " requested by " + playerID, LogLevel.SPAM);	
		SDRC_GMHelper.DeleteMission(missionId);
//		SyncMapSymbols(playerID);
	}
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Request for a mission deletion
	*/
 	void DoEndMission(int playerID, string missionId)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:DeleteMission] Ending of " + missionId + " requested by " + playerID, LogLevel.SPAM);	
		SDRC_GMHelper.EndMission(missionId, SDRC_EMissionSuccess.LOSE);
//		SyncMapSymbols(playerID);
	}
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Sync all map symbols to all players
	*/
 	void SyncMapSymbols(int playerID)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:SyncMapSymbols] Starting..", LogLevel.DEBUG);	
		//Clear symbols on server
		ClearSymbols();
		//Clear symbols on client
		Rpc(RpcDo_ClearSymbols);
		
		SDRC_GMHelper.AddSymbols();
		
		foreach(SDRC_GMMapSymbol symbol : m_Symbols)
		{
			SDRC_Log.Add("[SDRC_RplGMComp:SyncMapSymbols] Syncing: " + symbol.pos, LogLevel.SPAM);	
	        Rpc(RpcDo_SyncMapSymbol, symbol.symbolType, symbol.pos, symbol.timeLeft, symbol.radius, symbol.intval, symbol.id, symbol.strval, symbol.type); 	// broadcast to clients			
		}		
	}

	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_ClearSymbols()
    {
		SDRC_Log.Add("[SDRC_RplGMComp:RpcDo_ClearSymbols] Clearing.. ", LogLevel.SPAM);
		ClearSymbols();
    }
		
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_SyncMapSymbol(SDRC_EDrawSymbol symbolType, vector pos, int timeLeft, float radius, int color, string id, string strval, int type)
    {
		SDRC_Log.Add("[SDRC_RplGMComp:RpcDo_SyncMapSymbols] Adding " + SCR_Enum.GetEnumName(SDRC_EDrawSymbol, symbolType) + " at pos: " + pos, LogLevel.SPAM);
		
		SDRC_GMMapSymbol symbol = new SDRC_GMMapSymbol();
		symbol.symbolType = symbolType;
		symbol.pos = pos;
		symbol.timeLeft = timeLeft;
		symbol.radius = radius;
		symbol.intval = color;
		symbol.id = id;
		symbol.strval = strval;
		symbol.type = type;
		m_Symbols.Insert(symbol);
    }
}