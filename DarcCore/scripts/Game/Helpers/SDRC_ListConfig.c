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
	bool m_bPrintList = true;
	bool m_bScanReady = false;
	string m_sModListHash = "";			//Signature of the loaded mod set when items[] were last scanned (used by SDRC_Conf.cacheLists)

	//Config specific
	ref array<string> m_modList = {};
	ref array<ref SDRC_List> m_lists = {};
	ref array<ref SDRC_Aka> m_akas = {};
	
	//------------------------------------------------------------------------------------------------
	/*!
	Scan mods for items, enemies etc.
	\param fastScan True to do an immediate scan. False will use a delayed scan to avoid server load.
	\param printList If the lists will be printed for debugging purposes.
	*/	
	void Populate(bool fastScan = true, bool printList = true)
	{
		SDRC_Log.Add("[SDRC_ListConfig:Populate] Creating lists..", LogLevel.NORMAL);
	
		m_bPrintList = printList;
			
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

		//Cache: if enabled and the loaded mod set is unchanged since the last scan (and items[]
		//are already present), skip the expensive folder re-scan and reuse the saved lists.
		string currentHash = BuildModListHash();
		if (SDRC_Conf.cacheLists && m_sModListHash == currentHash && HasCachedItems())
		{
			SDRC_Log.Add("[SDRC_ListConfig:Populate] Cache hit (mod set unchanged) -> skipping scan.", LogLevel.NORMAL);
			EnsureFactions();
			m_bScanReady = true;
			return;
		}
		m_sModListHash = currentHash;

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
				GetGame().GetCallqueue().CallLater(DoScan, 2000 + idx * 300, false, list, lastItem);
			}
		}		
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Cheap, order-independent signature of the loaded mod set. Used to decide whether the cached
	items[] are still valid (i.e. no mod was added/removed/updated since the last scan).
	*/
	protected string BuildModListHash()
	{
		string sig = m_modList.Count().ToString();
		foreach (string m : m_modList)
			sig = sig + "|" + m;
		return sig;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	True if at least one list already has resolved items[] (so the cache is usable).
	*/
	protected bool HasCachedItems()
	{
		foreach (SDRC_List list : m_lists)
			if (!list.items.IsEmpty())
				return true;
		return false;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	factions[] is not persisted with the config, so rebuild it from the cached items[] when a scan
	is skipped. Still far cheaper than a full folder scan.
	*/
	protected void EnsureFactions()
	{
		foreach (SDRC_List list : m_lists)
		{
			if (list.items.Count() == list.factions.Count())
				continue;

			list.factions.Clear();
			foreach (string item : list.items)
				list.factions.Insert(SDRC_Resources.GetResourceFaction(item));
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
			
		if ( (SDRC_Log.GetLogLevel() > DC_LogLevel.NORMAL) && (m_bPrintList) )
		{
			SDRC_Log.Add("[SDRC_ListConfig:Populate] List: " + list.id + " (" + list.items.Count() + ")", LogLevel.DEBUG);
			list.items.Debug();
		}
		
		//Set scanning as done!
		if (lastItem)
		{
			m_bScanReady = true;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns true if all scannings are finalized.
	*/
	bool IsReady()
	{
		return m_bScanReady;
	}	
}