class SCR_DC_MapMarkerEntityClass : GenericEntityClass {}

//TBD: I should investigate SCR_MapMarkerManagerComponent

class SCR_DC_MapMarkerEntity : GenericEntity
{
	private string m_MarkerTxt = "";	
	private string m_Hint;				//TBD: Maybe this should be a FIFO array
	
    void SCR_DC_MapMarkerEntity(IEntitySource src, IEntity parent)
    {
        this.SetEventMask(EntityEvent.FRAME);
    }

	void AddMarkerHint(string title, string description, string id)	
	{
		m_Hint = title + "|" + description + "|" + id;
	}

	string GetMarkerHint()
	{
		return m_Hint;
	}
		
	void ClearMarkerHint()
	{
		m_Hint = "";
	}	
}

