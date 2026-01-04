//Mission SDRC_Mission_Stash_010.c

//------------------------------------------------------------------------------------------------
/*!
*/

//------------------------------------------------------------------------------------------------
class SDRC_StashConfig_010 : SDRC_StashConfig
{
	private const string DC_MISSIONCONFIG_FILE_STASH_010 = "dc_missionConfig_Stash_010.json";

	//------------------------------------------------------------------------------------------------
	//This will setup the filename correctly to create the file
	static string GetFileName()
	{		
		return DC_MISSIONCONFIG_FILE_STASH_010;
	}	
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
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
		missionList = {10};		
		//Mission specific		
		//----------------------------------------------------
		subMissions.Clear();
		subMissions.Insert(Stash010());				
	};
	
	//----------------------------------------------------
	SDRC_Camp Stash010()
	{
		ref SDRC_Camp stash = new SDRC_Camp();
		stash.general.Set(
			10, "index 10: Random stash",
			{"0 0 0"}, 3,
			{},
			"any",
			"Hidden loot near %l",
			"Loot is yours to take",
			SDRC_EMissionWinCondition.FIND_IN_15,
			"Loot found.",
			"Loot lost.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_STASH_MAP,
			SDRC_EMissionDifficulty.NORMAL,
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
			"100.47 1 144.562"
		);
		stash.campItems.Insert(item_0);
		
		return stash;
	};
}