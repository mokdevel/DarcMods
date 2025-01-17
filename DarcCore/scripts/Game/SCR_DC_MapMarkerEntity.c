class SCR_DC_MapMarkerEntityClass : GenericEntityClass {}

class SCR_DC_MapMarkerEntity : GenericEntity
{
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
 
/*    bool SetColorByIdx(int colorIdx)
    {
        if (colorIdx < 0 || colorIdx >= COLOR_COUNT)
            return false;
 
        m_Color = COLORS[colorIdx];
        return true;
    }*/
}

