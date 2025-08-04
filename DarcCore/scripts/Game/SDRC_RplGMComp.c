class SDRC_RplGMCompClass : ScriptComponentClass { }
SDRC_RplGMCompClass g_RplGMCompClass;

//------------------------------------------------------------------------------------------------
enum DC_EDrawSymbol
{
	NONE,
	CIRCLE
}
 
//------------------------------------------------------------------------------------------------
class SDRC_GMMapSymbol : Managed
{
	//Default information
	DC_EDrawSymbol type;
	vector pos;
	int color;
	float radius;
}

//------------------------------------------------------------------------------------------------
class SDRC_RplGMComp : ScriptComponent
{
	private static SDRC_RplGMComp s_Instance;	
	/*private*/ ref array<ref SDRC_GMMapSymbol> m_Symbols = {};
	 
    override void OnPostInit(IEntity owner)
    {
        auto hintEnt = SDRC_RplHintEntity.Cast(owner);
        if (!hintEnt)
        {
            SDRC_Log.Add("[SDRC_RplGMComp] Entity not found.", LogLevel.ERROR);
            return;
        }
				
		BaseRplComponent rplComponent = BaseRplComponent.Cast(hintEnt.FindComponent(BaseRplComponent));
        if (!rplComponent)
        {
            SDRC_Log.Add("[SDRC_RplGMComp] RplComponent not found.", LogLevel.ERROR);
            return;
        }
 
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
		BaseGameMode gameMode = GetGame().GetGameMode();
		
		if (!gameMode)
		{
			return null;
		}
		
		return SDRC_RplGMComp.Cast(gameMode.FindComponent(SDRC_RplGMComp));
	}	
	
	//------------------------------------------------------------------------------------------------
 	void SyncMapSymbols()
	{
		//TBD: We should sync the circles to newly joined players.
		foreach(SDRC_GMMapSymbol symbol : m_Symbols)
		{
//			AddMapCicrle(symbol.pos, symbol.radius, symbol.color);
	        Rpc(RpcDo_SyncMapCircle, DC_EDrawSymbol.CIRCLE, symbol.pos, symbol.radius, symbol.color); 	// broadcast to clients			
		}		
	}

	//------------------------------------------------------------------------------------------------
 	void AddMapCicrle(vector pos, float radius, int color)
    {
        Rpc(RpcDo_SyncMapCircle, DC_EDrawSymbol.CIRCLE, pos, radius, color); 	// broadcast to clients
//        RpcDo_SyncMapCircle(DC_EDrawSymbol.CIRCLE, pos, radius, color); 		// handle on authority
    }
	
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcDo_SyncMapCircle(DC_EDrawSymbol symbolType, vector pos, float radius, int color)
    {
		SDRC_Log.Add("[SDRC_RplGMComp:RpcDo_SyncMapCircle] Pos: " + pos, LogLevel.NORMAL);	
		
		SDRC_GMMapSymbol symbol = new SDRC_GMMapSymbol();
		symbol.type = symbolType;
		symbol.pos = pos;
		symbol.radius = radius;
		symbol.color = color;
		m_Symbols.Insert(symbol);
    }	
	

}