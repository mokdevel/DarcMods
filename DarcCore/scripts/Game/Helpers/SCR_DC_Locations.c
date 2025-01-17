//Helpers SCR_DC_Locations

//------------------------------------------------------------------------------------------------
/*!
Functions to find locations (for example cities) from map.
*/

sealed class SCR_DC_Locations
{
	static ref array<IEntity> m_TmpSlots = {};
	
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

		foreach (EMapDescriptorType locationType: locationTypeArray)
		{
			SCR_DC_Log.Add("[SCR_DC_Locations:GetLocations] Searching for: " + SCR_Enum.GetEnumName(EMapDescriptorType, locationType), LogLevel.SPAM);
			
			m_tmpLocationArray.Clear();
			SCR_MapEntity.GetMapInstance().GetByType(m_tmpLocationArray, locationType);
			
			foreach (MapItem tmpMapItem: m_tmpLocationArray)
			{
				tmpMapItem.Entity().SetName(tmpMapItem.GetDisplayName());
				locationArray.Insert(tmpMapItem.Entity());
			}			
			
			ShowDebugInfo(m_tmpLocationArray, locationType);			
		}
		
		SCR_DC_Log.Add("[SCR_DC_Locations:GetLocations] Found locations:" + locationArray.Count(), LogLevel.SPAM);
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
				locationArray.Insert(tmpMapItem);
			}			

			ShowDebugInfo(m_tmpLocationArray, locationType);
		}
		
		SCR_DC_Log.Add("[SCR_DC_Locations:GetLocations] Found locations:" + locationArray.Count(), LogLevel.SPAM);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Helper function that just prints the information for debugging purposes.
	*/	
	static private void ShowDebugInfo(array<MapItem> m_tmpLocationArray, EMapDescriptorType locationType)
	{
		array<IEntity> slots = {};

		if (SCR_DC_Log.GetLogLevel() != DC_LogLevel.NONE)
		{		
			SCR_DC_Log.Add( string.Format("[SCR_DC_Locations:GetLocations] Found %1 locations of type (%2) %3", 
				m_tmpLocationArray.Count(),
				locationType,
			 	SCR_Enum.GetEnumName(EMapDescriptorType, locationType),
				), LogLevel.DEBUG);
	
			foreach (MapItem location: m_tmpLocationArray)
			{	
				SCR_DC_Log.Add( string.Format("[SCR_DC_Locations:GetLocations] Name: %1 , Entity: %2 , Type: %3 , GetDisplayName: %4 , Pos: %5", 
					location.Entity().GetName(),
					location.Entity(),
					location.Type(),
					location.GetDisplayName(),
					location.GetPos()
					), LogLevel.SPAM);

				slots.Clear();
				int slotcount = GetLocationSlots(slots, location.GetPos(), 200);
				
				SCR_DC_Log.Add( string.Format("[SCR_DC_Locations:GetLocationsShowDebug] Found %1 slots.", 
					slotcount,
					), LogLevel.SPAM);
			}
		}
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