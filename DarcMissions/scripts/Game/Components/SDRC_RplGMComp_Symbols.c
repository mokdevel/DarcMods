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
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Clear symbols
	*/
	void ClearSymbols()
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
 	override void SyncMapSymbols(int playerID)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:SyncMapSymbols] Starting..", LogLevel.SPAM);	
		//Clear symbols on server
		ClearSymbols();
		//Clear symbols on client
		Rpc(RpcDo_ClearSymbols);
		
		SDRC_GMHelper.AddSymbols();
		
		foreach (SDRC_GMMapSymbol symbol : m_Symbols)
		{
			SDRC_Log.Add("[SDRC_RplGMComp:SyncMapSymbols] Syncing: " + symbol.vPos, LogLevel.SPAM);	
	        Rpc(RpcDo_SyncMapSymbol, symbol.symbolType, symbol.vPos, symbol.iTimeLeft, symbol.fRadius, symbol.sId, symbol.iIntval, symbol.sStrval, symbol.iType); 	// broadcast to clients			
		}		
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Clear symbols 
	*/
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_ClearSymbols()
    {
		SDRC_Log.Add("[SDRC_RplGMComp:RpcDo_ClearSymbols] Clearing.. ", LogLevel.SPAM);
		ClearSymbols();
    }
		
	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Syncronize symbols to clients
	*/
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_SyncMapSymbol(SDRC_EDrawSymbol symbolType, vector pos, int timeLeft, float radius, string id, int intval, string strval, int type)
    {
		SDRC_Log.Add("[SDRC_RplGMComp:RpcDo_SyncMapSymbols] Adding " + SCR_Enum.GetEnumName(SDRC_EDrawSymbol, symbolType) + " at pos: " + pos, LogLevel.SPAM);
		
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
}