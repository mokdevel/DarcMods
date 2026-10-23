//Helpers SDRC_SpawnerConfig_Creatures.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

//------------------------------------------------------------------------------------------------
class SDRC_SpawnerConfig_OutbreakCreatures : SDRC_SpawnerConfig
{
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_SpawnerConfig_OutbreakCreatures data = SDRC_SpawnerConfig_OutbreakCreatures.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{		
		version = 1;
		author = "darc";
		comment = "Project Outbreak Alien and Mushroom Creatures spawner using multiple Project Outbreak mods.";
		spawnWorldSizeMultiplier = 0;
		spawnCount = 10;
		spawnSetList = {0,
						1,1,
						2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
						3,3,3,3,3,3,
						4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
						5,5,5,5,5,5,
						};
		//----------------------------------------------------		
		spawnSets.Insert(SpawnSet0_Creatures());	//Large creature in the open
		spawnSets.Insert(SpawnSet1_Creatures());	//Huge creature in the open
		spawnSets.Insert(SpawnSet2_Creatures());	//Alien group x4
		
		spawnSets.Insert(SpawnSet3_Creatures());	//Alien group x16
		
		spawnSets.Insert(SpawnSet4_Creatures());	//Mushroom characters
		spawnSets.Insert(SpawnSet5_Creatures());	//Mushroom groups
	}	
	
	//Different spawner confs
	//----------------------------------------------------
	
	// --- Z ---
	
	SDRC_SpawnSet SpawnSet0_Creatures()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 0: Large creature in the open",
			true, "DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP, 
			{
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_NAME_GENERIC,
			},
			{},
			{
				"{7BA789A1FBD1F626}Prefabs/Characters/Alien_Titan.et",
				"{53F054F35FA81F03}Prefabs/Characters/Fungal.et",
				"{F4BED6AF2BF2606B}Prefabs/Characters/Lumenkin.et",
			},
			200, false, false,
		);
		
		return spawnSet;
	}
	
	SDRC_SpawnSet SpawnSet1_Creatures()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 1: Huge creature in the open",
			true, "DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP, 
			{
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_NAME_GENERIC,
			},
			{},
			{
				"{A40B9714611AB46F}Prefabs/Characters/MycelialEmpyrean.et",
			},
			50, false, false,
		);
		
		return spawnSet;
	}	
	
	SDRC_SpawnSet SpawnSet2_Creatures()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 2: Alien group x4",
			true, "DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, 
				EMapDescriptorType.MDT_BASE, EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_AIRPORT, EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_NAME_CITY,  
				EMapDescriptorType.MDT_NAME_TOWN,  
			},
			{},
			{
				"{A0337C20E0EDD092}Prefabs/Groups/Alien_Large_x4.et",
				"{67F624D0FD8F48CB}Prefabs/Groups/EyekinBird_x4.et", 
			},
			50, false, false,
		);
		
		return spawnSet;
	}	

	SDRC_SpawnSet SpawnSet3_Creatures()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 3: Alien group x16",
			true, "DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, 
				EMapDescriptorType.MDT_BASE, EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_AIRPORT, EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_NAME_CITY,  
				EMapDescriptorType.MDT_NAME_TOWN,  
			},
			{},
			{
				"{81AEB80877BADDE6}Prefabs/Groups/Alien_x16.et",
				"{13F4F1FC90544E2E}Prefabs/Groups/EyekinBird_x16.et", 
			},
			50, false, false,
		);
		
		return spawnSet;
	}		

	SDRC_SpawnSet SpawnSet4_Creatures()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 4: Mushroom characters",
			true, "DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_MUSHROOM_MAP_ROUND, 
			{
				EMapDescriptorType.MDT_FOREST,
				EMapDescriptorType.MDT_NAME_GENERIC, EMapDescriptorType.MDT_NAME_GENERIC,
				EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL, EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_VILLAGE, 
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			{},
			{
				"{820CEDB5CCAD73C0}Prefabs/Characters/Fantasy.et", 
				"{820CEDB5CCAD73C0}Prefabs/Characters/Fantasy.et", 
				"{E66DB026EED16810}Prefabs/Characters/Mushroom.et", 
				"{E66DB026EED16810}Prefabs/Characters/Mushroom.et", 
				"{E66DB026EED16810}Prefabs/Characters/Mushroom.et", 
				"{49403EB7AF31C0FC}Prefabs/Characters/Mushroom_Demon.et", 
			},
			200, false, false,
		);
		
		return spawnSet;
	}
	
	SDRC_SpawnSet SpawnSet5_Creatures()
	{
		SDRC_SpawnSet spawnSet = new SDRC_SpawnSet();
		spawnSet.Set(		
			"index 5: Mushroom groups",
			true, "DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_MUSHROOM_MAP_ROUND, 
			{
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, 
				EMapDescriptorType.MDT_BASE, EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_AIRPORT, EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_NAME_CITY,  
				EMapDescriptorType.MDT_NAME_TOWN,  
			},
			{},
			{
				"{D970230159A94601}Prefabs/Groups/Group_Dewcap_x16.et",
				"{A72D4F9E7A3349B2}Prefabs/Groups/Group_Dewcap_x8.et", "{A72D4F9E7A3349B2}Prefabs/Groups/Group_Dewcap_x8.et", "{A72D4F9E7A3349B2}Prefabs/Groups/Group_Dewcap_x8.et", "{A72D4F9E7A3349B2}Prefabs/Groups/Group_Dewcap_x8.et", 
				"{53FA67741AF54DE5}Prefabs/Groups/Group_Sporewarden_x16.et",
				"{9D1F1FED9579446F}Prefabs/Groups/Group_Sporewarden_x8.et", "{9D1F1FED9579446F}Prefabs/Groups/Group_Sporewarden_x8.et", "{9D1F1FED9579446F}Prefabs/Groups/Group_Sporewarden_x8.et", "{9D1F1FED9579446F}Prefabs/Groups/Group_Sporewarden_x8.et",			
			},
			150, false, false,
		);
		
		return spawnSet;
	}		
}
