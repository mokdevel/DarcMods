class SDRC_RplStoryCompClass : ScriptComponentClass { }
SDRC_RplStoryCompClass g_RplStoryCompClass;
 
//------------------------------------------------------------------------------------------------
class SDRC_RplStoryComp : ScriptComponent
{
	private static SDRC_RplStoryComp s_Instance;

	[RplProp(onRplName: "OnUpdateTime")]
	protected int m_iTimeLeft = 1000;
	
	[RplProp(onRplName: "OnUpdateChapter")]
	protected string m_sTitle = "";
		
	[RplProp(onRplName: "OnUpdateChapter")]
	protected string m_sText = "";
	
	//------------------------------------------------------------------------------------------------
    override void OnPostInit(IEntity owner)
    {
        auto ent = SDRC_RplStoryEntity.Cast(owner);
        if (!ent)
        {
            SDRC_Log.Add("[SDRC_RplStoryComp] Entity not found.", LogLevel.ERROR);
            return;
        }
				
		BaseRplComponent rplComponent = BaseRplComponent.Cast(ent.FindComponent(BaseRplComponent));
        if (!rplComponent)
        {
            SDRC_Log.Add("[SDRC_RplStoryComp] RplComponent not found.", LogLevel.ERROR);
            return;
        }
 
		s_Instance = this;				
    }
 
	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to component
	*/
	static SDRC_RplStoryComp GetInstance()
	{
		return s_Instance;
	}

	//------------------------------------------------------------------------------------------------
	// Time related RPL
	//------------------------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------------------------
	protected void OnUpdateTime()
	{
		SDRC_Log.Add("[SDRC_RplStoryComp:OnTimeUpdate] Time left: " + m_iTimeLeft, LogLevel.NORMAL);
	}	
	
	int GetTimeLeft()
	{
		return m_iTimeLeft;
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateTime(int timeLeft)
	{
		m_iTimeLeft = timeLeft;
		Rpc(RpcDo_UpdateTime, timeLeft);
	}	
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_UpdateTime(int timeLeft)
	{
		m_iTimeLeft = timeLeft;

		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	// Chapter related RPL
	//------------------------------------------------------------------------------------------------
	
	string GetTitle()	
	{
		return m_sTitle;
	}
	
	string GetText()	
	{
		return m_sText;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnUpdateChapter()
	{
		SDRC_Log.Add("[SDRC_RplStoryComp:OnUpdateChapter] Chapter updated.", LogLevel.NORMAL);
	}	
	
	//------------------------------------------------------------------------------------------------
	void UpdateChapter(string title, string text)
	{
		RpcDo_UpdateChapter(title, text);
		Rpc(RpcDo_UpdateChapter, title, text);
	}	
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_UpdateChapter(string title, string text)
	{
		m_sTitle = title;
		m_sText = text;

		Replication.BumpMe();
	}	
}