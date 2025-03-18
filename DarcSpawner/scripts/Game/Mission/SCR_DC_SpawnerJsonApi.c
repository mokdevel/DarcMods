//Helpers SCR_DC_SpawnerJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

//------------------------------------------------------------------------------------------------
class SCR_DC_SpawnerConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Spawner specific
	int spawnSetID;						//-1 = random, other numbers are the index of spawnSet
	bool spawnOnRoad;					//Spawn the cars on road
	int spawnRndRadius;					//Random radius where the spawnName spawns. 
	float spawnWorldSizeMultiplier;		//If spawnCount = 0, we search for the world size and multiple with this. 
										//For example: 4km wide map with spawnWorldSizeMultiplier = 2 results in spawnCount = 8 (4*2)
	ref array<ref SCR_DC_SpawnSet> spawnSets = {};	
}

//------------------------------------------------------------------------------------------------
class SCR_DC_SpawnSet : Managed
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
class SCR_DC_SpawnerJsonApi : SCR_DC_JsonApi
{
	const string DC_CONFIG_FILE = "dc_spawnerConfig.json";
	ref SCR_DC_SpawnerConfig conf = new SCR_DC_SpawnerConfig;

	//------------------------------------------------------------------------------------------------
	void SCR_DC_SpawnerJsonConfig()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_CONFIG_FILE);
		
		if(!loadContext)
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
		conf.spawnSetID = 0;
		conf.spawnOnRoad = true;
		conf.spawnRndRadius = 100;
		conf.spawnWorldSizeMultiplier = 2;
		
		//Different spawner confs
		//---
		SCR_DC_SpawnSet spawnSet0 = new SCR_DC_SpawnSet;
		spawnSet0.Set
		(			
			"Used for Escapists in Gogland. Spawns random ambulances with some gear",
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
				"{3B1EB924602C7A07}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance_MERDC.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
			},
			1.0,
			0,
			{
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			},
			0.9
		);
		conf.spawnSets.Insert(spawnSet0);
				
		//---
		SCR_DC_SpawnSet spawnSet1 = new SCR_DC_SpawnSet;
		spawnSet1.Set
		(			
			"Used for Escapists. Spawns random ambulances with some gear",
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
			},
			1.0,
			0,
			{
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			},
			0.9
		);
		conf.spawnSets.Insert(spawnSet1);
		
		//---
		SCR_DC_SpawnSet spawnSet2 = new SCR_DC_SpawnSet;
		spawnSet2.Set
		(			
			"Used for Escapists in Arland. Spawns random ambulances with some gear.",
			{
				EMapDescriptorType.MDT_NAME_CITY, 
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_HILL,
				EMapDescriptorType.MDT_NAME_RIDGE
			
			},
			{
				"{00C9BBE426F7D459}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance.et",
				"{3B1EB924602C7A07}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance_MERDC.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et"
			},
			0.6,
			0,
			{
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			},
			0.7
		);
		conf.spawnSets.Insert(spawnSet2);		
	}
}