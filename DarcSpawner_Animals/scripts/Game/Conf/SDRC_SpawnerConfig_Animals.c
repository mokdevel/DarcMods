//Helpers SDRC_SpawnerConfig_Animals.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

//------------------------------------------------------------------------------------------------
class SDRC_SpawnerConfig_Animals : SDRC_Config
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
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_SpawnerConfig_Animals data = SDRC_SpawnerConfig_Animals.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------

	override void SetDefaults()
	{		
		super.SetDefaults();	

		spawnOnRoad = false;
		spawnRndRadius = 200;
		spawnWorldSizeMultiplier = 0;
		containerCount = 20;
		disableArsenal = true;
		spawnSetList = {0,1};
		//----------------------------------------------------		
		spawnSets.Insert(SpawnSet0());
		spawnSets.Insert(SpawnSet1());
	}	
	
	//Different spawner confs
	//----------------------------------------------------
	SDRC_SpawnSet SpawnSet0()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 0: Spawn deer",
			true, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FORESTTRIANGLE,
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{D1509D4108A75FF4}Prefabs/Animals/Deer.et",
			}
		);
		
		return spawnSet;
	}
	
	SDRC_SpawnSet SpawnSet1()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 1: Spawn deer",
			true, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FORESTTRIANGLE,
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{2274A55C0B226999}Prefabs/Animals/Wolf.et",
			}
		);
		
		return spawnSet;
	}	
}
