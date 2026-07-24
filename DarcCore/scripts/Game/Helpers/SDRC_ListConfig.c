//------------------------------------------------------------------------------------------------
class SDRC_List : Managed
{
	string id;							//ID/name of the list
	ref array<string> modDir = {};		//List of mods to check
	ref array<string> include = {};		//Include filter
	ref array<string> exclude = {};		//Exclude filter
	ref array<string> items = {};		//List of items found
	ref array<string> factions = {};	//Faction the item belongs to. This is NOT autofilled.

	void Set(string id_, array<string> modDir_, array<string> include_, array<string> exclude_, array<string> items_)	
	{
		id = id_;
		modDir = modDir_;
		include = include_;
		exclude = exclude_;
		items = items_;		
		//factions = factions_;
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_Aka : Managed
{
	ref array<string> names = {};	//Two entries: {"faction expected to use","actual faction name"}
}

//------------------------------------------------------------------------------------------------
class SDRC_ListConfig : SDRC_Config
{
	//Default information
	string author = "darc";
	
	//Config specific
	ref array<string> m_modList = {};
	ref array<ref SDRC_List> m_lists = {};
	ref array<ref SDRC_Aka> m_akas = {};
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//NOTE: Any variable added here will be stored in the json!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	//------------------------------------------------------------------------------------------------
	/*!
	Scan mods for items, enemies etc.
	\param fastScan True to do an immediate scan. False will use a delayed scan to avoid server load.
	*/	
	void Populate(bool fastScan = true)
	{
		SDRC_Log.Add("[SDRC_ListConfig:Populate] Creating lists..", LogLevel.NORMAL);
	
		if (m_modList.IsEmpty())
		{
			array<string> addonList = {};
			
			SDRC_Misc.GetAddonList(addonList, false);
			
			foreach (string addon : addonList)
			{
				string name = addon;
				m_modList.Insert(name);
				//SDRC_Log.Add("[SDRC_ListConfig:Populate] Mod found: " + name, LogLevel.DEBUG);				
			}
		}
		
		foreach (int idx, SDRC_List list : m_lists)
		{
			bool lastItem = false;
			if (idx == m_lists.Count() - 1)
			{
				lastItem = true;
				SDRC_Log.Add("[SDRC_ListConfig:Populate] Adding last item.", LogLevel.DEBUG);
			}
			
			if (fastScan)
			{
				DoScan(list, lastItem);
			}
			else
			{
				GetGame().GetCallqueue().CallLater(DoScan, 2000 + idx * 1000, false, list, lastItem);
			}
		}		
	}	
	
	//------------------------------------------------------------------------------------------------
	void DoScan(SDRC_List list, bool lastItem)
	{
		SDRC_Log.Add("[SDRC_ListConfig:DoScan] Scanning: " + list.id, LogLevel.DEBUG);
		
		//Add with normal add
		foreach (string mod : m_modList)
		{
			SDRC_Resources.GetList(list.items, mod, list);
		}
		
		foreach (string res : list.include)
		{
			//If the item in include is a resourcename, check if it exists and add
			if (res[0] == "{")
			{
				Resource resource = Resource.Load(res);
				if (resource.IsValid())
				{
					list.items.Insert(res);
				}					
			}				
		}
		
		//Add factions
		foreach (string item : list.items)
		{
			list.factions.Insert(SDRC_Resources.GetResourceFaction(item));
		}
			
		SDRC_Log.Add("[SDRC_ListConfig:Populate] List: " + list.id + " (" + list.items.Count() + ")", LogLevel.DEBUG);
		if (SDRC_Log.GetLogLevel() > DC_LogLevel.DEBUG)
		{
			list.items.Debug();
		}
		
		//Set scanning as done!
		if (lastItem)
		{
			SDRC_Conf.lootListScanReady = true;
			//m_bScanReady = true;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns true if all scannings are finalized.
	*/
	bool IsReady()
	{
		return true;
		//return m_bScanReady;
	}	
}