class SCR_DC_RplMapMarkerEntityClass : ScriptComponentClass { }
SCR_DC_RplMapMarkerEntityClass g_SCR_DC_RplMapMarkerEntityClassInst;

//------------------------------------------------------------------------------------------------
class SCR_DC_RplMapMarkerEntity : ScriptComponent
{
    private static const float COLOR_CHANGE_PERIOD_S = 5.0;
 
    private float m_TimeAccumulator_s;
 
    // We mark hint as replicated property using RplProp attribute, making
    // it part of replicated state. We also say we want OnShowHint function
    // to be invoked whenever replication updates value of m_Hint.
    [RplProp(onRplName: "OnShowHint")]
	string m_Hint;
	
	string m_HintOld = "";
	
    override void OnPostInit(IEntity owner)
    {
        auto shapeEnt = SCR_DC_MapMarkerEntity.Cast(owner);
        if (!shapeEnt)
        {
            SCR_DC_Log.Add("[SCR_DC_RplMapMarkerEntity] SCR_DC_MapMarkerEntity not found", LogLevel.ERROR);
            return;
        }
 
        // We must belong to some RplComponent in order for replication to work.
        // We search for it and warn user when we can't find it.
        auto rplComponent = BaseRplComponent.Cast(shapeEnt.FindComponent(BaseRplComponent));
        if (!rplComponent)
        {
            SCR_DC_Log.Add("[SCR_DC_RplMapMarkerEntity] RplComponent not found.", LogLevel.ERROR);
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
		m_Hint = SCR_DC_MapMarkerEntity.Cast(owner).GetMarkerHint();
		
		if (m_Hint == m_HintOld)
			return;
		
		m_HintOld = m_Hint;
		
		Replication.BumpMe();
		
        // Presentation of replicated state on authority.
		ShowHint();		
		//SCR_DC_MapMarkerEntity.Cast(owner).ClearMarkerHint();
	}

	// Presentation of replicated state on proxy.
	private void OnShowHint()
	{
		SCR_DC_Log.Add("[SCR_DC_RplMapMarkerEntity] CLIENT SIDE!", LogLevel.NORMAL);        
		ShowHint();		
		//SCR_DC_MapMarkerEntity.Cast(GetOwner()).ClearMarkerHint();
	}	
		
	private void ShowHint()
	{
		array<string> hint = {"", "", ""};
		m_Hint.Split("|", hint, false);		
		SCR_HintManagerComponent hintComponent = SCR_HintManagerComponent.GetInstance();
		hintComponent.ShowCustomHint(hint[1], hint[0], 2);		
		//SCR_DC_MapMarkerEntity.Cast(GetOwner()).ClearMarkerHint();
	}	
}