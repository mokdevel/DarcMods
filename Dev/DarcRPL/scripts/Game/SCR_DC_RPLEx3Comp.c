class RplExample3ComponentColorAnimClass : ScriptComponentClass { }
RplExample3ComponentColorAnimClass g_RplExample3ComponentColorAnimClass;
 
class RplExample3ComponentColorAnim : ScriptComponent
{
    private static const float COLOR_CHANGE_PERIOD_S = 5.0;
 
    private float m_TimeAccumulator_s;
 
    // We mark color index as replicated property using RplProp attribute, making
    // it part of replicated state. We also say we want OnColorIdxChanged function
    // to be invoked whenever replication updates value of color index.
    [RplProp(onRplName: "OnColorIdxChanged")]
    private int m_ColorIdx;
 
    [RplProp(onRplName: "OnColorIdxChanged")]
	string m_SomeString;

//	private ref array<string> m_SomeArray = {"one", "two", "three", "four"};
	
    override void OnPostInit(IEntity owner)
    {
        auto shapeEnt = RplExampleDebugShape.Cast(owner);
        if (!shapeEnt)
        {
            Print("This example requires that the entity is of type `RplExampleDebugShape`.", LogLevel.WARNING);
            return;
        }
 
        shapeEnt.SetColorByIdx(m_ColorIdx);
 
        // We must belong to some RplComponent in order for replication to work.
        // We search for it and warn user when we can't find it.
        auto rplComponent = BaseRplComponent.Cast(shapeEnt.FindComponent(BaseRplComponent));
        if (!rplComponent)
        {
            Print("This example requires that the entity has an RplComponent.", LogLevel.WARNING);
            return;
        }
 
        // We only perform simulation on the authority instance, while all proxy
        // instances just show result of the simulation. Therefore, we only have to
        // subscribe to "frame" events on authority, leaving proxy instances as
        // passive components that do something only when necessary.
        if (rplComponent.Role() == RplRole.Authority)
        {
            SetEventMask(owner, EntityEvent.FRAME);
        }
    }
 
    override void EOnFrame(IEntity owner, float timeSlice)
    {
        int colorIdxDelta = CalculateColorIdxDelta(timeSlice);
        ApplyColorIdxDelta(owner, colorIdxDelta);
    }
 
    private int CalculateColorIdxDelta(float timeSlice)
    {
        m_TimeAccumulator_s += timeSlice;
        int colorIdxDelta = m_TimeAccumulator_s / COLOR_CHANGE_PERIOD_S;
        m_TimeAccumulator_s -= colorIdxDelta * COLOR_CHANGE_PERIOD_S;
        return colorIdxDelta;
    }
 
    private void ApplyColorIdxDelta(IEntity owner, int colorIdxDelta)
    {
        if (colorIdxDelta == 0)
            return;
 
        int newColorIdx = (m_ColorIdx + colorIdxDelta) % RplExampleDebugShape.COLOR_COUNT;
        if (newColorIdx == m_ColorIdx)
            return;
 
        // Update replicated state with results from the simulation.
        m_ColorIdx = newColorIdx;
 
//		m_SomeString = m_SomeArray.GetRandomElement();
		
        // After we have written new value of color index, we let replication know
        // that there are changes in our state that need to be replicated to proxies.
        // Without this call, even if we change our color index, new value would not
        // be replicated to proxies.
        Replication.BumpMe();
 
        // Presentation of replicated state on authority.
        RplExampleDebugShape.Cast(owner).SetColorByIdx(m_ColorIdx);
//		ShowHint();
    }
 
    // Presentation of replicated state on proxy.
    private void OnColorIdxChanged()
    {
        RplExampleDebugShape.Cast(GetOwner()).SetColorByIdx(m_ColorIdx);
//		ShowHint();
    }
	
/*	private void ShowHint()
	{
		SCR_HintManagerComponent hintComponent = SCR_HintManagerComponent.GetInstance();
		hintComponent.ShowCustomHint("Hello", m_SomeString, 2);
	}*/
}