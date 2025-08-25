//------------------------------------------------------------------------------------------------
class SDRC_MapSystem : GameSystem
{
	protected SCR_MapEntity m_MapEntity;
	protected Widget m_Widget;
	protected CanvasWidget m_wCanvasWidget;
	protected ref array<ref CanvasWidgetCommand> m_DrawCommands;
	protected vector m_previousPan;
	protected float m_previousZoom;
	protected int m_SymbolCount;

	protected ResourceName m_Layout = "{F928661E727CC639}UI/layouts/Map/SDRC_MapCanvasLayer.layout";

	int MARKER_WIDTH = (32 * 0.8);
	int MARKER_HEIGHT = (64 * 0.8);
	int MARKER_SIZE_BB = (24 * 0.8);		//Marker 'bounding box size' when searhing for mouse hit
			
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
		Init();
//		GetGame().GetCallqueue().Call(Init);
	}
	
	//! Inserts map listeners
	protected void Init()
	{
		SDRC_Log.Add("[SDRC_MapSystem:Init] Doing Init.", LogLevel.SPAM);
		m_MapEntity = SCR_MapEntity.GetMapInstance();
		
		if (!m_MapEntity)
			return;
		
		m_MapEntity.GetOnMapOpen().Insert(OnMapOpen);
		m_MapEntity.GetOnMapClose().Insert(OnMapClose);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Input managers for
	- Information
	- Deletion
	*/	
	protected void EnableInput()
	{
		InputManager inputMgr = GetGame().GetInputManager();
		if (inputMgr)
		{
			inputMgr.AddActionListener("MapSelect", EActionTrigger.DOWN, OnShowMarkerInfo);
			inputMgr.AddActionListener("MapMarkerDelete", EActionTrigger.DOWN, OnMarkerDelete);
		}
	}
			
	protected void DisableInput()
	{
		InputManager inputMgr = GetGame().GetInputManager();
		if (inputMgr)
		{
			inputMgr.RemoveActionListener("MapSelect", EActionTrigger.DOWN, OnShowMarkerInfo);
			inputMgr.RemoveActionListener("MapMarkerDelete", EActionTrigger.DOWN, OnMarkerDelete);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Show marker information
	*/	
	protected void OnShowMarkerInfo(float value, EActionTrigger reason)
	{
		//SDRC_Log.Add("[SDRC_MapSystem:ShowMarkerInfo] Click.", LogLevel.NORMAL);
		int markerIdx = FindMarkerIndex();
		
		if (markerIdx > -1)
		{		
			SDRC_RplGMComp gmComponent = SDRC_RplGMComp.GetInstance();
			
			if (gmComponent)
			{
				ShowChatMessage(WidgetManager.Translate("Mission ID: " + gmComponent.m_Symbols[markerIdx].id + " : " + gmComponent.m_Symbols[markerIdx].strval));
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Displays a message in chat
	*/	
	static void ShowChatMessage(string message)
	{
		PlayerController playerComponent = GetGame().GetPlayerController();
		if (!playerComponent)
			return;
		
		SCR_ChatComponent chatComponent = SCR_ChatComponent.Cast(playerComponent.FindComponent(SCR_ChatComponent));
		if (!chatComponent)
			return;
		
		chatComponent.ShowMessage(message);
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Request for a mission deletion
	*/	
	protected void OnMarkerDelete(float value, EActionTrigger reason)
	{
		int markerIdx = FindMarkerIndex();
		
		if (markerIdx > -1)
		{		
			SDRC_Log.Add("[SDRC_MapSystem:OnMarkerDelete] Deleting: " + markerIdx, LogLevel.SPAM);
			
			SDRC_RplPlayerComp playerComponent = SDRC_RplPlayerComp.FindLocalInstance();
			SDRC_RplGMComp gmComponent = SDRC_RplGMComp.GetInstance();
			
			if ( (playerComponent) && (gmComponent) )
			{
				playerComponent.AskForMissionDeletion(gmComponent.m_Symbols[markerIdx].id);
				ShowChatMessage(WidgetManager.Translate("Deletion requested for Mission ID: " + gmComponent.m_Symbols[markerIdx].id));
				gmComponent.m_Symbols[markerIdx].visible = false;
				m_SymbolCount = 0;	//Ask for map update
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnMapOpen(MapConfiguration mapConfig)
	{	
		//SDRC_Log.Add("[SDRC_MapSystem:OnMapOpen] Opened", LogLevel.DEBUG);
				
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
		
		m_DrawCommands = new array<ref CanvasWidgetCommand>();		
		m_SymbolCount = 0;

		SDRC_RplPlayerComp playerComponent = SDRC_RplPlayerComp.FindLocalInstance();
		if (playerComponent)
		{
			playerComponent.AskForInfo();
		}
		else
		{
			SDRC_Log.Add("[SDRC_MapSystem:OnMapOpen] SDRC_RplPlayerComp not found", LogLevel.WARNING);		
		}
		
		Enable(true);
		EnableInput();
	}

	//------------------------------------------------------------------------------------------------
	protected void OnMapClose(MapConfiguration mapConfig)
	{		
		if (!SDRC_PlayerHelper.IsInGMmode())
		{
			return;
		}

		if (m_Widget)				
		{
			m_Widget.RemoveFromHierarchy();
		}
		
		Enable(false);
		DisableInput();
		
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
	/*!	
	Update known symbols on map
	*/
	override protected void OnUpdate(ESystemPoint point)
	{		
		super.OnUpdate(point);
	
		if (!m_MapEntity)
			return;

		bool updateMap = false;			
		SDRC_RplGMComp gmComponent = SDRC_RplGMComp.GetInstance();
		if (gmComponent)
		{
			if (m_SymbolCount != gmComponent.m_Symbols.Count())
			{
				m_SymbolCount = gmComponent.m_Symbols.Count();
				updateMap = true;
			}
		}		
				
		vector currentPan = m_MapEntity.GetCurrentPan();
		float currentZoom = m_MapEntity.GetCurrentZoom();

		bool mapChange = ( (m_previousPan != currentPan) || (m_previousZoom != currentZoom) || updateMap );

		if (!mapChange)
		{
			return;
		}
		
		m_previousPan = currentPan;
		m_previousZoom = currentZoom;
		
		m_DrawCommands.Clear();
		
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
						if (symbol.visible)
						{
							ImageDrawCommand drawCommand = new ImageDrawCommand();		
							drawCommand = DrawMarker(symbol.pos, symbol.intval);
							if (drawCommand)
							{
								m_DrawCommands.Insert(drawCommand);						
							}
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
			drawCommand = DrawMarker("2000 0 1500", DC_EMissionIcon.GM_MISSION_HVTVIP_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("2100 0 1500", DC_EMissionIcon.GM_MISSION_HVTITEM_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("2200 0 1500", DC_EMissionIcon.GM_MISSION_WIN_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("2300 0 1500", DC_EMissionIcon.GM_MISSION_LOSE_MAP);
			m_DrawCommands.Insert(drawCommand);
		
//			DrawImage("2000 0 2000", 32, 64);
		#endif
		
		if(!m_DrawCommands.IsEmpty())
		{						
			m_wCanvasWidget.SetDrawCommands(m_DrawCommands);			
		}		
	}	

	//------------------------------------------------------------------------------------------------
	/*!	
	Create a drawCommand to draw a circle. 
	This is actually a filled round polygon.
	*/
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
	/*!	
	Create a drawCommand to draw a marker. 
	This is actually an image with marker texture.
	*/
	ImageDrawCommand DrawMarker(vector center, DC_EMissionIcon icon)
	{
		string texture = SDRC_MissionEnumHelper.GetMarkerTexture(icon);
		if (texture == "")
		{
			SDRC_Log.Add("[SDRC_MapSystem:DrawMarker] Could not find texture for: (" + icon + ") " + SCR_Enum.GetEnumName(DC_EMissionIcon, icon), LogLevel.WARNING);
			return null;
		}
		
		ImageDrawCommand drawCommand = new ImageDrawCommand();
		SharedItemRef tex = m_wCanvasWidget.LoadTexture(texture);
					
		int xpos, ypos;		
		m_MapEntity.WorldToScreen(center[0], center[2], xpos, ypos, true);
		
		drawCommand.m_Position = Vector(xpos - (MARKER_WIDTH/2), ypos - (MARKER_HEIGHT/2), 0);
		drawCommand.m_pTexture = tex;
		drawCommand.m_Size = Vector(MARKER_WIDTH, MARKER_HEIGHT, 0);
		drawCommand.m_iFlags = WidgetFlags.BLEND | WidgetFlags.STRETCH;
		
		return drawCommand;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Create a drawCommand to draw an image.
	TBD: This is currently not tested/working.
	\param center The center of the image
	*/	
	void DrawImage(vector center, int width, int height, string texture)
	{
		ImageDrawCommand drawCommand = new ImageDrawCommand();
		
		SharedItemRef tex = m_wCanvasWidget.LoadTexture(texture);
					
		int xpos, ypos;		
		m_MapEntity.WorldToScreen(center[0], center[2], xpos, ypos, true);
		
		drawCommand.m_Position = Vector(xpos - (width/2), ypos - (height/2), 0);
		drawCommand.m_pTexture = tex;
		drawCommand.m_Size = Vector(width, height, 0);
		drawCommand.m_iFlags = WidgetFlags.BLEND | WidgetFlags.STRETCH;
		
		m_DrawCommands.Insert(drawCommand);
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Find the clicked marker on the map.
	\return -1 if no marker found
	*/
	protected int FindMarkerIndex()
	{
		int symbolIdx = -1;
		
		SDRC_RplGMComp gmComponent = SDRC_RplGMComp.GetInstance();
		if (gmComponent)
		{
			float worldX, worldY;
			m_MapEntity.GetMapCursorWorldPosition(worldX, worldY);
			vector cursorPos = "0 0 0";
			cursorPos[0] = worldX;
			cursorPos[2] = worldY - (MARKER_SIZE_BB/2)/m_previousZoom;	//Move the point to check up a little
			
			float distanceCheck = MARKER_SIZE_BB/m_previousZoom;
			
			int idx = 0;
			
			foreach(SDRC_GMMapSymbol symbol : gmComponent.m_Symbols)
			{
				if (symbol.type == DC_EDrawSymbol.MARKER && symbol.visible)
				{
					float distance = vector.DistanceXZ(cursorPos, symbol.pos);
					//SDRC_Log.Add("[SDRC_MapSystem:ShowMarkerInfo] Checking: " + cursorPos + " vs " + symbol.pos + " d=" + distance + " (" + distanceCheck + ")", LogLevel.NORMAL);
					if (SDRC_Misc.IsPosNearPos(cursorPos, symbol.pos, distanceCheck))
					{
//						SDRC_Log.Add("[SDRC_MapSystem:ShowMarkerInfo] Found.", LogLevel.NORMAL);
						symbolIdx = idx;
						break;
					}								
				}
				idx++;
			}
			
			if (symbolIdx > -1)
			{
				SDRC_Log.Add("[SDRC_MapSystem:ShowMarkerInfo] Found marker - index: " + symbolIdx, LogLevel.SPAM);
			}
		}		
		
		return symbolIdx;
	}			
}