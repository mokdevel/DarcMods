class RplExampleDebugShapeClass: GenericEntityClass {}
RplExampleDebugShapeClass g_RplExampleDebugShapeClassInst;
 
class RplExampleDebugShape : GenericEntity
{
    static const int COLOR_COUNT = 4;
    static const int COLORS[] = {
        Color.BLACK,
        Color.RED,
        Color.GREEN,
        Color.BLUE,
    };
 
    private int m_Color;
 
    void RplExampleDebugShape(IEntitySource src, IEntity parent)
    {
        this.SetEventMask(EntityEvent.FRAME);
    }
 
    override void EOnFrame(IEntity owner, float timeSlice)
    {
        vector worldTransform[4];
        this.GetWorldTransform(worldTransform);
        Shape.CreateSphere(m_Color, ShapeFlags.ONCE, worldTransform[3], 0.5);
    }
 
    bool SetColorByIdx(int colorIdx)
    {
        if (colorIdx < 0 || colorIdx >= COLOR_COUNT)
            return false;
 
        m_Color = COLORS[colorIdx];
        return true;
    }
}