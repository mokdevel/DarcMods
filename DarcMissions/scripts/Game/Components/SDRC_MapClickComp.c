//SDRC_MapClickComp.c

//------------------------------------------------------------------------------------------------
/*!
This is the component that handles marker click.
Set this to ButtonWidget on SDRC_MapMarkerBase.layout
*/
class SDRC_MapClickComp : SCR_ScriptedWidgetComponent
{
	//------------------------------------------------------------------------------------------------
	//! An event called when the button, this component is attached to, is clicked
	override bool OnClick(Widget w, int x, int y, int button)
	{
		//SCR_MilitaryBaseManager.GetInstance().UpdateBasesSettings();
		//ShowChatMessage(WidgetManager.Translate("Mission ID: " + gmComponent.m_Symbols[markerIdx].id + " : " + gmComponent.m_Symbols[markerIdx].strval));

		SDRC_Log.Add("[SDRC_MapClickComp:OnClick]", LogLevel.SPAM);
				
		if (SDRC_GMHelper.IsShowMissionTimeLeft())
		{
			SCR_MapEntity m_MapEntity = SCR_MapEntity.GetMapInstance();
			
			if (!m_MapEntity)
				return false;
			
			float worldX, worldY;
			m_MapEntity.GetMapCursorWorldPosition(worldX, worldY);
			
			#ifndef SDRC_RELEASE
				int idx = SDRC_GMHelper.GetSymbolIndex(worldX, worldY);
				SDRC_Log.Add("[SDRC_MapClickComp:OnClick] Index: " + idx + " at " + worldX + "," + worldY, LogLevel.SPAM);
			#endif
			
			SDRC_GMMapSymbol symbol = SDRC_GMHelper.GetMarkerDetails(worldX, worldY);
			
			if (symbol)
			{		
				int minutes = (symbol.iTimeLeft / 60);
				string timeStr = "" + minutes + " minutes";
				
				if (symbol.iTimeLeft < 60)
				{
					timeStr = " less than a minute";
				}
				
				SDRC_PlayerHelper.ShowChatMessage(WidgetManager.Translate("You have " + timeStr + " to complete "  + symbol.sStrval));
			}
			else
			{
				SDRC_PlayerHelper.ShowChatMessage(WidgetManager.Translate("Intel not available."));
			}
		}
				
		return false;
	}
}
