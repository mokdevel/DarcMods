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
		comment = "Animal spawner using French Animals mod. ";
		spawnWorldSizeMultiplier = 0;
		spawnCount = 30;
		spawnSetList = {0,0,0,0,0,0,1,1,1,2,2,2,3,3,4,4,9,12,12,13,14,14 ,5,6,6,7,8,10};
		//----------------------------------------------------		
		spawnSets.Insert(SpawnSet0_Animals());	//Deer
		spawnSets.Insert(SpawnSet1_Animals());	//Wolf
		spawnSets.Insert(SpawnSet2_Animals());	//Doe
		spawnSets.Insert(SpawnSet3_Animals());	//Fox
		spawnSets.Insert(SpawnSet4_Animals());	//Rabbit
		//Territories
		spawnSets.Insert(SpawnSet5_Animals());	//Rabbit territory
		spawnSets.Insert(SpawnSet6_Animals());	//Deer territory
		spawnSets.Insert(SpawnSet7_Animals());	//Fox territory
		spawnSets.Insert(SpawnSet8_Animals());	//Wolf territory
		spawnSets.Insert(SpawnSet9_Animals());	//Bear
		spawnSets.Insert(SpawnSet10_Animals());	//Bear territory
		//Do not use these at this stage
		spawnSets.Insert(SpawnSet11_Animals());	//Deer & Doe herd
		spawnSets.Insert(SpawnSet12_Animals());	//Crow
		spawnSets.Insert(SpawnSet13_Animals());	//Crow
		spawnSets.Insert(SpawnSet14_Animals());	//Crow
		//spawnSets.Insert(SpawnSetXX_Animals());	//Animal spawner - do not use for the moment.	
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
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
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
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
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
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
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
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
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
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
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
	
	SDRC_SpawnSet SpawnSet5_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 5: Spawn rabbit territory",
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
				"{BA1181A14A63C725}Prefabs/Zones/BAR_TerritoryMarker_Rabbit.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}		

	SDRC_SpawnSet SpawnSet6_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 6: Spawn deer territory",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{2C5C9488DD7E937D}Prefabs/Zones/BAR_TerritoryMarker_Deer.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}		

	SDRC_SpawnSet SpawnSet7_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 7: Spawn fox territory",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{6C64DC5B5FA4904B}Prefabs/Zones/BAR_TerritoryMarker_Fox.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}		

	SDRC_SpawnSet SpawnSet8_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 8: Spawn wolf territory",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{5BBDC814E9639041}Prefabs/Zones/BAR_TerritoryMarker_Wolf.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}				
	
	SDRC_SpawnSet SpawnSet9_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 9: Spawn bear",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{DE061A93287F06F8}Prefabs/Animals/Bear.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}					
	
	SDRC_SpawnSet SpawnSet10_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 10: Spawn bear territory",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{A7CF77DBCA3EFF20}Prefabs/Zones/BAR_TerritoryMarker_Bear.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}					
	
	SDRC_SpawnSet SpawnSet11_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 11: Deer & Doe herd",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{C995128A47763199}PrefabsEditable/Animals_Groups/E_Deer_Doe_Group.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}

	SDRC_SpawnSet SpawnSet12_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 12: Crows",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_TOWN,
				EMapDescriptorType.MDT_NAME_VILLAGE,
			},
			{},
			{
				"{B7A1C7E192803CFE}Prefabs/Animals/Crow.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}		
	
	SDRC_SpawnSet SpawnSet13_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 13: Multiple crows",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_TOWN,
				EMapDescriptorType.MDT_NAME_VILLAGE,
			},
			{},
			{
				"{07C00BFA28960E0A}PrefabsEditable/Animals_Groups/E_Crow_Group.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}		

	SDRC_SpawnSet SpawnSet14_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 14: Cows",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_NAME_VILLAGE,
			},
			{},
			{
				"{11630E7955BEF10C}PrefabsEditable/Animals/E_Cow.et	",
			},
			200, false, false,
		);
		
		return spawnSet;
	}		
	
	SDRC_SpawnSet SpawnSetXX_Animals()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index XX: Animal spawner",
			false, "DARC_MISSION", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{7A9F719732B83B4A}Prefabs/Animals_Spawner/Animals_Spawner.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}		
}
