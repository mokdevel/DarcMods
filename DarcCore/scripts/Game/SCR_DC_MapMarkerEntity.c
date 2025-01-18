class SCR_DC_MapMarkerEntityClass : GenericEntityClass {}

class SCR_DC_MapMarkerEntity : GenericEntity
{
	private string m_MarkerTxt = "";	
	
    void SCR_DC_MapMarkerEntity(IEntitySource src, IEntity parent)
    {
        this.SetEventMask(EntityEvent.FRAME);
    }
 
    override void EOnFrame(IEntity owner, float timeSlice)
    {
//        vector worldTransform[4];
//        this.GetWorldTransform(worldTransform);
//        Shape.CreateSphere(m_Color, ShapeFlags.ONCE, worldTransform[3], 0.5);
    }
 
	void AddMarker(string idPrefix)	
	{
		m_MarkerTxt = idPrefix;
		SCR_DC_Log.Add("[SCR_DC_MapMarkerEntity] Added marker.");
	}

	string GetMarkerTxt()
	{
		return m_MarkerTxt;
	}
		
	void ClearMarkerTxt()
	{
		m_MarkerTxt = "";
	}
	
/*    bool SetColorByIdx(int colorIdx)
    {
        if (colorIdx < 0 || colorIdx >= COLOR_COUNT)
            return false;
 
        m_Color = COLORS[colorIdx];
        return true;
    }*/
}

