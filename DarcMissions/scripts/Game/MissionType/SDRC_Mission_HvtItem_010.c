//Mission SDRC_HvtItem_010.c

//------------------------------------------------------------------------------------------------
/*!
*/

//------------------------------------------------------------------------------------------------
class SDRC_HvtItemConfig_010 : SDRC_HvtItemConfig
{
	private const string DC_MISSIONCONFIG_FILE_HVTITEM_010 = "dc_missionConfig_HvtItem_010.json";

	//------------------------------------------------------------------------------------------------
	//This will setup the filename correctly to create the file
	static string GetFileName()
	{		
		return DC_MISSIONCONFIG_FILE_HVTITEM_010;
	}	
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_HvtItemConfig_010 data = SDRC_HvtItemConfig_010.Cast(T);
		return saveContext.WriteValue("", data);
	}	
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		missionType = SDRC_EMissionType.HVTITEM;
		
		//Default
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		missionList = {10, 11, 12};
		//Mission specific		
		//----------------------------------------------------
		subMissions.Clear();
		subMissions.Insert(HvtItem010());				
		subMissions.Insert(HvtItem011());				
		subMissions.Insert(HvtItem012());				
	};
	
	//----------------------------------------------------
	SDRC_HvtItem HvtItem010()
	{
		ref SDRC_HvtItem hvtItem = new SDRC_HvtItem();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Silo near to %l",
			"The materials inside shall not end up at the hands of the enemy.",
			"The silo is now just a bunch bricks.",
			"Material in the silo was secured by the enemy.",);
		hvtItem.general.Set(
			10, "index 10: Destroy silo",
			{"0 0 0"}, 20,
			{},
			"any",
			{message},
			SDRC_EMissionWinCondition.HVT_DESTROY_ITEM,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HVTITEM_MAP,		
			{SDRC_EDifficulty.RANDOM},
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

		ref SDRC_MissionConfigQrf qrf = new SDRC_MissionConfigQrf();		
		qrf.Set(
			{0, 2, 6, 7, 31, 32,}, SDRC_EMissionSuccess.WIN,
			0.35, {30, 240}
		);
		hvtItem.qrf = qrf;		
				
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
		    "{3C5ECB01F0CA19B0}Prefabs/Structures/Industrial/Containers/Silos/Silo_01/HvtSilo_01.et",
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
		
		// Metal Container - No RplComponent!
/*		SDRC_Structure item_12 = new SDRC_Structure;
		item_12.Set(
		    "{B292AEB29FA68532}PrefabLibrary/Props/Construction/MetalContainer_01.et",
		    "241.622 1 90.374",
		    "0 23.296 0"
		);
		hvtItem.campItems.Insert(item_12);*/
		
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
	
	//----------------------------------------------------
	SDRC_HvtItem HvtItem011()
	{
		ref SDRC_HvtItem hvtItem = new SDRC_HvtItem();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Soup kitchen near %l",
			"Make sure the enemy stays hungry.",
			"No cooking for the enemy.",
			"Bellies full, the enemy left.",);
		hvtItem.general.Set(
			11, "index 11: Destroy kitchen",
			{"0 0 0"}, 20,
			{},
			"any",
			{message},
			SDRC_EMissionWinCondition.HVT_DESTROY_ITEM,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HVTITEM_MAP,		
			{SDRC_EDifficulty.RANDOM},
			0
		);
		hvtItem.ai.Set(
			{1, 3},
			{"G_ADMIN", "G_SPECIAL", "G_LIGHT", "G_HEAVY"},
			50, 1.0,
			{10, 200},
			SDRC_EWaypointGenerationType.RADIUS,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);		
		hvtItem.targetIdx = 12;
		
		ref SDRC_MissionConfigQrf qrf = new SDRC_MissionConfigQrf();		
		qrf.Set(
			{0, 1, 2, 3, 4, }, SDRC_EMissionSuccess.WIN,
			0.35, {30, 240}
		);
		hvtItem.qrf = qrf;		
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE_BIG", "WEAPON_LAUNCHER", 
				"UTIL_ATTACHMENT", "UTIL_OPTIC",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", 
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_BAG", "GEAR_BAG", "GEAR_BAG", 
			};
		loot.Set(0.5, lootItems);
		hvtItem.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et",
		    "237.735 1 32.544"
		);
		hvtItem.campItems.Insert(item_0);

		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{199D1D4F8F75A122}PrefabsEditable/Auto/Props/Military/Camps/E_ConstructionMaterialPacked_WoodPlanks_01_US.et",
		    "235.451 1 40.67",
		    "0 38.41 0"
		);
		hvtItem.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{27158976295DB629}PrefabsEditable/Auto/Props/Garbage/Cardboard/E_Cardboard_Pile_06.et",
		    "231.764 1 30.967",
		    "0 44.15 0"
		);
		hvtItem.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{33748AC23F947B6E}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_round_burlap.et",
		    "227.737 1 36.194",
		    "0 -52.274 0"
		);
		hvtItem.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{4862CD3623FE4E31}PrefabsEditable/Auto/Props/Military/Furniture/E_TableMilitary_USSR_01.et",
		    "240.345 1 31.049",
		    "0 53.108 0"
		);
		hvtItem.campItems.Insert(item_4);
		
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{496DDD80744F2C44}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_single_burlap.et",
		    "233.152 1 34.887",
		    "0 -16.817 0"
		);
		hvtItem.campItems.Insert(item_5);
		
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{496DDD80744F2C44}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_single_burlap.et",
		    "234.886 1 37.186",
		    "0 -43.944 0"
		);
		hvtItem.campItems.Insert(item_6);
		
		ref SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
		    "{496DDD80744F2C44}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_single_burlap.et",
		    "236.072 1 36.082",
		    "0 12.438 0"
		);
		hvtItem.campItems.Insert(item_7);
		
		ref SDRC_Structure item_8 = new SDRC_Structure();
		item_8.Set(
		    "{58D920FC3C3BEA9A}PrefabsEditable/Auto/Props/Garbage/Military/Food/E_GarbageFoodMilitaryUSSR_01.et",
		    "240.643 1.898 30.882"
		);
		hvtItem.campItems.Insert(item_8);
		
		ref SDRC_Structure item_9 = new SDRC_Structure();
		item_9.Set(
		    "{8893B34565777A0E}PrefabsEditable/Auto/Props/Forest/WoodPile/E_WoodPile_02_A.et",
		    "228.254 1 29.847",
		    "0 43.147 0"
		);
		hvtItem.campItems.Insert(item_9);
		
		ref SDRC_Structure item_10 = new SDRC_Structure();
		item_10.Set(
		    "{8BABAE6E7F9B3E39}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_bunker_burlap.et",
		    "234.969 1 31.503"
		);
		hvtItem.campItems.Insert(item_10);
		
		ref SDRC_Structure item_11 = new SDRC_Structure();
		item_11.Set(
		    "{8CC739F5E368DEF5}PrefabsEditable/Auto/Props/Garbage/Bins/E_TrashBin_02_patched.et",
		    "237.538 1 29.633"
		);
		hvtItem.campItems.Insert(item_11);
		
		ref SDRC_Structure item_12 = new SDRC_Structure();
		item_12.Set(
		    "{2DCFC77703348481}Prefabs/Props/Military/HvtFieldKitchenTrailerUSSR_01.et",
		    "230.223 1 32.488",
		    "0 -29.567 0"
		);
		hvtItem.campItems.Insert(item_12);
		
		ref SDRC_Structure item_13 = new SDRC_Structure();
		item_13.Set(
		    "{AC5172F00941C060}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_wall_burlap.et",
		    "230.204 1 37.29"
		);
		hvtItem.campItems.Insert(item_13);
		
		ref SDRC_Structure item_14 = new SDRC_Structure();
		item_14.Set(
		    "{AEDDF16F997CAE6C}PrefabsEditable/Auto/Props/Garbage/Cardboard/E_Cardboard_Pile_04.et",
		    "229.853 1 30.142"
		);
		hvtItem.campItems.Insert(item_14);
		
		ref SDRC_Structure item_15 = new SDRC_Structure();
		item_15.Set(
		    "{BCC37CB613A921AF}PrefabsEditable/Auto/Props/Military/Furniture/E_ChairMilitary_US_02.et",
		    "239.997 1 32.052",
		    "0 172.935 0"
		);
		hvtItem.campItems.Insert(item_15);
		
		ref SDRC_Structure item_16 = new SDRC_Structure();
		item_16.Set(
		    "{BCC37CB613A921AF}PrefabsEditable/Auto/Props/Military/Furniture/E_ChairMilitary_US_02.et",
		    "240.502 1 29.857",
		    "0 -18.034 0"
		);
		hvtItem.campItems.Insert(item_16);
		
		ref SDRC_Structure item_17 = new SDRC_Structure();
		item_17.Set(
		    "{BCC37CB613A921AF}PrefabsEditable/Auto/Props/Military/Furniture/E_ChairMilitary_US_02.et",
		    "239.216 1 30.798",
		    "0 78.367 0"
		);
		hvtItem.campItems.Insert(item_17);
		
		ref SDRC_Structure item_18 = new SDRC_Structure();
		item_18.Set(
		    "{C2681EC98CAB2B3A}PrefabsEditable/Auto/Props/Military/Camps/E_WeaponRackCabinet_01.et",
		    "238.546 1 36.23",
		    "0 -176.301 0"
		);
		hvtItem.campItems.Insert(item_18);
		
		ref SDRC_Structure item_19 = new SDRC_Structure();
		item_19.Set(
		    "{C8A95738F4ED0644}PrefabsEditable/Auto/Props/Military/Sandbags/Deployable/E_Sandbag_01_round_high_burlap_Deployable.et",
		    "227.185 1 31.807",
		    "0 -108.143 0"
		);
		hvtItem.campItems.Insert(item_19);
		
		ref SDRC_Structure item_20 = new SDRC_Structure();
		item_20.Set(
		    "{C8A95738F4ED0644}PrefabsEditable/Auto/Props/Military/Sandbags/Deployable/E_Sandbag_01_round_high_burlap_Deployable.et",
		    "234.407 1 26.676",
		    "0 -160.711 0"
		);
		hvtItem.campItems.Insert(item_20);
		
		ref SDRC_Structure item_21 = new SDRC_Structure();
		item_21.Set(
		    "{C8A95738F4ED0644}PrefabsEditable/Auto/Props/Military/Sandbags/Deployable/E_Sandbag_01_round_high_burlap_Deployable.et",
		    "239.469 1 26.719",
		    "0 138.461 0"
		);
		hvtItem.campItems.Insert(item_21);
		
		ref SDRC_Structure item_22 = new SDRC_Structure();
		item_22.Set(
		    "{D2299274C13B1169}PrefabsEditable/Auto/Props/Military/Sandbags/E_Sandbag_01_wall_solid_burlap.et",
		    "238.316 1 36.922"
		);
		hvtItem.campItems.Insert(item_22);
		
		ref SDRC_Structure item_23 = new SDRC_Structure();
		item_23.Set(
		    "{E2B6B200FF6C86A3}PrefabsEditable/Auto/Props/Garbage/Generic/E_Litter_01_small.et",
		    "238.431 1 29.69"
		);
		hvtItem.campItems.Insert(item_23);
		
		ref SDRC_Structure item_24 = new SDRC_Structure();
		item_24.Set(
		    "{E6AE368A342034CB}PrefabsEditable/Auto/Props/Military/Fortification/E_CzechHedgehog_01_rusty.et",
		    "243.498 0 32.148"
		);
		hvtItem.campItems.Insert(item_24);
		
		ref SDRC_Structure item_25 = new SDRC_Structure();
		item_25.Set(
		    "{E6AE368A342034CB}PrefabsEditable/Auto/Props/Military/Fortification/E_CzechHedgehog_01_rusty.et",
		    "241.986 0 34.283",
		    "0 -40.091 0"
		);
		hvtItem.campItems.Insert(item_25);
		
		ref SDRC_Structure item_26 = new SDRC_Structure();
		item_26.Set(
		    "{FF3ED8B26C05A940}PrefabsEditable/Auto/Props/Military/Camps/E_LanternMilitary_US_01.et",
		    "240.281 1.899 31.122"
		);
		hvtItem.campItems.Insert(item_26);
		
		return hvtItem;
	};
	
	//----------------------------------------------------
	SDRC_HvtItem HvtItem012()
	{
		ref SDRC_HvtItem hvtItem = new SDRC_HvtItem();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Radar near %l",
			"Silence the radar by destroying the transformer.",
			"Enemy eyes are now shut.",
			"Radar commmunication working as enemy wanted.",);
		hvtItem.general.Set(
			12, "index 12: Destroy transformer",
			{"0 0 0"}, 20,
			{},
			"any",
			{message},
			SDRC_EMissionWinCondition.HVT_DESTROY_ITEM,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HVTITEM_MAP,		
			{SDRC_EDifficulty.RANDOM},
			0
		);
		hvtItem.ai.Set(
			{3, 4},
			{"G_SMALL", "G_SPECIAL", "G_SMALL", "G_SMALL"},
			50, 1.0,
			{10, 250},
			SDRC_EWaypointGenerationType.RADIUS,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);		
		hvtItem.targetIdx = 5;
		
		ref SDRC_MissionConfigQrf qrf = new SDRC_MissionConfigQrf();		
		qrf.Set(
			{0, 30, 31, 32, 60, }, SDRC_EMissionSuccess.WIN,
			0.35, {30, 240}
		);
		hvtItem.qrf = qrf;		
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_MG", "WEAPON_SHOTGUN", 
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", 
				"UTIL_ATTACHMENT", "UTIL_OPTIC",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", 
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
				"CLOTHING_HEADGEAR", "CLOTHING_HEADGEAR", 
			};
		loot.Set(0.5, lootItems);
		hvtItem.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{D15A294D5138ADFF}Prefabs/Props/Military/AmmoBoxes/US/LootEquipmentBoxWooden_Equipment_01_US.et",
		    "236.56 1.309 13.497",
		    "0 90 0"
		);
		hvtItem.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{11153A8B7A2F5D20}Prefabs/Structures/Military/Antennas/Antenna_BARS_01/Antenna_BARS_01.et",
		    "242.397 0 11.532"
		);
		hvtItem.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{18F341CBFAA3F78D}Prefabs/Structures/Infrastructure/Power/TransformerStation110kV_E_01/HDOCoil110kV_E_01.et",
		    "248.275 0 11.055"
		);
		hvtItem.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{48BD7D41CEBA6E0D}Prefabs/Structures/Military/Sirens/SirenMilitary_E_01/SirenMilitary_E_01.et",
		    "248.781 0 13.367"
		);
		hvtItem.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{504D8F0DB6B637EE}Prefabs/Structures/Signs/Large/SignLarge_01_PowerPlant.et",
		    "246.387 1 15.151"
		);
		hvtItem.campItems.Insert(item_4);
		
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{A5A186A2B8B07018}Prefabs/Structures/Infrastructure/Power/TransformerStation110kV_E_01/HvtTransformer110kV_E_01_scaled50_unplugged.et",
		    "245.283 0 11.717"
		);
		hvtItem.campItems.Insert(item_5);
		
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{D445D04331B6809E}Prefabs/Structures/Industrial/Houses/PortableCabin_E_01/PortableCabin_E_01_white.et",
		    "237.828 0 12.826"
		);
		hvtItem.campItems.Insert(item_6);
		
		return hvtItem;
	};		
}