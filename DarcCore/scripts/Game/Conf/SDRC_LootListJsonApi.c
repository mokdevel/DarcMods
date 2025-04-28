//Helpers SDRC_LootListJsonApi.c

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

class SDRC_LootListConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Config specific
	ref array<string> modList = {};
	ref array<ref SDRC_LootList> lootLists = {};
	
	void Populate()
	{
		SDRC_Log.Add("[SDRC_LootListConfig:Populate] Creating loot lists..", LogLevel.NORMAL);
		
		if (modList.IsEmpty())
		{
			array<string> addonList = SCR_AddonTool.GetAllAddonFileSystems();
			foreach (string addon : addonList)
			{
				string name = addon + "Prefabs";
				modList.Insert(name);
				SDRC_Log.Add("[SDRC_LootListConfig:Populate] Mod found: " + name, LogLevel.DEBUG);				
			}
		}
		
		foreach (SDRC_LootList lootList : lootLists)
		{
			foreach (string mod : modList)
			{
				SDRC_Resources.GetItemList(lootList.itemList, mod, lootList);
			}
			
			if (SDRC_Log.GetLogLevel() > DC_LogLevel.NORMAL)
			{
				lootList.itemList.Debug();
			}
		}
	}	
}

class SDRC_LootList : Managed
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
class SDRC_LootListJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_lootList.json";
		
	ref SDRC_LootListConfig conf = new SDRC_LootListConfig;

	//------------------------------------------------------------------------------------------------
	void SDRC_LootListJsonApi()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_MISSIONCONFIG_FILE);
		if (!loadContext)
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
		SDRC_LootList lootlist01 = new SDRC_LootList;
		lootlist01.Set(
			"WEAPON_RIFLE",
			"/Weapons/Rifles",
			{"Rifle"},
			{"_Base", "Tutorial"}
		);
		conf.lootLists.Insert(lootlist01);
		
		//Lootlist: Launchers
		SDRC_LootList lootlist02 = new SDRC_LootList;
		lootlist02.Set(
			"WEAPON_LAUNCHER",
			"/Weapons/Launchers",
			{"Launcher"},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist02);		
		
		//Lootlist: Handguns
		SDRC_LootList lootlist03 = new SDRC_LootList;
		lootlist03.Set(
			"WEAPON_HANDGUN",
			"/Weapons/Handguns",
			{"Handgun"},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist03);		

		//Lootlist: Grenades
		SDRC_LootList lootlist04 = new SDRC_LootList;
		lootlist04.Set(
			"WEAPON_GRENADE",
			"/Weapons/Grenades",
			{""},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist04);			
				
		//Lootlist: Medical items
		SDRC_LootList lootlist05 = new SDRC_LootList;
		lootlist05.Set(
			"ITEM_MEDICAL",
			"/Items/Medicine",
			{""},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist05);			
		
		//Lootlist: Weapon attachments
		SDRC_LootList lootlist06 = new SDRC_LootList;
		lootlist06.Set(
			"WEAPON_ATTACHMENT",
			"/Weapons/Attachments",
			{""},
			{"_Base", "Optic"}
		);
		conf.lootLists.Insert(lootlist06);			

		//Lootlist: Weapon attachments
		SDRC_LootList lootlist07 = new SDRC_LootList;
		lootlist07.Set(
			"WEAPON_OPTICS",
			"/Weapons/Attachments",
			{"Optic", "Scope"},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist07);			
		
		//Lootlist: General items
		SDRC_LootList lootlist08 = new SDRC_LootList;
		lootlist08.Set(
			"ITEM_GENERAL",
			"/Items",
			{"Binocular", "Compass", "Flashlight", "PaperMap_01_folded", "Watch_"},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist08);			
		
	}
}