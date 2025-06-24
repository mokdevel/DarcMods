//Helpers SDRC_MissionFrameJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

	//Release options
	#ifdef SDRC_RELEASE
		private const string SDRC_MISSION_PROFILE_DIR = "default";
		private const int SDRC_MISSION_COUNT = 10;										//Default amount of dynamic missions to run
		private const float SDRC_MISSION_COUNT_MUL = 1.5;
		private const int SDRC_MISSION_COUNT_STATIC = 6;								//Default amount of static missions to run
		private const float SDRC_MISSION_COUNT_STATIC_MUL = 1;
		private const int SDRC_MISSION_CYCLE_TIME_DEFAULT = 30;
		private const int SDRC_MISSION_START_DELAY = 1*60;								//Time to wait before spawning the first mission (seconds)
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS_DYNAMIC = 4*60;			//Minimum delay between dynamic missions. 
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS_STATIC = 5;				//Minimum delay between static missions. 
		private const int SDRC_MISSION_ACTIVE_TIME = 15*60;								//Time to keep the mission active (seconds)
		private const int SDRC_MISSION_ACTIVE_TIME_STATIC = 60*60;						//Static missions are to be kept alive longer
		private const int SDRC_MISSION_ACTIVE_DISTANCE = 200;							//Mission is to be removed if no players close to the position after the mission active time has passed.
		private const int SDRC_MISSION_ACTIVE_TIME_TO_END = 180;						//Mission is kept active this time once all AIs are dead.
		private const int SDRC_MISSIONFRAME_CYCLE_TIME = 30;							//The cycle to run the mission frame. 
		private const int SDRC_MISSIONFRAME_CYCLE_TIME_LIMIT = 20;						//You should not be running the frame too often as it's unncecessary
		private const bool SDRC_MISSION_RECREATE_CONFIGS = true;						//Force recreaction of config files. 
		private const int SDRC_MISSION_HINT_TIME = 30;									//Seconds to show the mission hint to players
	#endif
	
	//Development time options
	#ifndef SDRC_RELEASE	
		private const string SDRC_MISSION_PROFILE_DIR = "dummy";
		private const int SDRC_MISSION_COUNT = 0;//8;
		private const float SDRC_MISSION_COUNT_MUL = 2.0;
		private const int SDRC_MISSION_COUNT_STATIC = 5;//30;//10;
		private const float SDRC_MISSION_COUNT_STATIC_MUL = 3;
		private const int SDRC_MISSION_CYCLE_TIME_DEFAULT = 20;
		private const int SDRC_MISSION_START_DELAY = 10;					
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS_DYNAMIC = 1*20;
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS_STATIC = 1;
		private const int SDRC_MISSION_ACTIVE_TIME = 3*60;				
		private const int SDRC_MISSION_ACTIVE_TIME_STATIC = SDRC_MISSION_ACTIVE_TIME * 10;	
		private const int SDRC_MISSION_ACTIVE_DISTANCE = 200;		
		private const int SDRC_MISSION_ACTIVE_TIME_TO_END = 45;
		private const int SDRC_MISSIONFRAME_CYCLE_TIME = 20;
		private const int SDRC_MISSIONFRAME_CYCLE_TIME_LIMIT = 10;
		private const bool SDRC_MISSION_RECREATE_CONFIGS = true;
		private const int SDRC_MISSION_HINT_TIME = 30;
	#endif

class SDRC_MissionTypeConfig : Managed
{
	int count;
	float countMul;
	int activeTime;
	int delayBetween;
	ref array<DC_EMissionType> missionTypeArray = {};	//List mission types that spawn randomly
}

//------------------------------------------------------------------------------------------------
class SDRC_MissionFrameConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Mission specific
	string comment;
	bool recreateConfigs;			//If set to true, all configs are to be written to disk. Should be run only first time.
	int missionStartDelay;			//Time to wait before spawning the first mission (seconds).
	//Timing specific
	int missionFrameCycleTime;		//The cycle time to manage mission spawning, deletion etc... (seconds)
	int missionActiveDistance;		//The distance to a player to keep the mission active.
	int missionActiveTimeToEnd;		//Time to keep the mission active once all AI is dead. Used for both dynamic and static missions.
	int missionHintTime;			//Seconds to show mission hints to players. 0 disables hints.
	//Misc
	int minDistanceToMission;		//Distance to another mission. Two missions shall not be too close to each other.
	int minDistanceToPlayer;		//Mission shall not spawn too close to a player.
	ref array<string>enemyFactions;	//Factions to use for enemy selection
	ref SDRC_MissionTypeConfig missionDynamic = new SDRC_MissionTypeConfig();
	ref SDRC_MissionTypeConfig missionStatic = new SDRC_MissionTypeConfig();
}

