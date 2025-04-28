//Helpers SDRC_Locations

//------------------------------------------------------------------------------------------------
/*!
Functions to find locations (for example cities) from map.
*/

sealed class SDRC_Locations
{
	private static ref array<IEntity> m_aTmpSlots = {};
	private static string m_sName;
	
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
			
		SDRC_Locations.GetLocations(m_Locations, locationTypeArray);
	\endcode
	*/	
	static void GetLocations(out array<IEntity> locationArray, array<EMapDescriptorType> locationTypeArray)
	{
		private array<MapItem> m_tmpLocationArray = new array<MapItem>;
		private array<MapItem> m_debugLocationArray = new array<MapItem>;

		foreach (EMapDescriptorType locationType: locationTypeArray)
		{
			SDRC_Log.Add("[SDRC_Locations:GetLocations] Searching for: " + SCR_Enum.GetEnumName(EMapDescriptorType, locationType), LogLevel.SPAM);
			
			m_tmpLocationArray.Clear();
			SCR_MapEntity.GetMapInstance().GetByType(m_tmpLocationArray, locationType);
			
			foreach (MapItem tmpMapItem: m_tmpLocationArray)
			{
				tmpMapItem.SetDisplayName(SCR_StringHelper.Translate(tmpMapItem.GetDisplayName()));
				tmpMapItem.Entity().SetName(tmpMapItem.GetDisplayName());
				vector origin = tmpMapItem.Entity().GetOrigin();			//TBD: MapItem position on DS is "0 0 0". This is a bug in 1.3.0?
				tmpMapItem.SetPos(origin[0], origin[2]);
				//tmpMapItem.Entity().SetOrigin(origin);						//Need to put the entity pos ot the mapItem pos
				locationArray.Insert(tmpMapItem.Entity());
				
				m_debugLocationArray.Insert(tmpMapItem);
			}			
		}
				
		SDRC_Log.Add("[SDRC_Locations:GetLocations] Found locations:" + locationArray.Count(), LogLevel.DEBUG);
		ShowDebugInfo(m_debugLocationArray);
	}
	
	//------------------------------------------------------------------------------------------------
	static void GetLocations(out array<MapItem> locationArray, array<EMapDescriptorType> locationTypeArray)
	{
		private array<MapItem> m_tmpLocationArray = new array<MapItem>;

		foreach (EMapDescriptorType locationType: locationTypeArray)
		{		
			SDRC_Log.Add("[SDRC_Locations:GetLocations] Searching for: " + SCR_Enum.GetEnumName(EMapDescriptorType, locationType), LogLevel.SPAM);
			
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
		
		SDRC_Log.Add("[SDRC_Locations:GetLocations] Found locations:" + locationArray.Count(), LogLevel.DEBUG);
		ShowDebugInfo(locationArray);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Helper function that just prints the information for debugging purposes.
	*/	
	static private void ShowDebugInfo(array<MapItem> m_tmpLocationArray)
	{
		array<IEntity> slots = {};

		if (SDRC_Log.GetLogLevel() > DC_LogLevel.NORMAL)
		{		
/*			SDRC_Log.Add( string.Format("[SDRC_Locations:ShowDebugInfo] Found %1 locations of type (%2) %3", 
				m_tmpLocationArray.Count(),
				locationType,
			 	SCR_Enum.GetEnumName(EMapDescriptorType, locationType),
				), LogLevel.DEBUG);*/
	
			foreach (MapItem location: m_tmpLocationArray)
			{	
				IEntity entity = location.Entity();
				SDRC_Log.Add( string.Format("[SDRC_Locations:ShowDebugInfo] Name: %1 , DisplayName: %2 , CreatedName: %3, Type: %4 , Pos: %5 , Entity: %6", 
					location.Entity().GetName(),
					location.GetDisplayName(),
//					CreateName(entity),
					CreateName(location.GetPos()),
					location.GetBaseType(),
					location.GetPos(),
					entity
					), LogLevel.SPAM);

				slots.Clear();
				int slotcount = GetLocationSlots(slots, location.GetPos(), 200);
				
				SDRC_Log.Add( string.Format("[SDRC_Locations:ShowDebugInfo] Found %1 slots.", 
					slotcount,
					), LogLevel.SPAM);
				
				if (SDRC_Conf.SHOW_MARKER_FOR_LOCATION)
				{
					if (location) 
					{
						SDRC_DebugHelper.AddDebugPos(location);
					}
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Creates a name for a location or a position.
	The search for name is done in distance steps. The first fitting name is used.
	\param location/pos Either IEntity or position.
	\param nameDefault The default name to use. "any" will search for one, empty name will result in [REDACTED]
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
			array<int>distancesToTry = {10, 20, 30, 50, 100, 150, 300, 400};
			foreach (int distance : distancesToTry)
			{
				name = SDRC_Locations.GetNameCloseToPos(pos, distance);
				if (name != "")
				{
					break;
				}				
			}			
		}
		else
		{
			name = nameDefault;
		}
		
		if (name == "")
		{
			name = "[REDACTED]";
		}		
		
		return name;		
	}	
		
	//------------------------------------------------------------------------------------------------
	/*!
	Find a name close to a position. 
	Query for entities with SCR_MapDescriptorComponent and pick the first one found. 
	*/	
	static string GetNameCloseToPos(vector pos, int distance = 300)
	{
		m_sName = "";
		
		GetGame().GetWorld().QueryEntitiesBySphere(pos, distance, GetNameCloseToPosCallBack);
		
		return m_sName;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Call back filter for GetNameCloseToPos
	*/	
	static private bool GetNameCloseToPosCallBack(IEntity entity)
	{
	    SCR_MapDescriptorComponent mapDescr = SCR_MapDescriptorComponent.Cast(entity.FindComponent(SCR_MapDescriptorComponent));
		
		if (mapDescr)
		{
			MapItem mapItem = mapDescr.Item();
			if ( (mapItem.GetDisplayName() != "") && (SCR_StringHelper.Translate(mapItem.GetDisplayName()) != "") )
			{
				m_sName = SCR_StringHelper.Translate(mapItem.GetDisplayName());
				return false;			
			}
		}
		return true;
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
		m_aTmpSlots.Clear();
		GetGame().GetWorld().QueryEntitiesBySphere(position, 200, SDRC_Locations.GetLocationSlotsCallBack);
		
		foreach (IEntity slot: m_aTmpSlots)
		{
			slots.Insert(slot);
		}
		
		return slots.Count();				
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Call back filter for GetLocationsSlots
	*/	
	static private bool GetLocationSlotsCallBack(IEntity entity)
	{
		if (entity.Type() == SCR_SiteSlotEntity)
		{
			m_aTmpSlots.Insert(entity);
		}
		return true;
	}
}