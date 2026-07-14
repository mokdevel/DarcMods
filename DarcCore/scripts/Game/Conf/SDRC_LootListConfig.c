//Helpers SDRC_LootListConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
// Types defined by default
/*
	WEAPON_RIFLE,
	WEAPON_RIFLE_BIG,
	WEAPON_HANDGUN,
	WEAPON_LAUNCHER,
	WEAPON_GRENADE,
	WEAPON_SHOTGUN,
	WEAPON_MG,
	ITEM_MEDICAL,
	ITEM_GENERAL,
	UTIL_ATTACHMENT,
	UTIL_OPTIC,
	UTIL_MAGAZINE,			//Adds random magazines with ammoCount
	UTIL_AMMO,				//Adds random rocket, shell, flare, .. with ammoCount
	GEAR_HEADGEAR,			//Head Gear including helmets, gas masks
	GEAR_VEST,				//Vests
	GEAR_HANDWEAR,			//Gloves etc
	GEAR_UNIFORM,			//Jackets, pants
	GEAR_BAG,				//Bags, backpacks, etc.. 
	CLOTHING_HEADGEAR,		//Head Gear including hats
	CLOTHING_UNIFORM,		//Jackets, pants, shirts, 

	//Freedom Fighters specific items
	FF_ITEMS_LOW,			//Low tier loot in FF
	FF_ITEMS_MID,			//Mids tier loot in FF
	FF_ITEMS_HIGH,			//High tier loot in FF
*/

/*class SDRC_LootListConfig : SDRC_ListConfig
{
}*/

//------------------------------------------------------------------------------------------------
class SDRC_LootListConfig : SDRC_ListConfig
{
	ref array<int> ammoCount = {1, 6};	// min/max count of ammo/magazines added for weapon spawned in loot.
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_LootListConfig data = SDRC_LootListConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------

	override void SetDefaults()
	{		
		super.SetDefaults();
		
		modList = {};
		//Weapons
		lists.Insert(lootList00());		
		lists.Insert(lootList01());		
		lists.Insert(lootList02());		
		lists.Insert(lootList03());		
		lists.Insert(lootList04());		
		lists.Insert(lootList06());	
		//Attachments
		lists.Insert(lootList20());	
		lists.Insert(lootList21());	
		//Ammo, magazines
		lists.Insert(lootList40());	
		lists.Insert(lootList41());	
		//Items
		lists.Insert(lootList50());	
		lists.Insert(lootList51());
		//Gear
		lists.Insert(lootList60());		
		lists.Insert(lootList61());		
		lists.Insert(lootList62());		
		lists.Insert(lootList63());		
		lists.Insert(lootList64());		
		//Clothing
		lists.Insert(lootList80());		
		lists.Insert(lootList83());		
		//Freedom Fighters
		lists.Insert(lootList90());		
		lists.Insert(lootList91());		
		lists.Insert(lootList92());		
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
			{"Optic", "Scope",
			"1P69", "AimPoint", "Collim", "ANPVS", "G33", 	//RAYZIOPTICSPACK
			"PDC_cover", "Zhukov", 							//RAYZIOPTICSPACK
			},
			{"_Base", 
			"DoNotUse",										//RAYZIOPTICSPACK
			},
			{}
		);
		return lootList;
	}

	//Utility list: Magazine
	SDRC_List lootList40()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"UTIL_MAGAZINE",
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
			{"_Base", "BackPack", "Pouch", },
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

	//Lootlist: Bags and backpacks
	SDRC_List lootList64()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"GEAR_BAG",
			{"Prefabs/Items/Equipment"},
			{"BackPack", "Pouch", "/Test/"},
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
