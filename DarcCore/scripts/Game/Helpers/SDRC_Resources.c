//Helpers SDRC_Resources

//------------------------------------------------------------------------------------------------
/*!
Functions to find resources (for example cities) from game.
*/

sealed class SDRC_Resources
{
	private static ref array<ResourceName> m_resourceNames = {};
						
	//------------------------------------------------------------------------------------------------
	/*!
	Get items for the lootlist
	\param itemList The itemList to fill with loot items
	\param mod From which mod to search for items. Example: "$ArmaReforger:Prefabs/Weapons"
	\param lootList 
	*/	
	static void GetItemList(out array<ResourceName> itemList, string mod, SDRC_LootList lootList)
	{
		SearchResourcesFilter filter = new SearchResourcesFilter();
		filter.rootPath = mod + lootList.modDir;
		filter.fileExtensions = {"et"};
		filter.searchStr = {""};
		filter.recursive = true;

		m_resourceNames = {};
		ResourceDatabase.SearchResources(filter, GetResourcesCallback);		
		
		IncludeFilter(m_resourceNames, lootList.include);
		ExcludeFilter(m_resourceNames, lootList.exclude);
		
		itemList.InsertAll(m_resourceNames);
		
//		itemList.Debug();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Remove the items from array that match the filters
	*/	
	static void ExcludeFilter(out array<ResourceName> resourceNames, array<string> filters)
	{	
		foreach (string filter: filters)
		{
			filter.ToLower();
			for (int i = 0; i < resourceNames.Count(); i++)		
			{
				string resourceName = resourceNames[i];
				resourceName.ToLower();
				
				if (resourceName.Contains(filter))
				{
					resourceNames.Remove(i);
					i--;
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Include only the items in the array that match the filters. Remove others.
	*/	
	static void IncludeFilter(out array<ResourceName> resourceNames, array<string> filters)
	{
		for (int i = 0; i < resourceNames.Count(); i++)		
		{
			bool ToBeRemoved = true;
			foreach (string filter: filters)
			{
				filter.ToLower();
				string resourceName = resourceNames[i];
				resourceName.ToLower();
				
				if (resourceName.Contains(filter))
				{
					ToBeRemoved = false;
					break;
				}
			}
		
			if (ToBeRemoved)
			{
				resourceNames.Remove(i);
				i--;
			}
		}
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Call back filter for ResourceDatabase.SearchResources
	*/		
	static private bool GetResourcesCallback(ResourceName resourceName, string exactPath = "")
	{
		SDRC_Log.Add("[SDRC_Resources:GetResourcesFilter] Found: " + resourceName + " at " + exactPath, LogLevel.SPAM);
		m_resourceNames.Insert(resourceName);
		return true;
	}
}