//Helpers SDRC_SpawnerJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

//------------------------------------------------------------------------------------------------
class SDRC_SpawnerConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Spawner specific
	bool showMarker;
	int spawnSetID;						//-1 = random, other numbers are the index of spawnSet
	bool spawnOnRoad;					//Spawn the cars on road
	int spawnRndRadius;					//Random radius where the spawnName spawns. 
	float spawnWorldSizeMultiplier;		//If spawnCount = 0, we search for the world size and multiple with this. 
										//For example: 4km wide map with spawnWorldSizeMultiplier = 2 results in spawnCount = 8 (4*2)
	bool disableArsenal;				//Disable arsenal so that no other items are found
	ref array<ref SDRC_SpawnSet> spawnSets = {};	
}

//------------------------------------------------------------------------------------------------
class SDRC_SpawnSet : Managed
{
	//Mission specific
	string comment;
	ref array<EMapDescriptorType> locationTypes;
	ref array<string> spawnNames;					//What resource to spawn; cars, box, .. All of these will be spawned with spawnChance chance
	float spawnChance;								//The change to spawn an item from spawnNames. 0.5 = 50% chance
	int spawnCount;									//The maximum amount of spawnNames to spawn. For example: 10 with 0.5 chance would spawn 5 items on average. 0 = count a value depending on mapsize.
	ref array<string> itemNames;					//Items to add to each spawnNames
	float itemChance;								//The chance to spawn and item from itemNames. 0.5 = 50% chance
	
	void Set(string comment_, array<EMapDescriptorType> locationTypes_, array<string> spawnNames_, float spawnChance_, int spawnCount_, array<string> itemNames_, float itemChance_)
	{
		comment = comment_;
		locationTypes = locationTypes_;
		spawnNames = spawnNames_;
		spawnChance = spawnChance_;
		spawnCount = spawnCount_;
		itemNames = itemNames_;
		itemChance = itemChance_;
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_SpawnerJsonApi : SDRC_JsonApi
{
	const string DC_CONFIG_FILE = "dc_spawnerConfig.json";
	ref SDRC_SpawnerConfig conf = new SDRC_SpawnerConfig();

	//------------------------------------------------------------------------------------------------
	void SDRC_SpawnerJsonConfig()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_CONFIG_FILE);
		
		if (!loadContext)
		{
			SetDefaults();
			Save();
			return;
		}
		
		loadContext.ReadValue("", conf);
	}	

	//------------------------------------------------------------------------------------------------
	void Save()
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen(DC_CONFIG_FILE);
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}	
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		conf.showMarker = true;
//		conf.showMarker = false;
		conf.spawnSetID = -1;
		conf.spawnOnRoad = false;
		conf.spawnRndRadius = 100;
		conf.spawnWorldSizeMultiplier = 2;
		conf.disableArsenal = true;
		
		//Different spawner confs
		//---
		SDRC_SpawnSet spawnSet0 = new SDRC_SpawnSet();
		spawnSet0.Set
		(			
			"Spawn two cars",
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
				EMapDescriptorType.MDT_FORTRESS
			},
			{
				"{00C9BBE426F7D459}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
			},
			0.8,	//spawnChance
			2,		//spawnCount, max
			{
				"WEAPON_GRENADE",
				"ITEM_MEDICAL",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{3805C0B87B9D2AB0}prefabs/weapons/Melee/Knife.et",							//From Functional Knives
				"{CB716A41903571AD}prefabs/weapons/Melee/Knife_US.et"						//From Functional Knives
			},
			0.9
		);
		conf.spawnSets.Insert(spawnSet0);
				
		//---
		SDRC_SpawnSet spawnSet1 = new SDRC_SpawnSet();
		spawnSet1.Set
		(			
			"Spawn two cars.",
			{
				EMapDescriptorType.MDT_NAME_CITY, 
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_FORTRESS
			},
			{
				"{00C9BBE426F7D459}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance.et",
				"{3B1EB924602C7A07}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance_MERDC.et",			
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
			},
			0.5,
			3,
			{
				"WEAPON_RIFLE",
				"WEAPON_GRENADE", "WEAPON_GRENADE",
				"ITEM_MEDICAL",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et"
			},
			0.9
		);
		conf.spawnSets.Insert(spawnSet1);
		
		//---
		SDRC_SpawnSet spawnSet2 = new SDRC_SpawnSet();
		spawnSet2.Set
		(			
			"Spawn two civilian cars",
			{
				EMapDescriptorType.MDT_NAME_CITY, 
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_HILL,
				EMapDescriptorType.MDT_NAME_RIDGE
			
			},
			{
				"{54C3CC22DEBD57BE}Prefabs/Vehicles/Wheeled/S105/S105_beige.et",
				"{321016E0F9361A22}Prefabs/Vehicles/Wheeled/S105/S105_lightgreen.et",
				"{6E485048122CEEEE}Prefabs/Vehicles/Wheeled/S1203/S1203_cargo_red.et",
				"{F77C41245A580FD1}Prefabs/Vehicles/Wheeled/S1203/S1203_transport_blue.et"
			},
			0.6,
			2,
			{
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN", 
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
			},
			0.7
		);
		conf.spawnSets.Insert(spawnSet2);		
	}
}