//Helpers SCR_DC_Resources

//------------------------------------------------------------------------------------------------
/*!
Functions to find resources (for example cities) from game.
*/

sealed class SCR_DC_Resources
{
	private static ref array<ResourceName> m_resourceNames = {};
						
	//------------------------------------------------------------------------------------------------
	/*!
	Find slots around position. Slots are the ones where you can put depots and similar.
	\param slots Array where to store the slots entities
	\param position Middle position
	\param distance Radius to seach
	*/	
//	static int GetResources(out array<IEntity> slots, vector position, float distance = 200)
	
	static void GetItemList(out array<ResourceName> itemList, string mod, SCR_DC_LootList lootList)
	{
		SearchResourcesFilter filter = new SearchResourcesFilter;
		filter.rootPath = mod + lootList.modDir;
		filter.fileExtensions = {"et"};
		filter.searchStr = {""};
		filter.recursive = true;

		m_resourceNames = {};
		ResourceDatabase.SearchResources(filter, GetResourcesFilter);		
		itemList.InsertAll(m_resourceNames);
		
		LeaveFilter(itemList, lootList.include);
		RemoveFilter(itemList, lootList.exclude);
//		itemList.Debug();
	}
		
/*	static int GetResources()
	{
		SCR_DC_Log.Add("[SCR_DC_Resources:GetResources] Searching...", LogLevel.DEBUG);
		
		SearchResourcesFilter filter = new SearchResourcesFilter;
		filter.rootPath = "$ArmaReforger:Prefabs/Weapons";
//		filter.rootPath = "$WCS_Armaments:Prefabs/Weapons";
		filter.fileExtensions = {"et"};
		filter.searchStr = {""};
		filter.recursive = true;

		ResourceDatabase.SearchResources(filter, GetResourcesFilter);		
		
		m_resourceNames.Debug();
		LeaveFilter(m_resourceNames, {"Rifles", "Handguns"});
		RemoveFilter(m_resourceNames, {"Ammo", "_Base", "Magazines", "Tripods", "Explosives", "Attachments", "Warheads", "Grenades", "Mortars", "Aircraftweapons", "Cannons", "Core"});
		m_resourceNames.Debug();
		
		return 1;
	}	*/
	
	static void RemoveFilter(out array<ResourceName> m_resourceNames, array<string> filters)
	{
	
		foreach(string filter: filters)
		{
			filter.ToLower();
			for (int i = 0; i < m_resourceNames.Count(); i++)		
			{
				string resourceName = m_resourceNames[i];
				resourceName.ToLower();
				
				if (resourceName.Contains(filter))
				{
					m_resourceNames.Remove(i);
					i--;
				}
			}
		}
	}

	static void LeaveFilter(out array<ResourceName> m_resourceNames, array<string> filters)
	{
		for (int i = 0; i < m_resourceNames.Count(); i++)		
		{
			bool ToBeRemoved = true;
			foreach(string filter: filters)
			{
				filter.ToLower();
				string resourceName = m_resourceNames[i];
				resourceName.ToLower();
				
				if (resourceName.Contains(filter))
				{
					ToBeRemoved = false;
					break;
				}
			}
		
			if (ToBeRemoved)
			{
				m_resourceNames.Remove(i);
				i--;
			}
		}
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Call back filter for GetLocationsSlots
	*/	
	
	static private bool GetResourcesFilter(ResourceName resourceName, string exactPath = "")
	{
//		SCR_DC_Log.Add("[SCR_DC_Resources:GetResourcesFilter] Found: " + resourceName + " at " + exactPath, LogLevel.DEBUG);
		m_resourceNames.Insert(resourceName);
		return true;
	}
}