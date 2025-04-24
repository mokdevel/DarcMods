//Helpers SCR_DC_LootListJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
// Types defined by default
/*
	WEAPON_RIFLE,
	WEAPON_HANDGUN,
	WEAPON_LAUNCHER,
	WEAPON_GRENADE,
	WEAPON_ATTACHMENT,
	WEAPON_OPTICS,
	ITEM_MEDICAL,
	ITEM_GENERAL
*/

class SCR_DC_LootListConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Config specific
	ref array<string> modList = {};
	ref array<ref SCR_DC_LootList> lootLists = {};
	
	void Populate()
	{
		SCR_DC_Log.Add("[SCR_DC_LootListConfig:Populate] Creating loot lists..", LogLevel.NORMAL);
		
		if (modList.IsEmpty())
		{
			array<string> addonList = SCR_AddonTool.GetAllAddonFileSystems();
			foreach (string addon : addonList)
			{
				string name = addon + "Prefabs";
				modList.Insert(name);
				SCR_DC_Log.Add("[SCR_DC_LootListConfig:Populate] Mod found: " + name, LogLevel.DEBUG);				
			}
		}
		
		foreach(SCR_DC_LootList lootList : lootLists)
		{
			foreach(string mod : modList)
			{
				SCR_DC_Resources.GetItemList(lootList.itemList, mod, lootList);
			}
			
			if (SCR_DC_Log.GetLogLevel() > DC_LogLevel.NORMAL)
			{
				lootList.itemList.Debug();
			}
		}
	}	
}

class SCR_DC_LootList : Managed
{
	string lootListName;
	string modDir;
	ref array<string> include = {};
	ref array<string> exclude = {};
	ref array<ResourceName> itemList = {};

	void Set(string lootListName_, string modDir_, array<string> include_, array<string> exclude_)	
	{
		lootListName = lootListName_;
		modDir = modDir_;
		include = include_;
		exclude = exclude_;
		itemList = {};
	}
}

//------------------------------------------------------------------------------------------------
class SCR_DC_LootListJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_lootList.json";
		
	ref SCR_DC_LootListConfig conf = new SCR_DC_LootListConfig;

	//------------------------------------------------------------------------------------------------
	void SCR_DC_LootListJsonApi()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_MISSIONCONFIG_FILE);
		if(!loadContext)
		{
			SetDefaults();
			Save("");
			return;
		}
		
		loadContext.ReadValue("", conf);
	}	

	//------------------------------------------------------------------------------------------------
	void Save(string data)
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen(DC_MISSIONCONFIG_FILE);
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}	
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		
		conf.modList = {};
		/*conf.modList = {"$ArmaReforger:Prefabs",
						"$WCS_Armaments:Prefabs",
						"$M110MarksmanRifle:Prefabs",
						"$RISLaserAttachments:Prefabs"
		};*/
		
		//Lootlist: Rifles
		SCR_DC_LootList lootlist01 = new SCR_DC_LootList;
		lootlist01.Set(
			"WEAPON_RIFLE",
			"/Weapons/Rifles",
			{"Rifle"},
			{"_Base", "Tutorial"}
		);
		conf.lootLists.Insert(lootlist01);
		
		//Lootlist: Launchers
		SCR_DC_LootList lootlist02 = new SCR_DC_LootList;
		lootlist02.Set(
			"WEAPON_LAUNCHER",
			"/Weapons/Launchers",
			{"Launcher"},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist02);		
		
		//Lootlist: Handguns
		SCR_DC_LootList lootlist03 = new SCR_DC_LootList;
		lootlist03.Set(
			"WEAPON_HANDGUN",
			"/Weapons/Handguns",
			{"Handgun"},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist03);		

		//Lootlist: Grenades
		SCR_DC_LootList lootlist04 = new SCR_DC_LootList;
		lootlist04.Set(
			"WEAPON_GRENADE",
			"/Weapons/Grenades",
			{""},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist04);			
				
		//Lootlist: Medical items
		SCR_DC_LootList lootlist05 = new SCR_DC_LootList;
		lootlist05.Set(
			"ITEM_MEDICAL",
			"/Items/Medicine",
			{""},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist05);			
		
		//Lootlist: Weapon attachments
		SCR_DC_LootList lootlist06 = new SCR_DC_LootList;
		lootlist06.Set(
			"WEAPON_ATTACHMENT",
			"/Weapons/Attachments",
			{""},
			{"_Base", "Optic"}
		);
		conf.lootLists.Insert(lootlist06);			

		//Lootlist: Weapon attachments
		SCR_DC_LootList lootlist07 = new SCR_DC_LootList;
		lootlist07.Set(
			"WEAPON_OPTICS",
			"/Weapons/Attachments",
			{"Optic", "Scope"},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist07);			
		
		//Lootlist: General items
		SCR_DC_LootList lootlist08 = new SCR_DC_LootList;
		lootlist08.Set(
			"ITEM_GENERAL",
			"/Items",
			{"Binocular", "Compass", "Flashlight", "PaperMap_01_folded", "Watch_"},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist08);			
		
	}
}