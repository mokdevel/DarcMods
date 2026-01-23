//Mission SDRC_Occupation_010_horror.c

//------------------------------------------------------------------------------------------------
/*!
*/

//------------------------------------------------------------------------------------------------
class SDRC_OccupationConfig_010 : SDRC_OccupationConfig
{
	const string DC_MISSIONCONFIG_FILE_OCCUPATION_HORROR = "dc_missionConfig_Occupation_010_horror.json";
	
	//------------------------------------------------------------------------------------------------
	//This will setup the filename correctly to create the file
	static string GetFileName()
	{		
		return DC_MISSIONCONFIG_FILE_OCCUPATION_HORROR;
	}	

	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_OccupationConfig_010 data = SDRC_OccupationConfig_010.Cast(T);
		return saveContext.WriteValue("", data);
	}		
		
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		
		//Default		
		disableArsenal = true;
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
//		missionList = {10, 11, 12};		
		missionList = {10};				
		//Mission specific		
		//----------------------------------------------------
		subMissions.Clear();
		subMissions.Insert(Occupation010());				
		subMissions.Insert(Occupation011());				
		subMissions.Insert(Occupation012());				
	};
	
	//----------------------------------------------------
	SDRC_Camp Occupation010()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Creatures near %l",
			"Avoid the location. No loot available.",
			"Spawns from hell are dead!",
			"Are you scared of a few ghosts..?",);
		occupation.general.Set(
			10, "index 10: Zombie and Demon characters.",
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
	SDRC_Camp Occupation011()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Zombies near %l",
			"Braaaainss.....",
			"The living dead are just dead now.",
			"You were tasty.",);
		occupation.general.Set(
			11, "index 11: Zombie hordes",
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
	SDRC_Camp Occupation012()
	{
		ref SDRC_Camp occupation = new SDRC_Camp();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Horde of zombies seen near %l",
			"Avoid getting yourself eaten.",
			"The dead shall stay dead.",
			"Oh the horrors. You were slain.",);
		occupation.general.Set(
			12, "index 12: Zombies",
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