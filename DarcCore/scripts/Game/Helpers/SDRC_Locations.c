//Helpers SDRC_Locations

//------------------------------------------------------------------------------------------------
/*!
Functions to find locations (for example cities) from map.

NOTE: In order to use the caching, FillLocationsCache has to be run at startup.
*/

sealed class SDRC_Locations
{
	private static ref array<IEntity> m_aTmpSlots = {};
	private static string m_sName;
	
	private static ref array<MapItem> m_LocationsCache = {};
		
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
		#ifndef SDRC_RELEASE
			//If SCR_MapEntity does not exist, we most likely are playing in some debug map
			SCR_MapEntity mapEnt = SCR_MapEntity.GetMapInstance();
			if (!mapEnt)
			{
				return;
			}
		#endif
		
		//int stime = System.GetTickCount();
		
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
				vector origin = tmpMapItem.Entity().GetOrigin();			
				tmpMapItem.SetPos(origin[0], origin[2]);
				//tmpMapItem.Entity().SetOrigin(origin);						//Need to put the entity pos ot the mapItem pos
				locationArray.Insert(tmpMapItem.Entity());
				
				m_debugLocationArray.Insert(tmpMapItem);
			}			
		}
				
		SDRC_Log.Add("[SDRC_Locations:GetLocations] Found locations:" + locationArray.Count(), LogLevel.DEBUG);
		ShowDebugInfo(m_debugLocationArray);
		
		//int etime = System.GetTickCount();
		//SDRC_Log.Add("[SDRC_Locations:GetLocations] Searching took: " + (etime-stime), LogLevel.DEBUG);
	}
	
	//------------------------------------------------------------------------------------------------
	static void GetLocations(out array<MapItem> locationArray, array<EMapDescriptorType> locationTypeArray)
	{
		#ifndef SDRC_RELEASE
			//If SCR_MapEntity does not exist, we most likely are playing in some debug map
			SCR_MapEntity mapEnt = SCR_MapEntity.GetMapInstance();
			if (!mapEnt)
			{
				return;
			}
		#endif

		//int stime = System.GetTickCount();
				
		private array<MapItem> m_tmpLocationArray = new array<MapItem>;

		foreach (EMapDescriptorType locationType: locationTypeArray)
		{		
			SDRC_Log.Add("[SDRC_Locations:GetLocations] Searching for: " + SCR_Enum.GetEnumName(EMapDescriptorType, locationType), LogLevel.SPAM);
			
			m_tmpLocationArray.Clear();
			SCR_MapEntity.GetMapInstance().GetByType(m_tmpLocationArray, locationType);
			
			foreach (MapItem tmpMapItem: m_tmpLocationArray)
			{
				tmpMapItem.SetDisplayName(SCR_StringHelper.Translate(tmpMapItem.GetDisplayName()));
				vector origin = tmpMapItem.Entity().GetOrigin();			
				tmpMapItem.SetPos(origin[0], origin[2]);
				//tmpMapItem.Entity().SetOrigin(origin);						//Need to put the entity pos ot the mapItem pos
				locationArray.Insert(tmpMapItem);
			}			
		}
		
		SDRC_Log.Add("[SDRC_Locations:GetLocations] Found locations:" + locationArray.Count(), LogLevel.DEBUG);
		ShowDebugInfo(locationArray);
		
		//int etime = System.GetTickCount();
		//SDRC_Log.Add("[SDRC_Locations:GetLocations] Searching took: " + (etime-stime), LogLevel.DEBUG);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Helper function that just prints the information for debugging purposes.
	*/	
	static private void ShowDebugInfo(array<MapItem> m_tmpLocationArray)
	{
		array<IEntity> slots = {};

		if (SDRC_Log.GetLogLevel() == DC_LogLevel.ALL)
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
				
				#ifndef SDRC_RELEASE
					if (SDRC_Conf.SHOW_MARKER_FOR_LOCATION)
					{
						if (location) 
						{
							SDRC_DebugHelper.AddDebugPos(location);
						}
					}
				#endif
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Prepare an array with all locations on the map.
	*/		
	static void FillLocationsCache()
	{
		#ifndef SDRC_RELEASE
			//If SCR_MapEntity does not exist, we most likely are playing in some debug map
			SCR_MapEntity mapEnt = SCR_MapEntity.GetMapInstance();
			if (!mapEnt)
			{
				return;
			}
		#endif		
		
		SDRC_Log.Add("[SDRC_Locations:FillLocationsCache] Searching..", LogLevel.NORMAL);			
		
		m_LocationsCache.Clear();
		GetLocations(m_LocationsCache, m_LocationTypeArray);
		
		//Print debug information
		#ifndef SDRC_RELEASE
			foreach (MapItem location: m_LocationsCache)
			{
				SDRC_Log.Add("[SDRC_Locations:FillLocationsCache] Found: " + location.GetDisplayName() + " : " + SCR_Enum.GetEnumName(EMapDescriptorType, location.GetBaseType()) + " at: " + location.Entity().GetOrigin(), LogLevel.DEBUG);
			}		
		#endif
		
		SDRC_Log.Add("[SDRC_Locations:FillLocationsCache] Found " + m_LocationsCache.Count() + " items to location cache.", LogLevel.NORMAL);					
	}	

	//-----------------------------------------------------------------------------------------------
	/*!
	Search for locations from the world using the cache. The types to search are defined in locationTypeArray.
	*/		
	static void GetLocationsCached(out array<IEntity> locationArray, array<EMapDescriptorType> locationTypeArray)
	{
		//int stime = System.GetTickCount();
		
		foreach (MapItem location: m_LocationsCache)
		{
			if (locationTypeArray.Contains(location.GetBaseType()))
			{
				locationArray.Insert(location.Entity());
				SDRC_Log.Add("[SDRC_Locations:GetLocationsCached] Found: " + location.GetDisplayName() + " : " + SCR_Enum.GetEnumName(EMapDescriptorType, location.GetBaseType()), LogLevel.SPAM);
			}
		}		
		
		//int etime = System.GetTickCount();
		//SDRC_Log.Add("[SDRC_Locations:GetLocationsCached] Searching took: " + (etime-stime), LogLevel.DEBUG);		
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
			array<int>distancesToTry = {10, 20, 30, 50, 100, 150, 300, 400, 500, 600};
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
	
	//------------------------------------------------------------------------------------------------
	private static ref array<EMapDescriptorType>m_LocationTypeArray =
	{
		//EMapDescriptorType.MDT_TREE,
		//EMapDescriptorType.MDT_SMALLTREE,
		//EMapDescriptorType.MDT_BUSH,
		EMapDescriptorType.MDT_BUILDING,
		EMapDescriptorType.MDT_HOUSE,
		EMapDescriptorType.MDT_FORESTERLODGE,
		//EMapDescriptorType.MDT_FORESTBORDER,
		EMapDescriptorType.MDT_FORESTTRIANGLE,
		EMapDescriptorType.MDT_FORESTSQUARE,
		EMapDescriptorType.MDT_CALVARY,
		EMapDescriptorType.MDT_CHURCH,
		EMapDescriptorType.MDT_CHAPEL,
		//EMapDescriptorType.MDT_CROSS,
		//EMapDescriptorType.MDT_ROCK,
		EMapDescriptorType.MDT_BUNKER,
		EMapDescriptorType.MDT_FORTRESS,
		EMapDescriptorType.MDT_FOUNTAIN,
		//EMapDescriptorType.MDT_SPRING,
		EMapDescriptorType.MDT_VIEWPOINT,
		EMapDescriptorType.MDT_TOWER,
		EMapDescriptorType.MDT_VIEWTOWER,
		EMapDescriptorType.MDT_WATERTOWER,
		EMapDescriptorType.MDT_LIGHTHOUSE,
		//EMapDescriptorType.MDT_QUAY,
		//EMapDescriptorType.MDT_BUOY,
		EMapDescriptorType.MDT_FUELSTATION,
		EMapDescriptorType.MDT_HOSPITAL,
		//EMapDescriptorType.MDT_LIGHT,
		//EMapDescriptorType.MDT_FENCE,
		//EMapDescriptorType.MDT_WALL,
		EMapDescriptorType.MDT_HIDE,
		EMapDescriptorType.MDT_BUSSTOP,
		EMapDescriptorType.MDT_BUSSTATION,
		EMapDescriptorType.MDT_ROAD,
		//EMapDescriptorType.MDT_FOREST,
		EMapDescriptorType.MDT_CRANE,
		EMapDescriptorType.MDT_TRANSFORMER,
		EMapDescriptorType.MDT_TRANSMITTER,
		//EMapDescriptorType.MDT_STACK,
		EMapDescriptorType.MDT_RUIN,
		EMapDescriptorType.MDT_TOURISM,
		EMapDescriptorType.MDT_HILL,
		//EMapDescriptorType.MDT_TRACK,
		//EMapDescriptorType.MDT_MAINROAD,
		//EMapDescriptorType.MDT_ROCKS,
		EMapDescriptorType.MDT_PLAYINGFIELD,
		//EMapDescriptorType.MDT_POWERLINES,
		EMapDescriptorType.MDT_RAILWAY,
		EMapDescriptorType.MDT_SHIPWRECK,
		EMapDescriptorType.MDT_TOURISTSHELTER,
		//EMapDescriptorType.MDT_TOURISTSIGN,
		EMapDescriptorType.MDT_MONUMENT,
		//EMapDescriptorType.MDT_WATERPUMP,
		EMapDescriptorType.MDT_POLICE,
		EMapDescriptorType.MDT_STORE,
		EMapDescriptorType.MDT_HOTEL,
		EMapDescriptorType.MDT_PUB,
		EMapDescriptorType.MDT_FIREDEP,
		EMapDescriptorType.MDT_NAME_GENERIC,
		EMapDescriptorType.MDT_NAME_CITY,
		EMapDescriptorType.MDT_NAME_VILLAGE,
		EMapDescriptorType.MDT_NAME_TOWN,
		EMapDescriptorType.MDT_NAME_SETTLEMENT,
		EMapDescriptorType.MDT_NAME_HILL,
		EMapDescriptorType.MDT_NAME_LOCAL,
		EMapDescriptorType.MDT_NAME_ISLAND,
		//EMapDescriptorType.MDT_NAME_WATER_MINOR,
		//EMapDescriptorType.MDT_NAME_WATER_MAJOR,
		//EMapDescriptorType.MDT_NAME_SEA_MINOR,
		//EMapDescriptorType.MDT_NAME_SEA_MAJOR,
		EMapDescriptorType.MDT_NAME_RIDGE,
		EMapDescriptorType.MDT_NAME_VALLEY,
		EMapDescriptorType.MDT_PARKING,
		//EMapDescriptorType.MDT_UNIT,
		//EMapDescriptorType.MDT_WILDLIFE,
		EMapDescriptorType.MDT_CONSTRUCTION_SITE,
		//EMapDescriptorType.MDT_CURPOS,
		//EMapDescriptorType.MDT_WAYPOINT,
		//EMapDescriptorType.MDT_TARGET,
		EMapDescriptorType.MDT_BASE,
		EMapDescriptorType.MDT_PORT,
		EMapDescriptorType.MDT_AIRPORT,
		EMapDescriptorType.MDT_LANDMARK,
		EMapDescriptorType.MDT_CAVE,
		//EMapDescriptorType.MDT_RADIO,
		//EMapDescriptorType.MDT_SPAWNPOINT,
		//EMapDescriptorType.MDT_TASK,
		//EMapDescriptorType.MDT_ICON,
		//! remaining types are debug/ functional
		//EMapDescriptorType.MDT_IMAGE_COUNT,
		//EMapDescriptorType.MDT_DEBUG_SELECTED,
		//EMapDescriptorType.MDT_DEBUG_HOVER,
		//EMapDescriptorType.MDT_DEBUG_HIGHLIGHT,
		//! remaining types are not colorized by faction
		//EMapDescriptorType.MDT_COLORIZE_COUNT,
		//EMapDescriptorType.MDT_DEBUG_POINTS,
		//EMapDescriptorType.MDT_COUNT,
	}
}