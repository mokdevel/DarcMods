//Helpers SDRC_MissionFrameJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

	//Release options
	#ifdef SDRC_RELEASE
		private const int SDRC_MISSION_MIN_DISTANCE = 500;
		private const int SDRC_PLAYER_MIN_DISTANCE = 100;		
		private const int SDRC_MISSION_COUNT_DYNAMIC = 10;								//Default amount of dynamic missions to run
		private const float SDRC_MISSION_COUNT_DYNAMIC_MUL = 1.5;
		private const int SDRC_MISSION_COUNT_STATIC = 6;								//Default amount of static missions to run
		private const float SDRC_MISSION_COUNT_STATIC_MUL = 1;
		private const int SDRC_MISSION_CYCLE_TIME_DEFAULT = 30;
		private const int SDRC_MISSIONFRAME_START_DELAY = 1*60;							//Time to wait before spawning the first mission (seconds)
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS_DYNAMIC = 8*60;			//Minimum delay between dynamic missions. 
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS_STATIC = 5;				//Minimum delay between static missions. 
		private const int SDRC_MISSION_ACTIVE_TIME_DYNAMIC = 30*60;						//Time to keep the mission active (seconds)
		private const int SDRC_MISSION_ACTIVE_TIME_STATIC = 60*60*10; //10hrs			//Static missions are to be kept alive longer
		private const int SDRC_MISSION_ACTIVE_DISTANCE = 150;							//Mission is to be removed if no players close to the position after the mission active time has passed.
		private const int SDRC_MISSION_ACTIVE_TIME_TO_END = 180;						//Mission is kept active this time once all AIs are dead.
		private const float SDRC_MISSION_ACTIVE_MUL_TO_END = 0.9;
		private const int SDRC_MISSIONFRAME_CYCLE_TIME = 30;							//The cycle to run the mission frame. 
		private const bool SDRC_MISSION_RECREATE_CONFIGS = true;						//Force recreaction of config files. 
		private const int SDRC_MISSION_HINT_TIME = 90;									//Seconds to show the mission hint to players
		private const int SDRC_MISSION_RANDOM_POS = 100;								//The randomization for search radius for mission position 
		private const bool SDRC_MISSION_SHOW_STATIC_MARKER = true;						//Show/hide static mission markers
		private const bool SDRC_MISSION_SHOW_TIME_LEFT = true;						//Show/hide static mission markers
	#endif
	
	//Development time options
	#ifndef SDRC_RELEASE
		private const int SDRC_MISSION_MIN_DISTANCE = 200;		
		private const int SDRC_PLAYER_MIN_DISTANCE = 100;		
		private const int SDRC_MISSION_COUNT_DYNAMIC = 0;//10;//3;//3;//8;
		private const float SDRC_MISSION_COUNT_DYNAMIC_MUL = 2.0;
		private const int SDRC_MISSION_COUNT_STATIC = 1;//10;//15;//5;//3;//0;//10;
		private const float SDRC_MISSION_COUNT_STATIC_MUL = 3;
		private const int SDRC_MISSION_CYCLE_TIME_DEFAULT = 20;
		private const int SDRC_MISSIONFRAME_START_DELAY = 2;					
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS_DYNAMIC = 1;//10;//1*20;
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS_STATIC = 10;
		private const int SDRC_MISSION_ACTIVE_TIME_DYNAMIC = 5*60;				
		private const int SDRC_MISSION_ACTIVE_TIME_STATIC = SDRC_MISSION_ACTIVE_TIME_DYNAMIC * 10;	
		private const int SDRC_MISSION_ACTIVE_DISTANCE = 150;
		private const int SDRC_MISSION_ACTIVE_TIME_TO_END = 45;
		private const float SDRC_MISSION_ACTIVE_MUL_TO_END = 0.9;
		private const int SDRC_MISSIONFRAME_CYCLE_TIME = 10;//20;
		private const bool SDRC_MISSION_RECREATE_CONFIGS = true;
		private const int SDRC_MISSION_HINT_TIME = 90;
		private const int SDRC_MISSION_RANDOM_POS = 100;
		private const bool SDRC_MISSION_SHOW_STATIC_MARKER = true;
		private const bool SDRC_MISSION_SHOW_TIME_LEFT = true;
	#endif

