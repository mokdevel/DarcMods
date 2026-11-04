//Mission SDRC_HvtItemConfig_Horror.c

//------------------------------------------------------------------------------------------------
/*!
*/

//------------------------------------------------------------------------------------------------
class SDRC_HvtItemConfig_Horror : SDRC_HvtItemConfig
{
	private const string DC_MISSIONCONFIG_FILE_HVTITEM_HORROR = "dc_missionConfig_HvtItem_Horror.json";
	private const int DC_MISSIONCONFIG_FILE_HVTITEM_HORROR_JSONVER = DC_MISSIONCONFIG_FILE_HVTITEM_JSONVER;
	
	//------------------------------------------------------------------------------------------------
	static string GetFileName()
	{		
		return DC_MISSIONCONFIG_FILE_HVTITEM_HORROR;
	}	

	//------------------------------------------------------------------------------------------------
	static int GetFileVersion()
	{		
		return DC_MISSIONCONFIG_FILE_HVTITEM_HORROR_JSONVER;
	}	
		
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_HvtItemConfig_Horror data = SDRC_HvtItemConfig_Horror.Cast(T);
		return saveContext.WriteValue("", data);
	}	
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		missionType = SDRC_EMissionType.HVTITEM;
		
		//Default
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		missionList = {0,1};
		
		#ifndef SDRC_RELEASE
			//missionList = {1};	//DEBUG
		#endif
		
		//Mission specific		
		//----------------------------------------------------
		subMissions.Clear();
		subMissions.Insert(HvtItem_H_0());				
		subMissions.Insert(HvtItem_H_1());				
	};
	
	//----------------------------------------------------
	SDRC_HvtItem HvtItem_H_0()
	{
		ref SDRC_HvtItem hvtItem = new SDRC_HvtItem();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Portal to hell",
			"A portal has opened near %l. Destroy it!",
			"Portal has been closed and the world is a safer place.",
			"Creatures from another dimension have entered your world.",);
		hvtItem.general.modList.Insert("$BallienCreatures:");
		hvtItem.general.Set(
			0, "index 0: Destroy portal",
			{"0 0 0"}, 20,
			{
				EMapDescriptorType.MDT_NAME_TOWN,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL,
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.HVT_DESTROY_ITEM,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_PORTAL_MAP,		
			{SDRC_EDifficulty.RANDOM},
			0
		);
		hvtItem.ai.Set(
			{1, 3},
			{
				"{3AA5A683A32167CC}622120A5448725E3/Prefabs/Groups/Group_Cultists_Horde.et",
				"{39F908C49BEBD995}622120A5448725E3/Prefabs/Groups/Group_Cultists_Medium.et",
				"{68A45BE2E133650E}622120A5448725E3/Prefabs/Groups/Group_Cultists_Light.et",
				"{5D2115CF54B4B5DA}Prefabs/Characters/Shadow.et",
				"{794F9A7B2C1DD64F}Prefabs/Groups/Cultist/Group_Cultist_Warrior_S.et",
				"{641097008A6848AE}Prefabs/Groups/Cultist/Group_Cultist_Stalker.et",		
				"{0C48B680EF4FC708}Prefabs/Characters/FireMage_Cultist.et", "{0C48B680EF4FC708}Prefabs/Characters/FireMage_Cultist.et",
				"{E8C16E36B946D92D}Prefabs/Characters/Warrior_Cultist.et", "{E8C16E36B946D92D}Prefabs/Characters/Warrior_Cultist.et",
			},
			80, 1.0,
			{10, 200},
			SDRC_EWaypointGenerationType.RADIUS,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);		
		hvtItem.targetIdx.Insert(3);
				
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"UTIL_ATTACHMENT", "UTIL_OPTIC",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", 
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
			};
		loot.Set(0.5, lootItems);
		hvtItem.loot = loot;
		
		// Loot Crate (first position)
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{973BB83B14B97689}PrefabsEditable/Auto/Props/Civilian/LootE_SackUniversal_01.et",
		    "21.77 1 34.252",
		    "0 49.786 0"
		);
		hvtItem.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{21D15F6160864CF6}PrefabsEditable/Auto/Structures/Cultural/Statues/E_StatueFIA_01_big.et",
		    "31.779 0.715 33.284"
		);
		hvtItem.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{21D15F6160864CF6}PrefabsEditable/Auto/Structures/Cultural/Statues/E_StatueFIA_01_big.et",
		    "27.524 0.667 33.254"
		);
		hvtItem.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{4BCF7ABAC254083C}Prefabs/Portal/PortalDimension.et",
		    "29.482 0 30.468"
		);
		hvtItem.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{81CDBA11F502E520}PrefabsEditable/Auto/Structures/Cultural/Calvaries/E_CalvaryLarge_02.et",
		    "22.528 0 34.433",
		    "0 56.453 0"
		);
		hvtItem.campItems.Insert(item_4);
		
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{9E25698E084833D3}PrefabsEditable/Auto/Structures/Ruins/HouseRuins/E_HouseRuin_01_BrickPile_Big.et",
		    "29.113 0.292 33.782"
		);
		hvtItem.campItems.Insert(item_5);
		
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{D9423A468CABE765}PrefabsEditable/Auto/Rocks/Granite/E_Granite_Cluster_01.et",
		    "32.13 0 35.376"
		);
		hvtItem.campItems.Insert(item_6);		
		
		return hvtItem;
	};	
	
	//----------------------------------------------------
	SDRC_HvtItem HvtItem_H_1()
	{
		ref SDRC_HvtItem hvtItem = new SDRC_HvtItem();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Portal to hell",
			"Destroy the portal near %l.",
			"The portal gateway is closed.",
			"Your failure has made the world a bit darker.",);
		hvtItem.general.modList.Insert("$BallienCreatures:");
		hvtItem.general.Set(
			1, "index 1: Destroy portal",
			{"0 0 0"}, 30,
			{
				EMapDescriptorType.MDT_NAME_TOWN,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL,
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.HVT_DESTROY_ITEM,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_PORTAL_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		hvtItem.ai.Set(
			{2, 3},
			{
				"{3AA5A683A32167CC}622120A5448725E3/Prefabs/Groups/Group_Cultists_Horde.et",
				"{39F908C49BEBD995}622120A5448725E3/Prefabs/Groups/Group_Cultists_Medium.et",
				"{68A45BE2E133650E}622120A5448725E3/Prefabs/Groups/Group_Cultists_Light.et",
				"{794F9A7B2C1DD64F}Prefabs/Groups/Cultist/Group_Cultist_Warrior_S.et",
				"{641097008A6848AE}Prefabs/Groups/Cultist/Group_Cultist_Stalker.et",
			},
			80, 1.0,
			{10, 200},
			SDRC_EWaypointGenerationType.RADIUS,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);		
		hvtItem.targetIdx.Insert(8);
				
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE",
				"WEAPON_RIFLE_BIG", 
				"WEAPON_HANDGUN",
				"UTIL_ATTACHMENT", "UTIL_OPTIC",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO",
				"ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
			};
		loot.Set(0.5, lootItems);
		hvtItem.loot = loot;
		
		// Loot Crate (first position)
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{973BB83B14B97689}PrefabsEditable/Auto/Props/Civilian/LootE_SackUniversal_01.et",
		    "29.317 1 74.647",
		    "0 0 0"
		);
		hvtItem.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{155C3A8D0CDC2AD6}Prefabs/Props/Cultist/Prop_Cultist_SkullStake.et",
		    "30.733 1 77.345",
		    "0 -145.716 0"
		);
		hvtItem.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{27B95A994D0DC1D3}Prefabs/Props/Cultist/Prop_Cultist_Totem.et",
		    "25.328 1 75.272"
		);
		hvtItem.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{27B95A994D0DC1D3}Prefabs/Props/Cultist/Prop_Cultist_Totem.et",
		    "33.499 1 75.54"
		);
		hvtItem.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{4BCD2240C8EF8473}Prefabs/Props/Cultist/Prop_Cultist_Candles.et",
		    "29.197 1 73.783"
		);
		hvtItem.campItems.Insert(item_4);
		
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{4BCD2240C8EF8473}Prefabs/Props/Cultist/Prop_Cultist_Candles.et",
		    "30.077 1 74.587"
		);
		hvtItem.campItems.Insert(item_5);
		
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{4BCD2240C8EF8473}Prefabs/Props/Cultist/Prop_Cultist_Candles.et",
		    "29.289 1 75.32"
		);
		hvtItem.campItems.Insert(item_6);
		
		ref SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
		    "{4BCD2240C8EF8473}Prefabs/Props/Cultist/Prop_Cultist_Candles.et",
		    "28.364 1 74.526"
		);
		hvtItem.campItems.Insert(item_7);
		
		ref SDRC_Structure item_8 = new SDRC_Structure();
		item_8.Set(
		    "{4BCF7ABAC254083C}Prefabs/Portal/PortalDimension.et",
		    "29.612 0 65.518"
		);
		hvtItem.campItems.Insert(item_8);
		
		ref SDRC_Structure item_9 = new SDRC_Structure();
		item_9.Set(
		    "{56E50470D4FE39A6}Prefabs/Props/Cultist/Prop_Cultist_Altar.et",
		    "29.41 1 77.813",
		    "0 -177.546 0"
		);
		hvtItem.campItems.Insert(item_9);
		
		ref SDRC_Structure item_10 = new SDRC_Structure();
		item_10.Set(
		    "{9564A2C5DE570326}Prefabs/Props/Cultist/Prop_Cultist_Monolith.et",
		    "27.069 1.241 68.747"
		);
		hvtItem.campItems.Insert(item_10);
		
		ref SDRC_Structure item_11 = new SDRC_Structure();
		item_11.Set(
		    "{9564A2C5DE570326}Prefabs/Props/Cultist/Prop_Cultist_Monolith.et",
		    "32.507 1.131 68.632"
		);
		hvtItem.campItems.Insert(item_11);
		
		ref SDRC_Structure item_12 = new SDRC_Structure();
		item_12.Set(
		    "{9564A2C5DE570326}Prefabs/Props/Cultist/Prop_Cultist_Monolith.et",
		    "32.859 1 70.856"
		);
		hvtItem.campItems.Insert(item_12);
		
		ref SDRC_Structure item_13 = new SDRC_Structure();
		item_13.Set(
		    "{9564A2C5DE570326}Prefabs/Props/Cultist/Prop_Cultist_Monolith.et",
		    "33.475 1 73.315"
		);
		hvtItem.campItems.Insert(item_13);
		
		ref SDRC_Structure item_14 = new SDRC_Structure();
		item_14.Set(
		    "{9564A2C5DE570326}Prefabs/Props/Cultist/Prop_Cultist_Monolith.et",
		    "26.256 1 70.914"
		);
		hvtItem.campItems.Insert(item_14);
		
		ref SDRC_Structure item_15 = new SDRC_Structure();
		item_15.Set(
		    "{9564A2C5DE570326}Prefabs/Props/Cultist/Prop_Cultist_Monolith.et",
		    "25.742 1 73.221"
		);
		hvtItem.campItems.Insert(item_15);		
		return hvtItem;
	};						
}