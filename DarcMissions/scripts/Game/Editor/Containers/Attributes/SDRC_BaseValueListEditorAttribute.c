/**
Attribute base for slider values for other attributes to inherent from
*/
[BaseContainerProps()]
class SDRC_EditorAttributeBaseValues:SCR_EditorAttributeBaseValues
{
	override void GetSliderValues(out string sliderValueFormating, out float min, out float max, out float step, out int decimals)
	{
		m_fMin = 1;
		m_fMax = 12;
		m_fStep = 2;
		m_iDecimals = 0;
		
		sliderValueFormating = m_sSliderValueFormating;
		min = m_fMin;
		max = m_fMax;
		step = m_fStep;
		decimals = m_iDecimals;
	}
};