//------------------------------------------------------------------------------------------------
class SDRC_MissionFrameConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Mission specific
	string comment;
	bool recreateConfigs;					//If set to true, all configs are to be written to disk. Should be run only first time.
	//Timing specific
	int missionStartDelay;					//Time to wait before spawning the first mission (seconds).
	int missionFrameCycleTime;				//The cycle time to manage mission spawning, deletion etc... (seconds)
	int missionActiveDistance;				//The distance to a player to keep the mission active.
	int missionActiveTimeToEnd;				//Time to keep the mission active once all AI is dead. Used for both dynamic and static missions.
	float missionActiveDistanceMul;			//Multiplier to modify distance on every cycle when in win/lose state.
	float missionActiveTimeToEndMul;		//multiplier to modify time on every cycle when in win/lose state.
	int missionHintTime;					//Seconds to show mission hints to players. 0 disables hints.
	//Randomization
	int missionRandomPos;					//The distance to randomize the missions position. This avoids mission appearing always in same place.
	//Misc
	int minDistanceToMission;				//Distance to another mission. Two missions shall not be too close to each other.
	int minDistanceToPlayer;				//Mission shall not spawn too close to a player.
	bool showStaticMissionMarker;			//Show static mission marker
	bool showMissionTimeLeft;				//Show mission time left on marker click
	ref array<string>enemyFactions;			//Factions to use for enemy selection
	ref SDRC_MissionDifficulty missionDifficulty = new SDRC_MissionDifficulty();
	ref SDRC_MissionTypeConfig missionDynamic = new SDRC_MissionTypeConfig();
	ref SDRC_MissionTypeConfig missionStatic = new SDRC_MissionTypeConfig();
}

//------------------------------------------------------------------------------------------------
class SDRC_MissionTypeConfig : Managed
{
	int count;
	float countMul;
	int activeTime;
	int delayBetween;
	ref array<SDRC_EMissionType> missionTypeArray = {};	//List mission types that spawn randomly
}

//------------------------------------------------------------------------------------------------
class SDRC_MissionDifficulty : Managed
{
	//easy, moderate, normal, tough, hard
	ref array<float> aiCountCoef = 			{0.5, 0.6, 1.0, 2.0, 3.0};
	ref array<float> aiSkillCoef = 			{0.2, 0.6, 1.0, 1.3, 1.6};
	ref array<float> aiPerceptionCoef = 	{0.2, 0.6, 1.0, 1.3, 1.6};
	ref array<float> lootChanceCoef = 		{0.5, 0.6, 1.0, 1.1, 1.2};	
	ref array<float> lootCountCoef = 		{1.0, 1.0, 1.0, 1.0, 1.0};	
}

//------------------------------------------------------------------------------------------------
class SDRC_MissionFrameJsonApi : SDRC_JsonApi
{
	ref SDRC_MissionFrameConfig conf = new SDRC_MissionFrameConfig();

	//------------------------------------------------------------------------------------------------
	void SDRC_MissionFrameJsonApi(string fileName)
	{
		SetFileName(fileName);
	}
	
	//------------------------------------------------------------------------------------------------
	bool Load(bool createMissingFiles = true)
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(createMissingFiles);		
		if (!loadContext)
		{
			if (!createMissingFiles)
			{
				return false;
			}
			SetDefaults();
			Save();
			return true;
		}
		
		loadContext.ReadValue("", conf);
		return true;
	}	

	//------------------------------------------------------------------------------------------------
	void Save()
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen();
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		conf.comment = "Simple comment, not used in game";

		conf.recreateConfigs = SDRC_MISSION_RECREATE_CONFIGS;
		conf.missionStartDelay = SDRC_MISSIONFRAME_START_DELAY;
		conf.missionFrameCycleTime = SDRC_MISSIONFRAME_CYCLE_TIME;
		
		conf.missionDynamic.count = SDRC_MISSION_COUNT_DYNAMIC;
		conf.missionDynamic.countMul = SDRC_MISSION_COUNT_DYNAMIC_MUL;
		conf.missionDynamic.activeTime = SDRC_MISSION_ACTIVE_TIME_DYNAMIC;
		conf.missionDynamic.delayBetween = SDRC_MISSION_DELAY_BETWEEN_MISSIONS_DYNAMIC;
		
		conf.missionStatic.count = SDRC_MISSION_COUNT_STATIC;
		conf.missionStatic.countMul = SDRC_MISSION_COUNT_STATIC_MUL;
		conf.missionStatic.activeTime = SDRC_MISSION_ACTIVE_TIME_STATIC;
		conf.missionStatic.delayBetween = SDRC_MISSION_DELAY_BETWEEN_MISSIONS_STATIC;
		
		conf.missionActiveDistance = SDRC_MISSION_ACTIVE_DISTANCE;
		conf.missionActiveTimeToEnd = SDRC_MISSION_ACTIVE_TIME_TO_END;
		conf.missionActiveDistanceMul = SDRC_MISSION_ACTIVE_MUL_TO_END;
		conf.missionActiveTimeToEndMul = SDRC_MISSION_ACTIVE_MUL_TO_END;
		
		conf.missionHintTime = SDRC_MISSION_HINT_TIME;
		conf.missionRandomPos = SDRC_MISSION_RANDOM_POS;
		
		conf.minDistanceToMission = SDRC_MISSION_MIN_DISTANCE;
		conf.minDistanceToPlayer = SDRC_PLAYER_MIN_DISTANCE;
		conf.showStaticMissionMarker = SDRC_MISSION_SHOW_STATIC_MARKER;
		conf.showMissionTimeLeft = SDRC_MISSION_SHOW_TIME_LEFT;
		
		#ifdef SDRC_RELEASE
			conf.enemyFactions = {"USSR"};
			conf.missionDynamic.missionTypeArray = {
											SDRC_EMissionType.CRASHSITE, 
											SDRC_EMissionType.HUNTER, SDRC_EMissionType.HUNTER, 
											SDRC_EMissionType.CONVOY, SDRC_EMissionType.CONVOY, SDRC_EMissionType.CONVOY, 
											SDRC_EMissionType.ROADBLOCK, SDRC_EMissionType.ROADBLOCK, SDRC_EMissionType.ROADBLOCK, SDRC_EMissionType.ROADBLOCK, SDRC_EMissionType.ROADBLOCK, SDRC_EMissionType.ROADBLOCK, 
											SDRC_EMissionType.HVTVIP, SDRC_EMissionType.HVTVIP, SDRC_EMissionType.HVTVIP,
											SDRC_EMissionType.HVTITEM, SDRC_EMissionType.HVTITEM, SDRC_EMissionType.HVTITEM,
											SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.SQUATTERS, 
											SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.SQUATTERS, 
											SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, 
											SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, 
											};
			conf.missionStatic.missionTypeArray = {
											SDRC_EMissionType.PATROL, SDRC_EMissionType.PATROL, SDRC_EMissionType.PATROL, 
											SDRC_EMissionType.CONVOY, SDRC_EMissionType.CONVOY, 
											SDRC_EMissionType.ROADBLOCK, SDRC_EMissionType.ROADBLOCK
											};
		#endif	

		#ifndef SDRC_RELEASE				
