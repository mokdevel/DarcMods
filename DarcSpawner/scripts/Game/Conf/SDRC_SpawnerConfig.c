//Helpers SDRC_SpawnerConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

//------------------------------------------------------------------------------------------------
class SDRC_SpawnSet : Managed
{
	//Mission specific	
	string comment;
	bool showMarker;								//Show a marker 
	string markerType;								//marker type
	int markerIdx;									//marker ID		//TBD: Rename to markerIcon
	ref array<EMapDescriptorType> locationTypes;
	ref array<vector> positions;
	ref array<string> containers;					//What resource to spawn; cars, box, .. All of these will be spawned with spawnChance chance
	ref SDRC_Loot loot = null;
	
	void Set(string comment_, bool showMarker_, string markerType_, int markerIdx_, array<EMapDescriptorType> locationTypes_, array<vector> positions_, array<string> containers_)
	{
		comment = comment_;
		showMarker = showMarker_;
		markerType = markerType_;
		markerIdx = markerIdx_;		
		locationTypes = locationTypes_;
		positions = positions_;
		containers = containers_;
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_SpawnerConfig : SDRC_Config
{
	int version = 1;
	string author = "darc";
	string comment = "";
	//Spawner specific
	bool spawnOnRoad;					//Spawn the cars on road
	int spawnRndRadius;					//Random radius where the spawnName spawns. 
	int containerCount;					//Amount of containers (cars, lootboxes, etc..) to spawn
	float spawnWorldSizeMultiplier;		//If containerCount = 0, we search for the world size in km and multiple with this. For example: 4km wide map with spawnWorldSizeMultiplier = 2 results in containerCount = 8 (4*2)
	bool disableArsenal;				//Disable arsenal so that no other items are found	
	ref array<ref int> spawnSetList = {};	
	ref array<ref SDRC_SpawnSet> spawnSets = {};	
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_SpawnerConfig data = SDRC_SpawnerConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------

	override void SetDefaults()
	{		
		super.SetDefaults();	

		spawnOnRoad = false;
		spawnRndRadius = 100;
		spawnWorldSizeMultiplier = 0;
		containerCount = 10;//20;
		disableArsenal = true;
		spawnSetList = {0,1,2,2,3,3};
		//----------------------------------------------------		
		spawnSets.Insert(SpawnSet0());
		spawnSets.Insert(SpawnSet1());
		spawnSets.Insert(SpawnSet2());
		spawnSets.Insert(SpawnSet3());
		spawnSets.Insert(SpawnSet4());
	}	
	
	//Different spawner confs
	//----------------------------------------------------
	SDRC_SpawnSet SpawnSet0()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 0: Spawn ambulances",
			true, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_FORTRESS,
				EMapDescriptorType.MDT_CHURCH,
				EMapDescriptorType.MDT_FUELSTATION,
			},
			{},
			{
				"{00C9BBE426F7D459}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
			}
		);
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", 
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{3805C0B87B9D2AB0}prefabs/weapons/Melee/Knife.et",							//From Functional Knives
				"{CB716A41903571AD}prefabs/weapons/Melee/Knife_US.et"						//From Functional Knives
			};
		loot.Set(0.9, lootItems);
		spawnSet.loot = loot;
		
		return spawnSet;
	}
	
	SDRC_SpawnSet SpawnSet1()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(			
			"index 1: Spawn ambulances",
			true, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_NAME_CITY, 
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_FORTRESS,
				EMapDescriptorType.MDT_FUELSTATION,
				EMapDescriptorType.MDT_HOSPITAL, EMapDescriptorType.MDT_HOSPITAL, EMapDescriptorType.MDT_HOSPITAL,
				EMapDescriptorType.MDT_HOSPITAL, EMapDescriptorType.MDT_HOSPITAL,
			},
			{},
			{
				"{00C9BBE426F7D459}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance.et",
				"{3B1EB924602C7A07}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance_MERDC.et",			
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
			}
		);
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_GRENADE", "WEAPON_GRENADE",
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
				"{BA4866B5E0270FAA}Prefabs/Items/Medicine/JWK_EpinephrineInjection.et",			//From FF
			};
		loot.Set(0.7, lootItems);
		spawnSet.loot = loot;
		
		return spawnSet;
	}

	//----------------------------------------------------
	SDRC_SpawnSet SpawnSet2()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 2: Spawn civilian cars",
			true, "DARC_MISSION", SDRC_EMissionIcon.ICON_EXCLAMATION_SMALL_MAP, 		
			{
				EMapDescriptorType.MDT_NAME_CITY, 
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_HILL,
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_FUELSTATION,
				EMapDescriptorType.MDT_TOWER,
			},
			{},
			{
				"{54C3CC22DEBD57BE}Prefabs/Vehicles/Wheeled/S105/S105_beige.et",
				"{321016E0F9361A22}Prefabs/Vehicles/Wheeled/S105/S105_lightgreen.et",
				"{6E485048122CEEEE}Prefabs/Vehicles/Wheeled/S1203/S1203_cargo_red.et",
				"{F77C41245A580FD1}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_blue.et",
				"{C75DAB159B434B77}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_cargo_CIV_grey.et",
				"{5F5AAFE8465D9D62}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_cargo_FIA.et",
				"{BDE16A6AE9942D44}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_transport_FIA.et",
				"{9B3A89DD33FF0483}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_uncovered_CIV_blue.et",
			}
		);
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN", 
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
				"{50C1EC3C627018CE}Prefabs/Items/Lockpicks/Lockpick_base.et",						//From FF
			};
		loot.Set(0.7, lootItems);
		spawnSet.loot = loot;
		
		return spawnSet;
	}
	//----------------------------------------------------
	SDRC_SpawnSet SpawnSet3()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 3: Spawn loot crates",
			true, "DARC_MISSION", SDRC_EMissionIcon.ICON_CRATE_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_NAME_HILL,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_FORESTERLODGE,
				EMapDescriptorType.MDT_FORESTTRIANGLE,
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_NAME_VALLEY,
				EMapDescriptorType.MDT_CHURCH,
				EMapDescriptorType.MDT_FUELSTATION,
				EMapDescriptorType.MDT_TOWER,
				EMapDescriptorType.MDT_HOSPITAL,
			},
			{},
			{
				"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
				"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
				"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et",
				"{D15A294D5138ADFF}Prefabs/Props/Military/AmmoBoxes/US/LootEquipmentBoxWooden_Equipment_01_US.et",
				"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
				"{DBC8E6A4DD948C96}Prefabs/Props/Military/SupplyBox/SupplyPortableContainers/SupplyPortableContainer_01/LootSupplyPortableContainers_01_large_item.et",
			}
		);
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN", 
				"WEAPON_RIFLE",
				"WEAPON_LAUNCHER",
				"UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", "UTIL_MAGAZINE", 
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", 
				"ITEM_MEDICAL", 
				"GEAR_BAG",
				"{50C1EC3C627018CE}Prefabs/Items/Lockpicks/Lockpick_base.et",						//From FF
			};
		loot.Set(0.7, lootItems);
		spawnSet.loot = loot;
		
		return spawnSet;
	}
	
	//----------------------------------------------------
	SDRC_SpawnSet SpawnSet4()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 4: Test to spawn crates to positions",
			true, "DARC_MISSION", SDRC_EMissionIcon.ICON_CRATE_SMALL_MAP, 
			{
			},
			{"1388.253 37.246 2350.735", "1392.363 0.0 2349.161"},
			{
				"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
				"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
				"{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et",
				"{D15A294D5138ADFF}Prefabs/Props/Military/AmmoBoxes/US/LootEquipmentBoxWooden_Equipment_01_US.et",
				"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
				"{DBC8E6A4DD948C96}Prefabs/Props/Military/SupplyBox/SupplyPortableContainers/SupplyPortableContainer_01/LootSupplyPortableContainers_01_large_item.et",
			}
		);
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN", 
				"WEAPON_RIFLE",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
			};
		loot.Set(0.7, lootItems);
		spawnSet.loot = loot;
		
		return spawnSet;
	}
	
}
