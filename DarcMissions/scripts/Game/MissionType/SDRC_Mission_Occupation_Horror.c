//Mission SDRC_Occupation_010_horror.c

//------------------------------------------------------------------------------------------------
/*!
*/

//------------------------------------------------------------------------------------------------
class SDRC_OccupationConfig_Horror : SDRC_OccupationConfig
{
	private const string DC_MISSIONCONFIG_FILE_OCCUPATION_HORROR = "dc_missionConfig_Occupation_Horror.json";
	private const int DC_MISSIONCONFIG_FILE_OCCUPATION_HORROR_JSONVER = DC_MISSIONCONFIG_FILE_OCCUPATION_JSONVER;
	
	//------------------------------------------------------------------------------------------------
	static string GetFileName()
	{		
		return DC_MISSIONCONFIG_FILE_OCCUPATION_HORROR;
	}	

	//------------------------------------------------------------------------------------------------
	static int GetFileVersion()
	{		
		return DC_MISSIONCONFIG_FILE_OCCUPATION_HORROR_JSONVER;
	}	
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_OccupationConfig_Horror data = SDRC_OccupationConfig_Horror.Cast(T);
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
		missionList = {0,1,2,3,4};

		#ifndef SDRC_RELEASE
			//missionList = {4};	//DEBUG
		#endif
		
