//Helpers SDRC_MissionFrameJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

	//Release options
	#ifdef SDRC_RELEASE
		private const string SDRC_MISSION_PROFILE_DIR = "default";
		private const int SDRC_MISSION_COUNT = 10;										//Default amount of missions to run
		private const int SDRC_MISSION_STATIC_TRY_LIMIT = 2;
		private const int SDRC_MISSION_CYCLE_TIME_DEFAULT = 30;
		private const int SDRC_MISSION_START_DELAY = 1*60;								//Time to wait before spawning the first mission (seconds)
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS = 2*60;					//Minimum delay between missions. 
		private const int SDRC_MISSION_ACTIVE_TIME = 15*60;								//Time to keep the mission active (seconds)
		private const int SDRC_MISSION_ACTIVE_TIME_STATIC = 20*60;						//Static missions are to be kept alive longer
		private const int SDRC_MISSION_ACTIVE_DISTANCE = 200;							//Mission is to be removed if no players close to the position after the mission active time has passed.
		private const int SDRC_MISSION_ACTIVE_TIME_TO_END = 180;						//Mission is kept active this time once all AIs are dead.
		private const int SDRC_MISSIONFRAME_CYCLE_TIME = 30;							//The cycle to run the mission frame. 
		private const int SDRC_MISSIONFRAME_CYCLE_TIME_LIMIT = 20;						//You should not be running the frame too often as it's unncecessary
		private const bool SDRC_MISSION_RECREATE_CONFIGS = true;						//Force recreaction of config files. 
		private const int SDRC_MISSION_HINT_TIME = 30;									//Seconds to show the mission hint to players
		const bool SDRC_MISSION_FIRST_MISSION_HAS_SPAWNED = false;						//Has the first mission (=dynamic) spawned. Setting to true will spawn a static mission first
	#endif
	
	//Development time options
	#ifndef SDRC_RELEASE	
		private const string SDRC_MISSION_PROFILE_DIR = "dummy";
		private const int SDRC_MISSION_COUNT = 8;
		private const int SDRC_MISSION_STATIC_TRY_LIMIT = 2;
		private const int SDRC_MISSION_CYCLE_TIME_DEFAULT = 10;
		private const int SDRC_MISSION_START_DELAY = 10;					
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS = 1*20;
		private const int SDRC_MISSION_ACTIVE_TIME = 3*60;				
		private const int SDRC_MISSION_ACTIVE_TIME_STATIC = SDRC_MISSION_ACTIVE_TIME * 2;	
		private const int SDRC_MISSION_ACTIVE_DISTANCE = 200;		
		private const int SDRC_MISSION_ACTIVE_TIME_TO_END = 45;
		private const int SDRC_MISSIONFRAME_CYCLE_TIME = 20;
		private const int SDRC_MISSIONFRAME_CYCLE_TIME_LIMIT = 10;
		private const bool SDRC_MISSION_RECREATE_CONFIGS = false;
		private const int SDRC_MISSION_HINT_TIME = 30;
		const bool SDRC_MISSION_FIRST_MISSION_HAS_SPAWNED = true;
	#endif

