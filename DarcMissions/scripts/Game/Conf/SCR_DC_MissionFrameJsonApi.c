//Helpers SCR_DC_MissionFrameJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

	#ifdef SCR_DC_RELEASE
		private const string DC_MISSION_PROFILE_DIR = "default";
		private const int DC_MISSION_COUNT = 10;										//Default amount of missions to run
		private const int DC_MISSION_STATIC_FAIL_LIMIT = 2;
		private const int DC_MISSION_CYCLE_TIME_DEFAULT = 30;
		private const int DC_MISSION_START_DELAY = 1*60;								//Time to wait before spawning the first mission (seconds)
		private const int DC_MISSION_DELAY_BETWEEN_MISSIONS = 1*60;						//Minimum delay between missions. ##################################################################### -> 4
		private const int DC_MISSION_ACTIVE_TIME = 15*60;								//Time to keep the mission active (seconds)
		private const int DC_MISSION_ACTIVE_TIME_STATIC = DC_MISSION_ACTIVE_TIME * 2;	//Static missions are to be kept alive longer
		private const int DC_MISSION_ACTIVE_DISTANCE = 200;								//Mission is to be removed if no players close to the position after the mission active time has passed.
		private const int DC_MISSION_ACTIVE_TIME_TO_END = 180;							//Mission is kept active this time once all AIs are dead.
		private const int DC_MISSIONFRAME_CYCLE_TIME = 30;								//The cycle to run the mission frame. 
		private const int DC_MISSIONFRAME_CYCLE_TIME_LIMIT = 20;						//You should not be running the frame too often as it's unncecessary
		private const bool DC_MISSION_RECREATE_CONFIGS = true;							//Force recreaction of config files. 
		private const int DC_MISSION_HINT_TIME = 30;									//Seconds to show the mission hint to players
	#endif
	
	#ifndef SCR_DC_RELEASE	//Development time options
		private const string DC_MISSION_PROFILE_DIR = "dummy";
		private const int DC_MISSION_COUNT = 8;
		private const int DC_MISSION_STATIC_FAIL_LIMIT = 2;
		private const int DC_MISSION_CYCLE_TIME_DEFAULT = 10;
		private const int DC_MISSION_START_DELAY = 10;					
		private const int DC_MISSION_DELAY_BETWEEN_MISSIONS = 1*20;
		private const int DC_MISSION_ACTIVE_TIME = 2*60;				
		private const int DC_MISSION_ACTIVE_TIME_STATIC = DC_MISSION_ACTIVE_TIME * 2;	
		private const int DC_MISSION_ACTIVE_DISTANCE = 200;		
		private const int DC_MISSION_ACTIVE_TIME_TO_END = 45;		
		private const int DC_MISSIONFRAME_CYCLE_TIME = 20;
		private const int DC_MISSIONFRAME_CYCLE_TIME_LIMIT = 10;
		private const bool DC_MISSION_RECREATE_CONFIGS = false;
		private const int DC_MISSION_HINT_TIME = 30;
	#endif


//------------------------------------------------------------------------------------------------
class SCR_DC_MissionFrameConfig : Managed
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
	int staticFailLimit;			//How many static missions may fail, before trying a dynamic one
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
class SCR_DC_MissionFrameJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig.json";
		
	ref SCR_DC_MissionFrameConfig conf = new SCR_DC_MissionFrameConfig();

	//------------------------------------------------------------------------------------------------
	void SCR_DC_MissionFrameJsonApi()
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

		if (conf.missionFrameCycleTime < DC_MISSIONFRAME_CYCLE_TIME_LIMIT)
		{
			SCR_DC_Log.Add("[SCR_DC_MissionFrameConfig] missionFrameCycleTime is less than " + DC_MISSIONFRAME_CYCLE_TIME_LIMIT + ". This could lead to performance issues.", LogLevel.WARNING);
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
		conf.recreateConfigs = DC_MISSION_RECREATE_CONFIGS;
		conf.missionProfile = DC_MISSION_PROFILE_DIR;
		conf.missionStartDelay = DC_MISSION_START_DELAY;
		conf.missionDelayBetweeen = DC_MISSION_DELAY_BETWEEN_MISSIONS;
		conf.missionCount = DC_MISSION_COUNT;
		conf.staticFailLimit = DC_MISSION_STATIC_FAIL_LIMIT;
		conf.missionFrameCycleTime = DC_MISSIONFRAME_CYCLE_TIME;
		conf.missionActiveTime = DC_MISSION_ACTIVE_TIME;
		conf.missionActiveTimeStatic = DC_MISSION_ACTIVE_TIME_STATIC;
		conf.missionActiveDistance = DC_MISSION_ACTIVE_DISTANCE;
		conf.missionActiveTimeToEnd = DC_MISSION_ACTIVE_TIME_TO_END;
		conf.missionHintTime = DC_MISSION_HINT_TIME;
		conf.minDistanceToMission = 500;
		conf.minDistanceToPlayer = 100;
		
		#ifdef SCR_DC_RELEASE
			conf.missionTypeArrayDynamic = {DC_EMissionType.HUNTER, DC_EMissionType.CRASHSITE, DC_EMissionType.CONVOY, 
											DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, 
											DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION};
			conf.missionTypeArrayStatic = {DC_EMissionType.PATROL, DC_EMissionType.PATROL, DC_EMissionType.CONVOY, DC_EMissionType.CONVOY};
		#endif	

		#ifndef SCR_DC_RELEASE				
			conf.missionTypeArrayDynamic = {DC_EMissionType.CRASHSITE, DC_EMissionType.OCCUPATION};
//			conf.missionTypeArrayDynamic = {DC_EMissionType.HUNTER, DC_EMissionType.CRASHSITE, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION};
			conf.missionTypeArrayDynamic = {DC_EMissionType.HUNTER, DC_EMissionType.CRASHSITE, DC_EMissionType.OCCUPATION, DC_EMissionType.CONVOY, DC_EMissionType.PATROL};
			conf.missionTypeArrayDynamic = {DC_EMissionType.OCCUPATION};		
//			conf.missionTypeArrayDynamic = {DC_EMissionType.HUNTER};
//			conf.missionTypeArrayDynamic = {DC_EMissionType.CONVOY};		
//			conf.missionTypeArrayDynamic = {DC_EMissionType.PATROL};		
//			conf.missionTypeArrayDynamic = {DC_EMissionType.CRASHSITE};
//			conf.missionTypeArrayDynamic = {DC_EMissionType.CHOPPER};
		
//			conf.missionTypeArrayStatic = {DC_EMissionType.PATROL, DC_EMissionType.PATROL};
			conf.missionTypeArrayStatic = {DC_EMissionType.CONVOY, DC_EMissionType.CONVOY, DC_EMissionType.CONVOY};
//			conf.missionTypeArrayStatic = {DC_EMissionType.HUNTER};
//			conf.missionTypeArrayStatic = {DC_EMissionType.CRASHSITE};
//			conf.missionTypeArrayStatic = {DC_EMissionType.OCCUPATION};
//			conf.missionTypeArrayStatic = {DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER, DC_EMissionType.SQUATTER};
//			conf.missionTypeArrayStatic = {};
		#endif
	}
}