//Helpers SDRC_LootListJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
// Types defined by default
/*
	WEAPON_RIFLE,
	WEAPON_RIFLE_BIG,
	WEAPON_HANDGUN,
	WEAPON_SHOTGUN,
	WEAPON_LAUNCHER,
	WEAPON_GRENADE,
	UTIL_ATTACHMENT,
	UTIL_OPTIC,
	ITEM_MEDICAL,
	ITEM_GENERAL,
	UTIL_MAGAZINE,			//Adds a random magazine
	UTIL_AMMO,				//Adds a random rocket, shell, flare, ..
	GEAR_HEADGEAR,			//Head Gear including helmets, gas masks
	GEAR_VEST,				//Vests
	GEAR_HANDWEAR,			//Gloves etc
	GEAR_UNIFORM,			//Jackets, pants
	CLOTHING_HEADGEAR,		//Head Gear including hats
	CLOTHING_UNIFORM,		//Jackets, pants, shirts, 
*/

/*class SDRC_LootListConfig : SDRC_ListConfig
{
}*/

//------------------------------------------------------------------------------------------------
class SDRC_LootListJsonApi : SDRC_JsonApi
{
	ref SDRC_ListConfig conf = new SDRC_ListConfig();
	
	//------------------------------------------------------------------------------------------------
	void SDRC_LootListJsonApi(string fileName)
	{
		SetFileName(fileName);
	}
	
	//------------------------------------------------------------------------------------------------
	bool Load(bool createMissingFiles = true)
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(createMissingFiles);		
		if (!loadContext)
		{
			if (!createMissingFiles)
			{
				return false;
			}
			SetDefaults();
			Save();
			return true;
		}
		
