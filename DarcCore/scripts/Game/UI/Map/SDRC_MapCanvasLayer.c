//------------------------------------------------------------------------------------------------
//! Map module base class
//
// This piece of code is inspired from Overthrow.

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
	void DrawCircle(vector center, float range, int color, int n = 36)	
	{
		PolygonDrawCommand drawCommand = new PolygonDrawCommand();		
		
		drawCommand.m_iColor = color;		
		drawCommand.m_Vertices = new array<float>;
		
		float xpos, ypos;
		
		m_MapEntity.WorldToScreen(center[0], center[2], xpos, ypos, true);
		float r = range * m_MapEntity.GetCurrentZoom();

		vector pos_center = "0 0 0";
		pos_center[0] = xpos;
		pos_center[2] = ypos;
		
		for(int i = 0; i < n; i++)
		{			
			float angle = i * (350/n);
			vector pos = SDRC_Misc.GetCoordinatesOnCircle(pos_center, r, angle);
			drawCommand.m_Vertices.Insert(pos[0]);
			drawCommand.m_Vertices.Insert(pos[2]);			
		}
		
		m_DrawCommands.Insert(drawCommand);
	}
	
	//------------------------------------------------------------------------------------------------
	void DrawImage(vector center, int width, int height, SharedItemRef tex)
	{
		ImageDrawCommand drawCommand = new ImageDrawCommand();
		
		int xpos, ypos;		
		m_MapEntity.WorldToScreen(center[0], center[2], xpos, ypos, true);
		
		drawCommand.m_Position = Vector(xpos - (width/2), ypos - (height/2), 0);
		drawCommand.m_pTexture = tex;
		drawCommand.m_Size = Vector(width, height, 0);
		
		m_DrawCommands.Insert(drawCommand);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);
		
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