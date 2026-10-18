//Helpers SDRC_SpawnerConfig_Animals.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

//------------------------------------------------------------------------------------------------
class SDRC_SpawnerConfig_ArghFauna : SDRC_SpawnerConfig
{
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_SpawnerConfig_ArghFauna data = SDRC_SpawnerConfig_ArghFauna.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{		
		version = 1;
		author = "darc";
		comment = "Argh Fauna spawner using ArghFauna mod. ";
		spawnWorldSizeMultiplier = 0;
		spawnCount = 10;
		spawnSetList = {0,0,1,1,2,2,3,3, 4,4,5,5,5,6};
		//spawnSetList = {5};
		//----------------------------------------------------		
		spawnSets.Insert(SpawnSet0_Animals());	//Kangaroo
		spawnSets.Insert(SpawnSet1_Animals());	//Cobra A
		spawnSets.Insert(SpawnSet2_Animals());	//Cobra B
		spawnSets.Insert(SpawnSet3_Animals());	//Snake V2
		//Territories
		spawnSets.Insert(SpawnSet4_Animals());	//Kangaroo territory
		spawnSets.Insert(SpawnSet5_Animals());	//Snakes territory
		spawnSets.Insert(SpawnSet6_Animals());	//Snakes in specific areas
	}	
	
	//Different spawner confs
	//----------------------------------------------------
	SDRC_SpawnSet SpawnSet0_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 0: Spawn kangaroo",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
			},
			{},
			{
				"{92FC5FF61DAD100C}Prefabs/Characters/Animals/Character_Kangaroo_Roo.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}
	
	SDRC_SpawnSet SpawnSet1_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 1: Spawn Cobra A",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{8968DC7FA65F5A20}Prefabs/Characters/Animals/Character_Snake_Cobra_A.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}	
	
	SDRC_SpawnSet SpawnSet2_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 2: Spawn Cobra B",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{FB7FB9D9EB6ED8F5}Prefabs/Characters/Animals/Character_Snake_Cobra_B.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}	

	SDRC_SpawnSet SpawnSet3_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 3: Spawn Snake V2",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{D5567BAD5D891258}Prefabs/Characters/Animals/Character_Snake_V2.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}		
	
	SDRC_SpawnSet SpawnSet4_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 4: Kangaroo territory",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
			},
			{},
			{
				"{23D6EBDC498004D9}PrefabsEditable/Fauna/ARGH_AmbientFauna_SDRC_kangaroos.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}
	
	SDRC_SpawnSet SpawnSet5_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 5: Snake territory",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
			},
			{},
			{
				"{0AE95D649C3693BA}PrefabsEditable/Fauna/ARGH_AmbientFauna_SDRC_snakes.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}
	
	SDRC_SpawnSet SpawnSet6_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 6: Snakes in specific areas",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_NAME_ISLAND,
				EMapDescriptorType.MDT_CHURCH, EMapDescriptorType.MDT_CHURCH, EMapDescriptorType.MDT_CHURCH, EMapDescriptorType.MDT_CHURCH, EMapDescriptorType.MDT_CHURCH,
				EMapDescriptorType.MDT_AIRPORT, EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_STORE, EMapDescriptorType.MDT_STORE, EMapDescriptorType.MDT_STORE, EMapDescriptorType.MDT_STORE,
				EMapDescriptorType.MDT_STORE, EMapDescriptorType.MDT_STORE, EMapDescriptorType.MDT_STORE, EMapDescriptorType.MDT_STORE,
				EMapDescriptorType.MDT_PUB, EMapDescriptorType.MDT_PUB, EMapDescriptorType.MDT_PUB, EMapDescriptorType.MDT_PUB,
			},
			{},
			{
				"{0AE95D649C3693BA}PrefabsEditable/Fauna/ARGH_AmbientFauna_SDRC_snakes.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}	
}