		//Mission specific		
		//----------------------------------------------------
		subMissions.Clear();
		subMissions.Insert(Occupation_H_0());				
		subMissions.Insert(Occupation_H_1());				
		subMissions.Insert(Occupation_H_2());				
		subMissions.Insert(Occupation_H_3());				
		subMissions.Insert(Occupation_H_4());
	};
	
	//----------------------------------------------------
	SDRC_Camp Occupation_H_0()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Creatures near %l",
			"Avoid the location. No loot available.",
			"Spawns from hell are dead!",
			"Are you scared of a few ghosts..?",);
		occupation.general.modList.Insert("$BallienCreatures:");
		occupation.general.Set(
			0, "index 0: Zombie and Demon characters.",
			{"0 0 0"}, 3,
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			{"BALLIEN_BC_FACTION"},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{1, 8},
			{
				"C_DEMON", "C_DEMON", "C_DEMON", 
				"C_BEASTS", "C_BEASTS", 
				"C_DEMON_BOSS",
			},
			50, 1.0,
			{50, 100},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
	
		return occupation;
	}
	
	//----------------------------------------------------
	SDRC_Camp Occupation_H_1()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Zombies near %l",
			"Braaaainss.....",
			"The living dead are just dead now.",
			"You were tasty.",);
		occupation.general.modList.Insert("$BaconZombies:");
		occupation.general.Set(
			1, "index 1: Zombie hordes",
			{"0 0 0"}, 3,
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{"BACON_622120A5448725E3_FACTION"},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_ZOMBIE_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{1, 4},
			{
				"G_ZOMBIE_SMALL", "G_ZOMBIE_SMALL", "G_ZOMBIE_SMALL", 
				"G_ZOMBIE_MEDIUM", 
				"G_ZOMBIE_LARGE"
			},
			50, 0.3,
			{50, 120},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
	
		return occupation;
	}
	
	//----------------------------------------------------
	SDRC_Camp Occupation_H_2()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Horde of zombies seen near %l",
			"Avoid getting yourself eaten.",
			"The dead shall stay dead.",
			"Oh the horrors. You were slain.",);
		occupation.general.modList.Insert("$BaconZombies:");
		occupation.general.Set(
			2, "index 2: Zombies",
			{"0 0 0"}, 3,
			{
				EMapDescriptorType.MDT_CHURCH,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{"BACON_622120A5448725E3_FACTION"},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_ZOMBIE_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{1, 4},
			{
				"G_ZOMBIE_SMALL", "G_ZOMBIE_SMALL", 
				"G_ZOMBIE_MEDIUM", 
				"G_ZOMBIE_LARGE"
			},
			50, 0.3,
			{50, 150},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);

		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", 
				"GEAR_BAG", 
			};
		loot.Set(0.9, lootItems);
		occupation.loot = loot;
			
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
			"{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
			"0 0 0"
		);
		occupation.campItems.Insert(item_0);
		
		return occupation;
	}	
	
	//----------------------------------------------------
	SDRC_Camp Occupation_H_3()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Portal construction close to %l",
			"Cultists are building a portal close to %l. Get rid of them.",
			"The portal never finished.",
			"Portal is finished. Gates to hell are open.",);
		occupation.general.modList.Insert("$BallienCreatures:");
		occupation.general.Set(
			3, "index 3: Portal construction",
			{"0 0 0"}, 15,
			{
				EMapDescriptorType.MDT_CHURCH,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{""},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_PORTAL_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{1, 4},
			{
				"{68A45BE2E133650E}622120A5448725E3/Prefabs/Groups/Group_Cultists_Light.et", "{68A45BE2E133650E}622120A5448725E3/Prefabs/Groups/Group_Cultists_Light.et", "{68A45BE2E133650E}622120A5448725E3/Prefabs/Groups/Group_Cultists_Light.et",
				"{68A45BE2E133650E}622120A5448725E3/Prefabs/Groups/Group_Cultists_Light.et", "{68A45BE2E133650E}622120A5448725E3/Prefabs/Groups/Group_Cultists_Light.et", "{68A45BE2E133650E}622120A5448725E3/Prefabs/Groups/Group_Cultists_Light.et",
				"{68A45BE2E133650E}622120A5448725E3/Prefabs/Groups/Group_Cultists_Light.et", "{68A45BE2E133650E}622120A5448725E3/Prefabs/Groups/Group_Cultists_Light.et", "{68A45BE2E133650E}622120A5448725E3/Prefabs/Groups/Group_Cultists_Light.et",
				"{3AA5A683A32167CC}622120A5448725E3/Prefabs/Groups/Group_Cultists_Horde.et",
				"{39F908C49BEBD995}622120A5448725E3/Prefabs/Groups/Group_Cultists_Medium.et",
				"{991230F941E4F5D3}Prefabs/Groups/Cultist/Group_Cultist_Firemage_L.et", 
				"{794F9A7B2C1DD64F}Prefabs/Groups/Cultist/Group_Cultist_Warrior_S.et",
				"{641097008A6848AE}Prefabs/Groups/Cultist/Group_Cultist_Stalker.et",
				"{E8C16E36B946D92D}Prefabs/Characters/Warrior_Cultist.et",
			},
			50, 0.3,
			{30, 70},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);

		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE", "WEAPON_RIFLE",	"WEAPON_RIFLE",
				"WEAPON_LAUNCHER", "WEAPON_LAUNCHER", "WEAPON_LAUNCHER", 
				"WEAPON_MG", 
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", 
				"GEAR_BAG", 
			};
		loot.Set(0.9, lootItems);
		occupation.loot = loot;

		// Loot Crate (first position)
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{973BB83B14B97689}PrefabsEditable/Auto/Props/Civilian/LootE_SackUniversal_01.et",
		    "61.465 1 75.514",
		    "0 59.467 0"
		);
		occupation.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{155C3A8D0CDC2AD6}Prefabs/Props/Cultist/Prop_Cultist_SkullStake.et",
		    "72.256 1 74.485",
		    "0 -158.403 0"
		);
		occupation.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{27B95A994D0DC1D3}Prefabs/Props/Cultist/Prop_Cultist_Totem.et",
		    "74.018 1 73.534",
		    "0 51.883 0"
		);
		occupation.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{4BCD2240C8EF8473}Prefabs/Props/Cultist/Prop_Cultist_Candles.et",
		    "72.636 1 74.395"
		);
		occupation.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{663260647F5C5E0E}Prefabs/Props/Cultist/Prop_Cultist_Brazier.et",
		    "67.696 1 75.232"
		);
		occupation.campItems.Insert(item_4);
		
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{80EAE46C81FCED20}Prefabs/Portal/PortaleDistruttonoEFFECT.et",
		    "70.02 1 66.507"
		);
		occupation.campItems.Insert(item_5);
		
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{941C76E878C3C46E}Prefabs/Props/Cultist/Prop_Cultist_Tent.et",
		    "63.268 1 76.188",
		    "0 66.137 0"
		);
		occupation.campItems.Insert(item_6);
		
		ref SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
		    "{9564A2C5DE570326}Prefabs/Props/Cultist/Prop_Cultist_Monolith.et",
		    "65.634 1 81.546",
		    "0 107.21 0"
		);
		occupation.campItems.Insert(item_7);
		
		ref SDRC_Structure item_8 = new SDRC_Structure();
		item_8.Set(
		    "{E38660774DD60788}Prefabs/Props/Cultist/Prop_Cultist_Lectern.et",
		    "73.366 1 74.321",
		    "0 19.097 0"
		);
		occupation.campItems.Insert(item_8);					
		
		return occupation;
	}
	
	//----------------------------------------------------
	SDRC_Camp Occupation_H_4()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Burial site at %l",
			"Cultists are robbing the grave. Go get it before them.",
			"You got the goods. How does it feel like to be a grave robber?",
			"The grave was robbed before you..",);
		occupation.general.modList.Insert("$BallienCreatures:");
		occupation.general.Set(
			4, "index 4: Burial site",
			{"0 0 0"}, 15,
			{
				EMapDescriptorType.MDT_CHURCH,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			"any",
			{message},
			SDRC_EMissionWinCondition.AI_KILL_75,
			{""},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_GOLD_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{1, 3},
			{
				"{991230F941E4F5D3}Prefabs/Groups/Cultist/Group_Cultist_Firemage_L.et", 
				"{701F871F443A79BA}Prefabs/Groups/Cultist/Group_Cultist_Firemage_S.et", "{701F871F443A79BA}Prefabs/Groups/Cultist/Group_Cultist_Firemage_S.et", "{701F871F443A79BA}Prefabs/Groups/Cultist/Group_Cultist_Firemage_S.et", 
				"{39F908C49BEBD995}622120A5448725E3/Prefabs/Groups/Group_Cultists_Medium.et",
				"{2F5DB83FFD84CB21}Prefabs/Groups/Cultist/Group_Cultist_Kamikaze.et",
				"{974D5BF0A69711F3}Prefabs/Groups/Cultist/Group_Cultist_Mixed_Horde.et",
			},
			50, 0.3,
			{30, 70},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);

		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE", "WEAPON_RIFLE",	"WEAPON_RIFLE",
				"WEAPON_LAUNCHER", "WEAPON_LAUNCHER", "WEAPON_LAUNCHER", 
				"WEAPON_MG", 
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", 
				"GEAR_BAG", 
			};
		loot.Set(0.9, lootItems);
		occupation.loot = loot;

		// Loot Crate (first position)
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
		    "{B719A6C120A0EA5E}PrefabsEditable/Auto/Structures/Cultural/Cemeteries/Graves/E_Grave_E_01_Covered_Loot.et",
		    "63.686 0 108.585"
		);
		occupation.campItems.Insert(item_0);
		
		ref SDRC_Structure item_1 = new SDRC_Structure();
		item_1.Set(
		    "{155C3A8D0CDC2AD6}Prefabs/Props/Cultist/Prop_Cultist_SkullStake.et",
		    "69.868 1 108.715",
		    "0 -46.785 0"
		);
		occupation.campItems.Insert(item_1);
		
		ref SDRC_Structure item_2 = new SDRC_Structure();
		item_2.Set(
		    "{155C3A8D0CDC2AD6}Prefabs/Props/Cultist/Prop_Cultist_SkullStake.et",
		    "72.108 1 111.615",
		    "0 -58.138 0"
		);
		occupation.campItems.Insert(item_2);
		
		ref SDRC_Structure item_3 = new SDRC_Structure();
		item_3.Set(
		    "{4BCD2240C8EF8473}Prefabs/Props/Cultist/Prop_Cultist_Candles.et",
		    "64.789 1 107.492"
		);
		occupation.campItems.Insert(item_3);
		
		ref SDRC_Structure item_4 = new SDRC_Structure();
		item_4.Set(
		    "{4BCD2240C8EF8473}Prefabs/Props/Cultist/Prop_Cultist_Candles.et",
		    "62.42 1 107.473"
		);
		occupation.campItems.Insert(item_4);
		
		ref SDRC_Structure item_5 = new SDRC_Structure();
		item_5.Set(
		    "{4BCD2240C8EF8473}Prefabs/Props/Cultist/Prop_Cultist_Candles.et",
		    "74.05 1 105.182"
		);
		occupation.campItems.Insert(item_5);
		
		ref SDRC_Structure item_6 = new SDRC_Structure();
		item_6.Set(
		    "{663260647F5C5E0E}Prefabs/Props/Cultist/Prop_Cultist_Brazier.et",
		    "72.256 1 113.299"
		);
		occupation.campItems.Insert(item_6);
		
		ref SDRC_Structure item_7 = new SDRC_Structure();
		item_7.Set(
		    "{810071129368254F}PrefabsEditable/Auto/Structures/Ruins/HouseRuins/E_HouseRuin_01_BrickPile_Small.et",
		    "68.782 0 108.885",
		    "0 -16.021 0"
		);
		occupation.campItems.Insert(item_7);
		
		ref SDRC_Structure item_8 = new SDRC_Structure();
		item_8.Set(
		    "{81CDBA11F502E520}PrefabsEditable/Auto/Structures/Cultural/Calvaries/E_CalvaryLarge_02.et",
		    "63.559 0 106.673",
		    "0 0.6 0"
		);
		occupation.campItems.Insert(item_8);
		
		ref SDRC_Structure item_9 = new SDRC_Structure();
		item_9.Set(
		    "{941C76E878C3C46E}Prefabs/Props/Cultist/Prop_Cultist_Tent.et",
		    "73.693 1 107.912",
		    "0 -51.437 0"
		);
		occupation.campItems.Insert(item_9);
		
		ref SDRC_Structure item_10 = new SDRC_Structure();
		item_10.Set(
		    "{95BFB5C33EA78DE6}PrefabsEditable/Auto/Structures/Military/Fortifications/E_DirtPile_01_small.et",
		    "72.038 0 108.377"
		);
		occupation.campItems.Insert(item_10);
		
		ref SDRC_Structure item_11 = new SDRC_Structure();
		item_11.Set(
		    "{9E954B448BC92414}PrefabsEditable/Auto/Structures/Agriculture/VegetableBed_01/E_VegetableBed_01_small.et",
		    "74.113 0 107.692",
		    "0 40.5 0"
		);
		occupation.campItems.Insert(item_11);		
		return occupation;
	}	
}