		loadContext.ReadValue("", conf);
		return true;
	}	

	//------------------------------------------------------------------------------------------------
	void Save()
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen();
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}	
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{		
		conf.modList = {};
		//Weapons
		conf.lists.Insert(lootList00());		
		conf.lists.Insert(lootList01());		
		conf.lists.Insert(lootList02());		
		conf.lists.Insert(lootList03());		
		conf.lists.Insert(lootList04());		
		conf.lists.Insert(lootList06());	
		//Attachments
		conf.lists.Insert(lootList20());	
		conf.lists.Insert(lootList21());	
		//Ammo, magazines
		conf.lists.Insert(lootList40());	
		conf.lists.Insert(lootList41());	
		//Items
		conf.lists.Insert(lootList50());	
		conf.lists.Insert(lootList51());
		//Gear
		conf.lists.Insert(lootList60());		
		conf.lists.Insert(lootList61());		
		conf.lists.Insert(lootList62());		
		conf.lists.Insert(lootList63());		
		//Clothing
		conf.lists.Insert(lootList80());		
		conf.lists.Insert(lootList83());		
		//Freedom Fighters
		conf.lists.Insert(lootList90());		
		conf.lists.Insert(lootList91());		
		conf.lists.Insert(lootList92());		
	}
			
	//Lootlist: Rifles
	SDRC_List lootList00()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"WEAPON_RIFLE",
			{"Prefabs/Weapons/Rifles",
				"Prefabs/5AB890B71D748750/Weapon_Variants_B",	//M4BlockII
			},
			{"Rifle", 
			"Spear68_", //MCX Spear 6.8
			},
			{"_Base", "_Sample", "Tutorial", "Mosin", "SVD", "Barrett", "M110", "M21", "M40",},
			{}
		);
		return lootList;
	}
		
	//Lootlist: Large rifles
	SDRC_List lootList01()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"WEAPON_RIFLE_BIG",
			{"Prefabs/Weapons/Rifles"},
			{"Rifle", 
			"Spear68_", //MCX Spear 6.8
			},
			{"_Base", "_Sample", "Tutorial", "AK74", "AKS74", "VZ", "M16",
			"Spear68_",	//MCX Spear 6.8
			},
			{}
		);
		return lootList;
	}
		
	//Lootlist: Handguns
	SDRC_List lootList02()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"WEAPON_HANDGUN",
			{"Prefabs/Weapons/Handguns"},
			{"Handgun"},
			{"_Base"},
			{}
		);
		return lootList;
	}

	//Lootlist: Launchers
	SDRC_List lootList03()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"WEAPON_LAUNCHER",
			{"Prefabs/Weapons/Launchers"},
			{"Launcher", 
			"BC_M79"		//From BigChungusLaunchers
			},
			{"_Base"},
			{}
		);
		return lootList;
	}

	//Lootlist: Shotguns
	SDRC_List lootList04()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"WEAPON_SHOTGUN",
			{"Prefabs/Weapons/Shotguns"},
			{"Shotgun"},
			{"_Base"},
			{}
		);
		return lootList;
	}
		
	//Lootlist: Machineguns
	SDRC_List lootList05()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"WEAPON_MG",
			{"Prefabs/Weapons/MachineGuns"},
			{"MG_", "RPD", "RPK"},
			{"_Base", "_Mounted", "_rear", "_front", "PKMT"},
			{}
		);
		return lootList;
	}	
		
	//Lootlist: Grenades
	SDRC_List lootList06()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"WEAPON_GRENADE",
			{"Prefabs/Weapons/Grenades"},
			{""},
			{"_Base"},
			{}
		);
		return lootList;
	}
				
	//Lootlist: Weapon attachments
	SDRC_List lootList20()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"UTIL_ATTACHMENT",
			{"Prefabs/Weapons/Attachments",
				"Prefabs/5AB890B71D748750/Grip",		//M4BlockII
				"Prefabs/5AB890B71D748750/Handguard",
				"Prefabs/5AB890B71D748750/Lower",
				"Prefabs/5AB890B71D748750/Muzzle",
				"Prefabs/5AB890B71D748750/Stock",
				"Prefabs/5AB890B71D748750/UGL",
				"Prefabs/5AB890B71D748750/Upper",			
			},
			{""},
			{"_Base", "Optic", "Scope"},
			{}
		);
		return lootList;
	}

	//Lootlist: Weapon optics
	SDRC_List lootList21()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"UTIL_OPTIC",
			{"Prefabs/Weapons/Attachments", 
				"Scopes",									//RAYZIOPTICSPACK
				"Prefabs/Weapons/Attachments/BaconOptics/",	//RISLaserAttachments (BaconOptics)
				"Prefabs/5ABD0CB57F7E9EB1/Optics/",			//RISLaserAttachments
			},
			{"Optic", "Scope"},
			{"_Base"},
			{}
		);
		return lootList;
	}

	//Utility list: Magazine
	SDRC_List lootList40()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"UTIL_MAGAZINES",
			{"Prefabs/Weapons/Magazines"},
			{""},
			{"_Base", "Internal_"},
			{}
		);
		return lootList;
	}
		
	//Utility list: Ammo
	SDRC_List lootList41()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"UTIL_AMMO",
			{"Prefabs/Weapons/Ammo"},
			{"Shell", "Rocket", "Grenade", "Flare"},
			{"_Base", "Internal_", "_Effect", "FlareEffect", "Particles"},
			{}
		);
		return lootList;
	}
		
	//Lootlist: General items
	SDRC_List lootList50()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"ITEM_GENERAL",
			{"Prefabs/Items"},
			{"Binocular", "Compass", "Flashlight", "PaperMap_01_folded", "Watch_"},
			{"_Base"},
			{}
		);
		return lootList;
	}

	//Lootlist: Medical items
	SDRC_List lootList51()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"ITEM_MEDICAL",
			{"Prefabs/Items/Medicine"},
			{""},
			{"_Base"},
			{}
		);
		return lootList;
	}
	
	//Lootlist: Gear : Head gear
	SDRC_List lootList60()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"GEAR_HEADGEAR",
			{"Prefabs/Characters/HeadGear", "Prefabs/Clothing"},
			{"Helmet_",
			"GP-", "M50", "Unix_", //Gasmask items
			},
			{"_Base"},
			{},
		);
		return lootList;
	}

	SDRC_List lootList61()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"GEAR_VEST",
			{"Prefabs/Characters/Vests"},
			{"Vest_",},
			{"_Base"},
			{}	
		);
		return lootList;
	}

	SDRC_List lootList62()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"GEAR_HANDWEAR",
			{"Prefabs/Characters/Handwear"},
			{"Gloves_",},
			{"_Base"},
			{}	
		);
		return lootList;
	}

	SDRC_List lootList63()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"GEAR_UNIFORM",
			{"Prefabs/Characters/Uniforms"},
			{"Jacket_KZS", "Jacket_Pilot", "Jacket_Tanker", "Jacket_TAZ",
			 "Pants_KZS", "Pants_Pilot", "Pants_Tanker", "Pants_TAZ",
			 "Suit_KLMK", "Suit_Pilot", "Suit_Tanker",
			},
			{"_Base"},
			{}	
		);
		return lootList;
	}	
			
	//Lootlist: Clothing : Head gear
	SDRC_List lootList80()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"CLOTHING_HEADGEAR",
			{"Prefabs/Characters/HeadGear"},
			{"Hat_", 
			},
			{"_Base"},
			{}
		);
		return lootList;
	}

	//SDRC_List lootList81()
	//SDRC_List lootList82()
		
	SDRC_List lootList83()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"CLOTHING_UNIFORM",
			{"Prefabs/Characters/Uniforms"},
			{"_Denim", "_Raincoat", "_Fisherman", "_Trousers", "Shirt_", 
			 "Jacket_Suit", "Jacket_Pants",
			},
			{"_Base"},
			{}	
		);
		return lootList;
	}
	
	//Freedom Fighters specific stuff
	SDRC_List lootList90()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"FF_ITEMS_LOW",
			{"Prefabs/Items"},
			{"Backpack", "Radio_", "Tent_", "Lockpick_", "JWK_EpinephrineInjection", },
			{"_Base"},
			{}	
		);
		return lootList;
	}

	SDRC_List lootList91()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"FF_ITEMS_MID",
			{"Prefabs/Items"},
			{"Dogtags", "Briefcase", "CampingKit", "KeysOld", },
			{"_Base"},
			{}	
		);
		return lootList;
	}	
	
	SDRC_List lootList92()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"FF_ITEMS_HIGH",
			{"Prefabs/Items"},
			{"ShopProxy_Resource_Supplies", },
			{"_Base"},
			{}	
		);
		return lootList;
	}			
}