//------------------------------------------------------------------------------------------------
class SDRC_MissionFrameConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Mission specific
	string comment;
	bool recreateConfigs;			//If set to true, all configs are to be written to disk. Should be run only first time.
	string missionProfile;			//Directory specifying a certain conf for play. For example "Escapists"
	int missionStartDelay;			//Time to wait before spawning the first mission (seconds).
	int missionDelayBetweeen;		//Time delay between mission spawns (seconds)
	int missionCount;				//Maximum amount of missions (both static and dynamic) to be active at the same time. 
	int staticTryLimit;				//How many static missions may fail or succeed, before trying a dynamic one
	int missionFrameCycleTime;		//The cycle time to manage mission spawning, deletion etc... (seconds)
	int missionActiveTime;			//Time to keep the mission active (seconds)
	int missionActiveTimeStatic;	//Time to keep the static mission active (seconds). This typically is much longer than for dynamic.
	int missionActiveDistance;		//The distance to a player to keep the mission active.
	int missionActiveTimeToEnd;		//Time to keep the mission active once all AI is dead. Used for both dynamic and static missions.
	int missionHintTime;			//Seconds to show mission hints to players. 0 disables hints.
	int minDistanceToMission;		//Distance to another mission. Two missions shall not be too close to each other.
	int minDistanceToPlayer;		//Mission shall not spawn too close to a player.
	ref array<DC_EMissionType> missionTypeArrayDynamic = {};	//List mission types that spawn randomly
	ref array<DC_EMissionType> missionTypeArrayStatic = {};		//List mission types that are always active	
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
		conf.missionProfile = SDRC_MISSION_PROFILE_DIR;
		conf.missionStartDelay = SDRC_MISSION_START_DELAY;
		conf.missionDelayBetweeen = SDRC_MISSION_DELAY_BETWEEN_MISSIONS;
		conf.missionCount = SDRC_MISSION_COUNT;
		conf.staticTryLimit = SDRC_MISSION_STATIC_TRY_LIMIT;
		conf.missionFrameCycleTime = SDRC_MISSIONFRAME_CYCLE_TIME;
		conf.missionActiveTime = SDRC_MISSION_ACTIVE_TIME;
		conf.missionActiveTimeStatic = SDRC_MISSION_ACTIVE_TIME_STATIC;
		conf.missionActiveDistance = SDRC_MISSION_ACTIVE_DISTANCE;
		conf.missionActiveTimeToEnd = SDRC_MISSION_ACTIVE_TIME_TO_END;
		conf.missionHintTime = SDRC_MISSION_HINT_TIME;
		conf.minDistanceToMission = 500;
		conf.minDistanceToPlayer = 100;
		
		#ifdef SDRC_RELEASE
			conf.missionTypeArrayDynamic = {DC_EMissionType.HUNTER, DC_EMissionType.CRASHSITE, DC_EMissionType.CONVOY, 
											DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, 
											DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION};
			conf.missionTypeArrayStatic = {DC_EMissionType.PATROL, DC_EMissionType.PATROL, DC_EMissionType.CONVOY, DC_EMissionType.CONVOY};
		#endif	

		#ifndef SDRC_RELEASE				
			conf.missionTypeArrayDynamic = {DC_EMissionType.HUNTER, DC_EMissionType.CRASHSITE, DC_EMissionType.OCCUPATION, DC_EMissionType.CONVOY, DC_EMissionType.PATROL, DC_EMissionType.SQUATTER};
//			conf.missionTypeArrayDynamic = {DC_EMissionType.OCCUPATION};		
//			conf.missionTypeArrayDynamic = {DC_EMissionType.HUNTER};
//			conf.missionTypeArrayDynamic = {DC_EMissionType.CONVOY};		
//			conf.missionTypeArrayDynamic = {DC_EMissionType.PATROL};		
//			conf.missionTypeArrayDynamic = {DC_EMissionType.CRASHSITE};
//			conf.missionTypeArrayDynamic = {DC_EMissionType.CHOPPER};
			conf.missionTypeArrayDynamic = {DC_EMissionType.SQUATTER};
		
//			conf.missionTypeArrayStatic = {DC_EMissionType.PATROL, DC_EMissionType.PATROL};
//			conf.missionTypeArrayStatic = {DC_EMissionType.CONVOY, DC_EMissionType.CONVOY, DC_EMissionType.CONVOY};
//			conf.missionTypeArrayStatic = {DC_EMissionType.HUNTER};
//			conf.missionTypeArrayStatic = {DC_EMissionType.CRASHSITE};
//			conf.missionTypeArrayStatic = {DC_EMissionType.OCCUPATION};
			conf.missionTypeArrayStatic = {DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER};
//			conf.missionTypeArrayStatic = {DC_EMissionType.SQUATTER};
//			conf.missionTypeArrayStatic = {};
		#endif
	}
}