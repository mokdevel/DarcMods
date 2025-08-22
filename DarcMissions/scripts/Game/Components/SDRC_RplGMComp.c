//SDRC_RPLGMComp.c

//------------------------------------------------------------------------------------------------
/*!
RPL component to provide GM map information to clients.
*/
class SDRC_RplGMCompClass : ScriptComponentClass { }
SDRC_RplGMCompClass g_RplGMCompClass;

//------------------------------------------------------------------------------------------------
enum DC_EDrawSymbol
{
	NONE,
	CIRCLE,
	MARKER
}
 
//------------------------------------------------------------------------------------------------
class SDRC_GMMapSymbol : Managed
{
	//Default information
	bool visible = true;
	DC_EDrawSymbol type;
	vector pos;
	float radius;
	int intval;			//Integer value for color or icon or ..
	string id;
	string strval;
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
 	void AddSymbolCircle(vector pos, float radius, int color, string strval = "")
    {
		pos[1] = 0;			//Set to zero plane
		
		SDRC_GMMapSymbol symbol = new SDRC_GMMapSymbol();
		symbol.type = DC_EDrawSymbol.CIRCLE;
		symbol.pos = pos;
		symbol.radius = radius;
		symbol.intval = color;
		symbol.id = "";
		symbol.strval = "";
		m_Symbols.Insert(symbol);
    }
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Add a marker on symbol list
	*/
 	void AddSymbolMarker(vector pos, float radius, DC_EMissionIcon icon, string id, string strval)
    {
		pos[1] = 0;			//Set to zero plane
		
		SDRC_GMMapSymbol symbol = new SDRC_GMMapSymbol();
		symbol.type = DC_EDrawSymbol.MARKER;		
		symbol.pos = pos;
		symbol.radius = 0;
		symbol.intval = icon;
		symbol.id = id;
		symbol.strval = strval;
		m_Symbols.Insert(symbol);
    }

	//------------------------------------------------------------------------------------------------
	/*!	
	Request for a mission deletion
	*/
 	void DoDeleteMission(int playerID, string missionId)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:DeleteMission] Deletion of " + missionId + " requested by " + playerID, LogLevel.DEBUG);	
		SDRC_GMHelper.DeleteMission(missionId);
//		SyncMapSymbols(playerID);
	}
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Sync all map symbols to all players
	*/
 	void SyncMapSymbols(int playerID)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:SyncMapSymbols] Starting..", LogLevel.SPAM);	
		//Clear symbols on server
		ClearSymbols();
		//Clear symbols on client
		Rpc(RpcDo_ClearSymbols);
		
		SDRC_GMHelper.AddSymbols();
		
		foreach(SDRC_GMMapSymbol symbol : m_Symbols)
		{
			SDRC_Log.Add("[SDRC_RplGMComp:SyncMapSymbols] Syncing: " + symbol.pos, LogLevel.SPAM);	
	        Rpc(RpcDo_SyncMapSymbol, symbol.type, symbol.pos, symbol.radius, symbol.intval, symbol.id, symbol.strval); 	// broadcast to clients			
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
    protected void RpcDo_SyncMapSymbol(DC_EDrawSymbol symbolType, vector pos, float radius, int color, string id, string strval)
    {
		SDRC_Log.Add("[SDRC_RplGMComp:RpcDo_SyncMapSymbols] Adding " + SCR_Enum.GetEnumName(DC_EDrawSymbol, symbolType) + " at pos: " + pos, LogLevel.SPAM);
		
		SDRC_GMMapSymbol symbol = new SDRC_GMMapSymbol();
		symbol.type = symbolType;
		symbol.pos = pos;
		symbol.radius = radius;
		symbol.intval = color;
		symbol.id = id;
		symbol.strval = strval;
		m_Symbols.Insert(symbol);
    }
}