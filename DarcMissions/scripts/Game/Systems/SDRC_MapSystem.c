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
	protected bool m_bNvaChangesDone;	//If true, we have made changes to NonValidAreas and we should ask for a save from server

	protected ResourceName m_Layout = "{F928661E727CC639}UI/layouts/Map/SDRC_MapCanvasLayer.layout";

	int MARKER_WIDTH = (32 * 0.8);
	int MARKER_HEIGHT = (64 * 0.8);
			
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
	- Information (click on map)
	- Deletion (have cursor over item and press DELETE)
	*/	
	protected void EnableInput()
	{
		InputManager inputMgr = GetGame().GetInputManager();
		if (inputMgr)
		{
			inputMgr.AddActionListener("MapSelect", EActionTrigger.DOWN, OnShowSymbolInfo);
			inputMgr.AddActionListener("MapMarkerDelete", EActionTrigger.DOWN, OnSymbolDelete);
			inputMgr.AddActionListener("SDRCMapNonValidAreaIncrease", EActionTrigger.DOWN, OnNonValidAreaIncrease);
			inputMgr.AddActionListener("SDRCMapNonValidAreaDecrease", EActionTrigger.DOWN, OnNonValidAreaDecrease);
		}
	}
			
	protected void DisableInput()
	{
		InputManager inputMgr = GetGame().GetInputManager();
		if (inputMgr)
		{
			inputMgr.RemoveActionListener("MapSelect", EActionTrigger.DOWN, OnShowSymbolInfo);
			inputMgr.RemoveActionListener("MapMarkerDelete", EActionTrigger.DOWN, OnSymbolDelete);
			inputMgr.RemoveActionListener("SDRCMapNonValidAreaIncrease", EActionTrigger.DOWN, OnNonValidAreaIncrease);
			inputMgr.RemoveActionListener("SDRCMapNonValidAreaDecrease", EActionTrigger.DOWN, OnNonValidAreaDecrease);			
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Show marker information
	*/	
	protected void OnShowSymbolInfo(float value, EActionTrigger reason)
	{
		//SDRC_Log.Add("[SDRC_MapSystem:ShowMarkerInfo] Click.", LogLevel.NORMAL);
		int markerIdx = FindSymbolIndex();

		SDRC_RplGMComp gmComponent = SDRC_RplGMComp.GetInstance();
		if (!gmComponent)
		{
			return;
		}
		
		if (markerIdx > -1)
		{		
			if (gmComponent.m_Symbols[markerIdx].symbolType == SDRC_EDrawSymbol.MARKER)
			{
				SDRC_PlayerHelper.ShowChatMessage(WidgetManager.Translate("Mission ID: " + gmComponent.m_Symbols[markerIdx].sId + " : " + gmComponent.m_Symbols[markerIdx].sStrval + " : time left: " + gmComponent.m_Symbols[markerIdx].iTimeLeft));
			}
			
			if (gmComponent.m_Symbols[markerIdx].symbolType == SDRC_EDrawSymbol.NON_VALID_AREA)
			{
				SDRC_PlayerHelper.ShowChatMessage(WidgetManager.Translate("NonValidArea ID: " + gmComponent.m_Symbols[markerIdx].sId + " : " + gmComponent.m_Symbols[markerIdx].sStrval + " : radius: " + gmComponent.m_Symbols[markerIdx].fRadius));
			}
		}
	}
			
	//------------------------------------------------------------------------------------------------
	/*!
	Request for a symbol deletion
	*/	
	protected void OnSymbolDelete(float value, EActionTrigger reason)
	{
		int symbolIdx = FindSymbolIndex();
		
		SDRC_RplPlayerComp playerComponent = SDRC_RplPlayerComp.FindLocalInstance();
		SDRC_RplGMComp gmComponent = SDRC_RplGMComp.GetInstance();
		
		if ( (!gmComponent) || (!playerComponent) )
		{
			return;
		}

		SDRC_Log.Add("[SDRC_MapSystem:OnSymbolDelete] Deleting symbol: " + symbolIdx, LogLevel.SPAM);
				
		if ( (symbolIdx > -1) && (gmComponent.m_Symbols[symbolIdx].symbolType == SDRC_EDrawSymbol.MARKER) )
		{		
			playerComponent.AskForMissionDeletion(gmComponent.m_Symbols[symbolIdx].sId);
			SDRC_PlayerHelper.ShowChatMessage(WidgetManager.Translate("Deletion requested for Mission ID: " + gmComponent.m_Symbols[symbolIdx].sId));
			gmComponent.m_Symbols[symbolIdx].visible = false;
			m_SymbolCount = 0;	//Ask for map update
		}
		
		if ( (symbolIdx > -1) && (gmComponent.m_Symbols[symbolIdx].symbolType == SDRC_EDrawSymbol.NON_VALID_AREA) )
		{
			SDRC_PlayerHelper.ShowChatMessage(WidgetManager.Translate("Deletion requested for NonValidArea: " + gmComponent.m_Symbols[symbolIdx].sId + " : " + gmComponent.m_Symbols[symbolIdx].sStrval));
			playerComponent.AskForNonValidAreaDeletion(gmComponent.m_Symbols[symbolIdx].sId);
			m_bNvaChangesDone = true;
		}		
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnMapOpen(MapConfiguration mapConfig)
	{	
		//SDRC_Log.Add("[SDRC_MapSystem:OnMapOpen] Opened", LogLevel.DEBUG);

		m_bNvaChangesDone = false;	//No changes on nva done yet

		SDRC_RplPlayerComp playerComponent = SDRC_RplPlayerComp.FindLocalInstance();
		if (playerComponent)
		{
			playerComponent.AskForInfo();
		}
		else
		{
			SDRC_Log.Add("[SDRC_MapSystem:OnMapOpen] SDRC_RplPlayerComp not found", LogLevel.WARNING);		
		}

		//The rest of the stuff is only GM mode.								
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
		m_SymbolCount = 0;	//Ask for map update

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
		
		//If we made changes to NonValidAreas, request for a save
		if (m_bNvaChangesDone)
		{
			SDRC_RplPlayerComp playerComponent = SDRC_RplPlayerComp.FindLocalInstance();
			if (playerComponent)
			{
				SDRC_Log.Add("[SDRC_MapSystem:OnMapClose] Asking for NonValidArea save.", LogLevel.DEBUG);		
				playerComponent.AskForNonValidAreaSave();
			}
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
			foreach (SDRC_GMMapSymbol symbol : gmComponent.m_Symbols)
			{			
				switch (symbol.symbolType)
				{
					case SDRC_EDrawSymbol.NON_VALID_AREA:
					{
						PolygonDrawCommand drawCommand = new PolygonDrawCommand();		
						drawCommand = DrawCircle(symbol.vPos, symbol.fRadius, symbol.iIntval);
						if (drawCommand)
						{
							m_DrawCommands.Insert(drawCommand);
						}
						break;
					}
					case SDRC_EDrawSymbol.MARKER:
					{
						if (symbol.visible)
						{
							ImageDrawCommand drawCommand = new ImageDrawCommand();		
							drawCommand = DrawMarker(symbol.vPos, symbol.iIntval);
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
		/*
			ImageDrawCommand drawCommand = new ImageDrawCommand();		
			drawCommand = DrawMarker("1000 0 1500", SDRC_EMissionIcon.GM_MISSION_X_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1100 0 1500", SDRC_EMissionIcon.GM_MISSION_CONVOY_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1200 0 1500", SDRC_EMissionIcon.GM_MISSION_HUNTER_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1300 0 1500", SDRC_EMissionIcon.GM_MISSION_PATROL_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1400 0 1500", SDRC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1500 0 1500", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1600 0 1500", SDRC_EMissionIcon.GM_MISSION_CRASHSITE_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1700 0 1500", SDRC_EMissionIcon.GM_MISSION_OCCUPATION_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1800 0 1500", SDRC_EMissionIcon.GM_MISSION_HELICOPTER_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("1900 0 1500", SDRC_EMissionIcon.GM_MISSION_RADIOACTIVE_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("2000 0 1500", SDRC_EMissionIcon.GM_MISSION_HVTVIP_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("2100 0 1500", SDRC_EMissionIcon.GM_MISSION_HVTITEM_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("2200 0 1500", SDRC_EMissionIcon.GM_MISSION_WIN_MAP);
			m_DrawCommands.Insert(drawCommand);
			drawCommand = DrawMarker("2300 0 1500", SDRC_EMissionIcon.GM_MISSION_LOSE_MAP);
			m_DrawCommands.Insert(drawCommand);
		
//			DrawImage("2000 0 2000", 32, 64);
		*/
		#endif
		
		if (!m_DrawCommands.IsEmpty())
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
	ImageDrawCommand DrawMarker(vector center, SDRC_EMissionIcon icon)
	{
		string texture = SDRC_IconHelper.GetMarkerTexture(icon);
		if (texture == "")
		{
			SDRC_Log.Add("[SDRC_MapSystem:DrawMarker] Could not find texture for: (" + icon + ") " + SCR_Enum.GetEnumName(SDRC_EMissionIcon, icon), LogLevel.WARNING);
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
	Find the clicked symbol on the map.
	\return -1 if no marker found
	*/
	protected int FindSymbolIndex()
	{
		float worldX, worldY;
		m_MapEntity.GetMapCursorWorldPosition(worldX, worldY);
		
		return SDRC_GMHelper.GetSymbolIndex(worldX, worldY);
	}
	
	//------------------------------------------------------------------------------------------------	
	// NonValidArea stuff
	//------------------------------------------------------------------------------------------------	

	//------------------------------------------------------------------------------------------------
	/*!
	NonValidArea size increase/decrease
	*/	
	protected void OnNonValidAreaIncrease(float value, EActionTrigger reason)
	{
		NonValidAreaChangeSize(50);
	}

	protected void OnNonValidAreaDecrease(float value, EActionTrigger reason)
	{
		NonValidAreaChangeSize(-50);
	}
	
	protected void NonValidAreaChangeSize(float size)
	{
		int nvaIdx = FindSymbolIndex();

		SDRC_RplGMComp gmComponent = SDRC_RplGMComp.GetInstance();
		if (!gmComponent)
		{
			return;
		}

		if ( (nvaIdx > -1) && (gmComponent.m_Symbols[nvaIdx].symbolType == SDRC_EDrawSymbol.NON_VALID_AREA) )
		{		
			SDRC_Log.Add("[SDRC_MapSystem:NonValidAreaChangeSize] Changing: " + nvaIdx, LogLevel.SPAM);
			
			SDRC_RplPlayerComp playerComponent = SDRC_RplPlayerComp.FindLocalInstance();
			
			if ( (playerComponent) && (gmComponent) )
			{
				if ( ((gmComponent.m_Symbols[nvaIdx].fRadius + size) > 0) && 
				     ((gmComponent.m_Symbols[nvaIdx].fRadius + size) < SDRC_Misc.GetWorldSize()) 
				   )
				{
					playerComponent.AskForNonValidAreaSizeChange(gmComponent.m_Symbols[nvaIdx].sId, size);
					SDRC_PlayerHelper.ShowChatMessage(WidgetManager.Translate("NonValidArea ID: " + gmComponent.m_Symbols[nvaIdx].sId + " : Radius changed to : " + gmComponent.m_Symbols[nvaIdx].fRadius));
					m_SymbolCount = 0;	//Ask for map update
					m_bNvaChangesDone = true;
				}
			}
		}		
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!		
	The flow of a keypress and RPL stuff
	
	When in GM, open map:
	- Client: OnMapOpen happens 
	  - playerComponent.AskForInfo() asks for details from server
	  - SDRC_RplGMComp.gmComp.SyncMapSymbols() 
	- Client: EnableInput sets the actions for GM
	  - Keys are defined in "Configs/System/chimeraInputCommon.conf"
	- Client: A key is pressed to delete a symbol
	  - OnSymbolDelete() is called. 
	- Client: Player component is used to ask for the deletion from client to server
	  - playerComponent.AskForMissionDeletion()
	- Client->Server: With RPC, the server receives the request: RpcAsk_DeleteMission()
	- Server: GM component on server, performs the action: DoDeleteMission()
	- Server: DoDeleteMission() is done
	
	*/	
}