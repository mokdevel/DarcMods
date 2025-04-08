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
	Get items for the lootlist
	\param itemList The itemList to fill with loot items
	\param mod From which mod to search for items. Example: "$ArmaReforger:Prefabs/Weapons"
	\param lootList 
	*/	
	static void GetItemList(out array<ResourceName> itemList, string mod, SCR_DC_LootList lootList)
	{
		SearchResourcesFilter filter = new SearchResourcesFilter;
		filter.rootPath = mod + lootList.modDir;
		filter.fileExtensions = {"et"};
		filter.searchStr = {""};
		filter.recursive = true;

		m_resourceNames = {};
		ResourceDatabase.SearchResources(filter, GetResourcesFilter);		
		
		//TBD: Remove the old way
//		itemList.InsertAll(m_resourceNames);
//		IncludeFilter(itemList, lootList.include);
//		ExcludeFilter(itemList, lootList.exclude);
		
		IncludeFilter(m_resourceNames, lootList.include);
		ExcludeFilter(m_resourceNames, lootList.exclude);
		
		itemList.InsertAll(m_resourceNames);
		
//		itemList.Debug();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Remove the items from array that match the filters
	*/	
	static void ExcludeFilter(out array<ResourceName> m_resourceNames, array<string> filters)
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

	//------------------------------------------------------------------------------------------------
	/*!
	Include only the items in the array that match the filters. Remove others.
	*/	
	static void IncludeFilter(out array<ResourceName> m_resourceNames, array<string> filters)
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
	Call back filter for ResourceDatabase.SearchResources
	*/		
	static private bool GetResourcesFilter(ResourceName resourceName, string exactPath = "")
	{
//		SCR_DC_Log.Add("[SCR_DC_Resources:GetResourcesFilter] Found: " + resourceName + " at " + exactPath, LogLevel.DEBUG);
		m_resourceNames.Insert(resourceName);
		return true;
	}
}