//Mission SDRC_Mission_Stash_010.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns a stash for loot. This essentially the same as Occupation mission.if

The usage for Stash mission is mainly as a story ending when using DarcStories.
*/

//------------------------------------------------------------------------------------------------
class SDRC_Stash_010_JsonApi : SDRC_StashJsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Stash_010.json";

	void SDRC_Stash_010_JsonApi(string fileName = "")
	{		
		SetFileName(DC_MISSIONCONFIG_FILE);
		//SetDefaults();
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		//Default
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		conf.activeDistance = 50;
		conf.missionList = {10};
		//Mission specific		
		//----------------------------------------------------
		conf.subMissions.Insert(Stash10());				
	};
	
	//----------------------------------------------------
	SDRC_Camp Stash10()
	{
		ref SDRC_Camp stash = new SDRC_Camp();
		stash.general.Set(
			10, "index 10: Random stash",
			{"0 0 0"}, 3,
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
		stash.Set(
			{
			},
		);
		
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
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