//Mission SDRC_HvtItem_010.c

//------------------------------------------------------------------------------------------------
/*!
*/

//------------------------------------------------------------------------------------------------
class SDRC_HvtItem_010_JsonApi : SDRC_HvtItemJsonApi
{
	private const string DC_MISSIONCONFIG_FILE_HVTITEM_010 = "dc_missionConfig_HvtItem_010.json";

	//------------------------------------------------------------------------------------------------
	void SDRC_HvtItem_010_JsonApi(string fileName = "")
	{		
		SetFileName(DC_MISSIONCONFIG_FILE_HVTITEM_010);
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		//Default
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		conf.missionList = {10};
		//Mission specific		
		//----------------------------------------------------
		conf.subMissions.Insert(HvtItem010());				
	};
	
	//----------------------------------------------------
	SDRC_HvtItem HvtItem010()
	{
		ref SDRC_HvtItem hvtItem = new SDRC_HvtItem();
		hvtItem.general.Set(
			10, "index 10: Destroy silo",
			{"0 0 0"}, 20,
			{},
			"any",
			"Silo near to %l",
			"The materials inside shall not end up at the hands of the enemy.",
			SDRC_EMissionWinCondition.HVT_DESTROY_ITEM,
			"The silo is now just a bunch bricks.",
			"Material in the silo was secured by the enemy.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HVTITEM_MAP,		
			SDRC_EMissionDifficulty.NORMAL,
			0
		);
		hvtItem.ai.Set(
			{2, 4},
			{"G_ADMIN", "G_LIGHT", "G_LIGHT", "G_LIGHT", "G_HEAVY"},
			50, 1.0,
			{10, 200},
			SDRC_EWaypointGenerationType.RADIUS,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);		
		hvtItem.targetIdx = 5;
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", "WEAPON_RIFLE", 
				"UTIL_ATTACHMENT", "UTIL_OPTIC",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", 
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
			};
		loot.Set(0.5, lootItems);
		hvtItem.loot = loot;
		
		// Loot Crate (first position)
		SDRC_Structure item_0 = new SDRC_Structure;
		item_0.Set(
		    "{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
		    "231.897 1 93.097",
		    "0 15.421 0"
		);
		hvtItem.campItems.Insert(item_0);
		
		// Concrete Pipe Short Pile
		SDRC_Structure item_1 = new SDRC_Structure;
		item_1.Set(
		    "{05A379C7AF6B9F0E}PrefabLibrary/Props/Construction/ConcretePipe_01_Short_Pile.et",
		    "236.217 1 83.768",
		    "0 68.186 0"
		);
		hvtItem.campItems.Insert(item_1);
		
		// Trash Bin
		SDRC_Structure item_2 = new SDRC_Structure;
		item_2.Set(
		    "{0B141EBFAA428804}PrefabLibrary/Props/Garbage/TrashBin_02/TrashBin_02_patched.et",
		    "233.652 1 88.266",
		    "0 179.995 0"
		);
		hvtItem.campItems.Insert(item_2);
		
		// Gas Welder
		SDRC_Structure item_3 = new SDRC_Structure;
		item_3.Set(
		    "{1C550797FEBCB68F}PrefabLibrary/Props/Industrial/GasWelder_01.et",
		    "239.7 1 90.486",
		    "0 -30.634 0"
		);
		hvtItem.campItems.Insert(item_3);
		
		// Electric Cabinet
		SDRC_Structure item_4 = new SDRC_Structure;
		item_4.Set(
		    "{34B95ECB9BCFE6A5}PrefabLibrary/Props/Infrastructure/ElectricCabinet_01.et",
		    "235.905 0 91.037",
		    "0 -180 0"
		);
		hvtItem.campItems.Insert(item_4);
		
		// Silo
		SDRC_Structure item_5 = new SDRC_Structure;
		item_5.Set(
		    "{4D100F180B3EFEC1}Prefabs/Structures/Industrial/Containers/Silos/Silo_01/Silo_01.et",
		    "236.7 0 92.825"
		);
		hvtItem.campItems.Insert(item_5);
		
		// Sandbag Wall USSR
		SDRC_Structure item_6 = new SDRC_Structure;
		item_6.Set(
		    "{82666A967534A109}PrefabsEditable/Auto/Compositions/Misc/SubCompositions/E_Sandbag_Wall_USSR_01.et",
		    "230.493 1 90.616",
		    "0 -102.913 0"
		);
		hvtItem.campItems.Insert(item_6);
		
