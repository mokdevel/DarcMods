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
	ref array<string> modList = {};
	ref array<ref SDRC_List> lists = {};
	ref array<ref SDRC_Aka> akas = {};
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//NOTE: Any variable added here will be stored in the json!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	//------------------------------------------------------------------------------------------------
	/*!
	Scan mods for items, enemies etc.
	\param index The index of the 
	*/	
	bool Populate(int index, bool doPrint = true)
	{
		SDRC_Log.Add("[SDRC_ListConfig:Populate] Creating lists..", LogLevel.SPAM);
	
		if (modList.IsEmpty())
		{
			array<string> addonList = {};
			
			SDRC_Misc.GetAddonList(addonList, false);
			
			foreach (string addon : addonList)
			{
				string name = addon;
				modList.Insert(name);
				//SDRC_Log.Add("[SDRC_ListConfig:Populate] Mod found: " + name, LogLevel.DEBUG);				
			}
		}

		if (index > lists.Count())	//NOTE: This will go to the last index and not to -1!
		{
			SDRC_Log.Add("[SDRC_ListConfig:Populate] Wrong index number: " + index + ". Stopping!", LogLevel.ERROR);
			return true;
		}
				
		if (index == lists.Count())	//NOTE: This will go to the last index and not to -1!
		{
			SDRC_Log.Add("[SDRC_ListConfig:Populate] All scanned.", LogLevel.DEBUG);
			return true;
		}
		
		SDRC_List list = lists[index];
		DoScan(list, doPrint);
		
		return false;
	}	
	
	//------------------------------------------------------------------------------------------------
	void DoScan(SDRC_List list, bool doPrint = true)
	{
		SDRC_Log.Add("[SDRC_ListConfig:DoScan] Scanning: " + list.id, LogLevel.DEBUG);
		
		//Add with normal add
		foreach (string mod : modList)
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
	
		//Print a normal list report
		SDRC_Log.Add("[SDRC_ListConfig:Populate] List: " + list.id + " (" + list.items.Count() + ")", LogLevel.DEBUG);
		
		//Print a detailed list of items when debug level is ALL
		if ( (SDRC_Log.GetLogLevel() > DC_LogLevel.NORMAL) && (doPrint) )
		{
			list.items.Debug();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Returns true if all scannings are finalized.
	*/
/*	bool IsReady()
	{
		return true;
		//return m_bScanReady;
	}	*/
}