//------------------------------------------------------------------------------------------------
class SDRC_MissionFrameJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig.json";
		
	ref SDRC_MissionFrameConfig conf = new SDRC_MissionFrameConfig();

	//------------------------------------------------------------------------------------------------
	void SDRC_MissionFrameJsonApi()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_MISSIONCONFIG_FILE);
		if (!loadContext)
		{
			SetDefaults();
			Save("");
			return;
		}
		
		loadContext.ReadValue("", conf);

		if (conf.missionFrameCycleTime < SDRC_MISSIONFRAME_CYCLE_TIME_LIMIT)
		{
			SDRC_Log.Add("[SDRC_MissionFrameConfig] missionFrameCycleTime is less than " + SDRC_MISSIONFRAME_CYCLE_TIME_LIMIT + ". This could lead to performance issues.", LogLevel.WARNING);
		}
	}	

	//------------------------------------------------------------------------------------------------
	void Save(string data)
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen(DC_MISSIONCONFIG_FILE);
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}	
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		conf.comment = "Simple comment, not used in game";
		conf.recreateConfigs = SDRC_MISSION_RECREATE_CONFIGS;
		conf.missionStartDelay = SDRC_MISSION_START_DELAY;
		
		conf.missionDynamic.count = SDRC_MISSION_COUNT;
		conf.missionDynamic.countMul = SDRC_MISSION_COUNT_MUL;
		conf.missionDynamic.activeTime = SDRC_MISSION_ACTIVE_TIME;
		conf.missionDynamic.delayBetween = SDRC_MISSION_DELAY_BETWEEN_MISSIONS_DYNAMIC;
		
		conf.missionStatic.count = SDRC_MISSION_COUNT_STATIC;
		conf.missionStatic.countMul = SDRC_MISSION_COUNT_STATIC_MUL;
		conf.missionStatic.activeTime = SDRC_MISSION_ACTIVE_TIME_STATIC;
		conf.missionStatic.delayBetween = SDRC_MISSION_DELAY_BETWEEN_MISSIONS_STATIC;
		
		conf.missionFrameCycleTime = SDRC_MISSIONFRAME_CYCLE_TIME;
		conf.missionActiveDistance = SDRC_MISSION_ACTIVE_DISTANCE;
		conf.missionActiveTimeToEnd = SDRC_MISSION_ACTIVE_TIME_TO_END;
		conf.missionHintTime = SDRC_MISSION_HINT_TIME;
		conf.minDistanceToMission = 500;
		conf.minDistanceToPlayer = 100;
//		conf.enemyFactions = {"FIA", "USSR"};
//		conf.enemyFactions = {"FIA", "US"};
		conf.enemyFactions = {"USSR"};
		
		#ifdef SDRC_RELEASE
			conf.missionDynamic.missionTypeArray = {DC_EMissionType.HUNTER, DC_EMissionType.CRASHSITE, DC_EMissionType.CONVOY, 
											DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, 
											DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION};
			conf.missionStatic.missionTypeArray = {DC_EMissionType.PATROL, DC_EMissionType.PATROL, DC_EMissionType.PATROL, DC_EMissionType.CONVOY, DC_EMissionType.CONVOY};
		#endif	

		#ifndef SDRC_RELEASE				
//			conf.missionDynamic.missionTypeArray = {DC_EMissionType.HUNTER, DC_EMissionType.CRASHSITE, DC_EMissionType.OCCUPATION, DC_EMissionType.CONVOY, DC_EMissionType.PATROL, DC_EMissionType.SQUATTER};
			conf.missionDynamic.missionTypeArray = {DC_EMissionType.OCCUPATION};		
//			conf.missionDynamic.missionTypeArray = {DC_EMissionType.HUNTER};
//			conf.missionDynamic.missionTypeArray = {DC_EMissionType.CONVOY};		
//			conf.missionDynamic.missionTypeArray = {DC_EMissionType.PATROL};		
//			conf.missionDynamic.missionTypeArray = {DC_EMissionType.CRASHSITE};
//			conf.missionDynamic.missionTypeArray = {DC_EMissionType.CHOPPER};
//			conf.missionDynamic.missionTypeArray = {DC_EMissionType.SQUATTER};
		
//			conf.missionStatic.missionTypeArray = {DC_EMissionType.PATROL, DC_EMissionType.PATROL};
//			conf.missionStatic.missionTypeArray = {DC_EMissionType.CONVOY, DC_EMissionType.CONVOY, DC_EMissionType.CONVOY};
//			conf.missionStatic.missionTypeArray = {DC_EMissionType.CONVOY};
//			conf.missionStatic.missionTypeArray = {DC_EMissionType.HUNTER};
//			conf.missionStatic.missionTypeArray = {DC_EMissionType.CRASHSITE};
			conf.missionStatic.missionTypeArray = {DC_EMissionType.OCCUPATION};
//			conf.missionStatic.missionTypeArray = {DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER};
//			conf.missionStatic.missionTypeArray = {DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER};
//			conf.missionStatic.missionTypeArray = {DC_EMissionType.PATROL, DC_EMissionType.PATROL, DC_EMissionType.PATROL, DC_EMissionType.CONVOY, DC_EMissionType.CONVOY};
//			conf.missionStatic.missionTypeArray = {};
		#endif
	}
}