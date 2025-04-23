//Helpers SCR_DC_Locations

//------------------------------------------------------------------------------------------------
/*!
Functions to find locations (for example cities) from map.
*/

sealed class SCR_DC_Locations
{
	private static ref array<IEntity> m_TmpSlots = {};
	private static ref array<MapItem> m_TmpMapItems = {};
	
	//-----------------------------------------------------------------------------------------------
	/*!
	Search for locations from the world. The types to search are defined in locationTypeArray.
	\param locationArray Array to return the list of locations. Can be either MapItem or IEntity.
	\param locationTypeArray Array of EMapDescriptorType

	Example:
	\code
		private array<MapItem> m_Locations = {};
		private array<IEntity> m_Locations = {};
		private array<EMapDescriptorType> locationTypeArray = {
			EMapDescriptorType.MDT_NAME_TOWN, 
			EMapDescriptorType.MDT_NAME_CITY
		};
			
		SCR_DC_Locations.GetLocations(m_Locations, locationTypeArray);
	\endcode
	*/	
	static void GetLocations(out array<IEntity> locationArray, array<EMapDescriptorType> locationTypeArray)
	{
		private array<MapItem> m_tmpLocationArray = new array<MapItem>;
		private array<MapItem> m_debugLocationArray = new array<MapItem>;

		foreach (EMapDescriptorType locationType: locationTypeArray)
		{
			SCR_DC_Log.Add("[SCR_DC_Locations:GetLocations] Searching for: " + SCR_Enum.GetEnumName(EMapDescriptorType, locationType), LogLevel.SPAM);
			
			m_tmpLocationArray.Clear();
			SCR_MapEntity.GetMapInstance().GetByType(m_tmpLocationArray, locationType);
			
			foreach (MapItem tmpMapItem: m_tmpLocationArray)
			{
				tmpMapItem.SetDisplayName(SCR_StringHelper.Translate(tmpMapItem.GetDisplayName()));
				tmpMapItem.Entity().SetName(tmpMapItem.GetDisplayName());
				vector origin = tmpMapItem.Entity().GetOrigin();			//TBD: MapItem position on DS is "0 0 0". This is a bug in 1.3.0?
				tmpMapItem.SetPos(origin[0], origin[2]);
				tmpMapItem.Entity().SetOrigin(origin);						//Need to put the entity pos ot the mapItem pos
				locationArray.Insert(tmpMapItem.Entity());
				
				m_debugLocationArray.Insert(tmpMapItem);
			}			
		}
				
		SCR_DC_Log.Add("[SCR_DC_Locations:GetLocations] Found locations:" + locationArray.Count(), LogLevel.DEBUG);
//		#ifndef SCR_DC_RELEASE
			ShowDebugInfo(m_debugLocationArray);
//		#endif
	}
	
