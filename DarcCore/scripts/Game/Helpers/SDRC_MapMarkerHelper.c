//Helpers SDRC_MapMarkerHelper.c

class DC_Mmarker : Managed
{
	string id;		//Mission related ID
	int iID;		//Internal ID in SCR_MapMarkerManagerComponent
	
	void DC_Mmarker(int lifetime = 0)
	{
		if (lifetime != 0)
		{
			GetGame().GetCallqueue().CallLater(LifecycleEnd, lifetime * 1000, false);
		}
	}

	void ~DC_Mmarker()
	{
		SDRC_Log.Add("[SCR_DC:DC_Mmarker] Destroyer: " + id, LogLevel.SPAM);        							
	}
		
	void LifecycleEnd()
	{
		SDRC_Log.Add("[SCR_DC:DC_Mmarker:LifecycleEnd] Deleting marker: " + id, LogLevel.DEBUG);        							
		SDRC_MapMarkerHelper.DeleteMarker(id, true);
	}
}

//------------------------------------------------------------------------------------------------
sealed class SDRC_MapMarkerHelper
{
	static ref array<ref DC_Mmarker> m_markers = {};
	
	static void CreateMapMarker(vector pos, int icon, string id, string title = "", int lifetime = 0, string markerTypeString = "SCR_EMapMarkerType.DARC_MISSION")
	{		
		
		SCR_EMapMarkerType markerType = typename.StringToEnum(SCR_EMapMarkerType, markerTypeString);

		SCR_MapMarkerManagerComponent mapMarkerMgr = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		if (!mapMarkerMgr)
			return;

//		Faction faction = GetGame().GetFactionManager().GetFactionByKey("US");
//        int fIndex = GetGame().GetFactionManager().GetFactionIndex(faction);		
				
		SCR_MapMarkerBase markerst = new SCR_MapMarkerBase();
		markerst.SetType(markerType);
		markerst.SetCustomText(title);
		markerst.SetWorldPos(pos[0], pos[2]);
		markerst.SetIconEntry(icon);
		markerst.SetMarkerFactionFlags(0);	//Everyone can see the markers
		mapMarkerMgr.InsertStaticMarker(markerst, false, true);		
		
		DC_Mmarker dcmarker = new DC_Mmarker(lifetime);
		dcmarker.id = id;
		dcmarker.iID = markerst.GetMarkerID();
		m_markers.Insert(dcmarker);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Delete marker with certain id
	\param id Id of the marker to delete. This works as a wildcard so "A_" will delete all markers starting with A_
	\param exact The id must be exactly the same. Essentially disables the wildcard functionality.
	*/
	static void DeleteMarker(string id, bool exact = false)
	{
		if (GetGame().GetGameMode())
		{
			SCR_MapMarkerManagerComponent mapMarkerMgr = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
			if (!mapMarkerMgr)
				return;
			
			int startCount = m_markers.Count();
			
			//Clean up marker widgets
			for (int i = 0; i < m_markers.Count(); i++)		
			{
				if ( (m_markers[i].id.Contains(id) && exact == false) || (m_markers[i].id == id && exact) )
				{
					int iID = m_markers[i].iID;
					SCR_MapMarkerBase marker;
					marker = GetMarkerByID(m_markers[i].iID);
					if (marker)
					{
						mapMarkerMgr.RemoveStaticMarker(marker);
						m_markers.Remove(i);
						i--;
					}
					else
					{
						SDRC_Log.Add("[SDRC_MapMarkerHelper:DeleteMarker] SCR_MapMarkerBase NULL!", LogLevel.SPAM);        							
					}					
				}	
			}
			
			SDRC_Log.Add("[SDRC_MapMarkerHelper:DeleteMarker] " + m_markers.Count() + " markers. Deleted " + (startCount - m_markers.Count()) + " markers", LogLevel.DEBUG);        		
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Find marker id
	\param id Id of the marker to find.
	See: https://feedback.bistudio.com/T182932
	*/
	static SCR_MapMarkerBase GetMarkerByID(int markerID)
	{
		SCR_MapMarkerBase marker;

		SCR_MapMarkerManagerComponent mapMarkerMgr = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		if (!mapMarkerMgr)
			return null;
				
		marker = mapMarkerMgr.GetStaticMarkerByID(markerID);
		if (marker)
			return marker;

		marker = mapMarkerMgr.GetDisabledMarkerByID(markerID);
		if (marker)
			return marker;
		
		return null;
	}
}

/*	protected void CreateMapMarker2(vector pos)
	{
		SCR_MapMarkerManagerComponent mapMarkerMgr = SCR_MapMarkerManagerComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_MapMarkerManagerComponent));
		if (!mapMarkerMgr)
			return;
		m_MapMarker = new SCR_MapMarkerBase();
		
		SCR_ScenarioFrameworkMarkerCustom mapMarkerCustom = SCR_ScenarioFrameworkMarkerCustom.Cast(m_MapMarkerType);
		if (mapMarkerCustom)
		{
			m_MapMarker.SetType(SCR_EMapMarkerType.DARC_MISSION);
			m_MapMarker.SetIconEntry(SCR_EScenarioFrameworkMarkerCustom.FORTIFICATION);
			m_MapMarker.SetColorEntry(SCR_EScenarioFrameworkMarkerCustomColor.RED);
			m_MapMarker.SetCustomText("My marker");
//			m_MapMarker.SetIconEntry(mapMarkerCustom.m_eMapMarkerIcon);
//			m_MapMarker.SetRotation(mapMarkerCustom.m_iMapMarkerRotation);
//			m_MapMarker.SetColorEntry(mapMarkerCustom.m_eMapMarkerColor);
		}
		
		m_MapMarker.SetWorldPos(pos[0], pos[2]);
		m_MapMarker.SetCustomText(m_MapMarkerType.m_sMapMarkerText);
		
		mapMarkerMgr.InsertStaticMarker(m_MapMarker, false, true);
	}*/