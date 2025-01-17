//Helpers SCR_DC_SpawnerConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

//------------------------------------------------------------------------------------------------
class SCR_DC_Frame : Managed
{
	//Mission specific
	DC_LogLevel logLevel;
	int spawnSetID;			//-1 = random, other numbers are the index of spawnSet
	int spawnRndRadius;
}

//------------------------------------------------------------------------------------------------
class SCR_DC_SpawnSet : Managed
{
	//Mission specific
	string comment;
	ref array<EMapDescriptorType> locationTypes;
	ref array<string> spawnNames;					//What resource to spawn; cars, box, .. All of these will be spawned with spawnChance chance
	float spawnChance;								//The change to spawn an item from spawnNames. 0.5 = 50% chance
	ref array<string> itemNames;					//Items to add to each spawnNames
	float itemChance;								//The chance to spawn and item from itemNames. 0.5 = 50% chance
	
	void Set(string comment_, array<EMapDescriptorType> locationTypes_, array<string> spawnNames_, float spawnChance_, array<string> itemNames_, float itemChance_)
	{
		comment = comment_;
		locationTypes = locationTypes_;
		spawnNames = spawnNames_;
		spawnChance = spawnChance_;
		itemNames = itemNames_;
		itemChance = itemChance_;
	}
}

//------------------------------------------------------------------------------------------------
class SCR_DC_SpawnerConfig : SCR_DC_JsonConfig
{
	const string DC_CONFIG_FILE = "dc_spawnerConfig.json";
	ref array<ref SCR_DC_SpawnSet> spawnSets = {};
	ref SCR_DC_Frame frame = new SCR_DC_Frame;

	//------------------------------------------------------------------------------------------------
	void SCR_DC_SpawnerConfig()
	{
		version = 1;
		author = "darc";
	}
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_CONFIG_FILE);
		
		if(!loadContext)
		{
			SetDefaults();
			Save("");
			return;
		}
		
		loadContext.ReadValue("frame", frame);
		loadContext.ReadValue("spawnSets", spawnSets);		
	}	

	//------------------------------------------------------------------------------------------------
	void Save(string data)
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen(DC_CONFIG_FILE);
		saveContext.WriteValue("frame", frame);
		saveContext.WriteValue("spawnSets", spawnSets);		
		SaveConfigClose(saveContext);
	}	
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		frame.logLevel = DC_LogLevel.NORMAL;
		frame.spawnSetID = 1;
		frame.spawnRndRadius = 100;
		
		//Different spawner confs
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
				"{00C9BBE426F7D459}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance.et",
				"{3B1EB924602C7A07}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance_MERDC.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et"
			},
			1.0,
			{
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{AF3411AAF94D18C2}Prefabs/Items/Medicine/Gauze_02/Gauze_02.et",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			},
			0.9
		);
		spawnSets.Insert(spawnSet1);
		
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
				"{00C9BBE426F7D459}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance.et",
				"{00C9BBE426F7D459}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance.et",
				"{3B1EB924602C7A07}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance_MERDC.et",
				"{3B1EB924602C7A07}Prefabs/Vehicles/Wheeled/M998/M997_maxi_ambulance_MERDC.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
				"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et"
			},
			0.6,
			{
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{AF3411AAF94D18C2}Prefabs/Items/Medicine/Gauze_02/Gauze_02.et",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			},
			0.6
		);
		spawnSets.Insert(spawnSet2);		
	}
}