//Helpers SDRC_SpawnerConfig_Animals.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

//------------------------------------------------------------------------------------------------
class SDRC_SpawnerConfig_Animals : SDRC_SpawnerConfig
{
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_SpawnerConfig_Animals data = SDRC_SpawnerConfig_Animals.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{		
		version = 1;
		author = "darc";
		comment = "";
		spawnWorldSizeMultiplier = 0;
		spawnCount = 20;
		spawnSetList = {0,0,0,1,2,2,3,3,4};
		//----------------------------------------------------		
		spawnSets.Insert(SpawnSet0_Animals());
		spawnSets.Insert(SpawnSet1_Animals());
		spawnSets.Insert(SpawnSet2_Animals());
		spawnSets.Insert(SpawnSet3_Animals());
		spawnSets.Insert(SpawnSet4_Animals());
	}	
	
	//Different spawner confs
	//----------------------------------------------------
	SDRC_SpawnSet SpawnSet0_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 0: Spawn deer",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
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
			},
			200, false, false,
		);
		
		return spawnSet;
	}
	
	SDRC_SpawnSet SpawnSet1_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 1: Spawn wolf",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
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
			},
			200, false, false,
		);
		
		return spawnSet;
	}	
	
	SDRC_SpawnSet SpawnSet2_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 2: Spawn doe",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
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
				"{2C626C751F814327}Prefabs/Animals/Doe.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}	

	SDRC_SpawnSet SpawnSet3_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 3: Spawn fox",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
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
				"{97CEDAB6748616EB}Prefabs/Animals/Fox.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}	
	
	SDRC_SpawnSet SpawnSet4_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 4: Spawn rabbit",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
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
				"{3C106059EE4089FD}Prefabs/Animals/Rabbit.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}	
}
