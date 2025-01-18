class SCR_DC_RplMapMarkerEntityClass : ScriptComponentClass { }
SCR_DC_RplMapMarkerEntityClass g_SCR_DC_RplMapMarkerEntityClassInst;
 
class SCR_DC_RplMapMarkerEntity : ScriptComponent
{
    private static const float COLOR_CHANGE_PERIOD_S = 5.0;
 
    private float m_TimeAccumulator_s;
 
    // We mark color index as replicated property using RplProp attribute, making
    // it part of replicated state. We also say we want OnColorIdxChanged function
    // to be invoked whenever replication updates value of color index.
    [RplProp(onRplName: "OnShowHint")]
    private int m_ColorIdx;
 
    [RplProp(onRplName: "OnShowHint")]
	string m_SomeString;

	private ref array<string> m_SomeArray = {"one", "two", "three", "four"};
	
    override void OnPostInit(IEntity owner)
    {
		Print("[SCR_DC_RplMapMarkerEntity] Here we are!", LogLevel.WARNING);
		
        auto shapeEnt = SCR_DC_MapMarkerEntity.Cast(owner);
        if (!shapeEnt)
        {
            Print("This example requires that the entity is of type `RplExampleDebugShape`.", LogLevel.WARNING);
            return;
        }
 
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
        HintTime(owner);
    }
 
	private void HintTime(IEntity owner)	
	{
		string hint = SCR_DC_MapMarkerEntity.Cast(owner).GetMarkerTxt();
		
		if (hint == "")
			return;
		
		m_SomeString = "Title is: " + hint;
		Replication.BumpMe();
		
        // Presentation of replicated state on authority.
		ShowHint();		
		SCR_DC_MapMarkerEntity.Cast(owner).ClearMarkerTxt();
	}

	// Presentation of replicated state on proxy.
	private void OnShowHint()
	{
		SCR_DC_Log.Add("[SCR_DC_RplMapMarkerEntity] CLIENT SIDE!", LogLevel.NORMAL);        
		ShowHint();		
		SCR_DC_MapMarkerEntity.Cast(GetOwner()).ClearMarkerTxt();		
	}	
		
	private void ShowHint()
	{
		SCR_HintManagerComponent hintComponent = SCR_HintManagerComponent.GetInstance();
		hintComponent.ShowCustomHint("Hello", m_SomeString, 2);
	}	
}