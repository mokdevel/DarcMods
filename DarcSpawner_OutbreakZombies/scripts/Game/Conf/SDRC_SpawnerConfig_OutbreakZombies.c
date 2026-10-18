//Helpers SDRC_SpawnerConfig_ZombieGroups.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

//------------------------------------------------------------------------------------------------
class SDRC_SpawnerConfig_OutbreakZombies : SDRC_SpawnerConfig
{
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_SpawnerConfig_OutbreakZombies data = SDRC_SpawnerConfig_OutbreakZombies.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{		
		version = 1;
		author = "darc";
		comment = "Project Outbreak Zombies spawner using multiple Project Outbreak zombie mods.";
		spawnWorldSizeMultiplier = 0;
		spawnCount = 10;
		spawnSetList = {0,0,1,1,2,2,2,2,2,2,3,3, 4,4,5,6,6,6,6,6,7,7};
		//----------------------------------------------------		
		spawnSets.Insert(SpawnSet0_ZombieGroups());	//Z small group in random places
		spawnSets.Insert(SpawnSet1_ZombieGroups());	//Z large group
		spawnSets.Insert(SpawnSet2_ZombieGroups());	//Z groups in cities
		spawnSets.Insert(SpawnSet3_ZombieGroups());	//Z groups in special places
		
		spawnSets.Insert(SpawnSet4_ZombieGroups());	//Outbreak small group in random places
		spawnSets.Insert(SpawnSet5_ZombieGroups());	//Outbreak large group
		spawnSets.Insert(SpawnSet6_ZombieGroups());	//Outbreak groups in cities
		spawnSets.Insert(SpawnSet7_ZombieGroups());	//Outbreak groups in special places
	}	
	
	//Different spawner confs
	//----------------------------------------------------
	
	// --- Z ---
	
	SDRC_SpawnSet SpawnSet0_ZombieGroups()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 0: Z small group in random places",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC, EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{96D499F1705E9E55}Prefabs/Groups/INDFOR/Group_Z_Climber_x4.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}
	
