//Mission SDRC_Mission_Stash_010.c

//------------------------------------------------------------------------------------------------
/*!
*/

//------------------------------------------------------------------------------------------------
class SDRC_StashConfig_010 : SDRC_StashConfig
{
	private const string DC_MISSIONCONFIG_FILE_STASH_010 = "dc_missionConfig_Stash_010.json";
	private const int DC_MISSIONCONFIG_FILE_STASH_010_JSONVER = 2;
	
	//------------------------------------------------------------------------------------------------
	static string GetFileName()
	{		
		return DC_MISSIONCONFIG_FILE_STASH_010;
	}	

	//------------------------------------------------------------------------------------------------
	static int GetFileVersion()
	{		
		return DC_MISSIONCONFIG_FILE_STASH_010_JSONVER;
	}	
		
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_StashConfig_010 data = SDRC_StashConfig_010.Cast(T);
		return saveContext.WriteValue("", data);
	}		
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		
		//Default
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		activeDistance = 50;
		missionList = {0, 1};		
		
		#ifndef SDRC_RELEASE
			//missionList = {};
		#endif
		
		//Mission specific		
		//----------------------------------------------------
		subMissions.Clear();
		subMissions.Insert(Stash_010_0());				
		subMissions.Insert(Stash_010_1());				
	};
	
	//----------------------------------------------------
	SDRC_Camp Stash_010_0()
	{
		ref SDRC_Camp stash = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Hidden loot near %l",
			"Loot is yours to take",
			"Loot found.",
			"Loot lost.",);
		stash.general.Set(
			0, "index 0: Random stash",
			{"0 0 0"}, 3,
			{},
			"any",
			{message},		
			SDRC_EMissionWinCondition.FIND_IN_15,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_STASH_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);		
		stash.ai.Set(
			{0, 1},
			{"G_RECON"},
			50, 1.0,			
			{50, 300},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);	
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", "WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE", 
				"UTIL_ATTACHMENT",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
			};
		loot.Set(0.7, lootItems);
		stash.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
			"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
			"0 1 0"
		);
		stash.campItems.Insert(item_0);
		
		return stash;
	};
	
	//----------------------------------------------------
	SDRC_Camp Stash_010_1()
	{
		ref SDRC_Camp stash = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("A briefcase has been lost near %l",
			"There is valuable loot to find.",
			"Loot found.",
			"The loot was briefly your, but is gone now.",);
		stash.general.Set(
			1, "index 1: Briefcase in a city",
			{"0 0 0"}, 3,
			{EMapDescriptorType.MDT_NAME_CITY, EMapDescriptorType.MDT_NAME_TOWN},
			"any",
			{message},		
			SDRC_EMissionWinCondition.FIND_IN_45,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_STASH_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);		
		stash.ai.Set(
			{1, 2},
			{"G_RECON"},
			50, 1.0,			
			{50, 300},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);	
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"UTIL_ATTACHMENT", "UTIL_ATTACHMENT", "UTIL_ATTACHMENT", "UTIL_ATTACHMENT",
				"UTIL_OPTIC", "UTIL_OPTIC", 
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", 
			};
		loot.Set(0.7, lootItems);
		stash.loot = loot;
		
		ref SDRC_Structure item_0 = new SDRC_Structure();
		item_0.Set(
			"{14B16D7580478D1A}Prefabs/Props/Civilian/LootSuitcase_01.et",
			"0 1 0"
		);
		stash.campItems.Insert(item_0);
		
		return stash;
	};	
}