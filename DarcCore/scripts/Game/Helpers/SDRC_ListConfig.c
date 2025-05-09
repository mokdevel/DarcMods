
class SDRC_ListConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Config specific
	ref array<string> modList = {};
	ref array<ref SDRC_List> lists = {};
	
	void Populate()
	{
		SDRC_Log.Add("[SDRC_ListConfig:Populate] Creating lists..", LogLevel.NORMAL);
		
		if (modList.IsEmpty())
		{
			array<string> addonList = SCR_AddonTool.GetAllAddonFileSystems();
			foreach (string addon : addonList)
			{
				string name = addon + "Prefabs";
				modList.Insert(name);
				SDRC_Log.Add("[SDRC_ListConfig:Populate] Mod found: " + name, LogLevel.DEBUG);				
			}
		}
		
		foreach (SDRC_List list : lists)
		{
			foreach (string mod : modList)
			{
				SDRC_Resources.GetList(list.items, mod, list);
			}
			
			if (SDRC_Log.GetLogLevel() > DC_LogLevel.NORMAL)
			{
				SDRC_Log.Add("[SDRC_ListConfig:Populate] List: " + list.id, LogLevel.DEBUG);				
				list.items.Debug();
			}
		}
	}	
}

class SDRC_List : Managed
{
	string id;
	string modDir;
	ref array<string> include = {};
	ref array<string> exclude = {};
	ref array<ResourceName> items = {};

	void Set(string id_, string modDir_, array<string> include_, array<string> exclude_)	
	{
		id = id_;
		modDir = modDir_;
		include = include_;
		exclude = exclude_;
		items = {};
	}
}
