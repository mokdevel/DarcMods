//------------------------------------------------------------------------------------------------
//! Basic image & text entry
[BaseContainerProps(), SCR_MapMarkerTitle()]
class SCR_DC_MapMarkerEntryImageText : SCR_MapMarkerEntryImageText
{
	//------------------------------------------------------------------------------------------------
	override SCR_EMapMarkerType GetMarkerType()
	{
	 	return SCR_EMapMarkerType.DARC_MISSION;
	}	
}

//------------------------------------------------------------------------------------------------
//! Placed marker color entry
[BaseContainerProps(), SCR_MapMarkerSimpleTitle()]
class SCR_DC_MarkerSimpleConfig
{
	[Attribute("UNNAMED")]
	protected string m_sName;
	
	[Attribute("0", desc: "Unique secondary ID (primary is EMarkerType) used to reduce toll on the network during synchronization by loading const data from config")]
	protected int m_iEntryID;
	
	[Attribute("{2EFEA2AF1F38E7F0}UI/Textures/Icons/icons_wrapperUI-64.imageset", UIWidgets.ResourcePickerThumbnail, desc: "Image resource", params: "imageset")]
	protected ResourceName m_sIconImageset;
	
	[Attribute("", desc: "Imageset quad")]
	protected string m_sIconImagesetQuad;
	
	[Attribute("1.0 0.0 1.0 1.0")]
	protected ref Color m_Color;
	
	[Attribute("", desc: "Text under the marker icon")]
	protected string m_sText;
	
	//------------------------------------------------------------------------------------------------
	int GetEntryID()
	{
		return m_iEntryID;
	}
	
	//------------------------------------------------------------------------------------------------
	void GetIconResource(out ResourceName imageset, out string imageQuad)
	{
		imageset = m_sIconImageset;
		imageQuad = m_sIconImagesetQuad;
	}
	
	//------------------------------------------------------------------------------------------------
	Color GetColor()
	{
		return Color.FromInt(m_Color.PackToInt());
	}
	
	//------------------------------------------------------------------------------------------------
	string GetText()
	{
	 	return m_sText;
	}
	
}

//------------------------------------------------------------------------------------------------
class SCR_DC_MapMarkerSimpleTitle: BaseContainerCustomTitle
{
	//------------------------------------------------------------------------------------------------
	override bool _WB_GetCustomTitle(BaseContainer source, out string title)
	{		
		source.Get("m_sName", title);

		return true;
	}
}