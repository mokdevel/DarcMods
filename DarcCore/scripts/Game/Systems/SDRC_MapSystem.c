//------------------------------------------------------------------------------------------------
class SDRC_MapSystem : GameSystem
{
	
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
		m_mapEntity = SCR_MapEntity.GetMapInstance();
		
		if (!m_mapEntity)
			return;
		
		m_mapEntity.GetOnMapOpen().Insert(OnMapOpen);
		m_mapEntity.GetOnMapClose().Insert(OnMapClose);
	}

	//------------------------------------------------------------------------------------------------
	// Map initialization
	//------------------------------------------------------------------------------------------------
	
	protected SCR_MapEntity m_mapEntity;
	protected CanvasWidget m_wCanvasWidget;
	
/*	protected ref array<ref TILW_Drawing> m_drawings = {};
	protected ref array<ref CanvasWidgetCommand> m_drawCommands = null;
	protected ref TILW_Drawing m_previewDrawing = null;
	protected ref array<int> m_drawPoints = null;*/
	
	//! Creates widget and assigns draw commands when the map is opened
	protected void OnMapOpen(MapConfiguration mapConfig)
	{	
		SDRC_Log.Add("[SDRC_MapSystem:OnMapOpen]", LogLevel.DEBUG);		
		
/*		Widget mapFrame = m_mapEntity.GetMapMenuRoot().FindAnyWidget(SCR_MapConstants.MAP_FRAME_NAME);
		if (!mapFrame)
			mapFrame = m_mapEntity.GetMapMenuRoot();
		if (!mapFrame)
			 return;
		
		Widget w = GetGame().GetWorkspace().CreateWidgets("{F928661E727CC638}UI/Map/TILW_DrawingCanvas.layout", mapFrame);
		m_wCanvasWidget = CanvasWidget.Cast(w);
		
		int factionIndex = TILW_DrawingHelper.GetLocalFactionIndex();
		
		m_drawCommands = {};
		foreach (TILW_Drawing drawing : m_drawings)
		{
			if (!TILW_DrawingHelper.IsVisibleFaction(drawing.m_factionIndex, factionIndex))
				continue;
			m_drawCommands.Insert(drawing.m_drawCommand);
		}
		m_wCanvasWidget.SetDrawCommands(m_drawCommands);
		
		Enable(true);
		EnableDrawing();*/
	}
	
	//! Destroys widget and draw commands when the map is closed
	protected void OnMapClose(MapConfiguration mapConfig)
	{
		SDRC_Log.Add("[SDRC_MapSystem:OnMapClose]", LogLevel.DEBUG);				
/*		Enable(false);
		m_wCanvasWidget = null;
		m_drawCommands = null;
		m_previewDrawing = null;
		m_drawPoints = null;
		DisableDrawing();*/
	}
	
	//------------------------------------------------------------------------------------------------
	// Drawing processing - updates vertices of existing drawings
	//------------------------------------------------------------------------------------------------
	
/*	protected vector m_previousPan;
	protected float m_previousZoom;
	
	protected float m_previousCursorX;
	protected float m_previousCursorY;
	
	//! Updates existing drawings if the map has been transformed
	override protected void OnUpdate(ESystemPoint point)
	{
		super.OnUpdate(point);
		
		if (!m_mapEntity)
			return;
		
		vector currentPan = m_mapEntity.GetCurrentPan();
		float currentZoom = m_mapEntity.GetCurrentZoom();
		
		bool mapChange = (m_previousPan != currentPan) || (m_previousZoom != currentZoom);
		
		if (m_previewDrawing)
		{
			float cursorX, cursorY;
			m_mapEntity.GetMapCursorWorldPosition(cursorX, cursorY);
			if (mapChange || cursorX != m_previousCursorX || cursorY != m_previousCursorY)
			{
				UpdatePreviewCommand(currentZoom);
				m_previousCursorX = cursorX;
				m_previousCursorY = cursorX;
			}
		}
		
		if (!mapChange)
			return;
		
		foreach (TILW_Drawing drawing : m_drawings)
			drawing.UpdateCommand(currentZoom);
		
		m_previousPan = currentPan;
		m_previousZoom = currentZoom;
	}*/
}