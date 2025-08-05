//------------------------------------------------------------------------------------------------
class SDRC_MapSystem : GameSystem
{
	protected SCR_MapEntity m_MapEntity;
	protected Widget m_Widget;
//	protected CanvasWidget m_Canvas;
	protected CanvasWidget m_wCanvasWidget;
	protected ref array<ref CanvasWidgetCommand> m_DrawCommands;
	protected vector m_previousPan;
	protected float m_previousZoom;
	
	protected ResourceName m_Layout = "{F928661E727CC639}UI/layouts/Map/SDRC_MapCanvasLayer.layout";
		
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
	protected void OnMapOpen(MapConfiguration mapConfig)
	{	
		//SDRC_Log.Add("[SDRC_MapSystem:OnMapOpen]", LogLevel.DEBUG);
		
		if (!SDRC_PlayerHelper.IsInGMmode())
		{
			return;
		}
		
		Widget mapFrame = m_MapEntity.GetMapMenuRoot().FindAnyWidget(SCR_MapConstants.MAP_FRAME_NAME);
		if (!mapFrame)
			mapFrame = m_MapEntity.GetMapMenuRoot();
		if (!mapFrame)
			 return;
		
		m_Widget = GetGame().GetWorkspace().CreateWidgets(m_Layout, mapFrame);
		m_wCanvasWidget = CanvasWidget.Cast(m_Widget);
//		m_wCanvasWidget = CanvasWidget.Cast(m_Widget.FindAnyWidget("Canvas"));
		
		m_DrawCommands = new array<ref CanvasWidgetCommand>();		
		m_previousPan = "-1000 0 -1000";
		m_previousZoom = -1000;
		
		SDRC_RplPlayerComp playerComponent = SDRC_RplPlayerComp.FindLocalInstance();
		if (playerComponent)
		{
			playerComponent.AskForInfo();
		}
		
		Enable(true);
	}
		
	//------------------------------------------------------------------------------------------------
	protected void OnMapClose(MapConfiguration mapConfig)
	{		
		if (!SDRC_PlayerHelper.IsInGMmode())
		{
			return;
		}

		SDRC_Log.Add("[SDRC_MapSystem:OnMapClose]", LogLevel.NORMAL);
				
		m_Widget.RemoveFromHierarchy();
		
		Enable(false);
		
		//When map is closed, clear the information
		m_DrawCommands = null;		
		m_wCanvasWidget = null;
		
		SDRC_RplGMComp gmComponent = SDRC_RplGMComp.GetInstance();
		if (gmComponent)
		{
			gmComponent.ClearSymbols();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update known symbols on map
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
		
		SDRC_RplGMComp gmComponent = SDRC_RplGMComp.GetInstance();
		if (gmComponent)
		{
			foreach(SDRC_GMMapSymbol symbol : gmComponent.m_Symbols)
			{			
				switch (symbol.type)
				{
					case DC_EDrawSymbol.CIRCLE:
					{
						PolygonDrawCommand drawCommand = new PolygonDrawCommand();		
						drawCommand = DrawCircle(symbol.pos, symbol.radius, symbol.intval);
						if (drawCommand)
						{
							m_DrawCommands.Insert(drawCommand);
						}
						break;
					}
					case DC_EDrawSymbol.MARKER:
					{
						ImageDrawCommand drawCommand = new ImageDrawCommand();		
						drawCommand = DrawMarker(symbol.pos, symbol.intval);
						if (drawCommand)
						{
							m_DrawCommands.Insert(drawCommand);
						}
						break;
					}
					default:
					{
					}
				}
			}			
		}		
		
		#ifndef SDRC_RELEASE
			ImageDrawCommand drawCommand = new ImageDrawCommand();		
			drawCommand = DrawMarker("1000 0 1500", DC_EMissionIcon.GM_MISSION_X_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1100 0 1500", DC_EMissionIcon.GM_MISSION_CONVOY_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1200 0 1500", DC_EMissionIcon.GM_MISSION_HUNTER_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1300 0 1500", DC_EMissionIcon.GM_MISSION_PATROL_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1400 0 1500", DC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1500 0 1500", DC_EMissionIcon.GM_MISSION_SQUATTERS_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1600 0 1500", DC_EMissionIcon.GM_MISSION_CRASHSITE_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1700 0 1500", DC_EMissionIcon.GM_MISSION_OCCUPATION_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1800 0 1500", DC_EMissionIcon.GM_MISSION_HELICOPTER_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1900 0 1500", DC_EMissionIcon.GM_MISSION_RADIOACTIVE_MAP);
			m_DrawCommands.Insert(drawCommand);
		
//			DrawImage("2000 0 2000", 32, 64);
		#endif
		
		if(!m_DrawCommands.IsEmpty())
		{						
			m_wCanvasWidget.SetDrawCommands(m_DrawCommands);			
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
	}

	//------------------------------------------------------------------------------------------------
	ImageDrawCommand DrawMarker(vector pos, DC_EMissionIcon icon)
	{
		string texture = SDRC_MapMarkerHelper.GetMarkerTexture(icon);
		if (texture == "")
		{
			//TBD: Add errormessage
			return null;
		}
		
		int width = 32;
		int height = 64;
		
		ImageDrawCommand drawCommand = new ImageDrawCommand();
		SharedItemRef tex = m_wCanvasWidget.LoadTexture(texture);
					
		int xpos, ypos;		
		m_MapEntity.WorldToScreen(pos[0], pos[2], xpos, ypos, true);
		
		drawCommand.m_Position = Vector(xpos - (width/2), ypos - (height/2), 0);
		drawCommand.m_pTexture = tex;
		drawCommand.m_Size = Vector(width, height, 0);
		drawCommand.m_iFlags = WidgetFlags.BLEND | WidgetFlags.STRETCH;
		
		return drawCommand;
	}	
	
	//------------------------------------------------------------------------------------------------
//	void DrawImage(vector center, int width, int height, string texture)
	void DrawImage(vector pos, int width, int height)
	{
		ImageDrawCommand drawCommand = new ImageDrawCommand();
		
		SharedItemRef tex = m_wCanvasWidget.LoadTexture("{8F0F7AD0EF00FCDB}UI/Textures/Icons/gm_mission_Convoy_map.edds");
					
		int xpos, ypos;		
		m_MapEntity.WorldToScreen(pos[0], pos[2], xpos, ypos, true);
		
		drawCommand.m_Position = Vector(xpos - (width/2), ypos - (height/2), 0);
		drawCommand.m_pTexture = tex;
		drawCommand.m_Size = Vector(width, height, 0);
		drawCommand.m_iFlags = WidgetFlags.BLEND | WidgetFlags.STRETCH;
		
		m_DrawCommands.Insert(drawCommand);
	}			
}