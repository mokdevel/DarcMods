class SDRC_MapClickComp : SCR_ScriptedWidgetComponent
{
	//------------------------------------------------------------------------------------------------
	//! An event called when the button, this component is attached to, is clicked
	override bool OnClick(Widget w, int x, int y, int button)
	{
		//SCR_MilitaryBaseManager.GetInstance().UpdateBasesSettings();
		//ShowChatMessage(WidgetManager.Translate("Mission ID: " + gmComponent.m_Symbols[markerIdx].id + " : " + gmComponent.m_Symbols[markerIdx].strval));
		
		SCR_MapEntity m_MapEntity = SCR_MapEntity.GetMapInstance();
		
		if (!m_MapEntity)
			return false;
		
		float worldX, worldY;
		m_MapEntity.GetMapCursorWorldPosition(worldX, worldY);
		
		SDRC_GMMapSymbol symbol = SDRC_GMHelper.GetMarkerDetails(worldX, worldY);
		
//		SDRC_PlayerHelper.ShowChatMessage(WidgetManager.Translate("Mission ID: " + worldX + "," + worldY));
		
		SDRC_PlayerHelper.ShowChatMessage(WidgetManager.Translate("Mission " + symbol.strval + " has " + symbol.timeLeft + " seconds left to complete."));
		
		return false;
	}
}
