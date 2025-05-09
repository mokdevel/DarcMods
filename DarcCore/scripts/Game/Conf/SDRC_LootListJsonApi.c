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

/*class SDRC_LootListConfig : SDRC_ListConfig
{
}*/

//------------------------------------------------------------------------------------------------
class SDRC_LootListJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_lootList.json";
		
	ref SDRC_ListConfig conf = new SDRC_ListConfig();

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
		SDRC_List lootlist01 = new SDRC_List;
		lootlist01.Set(
			"WEAPON_RIFLE",
			"/Weapons/Rifles",
			{"Rifle"},
			{"_Base", "Tutorial"}
		);
		conf.lists.Insert(lootlist01);
		
		//Lootlist: Launchers
		SDRC_List lootlist02 = new SDRC_List;
		lootlist02.Set(
			"WEAPON_LAUNCHER",
			"/Weapons/Launchers",
			{"Launcher"},
			{"_Base"}
		);
		conf.lists.Insert(lootlist02);		
		
		//Lootlist: Handguns
		SDRC_List lootlist03 = new SDRC_List;
		lootlist03.Set(
			"WEAPON_HANDGUN",
			"/Weapons/Handguns",
			{"Handgun"},
			{"_Base"}
		);
		conf.lists.Insert(lootlist03);		

		//Lootlist: Grenades
		SDRC_List lootlist04 = new SDRC_List;
		lootlist04.Set(
			"WEAPON_GRENADE",
			"/Weapons/Grenades",
			{""},
			{"_Base"}
		);
		conf.lists.Insert(lootlist04);			
				
		//Lootlist: Medical items
		SDRC_List lootlist05 = new SDRC_List;
		lootlist05.Set(
			"ITEM_MEDICAL",
			"/Items/Medicine",
			{""},
			{"_Base"}
		);
		conf.lists.Insert(lootlist05);			
		
		//Lootlist: Weapon attachments
		SDRC_List lootlist06 = new SDRC_List;
		lootlist06.Set(
			"WEAPON_ATTACHMENT",
			"/Weapons/Attachments",
			{""},
			{"_Base", "Optic"}
		);
		conf.lists.Insert(lootlist06);			

		//Lootlist: Weapon attachments
		SDRC_List lootlist07 = new SDRC_List;
		lootlist07.Set(
			"WEAPON_OPTICS",
			"/Weapons/Attachments",
			{"Optic", "Scope"},
			{"_Base"}
		);
		conf.lists.Insert(lootlist07);			
		
		//Lootlist: General items
		SDRC_List lootlist08 = new SDRC_List;
		lootlist08.Set(
			"ITEM_GENERAL",
			"/Items",
			{"Binocular", "Compass", "Flashlight", "PaperMap_01_folded", "Watch_"},
			{"_Base"}
		);
		conf.lists.Insert(lootlist08);		
	}
}