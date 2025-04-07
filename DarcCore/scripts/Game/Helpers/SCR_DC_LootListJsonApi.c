//Helpers SCR_DC_LootListJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
enum DC_ELootListType
{
	WEAPON_RIFLE,
	WEAPON_HANDGUN,
	WEAPON_LAUNCHER,
	WEAPON_GRENADE,
	ITEM_MEDICAL
};

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
		foreach(SCR_DC_LootList lootList : lootLists)
		{
			foreach(string mod : modList)
			{
				SCR_DC_Resources.GetItemList(lootList.itemList, mod, lootList);
			}
			lootList.itemList.Debug();
		}
	}	
}

class SCR_DC_LootList : Managed
{
	string lootListName;
	DC_ELootListType lootListType;
	string modDir;
	ref array<string> include = {};
	ref array<string> exclude = {};
	ref array<ResourceName> itemList = {};

	void Set(string lootListName_, DC_ELootListType lootListType_, string modDir_, array<string> include_, array<string> exclude_)	
	{
		lootListName = lootListName_;
		lootListType = lootListType_;
		modDir = modDir_;
		include = include_;
		exclude = exclude_;
		itemList = {};
	}
}

//------------------------------------------------------------------------------------------------
class SCR_DC_LootListJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_LootList.json";
		
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
		
		conf.modList = {"$ArmaReforger:Prefabs",
						"$WCS_Armaments:Prefabs",
						"$M110MarksmanRifle:Prefabs"
		};
		
		//Lootlist: Rifles
		SCR_DC_LootList lootlist01 = new SCR_DC_LootList;
		lootlist01.Set(
			"WEAPON_RIFLE",
			DC_ELootListType.WEAPON_RIFLE,
			"/Weapons/Rifles",
			{"Rifle"},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist01);
		
		//Lootlist: Launchers
		SCR_DC_LootList lootlist02 = new SCR_DC_LootList;
		lootlist02.Set(
			"WEAPON_LAUNCHER",
			DC_ELootListType.WEAPON_LAUNCHER,
			"/Weapons/Launchers",
			{"Launcher"},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist02);		
		
		//Lootlist: Handguns
		SCR_DC_LootList lootlist03 = new SCR_DC_LootList;
		lootlist03.Set(
			"WEAPON_HANDGUN",
			DC_ELootListType.WEAPON_HANDGUN,
			"/Weapons/Handguns",
			{"Handgun"},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist03);		

		//Lootlist: Grenades
		SCR_DC_LootList lootlist04 = new SCR_DC_LootList;
		lootlist04.Set(
			"WEAPON_GRENADE",
			DC_ELootListType.WEAPON_GRENADE,
			"/Weapons/Grenades",
			{""},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist04);			
				
		//Lootlist: Medical items
		SCR_DC_LootList lootlist05 = new SCR_DC_LootList;
		lootlist05.Set(
			"ITEM_MEDICAL",
			DC_ELootListType.ITEM_MEDICAL,
			"/Items/Medicine",
			{""},
			{"_Base"}
		);
		conf.lootLists.Insert(lootlist05);			
	}
}