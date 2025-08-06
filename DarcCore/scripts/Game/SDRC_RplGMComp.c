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
	DC_EDrawSymbol type;
	vector pos;
	float radius;
	int intval;			//Integer value for color or icon or ..
	string id; 			
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
		SDRC_RplHintEntity hintEnt = SDRC_RplHintEntity.GetInstance();
		
		if (!hintEnt)
		{
			return null;
		}
		
		return SDRC_RplGMComp.Cast(hintEnt.FindComponent(SDRC_RplGMComp));
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
 	void AddSymbolCircle(vector pos, float radius, int color)
    {
		pos[1] = 0;			//Set to zero plane
		
		SDRC_GMMapSymbol symbol = new SDRC_GMMapSymbol();
		symbol.type = DC_EDrawSymbol.CIRCLE;
		symbol.pos = pos;
		symbol.radius = radius;
		symbol.intval = color;
		symbol.id = "";
		m_Symbols.Insert(symbol);
    }
	
	//------------------------------------------------------------------------------------------------
 	void AddSymbolMarker(vector pos, float radius, DC_EMissionIcon icon, string id)
    {
		pos[1] = 0;			//Set to zero plane
		
		SDRC_GMMapSymbol symbol = new SDRC_GMMapSymbol();
		symbol.type = DC_EDrawSymbol.MARKER;		
		symbol.pos = pos;
		symbol.radius = 0;
		symbol.intval = icon;
		symbol.id = id;
		m_Symbols.Insert(symbol);
    }
	
	//------------------------------------------------------------------------------------------------
 	void SyncMapSymbols(int playerID)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:SyncMapSymbols] Starting..", LogLevel.DEBUG);	
		ClearSymbols();
		SDRC_GMHelper.AddSymbols();
		
		foreach(SDRC_GMMapSymbol symbol : m_Symbols)
		{
//			SDRC_Log.Add("[SDRC_RplGMComp:SyncMapSymbols] Syncing: " + symbol.pos, LogLevel.NORMAL);	
	        Rpc(RpcDo_SyncMapSymbol, symbol.type, symbol.pos, symbol.radius, symbol.intval, symbol.id); 	// broadcast to clients			
		}		
	}

	//------------------------------------------------------------------------------------------------
//    [RplRpc(RplChannel.Reliable, RplRcver.Owner)]
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_SyncMapSymbol(DC_EDrawSymbol symbolType, vector pos, float radius, int color, string id)
    {
		//SDRC_Log.Add("[SDRC_RplGMComp:RpcDo_SyncMapSymbols] Adding " + SCR_Enum.GetEnumName(DC_EDrawSymbol, symbolType) + " at pos: " + pos, LogLevel.NORMAL);	//TBD: SPAM
		
		SDRC_GMMapSymbol symbol = new SDRC_GMMapSymbol();
		symbol.type = symbolType;
		symbol.pos = pos;
		symbol.radius = radius;
		symbol.intval = color;
		symbol.id = id;
		m_Symbols.Insert(symbol);
    }
}