//Mission SDRC_Occupation_010_horror.c

//------------------------------------------------------------------------------------------------
/*!
*/

//------------------------------------------------------------------------------------------------
class SDRC_OccupationConfig_Horror : SDRC_OccupationConfig
{
	private const string DC_MISSIONCONFIG_FILE_OCCUPATION_HORROR = "dc_missionConfig_Occupation_Horror.json";
	private const int DC_MISSIONCONFIG_FILE_OCCUPATION_HORROR_JSONVER = 2;
	
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
//		missionList = {0, 1, 2};		
		missionList = {0};				
		//Mission specific		
		//----------------------------------------------------
		subMissions.Clear();
		subMissions.Insert(Occupation_H_0());				
		subMissions.Insert(Occupation_H_1());				
		subMissions.Insert(Occupation_H_2());				
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
		occupation.general.modList.Insert("$Zombies:");	//TBD: Fix to correct one!
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
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{1, 8},
			{
			 "C_DEMON", "C_DEMON_BOSS",
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
	SDRC_Camp Occupation_H_1()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Zombies near %l",
			"Braaaainss.....",
			"The living dead are just dead now.",
			"You were tasty.",);
		occupation.general.modList.Insert("$Zombies:");	//TBD: Fix to correct one!
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
			 "G_ZOMBIE_SMALL", "G_ZOMBIE_MEDIUM", "G_ZOMBIE_LARGE"
			},
			50, 0.3,
			{50, 300},
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
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP,
			{SDRC_EDifficulty.RANDOM},
			0
		);
		occupation.ai.Set(
			{1, 4},
			{
			 "G_ZOMBIE_SMALL", "G_ZOMBIE_MEDIUM", "G_ZOMBIE_LARGE"
			},
			50, 0.3,
			{50, 300},
			SDRC_EWaypointGenerationType.RANDOM,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
	
		return occupation;
	}	
}