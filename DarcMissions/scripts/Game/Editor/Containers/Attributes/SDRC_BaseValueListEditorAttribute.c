/**
Attribute base for slider values for other attributes to inherent from
*/
[BaseContainerProps()]
class SDRC_MissionSubIdxEditorAttributeBaseValues:SCR_EditorAttributeBaseValues
{
	override void GetSliderValues(out string sliderValueFormating, out float min, out float max, out float step, out int decimals)
	{		
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		
		if (baseGameMode)		
		{		
			int num = baseGameMode.missionFrame.justAnumber;	//TBD: Remove, just for testing
		
			m_fMin = -1;
			m_fMax = num;
			m_fStep = 1;
			m_iDecimals = 0;
			
			sliderValueFormating = m_sSliderValueFormating;
			min = m_fMin;
			max = m_fMax;
			step = m_fStep;
			decimals = m_iDecimals;
		}			
	}
};
