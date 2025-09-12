class SDRC_RplStoryCompClass : ScriptComponentClass { }
SDRC_RplStoryCompClass g_RplStoryCompClass;
 
//------------------------------------------------------------------------------------------------
class SDRC_RplStoryComp : ScriptComponent
{
	private static SDRC_RplStoryComp s_Instance;

	[RplProp(onRplName: "OnUpdateTime")]
	protected int m_iTimeLeft = 1000;

	[RplProp(onRplName: "OnUpdateBrief")]
	protected string m_sStoryTitle = "";
		
	[RplProp(onRplName: "OnUpdateChapter")]
	protected string m_sTitle = "";
		
	[RplProp(onRplName: "OnUpdateChapter")]
	protected string m_sText = "";

	[RplProp(onRplName: "OnUpdateChapter")]
	protected string m_sSuccess = "";
		
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
	
	string GetStoryTitle()
	{
		return m_sStoryTitle;
	}
	
	string GetTitle()
	{
		return m_sTitle;
	}
	
	string GetText()
	{
		return m_sText;
	}

	string GetSuccess()
	{
		return m_sSuccess;
	}
		
	//------------------------------------------------------------------------------------------------
	protected void OnUpdateChapter()
	{
		SDRC_Log.Add("[SDRC_RplStoryComp:OnUpdateChapter] Chapter updated.", LogLevel.NORMAL);
	}	
	
	//------------------------------------------------------------------------------------------------
	void UpdateChapter(string title, string text, string success)
	{
		RpcDo_UpdateChapter(title, text, success);
		Rpc(RpcDo_UpdateChapter, title, text, success);
	}	
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_UpdateChapter(string title, string text, string success)
	{
		m_sTitle = title;
		m_sText = text;
		m_sSuccess = success;

		Replication.BumpMe();
	}	
	
	//------------------------------------------------------------------------------------------------
	protected void OnUpdateBrief()
	{
		SDRC_Log.Add("[SDRC_RplStoryComp:OnUpdateBrief] Brief updated.", LogLevel.NORMAL);
	}	
	
	//------------------------------------------------------------------------------------------------
	void UpdateBrief(string title)
	{
		RpcDo_UpdateBrief(title);
		Rpc(RpcDo_UpdateBrief, title);
	}	
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_UpdateBrief(string title)
	{
		m_sStoryTitle = title;

		Replication.BumpMe();
	}	
	
}