	//------------------------------------------------------------------------------------------------
	static void GetLocations(out array<MapItem> locationArray, array<EMapDescriptorType> locationTypeArray)
	{
		private array<MapItem> m_tmpLocationArray = new array<MapItem>;

		foreach (EMapDescriptorType locationType: locationTypeArray)
		{		
			SCR_DC_Log.Add("[SCR_DC_Locations:GetLocations] Searching for: " + SCR_Enum.GetEnumName(EMapDescriptorType, locationType), LogLevel.SPAM);
			
			m_tmpLocationArray.Clear();
			SCR_MapEntity.GetMapInstance().GetByType(m_tmpLocationArray, locationType);
			
			foreach (MapItem tmpMapItem: m_tmpLocationArray)
			{
				tmpMapItem.SetDisplayName(SCR_StringHelper.Translate(tmpMapItem.GetDisplayName()));
				vector origin = tmpMapItem.Entity().GetOrigin();			//TBD: MapItem position on DS is "0 0 0". This is a bug in 1.3.0?
				tmpMapItem.SetPos(origin[0], origin[2]);
				//tmpMapItem.Entity().SetOrigin(origin);						//Need to put the entity pos ot the mapItem pos
				locationArray.Insert(tmpMapItem);
			}			
		}
		
		SCR_DC_Log.Add("[SCR_DC_Locations:GetLocations] Found locations:" + locationArray.Count(), LogLevel.DEBUG);
//		#ifndef SCR_DC_RELEASE
			ShowDebugInfo(locationArray);
//		#endif
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Helper function that just prints the information for debugging purposes.
	*/	
	static private void ShowDebugInfo(array<MapItem> m_tmpLocationArray)
	{
		array<IEntity> slots = {};

		if (SCR_DC_Log.GetLogLevel() > DC_LogLevel.NORMAL)
		{		
/*			SCR_DC_Log.Add( string.Format("[SCR_DC_Locations:ShowDebugInfo] Found %1 locations of type (%2) %3", 
				m_tmpLocationArray.Count(),
				locationType,
			 	SCR_Enum.GetEnumName(EMapDescriptorType, locationType),
				), LogLevel.DEBUG);*/
	
			foreach (MapItem location: m_tmpLocationArray)
			{	
				IEntity entity = location.Entity();
				SCR_DC_Log.Add( string.Format("[SCR_DC_Locations:ShowDebugInfo] Name: %1 , DisplayName: %2 , CreatedName: %3, Type: %4 , Pos: %5 , Entity: %6", 
					location.Entity().GetName(),
					location.GetDisplayName(),
//					CreateName(entity),
					CreateName(location.GetPos()),
					location.GetBaseType(),
					location.GetPos(),
					entity
					), LogLevel.DEBUG);

				slots.Clear();
				int slotcount = GetLocationSlots(slots, location.GetPos(), 200);
				
				SCR_DC_Log.Add( string.Format("[SCR_DC_Locations:ShowDebugInfo] Found %1 slots.", 
					slotcount,
					), LogLevel.SPAM);
				
				if(SCR_DC_Conf.SHOW_MARKER_FOR_LOCATION)
				{
					if (location) 
					{
						SCR_DC_DebugHelper.AddDebugPos(location);
					}
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	static string GetNameCloseToPos(vector pos, int distance = 300)
	{
//		array<MapItem> mapItems = {};
//		array<IEntity> mapEntities = {};
		string name = "";
		
		pos[1] = SCR_TerrainHelper.GetTerrainY(pos);
		m_TmpMapItems.Clear();
		
//		SCR_MapEntity.GetMapInstance().GetInsideCircle(mapItems, pos, distance);
		GetGame().GetWorld().QueryEntitiesBySphere(pos, distance, GetMapItemsCallBack);
		
		SCR_DC_Log.Add("[SCR_DC_Locations:GetNameCloseToPos] Start.. " + pos + " mapitems: " + m_TmpMapItems.Count(), LogLevel.SPAM);		
		foreach (MapItem mapItem: m_TmpMapItems)
		{
			string tmpName = SCR_StringHelper.Translate(mapItem.GetDisplayName());
			if (tmpName != "")
			{
				name = tmpName;
				SCR_DC_Log.Add("[SCR_DC_Locations:GetNameCloseToPos] Found name: " + tmpName + " " + SCR_Enum.GetEnumName(EMapDescriptorType, mapItem.GetBaseType()) + " at " + pos, LogLevel.DEBUG);
				break;
			}
			else
			{
				SCR_DC_Log.Add("[SCR_DC_Locations:GetNameCloseToPos] Found name: EMPTY", LogLevel.SPAM);
			}
		}			
		
/*		foreach (MapItem mapItem: mapItems)
		{
			string tmpName = SCR_StringHelper.Translate(mapItem.GetDisplayName());
			if (tmpName != "")
			{
				name = tmpName;
				SCR_DC_Log.Add("[SCR_DC_Locations:GetNameCloseToPos] Found name: " + tmpName + " " + SCR_Enum.GetEnumName(EMapDescriptorType, mapItem.GetBaseType()) + " at " + pos, LogLevel.DEBUG);
				break;
			}
			else
			{
				SCR_DC_Log.Add("[SCR_DC_Locations:GetNameCloseToPos] Found name: EMPTY", LogLevel.SPAM);
			}
		}			*/
		
		return name;	
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Call back filter for GetMapItems
	*/	
	static private bool GetMapItemsCallBack(IEntity entity)
	{
	    SCR_MapDescriptorComponent mapDescr = SCR_MapDescriptorComponent.Cast(entity.FindComponent(SCR_MapDescriptorComponent));
		
		if (mapDescr)
		{
			MapItem mapItem = mapDescr.Item();
			m_TmpMapItems.Insert(mapItem);
		}
		return true;
	}
	
		
	//------------------------------------------------------------------------------------------------
	/*!
	Creates a name for a location or a position
	*/	
	static string CreateName(IEntity location, string nameDefault = "any")
	{
		string name = CreateName(location.GetOrigin(), nameDefault);
		
		return name;		
	}

	//----------------------------------------------------------
	static string CreateName(vector pos, string nameDefault = "any")
	{
		string name;
		
		if (nameDefault == "any")
		{	
			array<int>distancesToTry = {50, 150, 300, 400};
			foreach(int distance : distancesToTry)
			{
				name = SCR_DC_Locations.GetNameCloseToPos(pos, distance);
				if (name != "")
				{
					break;
				}				
			}
			
			if (name == "")
			{
				name = "[REDACTED]";
			}		
		}
		else
		{
			name = nameDefault;
		}
		
		return name;		
	}	
						
	//------------------------------------------------------------------------------------------------
	/*!
	Find slots around position. Slots are the ones where you can put depots and similar.
	\param slots Array where to store the slots entities
	\param position Middle position
	\param distance Radius to seach
	*/	
	static int GetLocationSlots(out array<IEntity> slots, vector position, float distance = 200)
	{
		m_TmpSlots.Clear();
		GetGame().GetWorld().QueryEntitiesBySphere(position, 200, SCR_DC_Locations.GetLocationsFilterSlot);
		
		foreach(IEntity slot: m_TmpSlots)
		{
			slots.Insert(slot);
		}
		
		return slots.Count();				
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Call back filter for GetLocationsSlots
	*/	
	static private bool GetLocationsFilterSlot(IEntity ent)
	{
		if (ent.Type() == SCR_SiteSlotEntity)
		{
			m_TmpSlots.Insert(ent);
		}
		return true;
	}
}