	SDRC_SpawnSet SpawnSet1_ZombieGroups()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 1: Z large group",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC, EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{96D499F1705E9E55}Prefabs/Groups/INDFOR/Group_Z_Climber_x4.et",
				"{A07B0E7A0F100005}Prefabs/Groups/INDFOR/Group_Z.et",
				"{A07B0E7A0F1000D8}Prefabs/Groups/INDFOR/Group_Z_Feral.et",
				"{A07B0E7A0F1000F0}Prefabs/Groups/INDFOR/Group_Z_Sprinter_x16.et",
			},
			50, false, false,
		);
		
		return spawnSet;
	}	
	
	SDRC_SpawnSet SpawnSet2_ZombieGroups()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 2: Z groups in cities",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_NAME_CITY, EMapDescriptorType.MDT_NAME_CITY, 
				EMapDescriptorType.MDT_NAME_VILLAGE, EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_TOWN, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{96D499F1705E9E55}Prefabs/Groups/INDFOR/Group_Z_Climber_x4.et",
				"{A07B0E7A0F100005}Prefabs/Groups/INDFOR/Group_Z.et",
				"{A07B0E7A0F1000D8}Prefabs/Groups/INDFOR/Group_Z_Feral.et",
				"{A07B0E7A0F1000F0}Prefabs/Groups/INDFOR/Group_Z_Sprinter_x16.et",
			},
			50, false, false,
		);
		
		return spawnSet;
	}	

	SDRC_SpawnSet SpawnSet3_ZombieGroups()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 3: Z groups in special places",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_CHAPEL, EMapDescriptorType.MDT_CHAPEL, EMapDescriptorType.MDT_CHAPEL, 
				EMapDescriptorType.MDT_CHURCH, EMapDescriptorType.MDT_CHURCH, EMapDescriptorType.MDT_CHURCH,
				EMapDescriptorType.MDT_PUB, EMapDescriptorType.MDT_PUB,
				EMapDescriptorType.MDT_BASE, EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_AIRPORT, EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_NAME_GENERIC,
			},
			{},
			{
				"{96D499F1705E9E55}Prefabs/Groups/INDFOR/Group_Z_Climber_x4.et",
				"{A07B0E7A0F100005}Prefabs/Groups/INDFOR/Group_Z.et",
				"{A07B0E7A0F1000D8}Prefabs/Groups/INDFOR/Group_Z_Feral.et",
				"{A07B0E7A0F1000F0}Prefabs/Groups/INDFOR/Group_Z_Sprinter_x16.et",
			},
			20, false, false,
		);
		
		return spawnSet;
	}		

	// --- Outbreak ---
	
	SDRC_SpawnSet SpawnSet4_ZombieGroups()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 4: Outbreak small group in random places",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC, EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{F3587B8613CF4D5B}Prefabs/Groups/INDFOR/Outbreak/Group_Ferals_x4.et", 
				"{2F5161D4809D40E6}Prefabs/Groups/INDFOR/Outbreak/Group_Shamblers_x4.et", 
				"{43999BB6B7F149EF}Prefabs/Groups/INDFOR/Outbreak/Group_Sprinters_x4.et", 
				"{6089EC0A215D4DE8}Prefabs/Groups/INDFOR/Outbreak/Group_Sprinters_x8.et", 			
			},
			200, false, false,
		);
		
		return spawnSet;
	}
	
	SDRC_SpawnSet SpawnSet5_ZombieGroups()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 5: Outbreak large group",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC, EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{B2535120A3574A97}Prefabs/Groups/INDFOR/Outbreak/Group_Ferals_x16.et", 
				"{358C4AC3CD784AFF}Prefabs/Groups/INDFOR/Outbreak/Group_MixedHorde_x16.et", 
				"{925119D0E3E843F7}Prefabs/Groups/INDFOR/Outbreak/Group_Shamblers_x16.et", 
			},
			50, false, false,
		);
		
		return spawnSet;
	}	
	
	SDRC_SpawnSet SpawnSet6_ZombieGroups()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 6: Outbreak groups in cities",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_NAME_CITY, EMapDescriptorType.MDT_NAME_CITY, 
				EMapDescriptorType.MDT_NAME_VILLAGE, EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_TOWN, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{F3587B8613CF4D5B}Prefabs/Groups/INDFOR/Outbreak/Group_Ferals_x4.et", "{F3587B8613CF4D5B}Prefabs/Groups/INDFOR/Outbreak/Group_Ferals_x4.et", 
				"{2F5161D4809D40E6}Prefabs/Groups/INDFOR/Outbreak/Group_Shamblers_x4.et", "{2F5161D4809D40E6}Prefabs/Groups/INDFOR/Outbreak/Group_Shamblers_x4.et", 
				"{43999BB6B7F149EF}Prefabs/Groups/INDFOR/Outbreak/Group_Sprinters_x4.et", "{43999BB6B7F149EF}Prefabs/Groups/INDFOR/Outbreak/Group_Sprinters_x4.et", 
				"{6089EC0A215D4DE8}Prefabs/Groups/INDFOR/Outbreak/Group_Sprinters_x8.et", "{6089EC0A215D4DE8}Prefabs/Groups/INDFOR/Outbreak/Group_Sprinters_x8.et", 
				"{B2535120A3574A97}Prefabs/Groups/INDFOR/Outbreak/Group_Ferals_x16.et", 
				"{358C4AC3CD784AFF}Prefabs/Groups/INDFOR/Outbreak/Group_MixedHorde_x16.et", 
				"{925119D0E3E843F7}Prefabs/Groups/INDFOR/Outbreak/Group_Shamblers_x16.et", 
			},
			50, false, false,
		);
		
		return spawnSet;
	}	

	SDRC_SpawnSet SpawnSet7_ZombieGroups()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 7: Outbreak groups in special places",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_CHAPEL, EMapDescriptorType.MDT_CHAPEL, EMapDescriptorType.MDT_CHAPEL, 
				EMapDescriptorType.MDT_CHURCH, EMapDescriptorType.MDT_CHURCH, EMapDescriptorType.MDT_CHURCH,
				EMapDescriptorType.MDT_PUB, EMapDescriptorType.MDT_PUB,
				EMapDescriptorType.MDT_BASE, EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_AIRPORT, EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_NAME_GENERIC,
			},
			{},
			{
				"{F3587B8613CF4D5B}Prefabs/Groups/INDFOR/Outbreak/Group_Ferals_x4.et", 
				"{2F5161D4809D40E6}Prefabs/Groups/INDFOR/Outbreak/Group_Shamblers_x4.et", 
				"{43999BB6B7F149EF}Prefabs/Groups/INDFOR/Outbreak/Group_Sprinters_x4.et", 
				"{6089EC0A215D4DE8}Prefabs/Groups/INDFOR/Outbreak/Group_Sprinters_x8.et", 			
			},
			20, false, false,
		);
		
		return spawnSet;
	}			
	
}
