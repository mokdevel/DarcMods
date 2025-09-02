class SDRC_RplStoryCompClass : ScriptComponentClass { }
SDRC_RplStoryCompClass g_RplStoryCompClass;
 
//------------------------------------------------------------------------------------------------
class SDRC_RplStoryComp : ScriptComponent
{
	private static SDRC_RplStoryComp s_Instance;

	[RplProp(onRplName: "OnTimeUpdate")]
	protected int m_iTimeLeft = 1000;
	
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
	protected void OnTimeUpdate()
	{
		SDRC_Log.Add("[SDRC_RplStoryComp:OnTimeUpdate] Time left: " + m_iTimeLeft, LogLevel.NORMAL);
	}	
	
	//------------------------------------------------------------------------------------------------
	void UpdateTime(int timeLeft)
	{
		m_iTimeLeft--;
		timeLeft = m_iTimeLeft;
		Rpc(RpcAsk_Authority_Method, timeLeft);
	}	
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_Authority_Method(int timeLeft)
	{
		m_iTimeLeft = timeLeft;

		Replication.BumpMe();
	}
}