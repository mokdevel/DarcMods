//------------------------------------------------------------------------------------------------
//! Map module base class
//
// This piece of code is inspired from Overthrow.

/*
TBD: Remove this. All map drawing stuff is in the SDRC_MapSystem


//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class SDRC_MapCanvasLayer : SCR_MapModuleBase
{		
	protected Widget m_Widget;
	protected CanvasWidget m_Canvas;
	protected ref array<ref CanvasWidgetCommand> m_DrawCommands;
	
	protected ResourceName m_Layout = "{A6A79ABB08D490BF}UI/layouts/Map/SDRC_MapCanvasLayer.layout";
		
	//------------------------------------------------------------------------------------------------
	void Draw()
	{		
		
	}
	
	//------------------------------------------------------------------------------------------------
	override void Update(float timeSlice)
	{	
		Draw();
		
		if(!m_DrawCommands.IsEmpty())
		{						
			m_Canvas.SetDrawCommands(m_DrawCommands);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);
		
		#ifndef SDRC_RELEASE
			SDRC_Log.Add("[SDRC_MapCanvasLayer:OnMapOpen]", LogLevel.DEBUG);
		#endif
		
		m_DrawCommands = new array<ref CanvasWidgetCommand>();
		m_Widget = GetGame().GetWorkspace().CreateWidgets(m_Layout);
		m_Canvas = CanvasWidget.Cast(m_Widget.FindAnyWidget("Canvas"));
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMapClose(MapConfiguration config)
	{
		super.OnMapClose(config);
		
		m_Widget.RemoveFromHierarchy();
	}
}

*/