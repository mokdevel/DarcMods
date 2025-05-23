//[BaseContainerProps()]
class DC_Marker : Managed
{
	string id;
	vector pos;
	string text; 
	vector angle;
	DC_EMarkerType markerType;
	Widget widget;
}

enum DC_EMarkerType
{
	NONE,
	DEBUG,
	MISSION
};

const string DC_LAYOUT_MISSION = "{B101DDAED7CA6B3C}UI/layouts/Map/Map_DC_Marker.layout";
const string DC_MARKERIMAGE_MISSION = "{40C90A13FA693EA0}UI/Textures/Icons/iconmission_ca.edds";

const string DC_LAYOUT_DEBUG = "{2E303E83CC84794E}UI/layouts/Map/Map_DC_MarkerDebug.layout";
const string DC_MARKERIMAGE_DEBUG = "{80A57BA69874356D}UI/Textures/Icons/icondebug_ca.edds";

//------------------------------------------------------------------------------------------------
class SCR_DC_MapMarkersUI : SCR_MapUIBaseComponent
{
	static bool m_isMapOpen = false;

	//Queue for widgets to be created	
	static int m_markersQueueIndex = 0;
	static ref array<ref DC_Marker> m_markers = {};

	//------------------------------------------------------------------------------------------------
	override void Init()
	{
		SCR_DC_Log.Add("[SCR_DC_MapMarkersUI:Init]", LogLevel.NORMAL);
	
		m_bHookToRoot = true;
	}
		
	//------------------------------------------------------------------------------------------------
	override void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);
		
		m_MapEntity = SCR_MapEntity.GetMapInstance();
		if (!m_MapEntity) 
			return;

		if (!m_isMapOpen)
		{
			m_markersQueueIndex = 0;
			
			SCR_DC_Log.Add("[SCR_DC_MapMarkersUI] Count: " + m_markers.Count(), LogLevel.NORMAL);			
		}

		if (!SCR_DC_Core.RELEASE)
		{
//			SCR_DC_MissionHelper.DebugTestMissionPos();
		}
		
		m_isMapOpen = true;			
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnMapClose(MapConfiguration config)
	{
		m_isMapOpen = false;
		
		//Clean up marker widgets
		foreach(DC_Marker marker: m_markers)		
		{
			delete marker.widget;
		}
		
		if (!SCR_DC_Core.RELEASE)
		{
			SCR_DC_MapMarkerHelper.DeleteMarker("DUMMY_");
		}
		
		super.OnMapClose(config);
	}
	
	//------------------------------------------------------------------------------------------------
	override void Update(float timeSlice)
	{
		super.Update(timeSlice);
		
		if (!m_isMapOpen)
			return;
		
		CreateMarkers();		
		UpdateMarkers();		
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Add a marker to the marker list
	\param idPrefix The prefix for the unique id of the marker. 
	\param pos Position of the marker 
	\param markerText Test of the marker
	\param angle Angle of the marker
	\return id 
	*/
	static string AddMarker(string idPrefix, vector pos, string markerText = "", DC_EMarkerType markerType = DC_EMarkerType.MISSION, vector angle = "0 0 0")	
	{
		DC_Marker marker();
		marker.id = idPrefix + string.ToString(System.GetTickCount());
		marker.pos = pos;
		if (markerText == "")
			markerText = " ";
		marker.text = markerText;
		marker.angle = angle;
		marker.markerType = markerType;
		marker.widget = null;
		
		m_markers.Insert(marker);

		return marker.id;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Shows a hint
	TBD: Not sure how to properly show hint for all players. Trying various things...
	*/
	static void AddMarkerHint(string title, string description, string id)	
	{	
		SCR_DC_MapMarkerEntity dummy = SCR_DC_MapMarkerEntity.Cast(m_MarkerEntity);		
		dummy.AddMarkerHint(title, description, id);
	}			
	
	//------------------------------------------------------------------------------------------------
	/*!
	Create widgets for markers
	*/
	protected void CreateMarkers()
	{
		int queueCount = m_markers.Count();
		if (queueCount > 0)
		{
			for (int i = m_markersQueueIndex; i < queueCount; i++)		
			{
				Widget widget = CreateMarkerWidget(m_markers[i]);
				if (widget)
					m_markers[i].widget = widget;
			}
			m_markersQueueIndex = queueCount;
		}
	}	
			
	//------------------------------------------------------------------------------------------------
	/*!
	Draw marker widgets to screen
	*/
	protected void UpdateMarkers()
	{
		float screenPosX, screenPosY;
		//float posW, posH;
 		WorkspaceWidget workspace = GetGame().GetWorkspace();

		for (int i = 0; i < m_markers.Count(); i++)
		{
			m_MapEntity.WorldToScreen(m_markers[i].pos[0], m_markers[i].pos[2], screenPosX, screenPosY, true);		
			screenPosX = workspace.DPIUnscale(screenPosX);
			screenPosY = workspace.DPIUnscale(screenPosY);

			FrameSlot.SetPos(
				m_markers[i].widget, 
				screenPosX,
				screenPosY
			);						
		}
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Create widgets for markers
	/param marker The marker for which the widget is to be created
	*/
	protected Widget CreateMarkerWidget(DC_Marker marker)	
	{	
		if (!m_RootWidget)
			return null;
		
		string markerImage;
		string markerLayout;
		
		switch (marker.markerType)
		{
			case DC_EMarkerType.DEBUG:
				markerImage = DC_MARKERIMAGE_DEBUG;
				markerLayout = DC_LAYOUT_DEBUG;
				break;
			case DC_EMarkerType.MISSION:
				markerImage = DC_MARKERIMAGE_MISSION;
				markerLayout = DC_LAYOUT_MISSION;
				break;
			default:
				SCR_DC_Log.Add("[SCR_DC_MapMarkersUI:CreateMarker] Incorrect DC_EMarkerType.", LogLevel.ERROR);
				return null;
				break;
		}
		
		Widget widget = GetGame().GetWorkspace().CreateWidgets(markerLayout, m_RootWidget);
		
		if (!widget) 
			return null;
						
		ImageWidget image = ImageWidget.Cast(widget.FindAnyWidget("Image"));
		if(image)
		{
			image.LoadImageTexture(0, markerImage);
			image.SetRotation(marker.angle[1]);
			image.SetVisible(true);
		}

		if (marker.text != "")
		{		
			TextWidget text = TextWidget.Cast(widget.FindAnyWidget("Text"));
			if(text)
			{
				text.SetText(marker.text);
				text.SetRotation(marker.angle[1]);
				text.SetVisible(true);
			}		
		}

		return widget;
	}
}