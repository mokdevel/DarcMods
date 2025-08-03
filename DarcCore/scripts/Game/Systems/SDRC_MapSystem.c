//------------------------------------------------------------------------------------------------
class SDRC_MapSystem : GameSystem
{
#ifndef SDRC_RELEASE	
	
	protected Widget m_Widget;
	protected CanvasWidget m_Canvas;
	protected ref array<ref CanvasWidgetCommand> m_DrawCommands;
	
	protected ResourceName m_Layout = "{A6A79ABB08D490BF}UI/layouts/Map/SDRC_MapCanvasLayer.layout";
		
	//------------------------------------------------------------------------------------------------
	// System initialization
	//------------------------------------------------------------------------------------------------
	
	//! Gets instance of the drawing system
	static SDRC_MapSystem GetInstance()
	{
		World world = GetGame().GetWorld();
		if (!world)
			return null;
		return SDRC_MapSystem.Cast(world.FindSystem(SDRC_MapSystem));
	}
	
	//! Initializes info
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		outInfo
			.SetAbstract(false)
			.SetLocation(ESystemLocation.Client)
			.AddPoint(ESystemPoint.PostFrame);
	}
	
	//! Initializes system
	override void OnInit()
	{
		super.OnInit();
		Enable(false);
		GetGame().GetCallqueue().Call(Init);
	}
	
	//! Inserts map listeners
	protected void Init()
	{
		m_MapEntity = SCR_MapEntity.GetMapInstance();
		
		if (!m_MapEntity)
			return;
		
		m_MapEntity.GetOnMapOpen().Insert(OnMapOpen);
		m_MapEntity.GetOnMapClose().Insert(OnMapClose);
	}

	//------------------------------------------------------------------------------------------------
	// Map initialization
	//------------------------------------------------------------------------------------------------
	
	protected SCR_MapEntity m_MapEntity;
	protected CanvasWidget m_wCanvasWidget;
	protected vector m_previousPan;
	protected float m_previousZoom;
	
	//! Creates widget and assigns draw commands when the map is opened
	protected void OnMapOpen(MapConfiguration mapConfig)
	{	
		SDRC_Log.Add("[SDRC_MapSystem:OnMapOpen]", LogLevel.DEBUG);		
		
		if (!SDRC_PlayerHelper.IsInGMmode())
		{
			return;
		}
		
		Widget mapFrame = m_MapEntity.GetMapMenuRoot().FindAnyWidget(SCR_MapConstants.MAP_FRAME_NAME);
		if (!mapFrame)
			mapFrame = m_MapEntity.GetMapMenuRoot();
		if (!mapFrame)
			 return;
		
		m_Widget = GetGame().GetWorkspace().CreateWidgets(m_Layout);
		m_Canvas = CanvasWidget.Cast(m_Widget.FindAnyWidget("Canvas"));
		m_DrawCommands = new array<ref CanvasWidgetCommand>();		
		m_previousPan = "-1000 0 -1000";
		m_previousZoom = -1000;
		Enable(true);
	}
		
	//! Destroys widget and draw commands when the map is closed
	protected void OnMapClose(MapConfiguration mapConfig)
	{
		SDRC_Log.Add("[SDRC_MapSystem:OnMapClose]", LogLevel.DEBUG);				
		
		if (!SDRC_PlayerHelper.IsInGMmode())
		{
			return;
		}
		
		m_Widget.RemoveFromHierarchy();
		
		Enable(false);
		m_DrawCommands = null;
		
/*		m_wCanvasWidget = null;
		m_previewDrawing = null;
		m_drawPoints = null;
		DisableDrawing();*/
	}
	
	//! Updates existing drawings if the map has been transformed
	override protected void OnUpdate(ESystemPoint point)
	{
		super.OnUpdate(point);
		
		if (!m_MapEntity)
			return;
		
		vector currentPan = m_MapEntity.GetCurrentPan();
		float currentZoom = m_MapEntity.GetCurrentZoom();

		bool mapChange = (m_previousPan != currentPan) || (m_previousZoom != currentZoom);
		
		if (!mapChange)
		{
			return;
		}
		
		m_DrawCommands.Clear();
		
		PolygonDrawCommand drawCommand = new PolygonDrawCommand();		
		drawCommand = DrawCircle("2000 0 2000", 300, ARGB(75, 255, 75, 0));
		m_DrawCommands.Insert(drawCommand);
		
		foreach(SDRC_DebugHelperPos mapCircle : SDRC_DebugHelper.m_MapCircle)
		{			
			drawCommand = DrawCircle(mapCircle.pos, mapCircle.radius, mapCircle.color);
			m_DrawCommands.Insert(drawCommand);
		}		
		
		if(!m_DrawCommands.IsEmpty())
		{						
			m_Canvas.SetDrawCommands(m_DrawCommands);			
		}
		
		m_previousPan = currentPan;
		m_previousZoom = currentZoom;
	}	

	//------------------------------------------------------------------------------------------------
	PolygonDrawCommand DrawCircle(vector center, float range, int color, int n = 36)	
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
		
		return drawCommand;
		//m_DrawCommands.Insert(drawCommand);
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
#endif
}