		// Sandbag Wall Solid group
		SDRC_Structure item_7 = new SDRC_Structure;
		item_7.Set(
		    "{9C9C4BED9E19C374}Prefabs/Props/Military/Sandbags/Sandbag_01_wall_solid_burlap.et",
		    "233.871 1 96.5"
		);
		hvtItem.campItems.Insert(item_7);
		
		SDRC_Structure item_8 = new SDRC_Structure;
		item_8.Set(
		    "{9C9C4BED9E19C374}Prefabs/Props/Military/Sandbags/Sandbag_01_wall_solid_burlap.et",
		    "231.741 1 95.613",
		    "0 -39.979 0"
		);
		hvtItem.campItems.Insert(item_8);
		
		SDRC_Structure item_9 = new SDRC_Structure;
		item_9.Set(
		    "{9C9C4BED9E19C374}Prefabs/Props/Military/Sandbags/Sandbag_01_wall_solid_burlap.et",
		    "242.434 1 85.086",
		    "0 -65.317 0"
		);
		hvtItem.campItems.Insert(item_9);
		
		SDRC_Structure item_10 = new SDRC_Structure;
		item_10.Set(
		    "{9C9C4BED9E19C374}Prefabs/Props/Military/Sandbags/Sandbag_01_wall_solid_burlap.et",
		    "240.733 1 83.284",
		    "0 -29.609 0"
		);
		hvtItem.campItems.Insert(item_10);
		
		// Sandbag Wall Burlap USSR
		SDRC_Structure item_11 = new SDRC_Structure;
		item_11.Set(
		    "{AACF7C732B82F6B2}PrefabsEditable/Auto/Compositions/Misc/FreeRoamBuilding/E_SandbagWallBurlap_S_USSR_01.et",
		    "241.156 1 95.904",
		    "0 47.28 0"
		);
		hvtItem.campItems.Insert(item_11);
		
		// Metal Container
		SDRC_Structure item_12 = new SDRC_Structure;
		item_12.Set(
		    "{B292AEB29FA68532}PrefabLibrary/Props/Construction/MetalContainer_01.et",
		    "241.622 1 90.374",
		    "0 23.296 0"
		);
		hvtItem.campItems.Insert(item_12);
		
		// Sandbag Wall Solid Burlap USSR (with nested entity)
		SDRC_Structure item_13 = new SDRC_Structure;
		item_13.Set(
		    "{BBA9E7B2D06B7E3E}PrefabsEditable/Auto/Compositions/Misc/FreeRoamBuilding/E_SandbagWallSolidBurlap_S_USSR_01.et",
		    "238.244 1 97.124"
		);
		hvtItem.campItems.Insert(item_13);
		
/*		// Nested entity for item_13
		SDRC_Structure item_13_child = new SDRC_Structure;
		item_13_child.Set(
		    "5D70BA356CD2E641",
		    "0.605 0 -0.28"
		);
		item_13.children.Insert(item_13_child);*/
		
		// Brick Pile Ruin
		SDRC_Structure item_14 = new SDRC_Structure;
		item_14.Set(
		    "{CCB19FE7B8422989}PrefabLibrary/Structures/Ruins/HouseRuin_01_BrickPile_Big.et",
		    "243.369 0 89.788",
		    "0 144.595 0"
		);
		hvtItem.campItems.Insert(item_14);
		
		// Sandbag Wall Sign USSR (with nested entity)
		SDRC_Structure item_15 = new SDRC_Structure;
		item_15.Set(
		    "{CD37561561F93B58}PrefabsEditable/Auto/Compositions/Misc/SubCompositions/E_Sandbag_Wall_Sign_USSR_01.et",
		    "232.63 1 87.308",
		    "0 -140.348 0"
		);
		hvtItem.campItems.Insert(item_15);
		
/*		// Nested entity for item_15
		SDRC_Structure item_15_child = new SDRC_Structure;
		item_15_child.Set(
		    "5CB0D996E74F360C",
		    "1.113 0 0.034"
		);
		item_15.children.Insert(item_15_child);*/
		
		// Sandbag Wall Burlap
		SDRC_Structure item_16 = new SDRC_Structure;
		item_16.Set(
		    "{CD67070EFAFC28C7}Prefabs/Props/Military/Sandbags/Sandbag_01_wall_burlap.et",
		    "230.637 1 93.455",
		    "0 -79.273 0"
		);
		hvtItem.campItems.Insert(item_16);
		
		// Fuel Pallet
		SDRC_Structure item_17 = new SDRC_Structure;
		item_17.Set(
		    "{FEA6D4F59C34A6C4}PrefabLibrary/Props/Military/PalletFuel_01.et",
		    "236.654 1 99.285",
		    "0 33.171 0"
		);
		hvtItem.campItems.Insert(item_17);		
		return hvtItem;
	};			
}