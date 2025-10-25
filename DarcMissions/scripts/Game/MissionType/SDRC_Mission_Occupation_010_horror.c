//Mission SDRC_Occupation_010_horror.c

//------------------------------------------------------------------------------------------------
/*!
*/

//------------------------------------------------------------------------------------------------
class SDRC_Occupation_010_horror_JsonApi : SDRC_OccupationJsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Occupation_010_horror.json";
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Occupation_010_horror_JsonApi(string fileName = "")
	{
		SetFileName(DC_MISSIONCONFIG_FILE);
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		//Default		
		conf.disableArsenal = true;
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
//		conf.missionList = {10, 11, 12};		
		conf.missionList = {10};		
		//Mission specific		
		//----------------------------------------------------
		conf.subMissions.Insert(Occupation010());				
		conf.subMissions.Insert(Occupation011());				
		conf.subMissions.Insert(Occupation012());				
	};
	
	//----------------------------------------------------
	SDRC_Camp Occupation010()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		occupation.general.Set(
			10, "index 10: Zombies and Demons.",
			{"0 0 0"}, 3,
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL
			},
			"any",
			"Creatures near %l",
			"Avoid the location. No loot available.",
			SDRC_EMissionWinCondition.AI_KILL_ALL,
			"Spawns from hell are dead!",
			"Are you scared of a few ghosts..?",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP,
			SDRC_EMissionDifficulty.NORMAL,
			0
		);
		occupation.ai.Set(
			{1, 8},
			{//"C_DEMON", "C_DEMON_BOSS"
			 "C_BEASTS", //"C_BEASTS", "C_BEASTS", "C_BEASTS", "C_BEASTS", "C_BEASTS", "C_BEASTS", 
			},
			50, 1.0,
			{50, 300},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
	
		return occupation;
	}
	
	//----------------------------------------------------
	SDRC_Camp Occupation011()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		occupation.general.Set(
			11, "index 11: Zombie hordes",
			{"0 0 0"}, 3,
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL
			},
			"any",
			"Zombies near %l",
			"Braaaainss.....",
			SDRC_EMissionWinCondition.AI_KILL_75,
			"The living dead are just dead now.",
			"You were tasty.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_ZOMBIE_MAP,
			SDRC_EMissionDifficulty.NORMAL,
			0
		);
		occupation.ai.Set(
			{1, 4},
			{"G_ZOMBIE_SMALL", "G_ZOMBIE_MEDIUM", "G_ZOMBIE_LARGE"
			},
			50, 0.3,
			{50, 300},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
	
		return occupation;
	}
	
	//----------------------------------------------------
	SDRC_Camp Occupation012()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		occupation.general.Set(
			12, "index 12: Demons",
			{"0 0 0"}, 3,
			{
				EMapDescriptorType.MDT_CHURCH,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
			},
			"any",
			"Zombies near %l",
			"Braaaainss.....",
			SDRC_EMissionWinCondition.AI_KILL_75,
			"The living dead are just dead now.",
			"You were tasty.",
			"",
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP,
			SDRC_EMissionDifficulty.NORMAL,
			0
		);
		occupation.ai.Set(
			{1, 4},
			{"G_ZOMBIE_SMALL", "G_ZOMBIE_MEDIUM", "G_ZOMBIE_LARGE"
			},
			50, 0.3,
			{50, 300},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
	
		return occupation;
	}	
}