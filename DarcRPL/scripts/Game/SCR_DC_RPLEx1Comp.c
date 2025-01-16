class RplExample1ComponentColorAnimClass : ScriptComponentClass { }
RplExample1ComponentColorAnimClass g_RplExample1ComponentColorAnimClass;
 
class RplExample1ComponentColorAnim : ScriptComponent
{
    // Constant specifying how often (in seconds) to change the color index. For
    // example, setting this to 5 will change the color index every 5 seconds.
    private static const float COLOR_CHANGE_PERIOD_S = 5.0;
 
    // Helper variable for accumulating time (in seconds) every frame and to calculate
    // color index changes.
    private float m_TimeAccumulator_s;
 
    // Color index currently used for drawing the sphere.
    private int m_ColorIdx;
 
    override void OnPostInit(IEntity owner)
    {
        // We check whether this component is attached to entity of correct type and
        // report a problem if not. Once this test passes during initialization, we
        // do not need to worry about owner entity being wrong type anymore.
        auto shapeEnt = RplExampleDebugShape.Cast(owner);
        if (!shapeEnt)
        {
            Print("This example requires that the entity is of type `RplExampleDebugShape`.", LogLevel.WARNING);
            return;
        }
 
        // We initialize shape entity to correct color.
        shapeEnt.SetColorByIdx(m_ColorIdx);
 
        // We subscribe to "frame" events, so that we can run our logic in `EOnFrame`
        // event handler.
        SetEventMask(owner, EntityEvent.FRAME);
    }
 
    override void EOnFrame(IEntity owner, float timeSlice)
    {
        // We calculate change of color index based on time (and configured color
        // change period), then apply the change in color.
        int colorIdxDelta = CalculateColorIdxDelta(timeSlice);
        ApplyColorIdxDelta(owner, colorIdxDelta);
    }
 
    private int CalculateColorIdxDelta(float timeSlice)
    {
        // We first accumulate time and then calculate how many color change periods
        // have occurred, giving us number of colors we've cycled through.
        m_TimeAccumulator_s += timeSlice;
        int colorIdxDelta = m_TimeAccumulator_s / COLOR_CHANGE_PERIOD_S;
 
        // We remove full periods from the accumulator, only carrying over how much
        // time from current period has elapsed.
        m_TimeAccumulator_s -= colorIdxDelta * COLOR_CHANGE_PERIOD_S;
 
        return colorIdxDelta;
    }
 
    private void ApplyColorIdxDelta(IEntity owner, int colorIdxDelta)
    {
        // If there is no change to color index, we do nothing.
        if (colorIdxDelta == 0)
            return;
 
        // We calculate new color index.
        int newColorIdx = (m_ColorIdx + colorIdxDelta) % RplExampleDebugShape.COLOR_COUNT;
 
        // We check also new color index, since shorter periods and lower frame-rate
        // may result in new and old color index values being the same.
        if (newColorIdx == m_ColorIdx)
            return;
 
        // Now we can update the color index ...
        m_ColorIdx = newColorIdx;
 
        // ... and set new color based on new color index value.
        RplExampleDebugShape.Cast(owner).SetColorByIdx(m_ColorIdx);
    }
}