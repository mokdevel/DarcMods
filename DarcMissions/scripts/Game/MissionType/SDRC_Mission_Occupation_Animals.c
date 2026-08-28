//Mission SDRC_Occupation_010_horror.c

//------------------------------------------------------------------------------------------------
/*!
*/

//------------------------------------------------------------------------------------------------
class SDRC_OccupationConfig_Animals : SDRC_OccupationConfig
{
	const string DC_MISSIONCONFIG_FILE_OCCUPATION_HORROR = "dc_missionConfig_Occupation_Animals.json";
	
	//------------------------------------------------------------------------------------------------
	//This will setup the filename correctly to create the file
	static string GetFileName()
	{		
		return DC_MISSIONCONFIG_FILE_OCCUPATION_HORROR;
	}	

	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_OccupationConfig_Animals data = SDRC_OccupationConfig_Animals.Cast(T);
		return saveContext.WriteValue("", data);
	}		
		
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		missionType = SDRC_EMissionType.OCCUPATION;
		
		//Default		
		disableArsenal = true;
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		missionList = {0,1,2};				
		//Mission specific		
		//----------------------------------------------------
		subMissions.Clear();
		subMissions.Insert(Occupation_A_0());				
		subMissions.Insert(Occupation_A_1());				
		subMissions.Insert(Occupation_A_2());				
		//subMissions.Insert(Occupation_A_3());				
	};
	
	//----------------------------------------------------
	SDRC_Camp Occupation_A_0()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Poachers near %l",
			"Poachers are hunting a stag.",
			"Humans, the enemies of the nature are cleared.",
			"Oh deer, you failed.",);
		occupation.general.modList.Insert("$BARFrenchAnimals:");
		occupation.general.Set(
			0, "index 0: Poachers",
			{"0 0 0"}, 3,
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_FOREST,			
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{1, 1},
			{
			 "G_SMALL",
			},
			50, 1.0,
			{50, 80},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"UTIL_OPTIC", 
				"GEAR_HEADGEAR", "GEAR_HEADGEAR", "GEAR_HEADGEAR",
				"GEAR_HANDWEAR", "GEAR_HANDWEAR", 
				"CLOTHING_UNIFORM", "CLOTHING_UNIFORM", "CLOTHING_UNIFORM", 
			};
		loot.Set(0.9, lootItems);
		occupation.loot = loot;		

		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
			"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
			"20 0 20"
		);
		occupation.campItems.Insert(item_0);		
				
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
			"{D1509D4108A75FF4}Prefabs/Animals/Deer.et",
			"0 0 0"
		);
		occupation.campItems.Insert(item_1);

		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
			"{2C5C9488DD7E937D}Prefabs/Zones/BAR_TerritoryMarker_Deer.et",
			"50 0 0"
		);
		occupation.campItems.Insert(item_2);
				
		return occupation;
	}
	
	//----------------------------------------------------
	SDRC_Camp Occupation_A_1()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Hunter camp near %l",
			"Hunters are looking for meat.",
			"Camp cleared. Time to loot.",
			"Hunters left with a nice catch.",);
		occupation.general.modList.Insert("$BARFrenchAnimals:");
		occupation.general.Set(
			1, "index 1: Deer Hunters",
			{"0 0 0"}, 10,
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_FOREST,			
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{1, 1},
			{
			 "G_SMALL", "G_SNIPER",
			},
			50, 1.0,
			{50, 100},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"UTIL_OPTIC",
				"WEAPON_RIFLE_BIG", "WEAPON_RIFLE_BIG", 
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", 			
			};
		loot.Set(0.6, lootItems);
		occupation.loot = loot;		

		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
			"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
			"58.769 1 150.205"
		);
		occupation.campItems.Insert(item_0);
	
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
			"{39C308BBB5945B85}Prefabs/Props/Military/Furniture/ChairMilitary_US_02.et",
			"55.428 1 148.327",
			"0 61.122 0"
		);
		occupation.campItems.Insert(item_1);
	
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
			"{8BF02124660AB745}PrefabsEditable/Auto/Structures/Military/Camps/TentSmallFIA_01/E_TentSmallFIA_01.et",
			"56.363 1 152.164"
		);
		occupation.campItems.Insert(item_2);
	
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
			"{C9CFDED29542A968}Prefabs/Props/Military/Furniture/CotMilitary_US_01.et",
			"57.644 1 147.257",
			"0 -24.081 0"
		);
		occupation.campItems.Insert(item_3);
	
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
			"{D9842C11742C00CF}Prefabs/Props/Civilian/Fireplace_01.et",
			"56.534 1 149.107"
		);
		occupation.campItems.Insert(item_4);
	
				
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
			"{7E392EFF0B595795}Prefabs/Animals_Groups/Deer_Doe_Group.et",
			"56 0 100"
		);
		occupation.campItems.Insert(item_5);

		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
			"{2C5C9488DD7E937D}Prefabs/Zones/BAR_TerritoryMarker_Deer.et",
			"50 0 100"
		);
		occupation.campItems.Insert(item_6);
				
		return occupation;
	}
	
	//----------------------------------------------------
	SDRC_Camp Occupation_A_2()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Meat packers near %l",
			"Not good, wolves seen near the area.",
			"Loot is yours but look out for wild life.",
			"Packing done.",);
		occupation.general.modList.Insert("$BARFrenchAnimals:");
		occupation.general.Set(
			2, "index 2: Wolves and Hunters",
			{"0 0 0"}, 20,
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_FOREST,			
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{1, 1},
			{
			 "G_RECON",
			},
			50, 1.0,
			{50, 100},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"UTIL_OPTIC",
				"WEAPON_RIFLE_BIG", "WEAPON_RIFLE_BIG", 
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL", 			
			};
		loot.Set(0.6, lootItems);
		occupation.loot = loot;	
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
			"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
			"76.974 1 146.917",
			"0 -15.517 0"
		);
		occupation.campItems.Insert(item_0);
	
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
			"{0765187447161CA5}PrefabsEditable/Auto/Props/Agriculture/Farm/E_HaystackDryer_01.et",
			"76.469 -0 153.561"
		);
		occupation.campItems.Insert(item_1);
	
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
			"{3EC72887AEC40F22}PrefabsEditable/Auto/Props/Furniture/TableOld_01/E_TableOld_01_white.et",
			"78.828 1 152.901",
			"0 49.265 0"
		);
		occupation.campItems.Insert(item_2);
	
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
			"{61F3E9080A364769}PrefabsEditable/Auto/Props/Commercial/ScaleShop_01/E_ScaleShop_01_white.et",
			"79.395 1.9 152.504",
			"0 -133.511 0"
		);
		occupation.campItems.Insert(item_3);
	
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
			"{71ED9316907C2D93}PrefabsEditable/Auto/Props/Civilian/BenchStreet/E_BenchStreet_02.et",
			"78.843 1 148.518",
			"0 -42.813 0"
		);
		occupation.campItems.Insert(item_4);
	
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
			"{833407E494B205DA}PrefabsEditable/Auto/Props/Decorations/E_AnimalHide_01.et",
			"78.739 1.898 152.984",
			"0 -42.731 0"
		);
		occupation.campItems.Insert(item_5);
	
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
			"{D9842C11742C00CF}Prefabs/Props/Civilian/Fireplace_01.et",
			"77.6 1 149.62"
		);
		occupation.campItems.Insert(item_6);
	
		ref SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
			"{EED14401BED18DBF}PrefabsEditable/Auto/Props/Crates/E_BoxWooden_01.et",
			"78.373 1 152.144"
		);
		occupation.campItems.Insert(item_7);
	
		ref SDRC_Structure item_8 = new SDRC_Structure();
		item_8.Set(
			"{FFD8A0190FACB2B2}PrefabsEditable/Auto/Props/Agriculture/Farm/E_HayPile_01.et",
			"74.709 1 149.956"
		);
		occupation.campItems.Insert(item_8);

		ref SDRC_Structure item_9 = new SDRC_Structure();
		item_9.Set(
			"{2274A55C0B226999}Prefabs/Animals/Wolf.et",
			"0 1 50"
		);
		occupation.campItems.Insert(item_9);
		
		ref SDRC_Structure item_10 = new SDRC_Structure();
		item_10.Set(
			"{2274A55C0B226999}Prefabs/Animals/Wolf.et",
			"0 1 60"
		);
		occupation.campItems.Insert(item_10);
		
		ref SDRC_Structure item_11 = new SDRC_Structure();
		item_11.Set(
			"{5BBDC814E9639041}Prefabs/Zones/BAR_TerritoryMarker_Wolf.et",
			"0 1 30"
		);
		occupation.campItems.Insert(item_11);		
		
		return occupation;
	}		
	
	//----------------------------------------------------
	SDRC_Camp Occupation_A_3()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("City wolves in %l",
			"Wolves are scaring the people. Help them!",
			"Roads are safe again.",
			"Nature was stronger this time.",);
		occupation.general.modList.Insert("$BARFrenchAnimals:");
		occupation.general.Set(
			3, "index 3: Wolves in cities",
			{"0 0 0"}, 3,
			{
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_TOWN,	
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{2, 5},
			{
			 "{2274A55C0B226999}Prefabs/Animals/Wolf.et",
			},
			50, 1.0,
			{50, 80},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"UTIL_ATTACHMENT", 
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_HANDWEAR", "GEAR_HANDWEAR", 
				"CLOTHING_UNIFORM", 
			};
		loot.Set(0.9, lootItems);
		occupation.loot = loot;		

		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
			"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
			"20 0 20"
		);
		occupation.campItems.Insert(item_0);		
				
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
			"{5BBDC814E9639041}Prefabs/Zones/BAR_TerritoryMarker_Wolf.et",
			"0 0 0"
		);
		occupation.campItems.Insert(item_1);
								
		return occupation;
	}	
}