//			conf.enemyFactions = {"US"};
//			conf.enemyFactions = {"BACON_622120A5448725E3_FACTION", "BALLIEN_BC_FACTION"};
//			conf.enemyFactions = {"BALLIEN_BC_FACTION"};
			conf.enemyFactions = {"USSR"};
//			conf.enemyFactions = {"TF_RF"};
//			conf.enemyFactions = {"TF_US"};
//			conf.enemyFactions = {"RHS_ION"};
//			conf.enemyFactions = {"US","UK"};
//			conf.enemyFactions = {"UK"};
//			conf.enemyFactions = {"FIA", "US"};
//			conf.enemyFactions = {"USSR", "FIA", "FIA", "FIA"};
//			conf.enemyFactions = {"RHS_USAF", "RHS_AFRF"};
//			conf.enemyFactions = {"USAF_USMC", "RHS_RF"};
//			conf.enemyFactions = {"MEI"};
		
			conf.missionDynamic.missionTypeArray = {SDRC_EMissionType.CONVOY, SDRC_EMissionType.CRASHSITE, SDRC_EMissionType.HUNTER, SDRC_EMissionType.HVTITEM, SDRC_EMissionType.HVTVIP, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.PATROL, SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.STASH};
//			conf.missionDynamic.missionTypeArray = {SDRC_EMissionType.OCCUPATION};
//			conf.missionDynamic.missionTypeArray = {SDRC_EMissionType.HUNTER};
//			conf.missionDynamic.missionTypeArray = {SDRC_EMissionType.CONVOY};		
//			conf.missionDynamic.missionTypeArray = {SDRC_EMissionType.PATROL};		
//			conf.missionDynamic.missionTypeArray = {SDRC_EMissionType.CRASHSITE};
//			conf.missionDynamic.missionTypeArray = {SDRC_EMissionType.SQUATTERS};
//			conf.missionDynamic.missionTypeArray = {SDRC_EMissionType.STASH};
//			conf.missionDynamic.missionTypeArray = {SDRC_EMissionType.OCCUPATION};
//			conf.missionDynamic.missionTypeArray = {SDRC_EMissionType.HVTITEM};
		
//			conf.missionStatic.missionTypeArray = {SDRC_EMissionType.CHOPPER};
//			conf.missionStatic.missionTypeArray = {SDRC_EMissionType.CONVOY};
//			conf.missionStatic.missionTypeArray = {SDRC_EMissionType.CRASHSITE};
//			conf.missionStatic.missionTypeArray = {SDRC_EMissionType.HUNTER};
//			conf.missionStatic.missionTypeArray = {SDRC_EMissionType.HVTITEM};
//			conf.missionStatic.missionTypeArray = {SDRC_EMissionType.HVTVIP};
//			conf.missionStatic.missionTypeArray = {SDRC_EMissionType.OCCUPATION};
//			conf.missionStatic.missionTypeArray = {SDRC_EMissionType.PATROL};
//			conf.missionStatic.missionTypeArray = {SDRC_EMissionType.ROADBLOCK};
//			conf.missionStatic.missionTypeArray = {SDRC_EMissionType.SQUATTERS};
			conf.missionStatic.missionTypeArray = {SDRC_EMissionType.STASH};
//			conf.missionStatic.missionTypeArray = {};
//			conf.missionStatic.missionTypeArray = {SDRC_EMissionType.CONVOY, SDRC_EMissionType.CRASHSITE, SDRC_EMissionType.HUNTER, SDRC_EMissionType.HVTITEM, SDRC_EMissionType.HVTVIP, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.PATROL, SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.STASH};		
//			conf.missionStatic.missionTypeArray = {SDRC_EMissionType.CONVOY, SDRC_EMissionType.CRASHSITE};		
		
		#endif
	}
}