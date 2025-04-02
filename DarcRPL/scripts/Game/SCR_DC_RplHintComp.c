class RplHintCompClass : ScriptComponentClass { }
RplHintCompClass g_RplHintCompClass;
 
class RplHintComp : ScriptComponent
{
//	private IEntity m_HintEntity;	
	 
    [RplProp(onRplName: "OnShowHint")]
	string m_SomeString;
	BaseRplComponent m_rplComponent = null;
	
	private ref array<string> m_SomeArray = {"one", "two", "three", "four"};
	
    override void OnPostInit(IEntity owner)
    {
        // We must belong to some RplComponent in order for replication to work.
        // We search for it and warn user when we can't find it.
		
//		m_HintEntity = IEntity.Cast(GetGame().SpawnEntity(SCR_DC_HintEntity, GetGame().GetWorld(), null));		

        auto hintEnt = RplDCHint.Cast(owner);
        if (!hintEnt)
        {
            Print("SCR_DC This example requires that the entity is of type `RplDCHint`.", LogLevel.WARNING);
            return;
        }
				
        //auto 
		m_rplComponent = BaseRplComponent.Cast(hintEnt.FindComponent(BaseRplComponent));
        if (!m_rplComponent)
        {
            Print("SCR_DC This example requires that the entity has an RplComponent.", LogLevel.WARNING);
            return;
        }
 
        // We only perform simulation on the authority instance, while all proxy
        // instances just show result of the simulation. Therefore, we only have to
        // subscribe to "frame" events on authority, leaving proxy instances as
        // passive components that do something only when necessary.
        if (m_rplComponent.Role() == RplRole.Authority)
        {
            SetEventMask(owner, EntityEvent.FRAME);
        }

		GetGame().GetCallqueue().CallLater(HintTime, 3000, false);				
    }
 
    override void EOnFrame(IEntity owner, float timeSlice)
    {
//		GetGame().GetCallqueue().CallLater(HintTime, 3000, true);
    }	
	
    private void HintTime()
    {
		if (m_rplComponent.Role() == RplRole.Authority)
//		if (true)
		{
			Print("SCR_DC Hinttime.", LogLevel.NORMAL);
			
			m_SomeString = m_SomeArray.GetRandomElement() + " - " + System.GetTickCount();		
			
	        Replication.BumpMe();
			ShowHint();
			
			GetGame().GetCallqueue().CallLater(HintTime, 5000, false);						
		}
		else
		{
			Print("SCR_DC Hinttime for client. Only run once.", LogLevel.NORMAL);
		}
    }
 
    // Presentation of replicated state on proxy.
    private void OnShowHint()
    {
		ShowHint();
    }
	
	private void ShowHint()
	{
		SCR_HintManagerComponent hintComponent = SCR_HintManagerComponent.GetInstance();
		hintComponent.ShowCustomHint("Hello", m_SomeString, 2);
	}	
}