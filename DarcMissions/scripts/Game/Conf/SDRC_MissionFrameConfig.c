//Helpers SDRC_MissionFrameConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

	//Release options
	#ifdef SDRC_RELEASE
		private const int SDRC_MISSION_MIN_DISTANCE = 400;
		private const int SDRC_PLAYER_MIN_DISTANCE = 300;		
		private const int SDRC_PLAYER_MAX_DISTANCE = 2500;		
		private const int SDRC_MISSION_COUNT_DYNAMIC = 10;								//Default amount of dynamic missions to run
		private const float SDRC_MISSION_COUNT_DYNAMIC_MUL = 1.5;
		private const int SDRC_MISSION_COUNT_STATIC = 15;								//Default amount of static missions to run
		private const float SDRC_MISSION_COUNT_STATIC_MUL = 1.1;
		private const int SDRC_MISSION_CYCLE_TIME_DEFAULT = 30;
		private const int SDRC_MISSIONFRAME_START_DELAY = 1*60;							//Time to wait before spawning the first mission (seconds)
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS_DYNAMIC = 8*60;			//Minimum delay between dynamic missions. 
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS_STATIC = 5;				//Minimum delay between static missions. 
		private const int SDRC_MISSION_ACTIVE_TIME_DYNAMIC = 45*60;						//Time to keep the mission active (seconds)
		private const int SDRC_MISSION_ACTIVE_TIME_STATIC = 60*60*10; //10hrs			//Static missions are to be kept alive longer
		private const int SDRC_MISSION_ACTIVE_DISTANCE = 150;							//Mission is to be removed if no players close to the position after the mission active time has passed.
		private const int SDRC_MISSION_ACTIVE_TIME_TO_END = 180;						//Mission is kept active this time once all AIs are dead.
		private const float SDRC_MISSION_ACTIVE_MUL_TO_END = 0.9;
		private const int SDRC_MISSIONFRAME_CYCLE_TIME = 30;							//The cycle to run the mission frame. 
		private const bool SDRC_MISSION_RECREATE_CONFIGS = true;						//Force recreaction of config files. 
		private const int SDRC_MISSION_HINT_TIME = 90;									//Seconds to show the mission hint to players
		private const int SDRC_MISSION_RANDOM_POS = 100;								//The randomization for search radius for mission position 
		private const bool SDRC_MISSION_SHOW_STATIC_MARKER = true;						//Show/hide static mission markers
		private const bool SDRC_MISSION_SHOW_DYNAMIC_MARKER = true;
		private const bool SDRC_MISSION_SHOW_DIFFICULTY_MARKER = true;
		private const bool SDRC_MISSION_SHOW_TIME_LEFT = true;							//Show/hide static mission markers
	#endif
	
	//Development time options
	#ifndef SDRC_RELEASE
		private const int SDRC_MISSION_MIN_DISTANCE = 10;//200;		
		private const int SDRC_PLAYER_MIN_DISTANCE = 300;		
		private const int SDRC_PLAYER_MAX_DISTANCE = 1200;		
		private const int SDRC_MISSION_COUNT_DYNAMIC = 4;//10;//3;//3;//8;
		private const float SDRC_MISSION_COUNT_DYNAMIC_MUL = 2.0;
		private const int SDRC_MISSION_COUNT_STATIC = 8;
		private const float SDRC_MISSION_COUNT_STATIC_MUL = 3;
		private const int SDRC_MISSION_CYCLE_TIME_DEFAULT = 20;
		private const int SDRC_MISSIONFRAME_START_DELAY = 2;					
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS_DYNAMIC = 1;
		private const int SDRC_MISSION_DELAY_BETWEEN_MISSIONS_STATIC = 1;
		private const int SDRC_MISSION_ACTIVE_TIME_DYNAMIC = 5*60;				
		private const int SDRC_MISSION_ACTIVE_TIME_STATIC = 8*60;//SDRC_MISSION_ACTIVE_TIME_DYNAMIC * 10;	
		private const int SDRC_MISSION_ACTIVE_DISTANCE = 150;
		private const int SDRC_MISSION_ACTIVE_TIME_TO_END = 45;
		private const float SDRC_MISSION_ACTIVE_MUL_TO_END = 0.9;
		private const int SDRC_MISSIONFRAME_CYCLE_TIME = 10;//20;
		private const bool SDRC_MISSION_RECREATE_CONFIGS = true;
		private const int SDRC_MISSION_HINT_TIME = 90;
		private const int SDRC_MISSION_RANDOM_POS = 100;
		private const bool SDRC_MISSION_SHOW_STATIC_MARKER = true;
		private const bool SDRC_MISSION_SHOW_DYNAMIC_MARKER = true;
		private const bool SDRC_MISSION_SHOW_DIFFICULTY_MARKER = true;
		private const bool SDRC_MISSION_SHOW_TIME_LEFT = true;
	#endif

//------------------------------------------------------------------------------------------------
class SDRC_MissionFrameConfig : SDRC_Config
{
	//Default information
	string author = "darc";
	//Mission specific
	string comment;
	bool recreateConfigs;						//If set to true, all configs are to be written to disk. Should be run only first time.
	bool safeUpdate;							//Stub for safe update functionality.
	//Timing specific
	int missionStartDelay;						//Time to wait before spawning the first mission (seconds).
	int missionFrameCycleTime;					//The cycle time to manage mission spawning, deletion etc... (seconds)
	int missionActiveDistance;					//The distance to a player to keep the mission active.
	int missionActiveTimeToEnd;					//Time to keep the mission active once all AI is dead. Used for both dynamic and static missions.
	float missionActiveDistanceMul;				//Multiplier to modify distance on every cycle when in win/lose state.
	float missionActiveTimeToEndMul;			//Multiplier to modify time on every cycle when in win/lose state.
	int missionHintTime;						//Seconds to show mission hints to players. 0 disables hints.
	SDRC_EHintPosition missionHintPosition;		//Mission hint position
	//Randomization
	int missionRandomPos;						//The distance to randomize the missions position. This avoids mission appearing always in same place.
	//Misc
	int minDistanceToMission;					//Distance to another mission. Two missions shall not be too close to each other.
	int minDistanceToPlayer;					//Mission shall not spawn too close to a player.
	int maxDistanceToPlayer;					//Mission shall not spawn further than this to a player.
	bool showStaticMissionMarker;				//Show static mission marker
	bool showDynamicMissionMarker;				//Show dynamic mission marker
	bool showMissionDifficulty;					//Show mission difficulty on marker
	bool showMissionTimeLeft;					//Show mission time left on marker click
	ref array<string> enemyFactions;			//Factions to use for enemy selection
	ref array<int> missionLimit = {};			//Limits for different types of missions
	ref array<int> missionDifficultyList = {};	//Mission difficulty selection list
	ref SDRC_MissionDifficulty missionDifficulty = new SDRC_MissionDifficulty();
	ref SDRC_MissionTypeConfig missionDynamic = new SDRC_MissionTypeConfig();
	ref SDRC_MissionTypeConfig missionStatic = new SDRC_MissionTypeConfig();
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_MissionFrameConfig data = SDRC_MissionFrameConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		
		comment = "Simple comment, not used in game";

		recreateConfigs = SDRC_MISSION_RECREATE_CONFIGS;
		missionStartDelay = SDRC_MISSIONFRAME_START_DELAY;
		missionFrameCycleTime = SDRC_MISSIONFRAME_CYCLE_TIME;
		
		missionDynamic.count = SDRC_MISSION_COUNT_DYNAMIC;
		missionDynamic.countMul = SDRC_MISSION_COUNT_DYNAMIC_MUL;
		missionDynamic.activeTime = SDRC_MISSION_ACTIVE_TIME_DYNAMIC;
		missionDynamic.delayBetween = SDRC_MISSION_DELAY_BETWEEN_MISSIONS_DYNAMIC;
		
		missionStatic.count = SDRC_MISSION_COUNT_STATIC;
		missionStatic.countMul = SDRC_MISSION_COUNT_STATIC_MUL;
		missionStatic.activeTime = SDRC_MISSION_ACTIVE_TIME_STATIC;
		missionStatic.delayBetween = SDRC_MISSION_DELAY_BETWEEN_MISSIONS_STATIC;
		
		missionActiveDistance = SDRC_MISSION_ACTIVE_DISTANCE;
		missionActiveTimeToEnd = SDRC_MISSION_ACTIVE_TIME_TO_END;
		missionActiveDistanceMul = SDRC_MISSION_ACTIVE_MUL_TO_END;
		missionActiveTimeToEndMul = SDRC_MISSION_ACTIVE_MUL_TO_END;
		
		missionHintTime = SDRC_MISSION_HINT_TIME;
		missionHintPosition = SDRC_EHintPosition.UP_LEFT;
//		missionHintPosition = SDRC_EHintPosition.DOWN_LEFT;
		missionRandomPos = SDRC_MISSION_RANDOM_POS;
		
		minDistanceToMission = SDRC_MISSION_MIN_DISTANCE;
		minDistanceToPlayer = SDRC_PLAYER_MIN_DISTANCE;
		maxDistanceToPlayer = SDRC_PLAYER_MAX_DISTANCE;
		showStaticMissionMarker = SDRC_MISSION_SHOW_STATIC_MARKER;
		showDynamicMissionMarker = SDRC_MISSION_SHOW_DYNAMIC_MARKER;
		showMissionDifficulty = SDRC_MISSION_SHOW_DIFFICULTY_MARKER;
		showMissionTimeLeft = SDRC_MISSION_SHOW_TIME_LEFT;
		
		#ifdef SDRC_RELEASE
			enemyFactions = {"USSR"};
			missionLimit = {
				  -1, // 0 - NONE
				  2,  // 1 - HUNTER
				  5,  // 2 - OCCUPATION
				  7,  // 3 - CONVOY
				  2,  // 4 - CRASHSITE
				  8,  // 5 - PATROL
				  3,  // 6 - SQUATTERS
				  8,  // 7 - ROADBLOCK
				  2,  // 8 - HVTVIP
				  2,  // 9 - HVTITEM
				  2,  //10 - STASH
				  4,  //11 - CHOPPER
			};
			missionDifficultyList = {0,1,1,2,2,2,2,2,2,2,2,2,2,2,3,3,3,4,4};
		
			missionDynamic.missionTypeArray = {
											SDRC_EMissionType.STASH, 
											SDRC_EMissionType.CRASHSITE, 
											SDRC_EMissionType.CHOPPER, SDRC_EMissionType.CHOPPER, SDRC_EMissionType.CHOPPER, 
											SDRC_EMissionType.HUNTER, SDRC_EMissionType.HUNTER, SDRC_EMissionType.HUNTER, 
											SDRC_EMissionType.CONVOY, SDRC_EMissionType.CONVOY, 
											SDRC_EMissionType.ROADBLOCK, SDRC_EMissionType.ROADBLOCK, SDRC_EMissionType.ROADBLOCK,
											SDRC_EMissionType.HVTVIP, SDRC_EMissionType.HVTVIP, SDRC_EMissionType.HVTVIP,
											SDRC_EMissionType.HVTITEM, SDRC_EMissionType.HVTITEM, SDRC_EMissionType.HVTITEM,
											SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.SQUATTERS, 
											SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.SQUATTERS, 
											SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, 
											SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.OCCUPATION, 
											};
			missionStatic.missionTypeArray = {
											SDRC_EMissionType.PATROL, SDRC_EMissionType.PATROL, SDRC_EMissionType.PATROL, SDRC_EMissionType.PATROL, 
											SDRC_EMissionType.CONVOY, SDRC_EMissionType.CONVOY, SDRC_EMissionType.CONVOY, 
											SDRC_EMissionType.ROADBLOCK, SDRC_EMissionType.ROADBLOCK, SDRC_EMissionType.ROADBLOCK,
											SDRC_EMissionType.CHOPPER
											};
		#endif	

		#ifndef SDRC_RELEASE				
			enemyFactions = {"USSR"};
//			enemyFactions = {"FIA"};
//			enemyFactions = {"AFOA"};
//			enemyFactions = {"DSF"};
//			enemyFactions = {"US"};
//			enemyFactions = {"BACON_622120A5448725E3_FACTION", "BALLIEN_BC_FACTION"};
//			enemyFactions = {"BACON_622120A5448725E3_FACTION"};
//			enemyFactions = {"BALLIEN_BC_FACTION"};
//			enemyFactions = {"TF_RF"};
//			enemyFactions = {"TF_US"};
//			enemyFactions = {"US","UK"};
//			enemyFactions = {"UK"};
//			enemyFactions = {"ChinesePLA"};
//			enemyFactions = {"JSDF_RAS"};
//			enemyFactions = {"FIA", "US"};
//			enemyFactions = {"USSR", "FIA", "FIA", "FIA"};
//			enemyFactions = {"RHS_USAF", "RHS_AFRF"};
//			enemyFactions = {"USAF_USMC", "RHS_RF"};
//			enemyFactions = {"RHS_ION"};
//			enemyFactions = {"MEI"};
//			enemyFactions = {"PLASTICBANDIT"};
//			enemyFactions = {"Takistan"};
//			enemyFactions = {"GRSPMC"};
		
			missionLimit = {
				  -1, // 0 - NONE
				  2,  // 1 - HUNTER
				  5,  // 2 - OCCUPATION
				  5,  // 3 - CONVOY
				  2,  // 4 - CRASHSITE
				  8,  // 5 - PATROL
				  3,  // 6 - SQUATTERS
				  8,  // 7 - ROADBLOCK
				  2,  // 8 - HVTVIP
				  2,  // 9 - HVTITEM
				  2,  //10 - STASH
				  -1,  //11 - CHOPPER
			};
		
			missionLimit = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
		
			missionDifficultyList = {0,1,2,3,4};
//			missionDifficultyList = {0};
		
			missionDynamic.missionTypeArray = {SDRC_EMissionType.CHOPPER};
//			missionDynamic.missionTypeArray = {SDRC_EMissionType.CONVOY};
//			missionDynamic.missionTypeArray = {SDRC_EMissionType.CRASHSITE};
//			missionDynamic.missionTypeArray = {SDRC_EMissionType.HUNTER};
//			missionDynamic.missionTypeArray = {SDRC_EMissionType.HVTITEM};
//			missionDynamic.missionTypeArray = {SDRC_EMissionType.HVTVIP};
//			missionDynamic.missionTypeArray = {SDRC_EMissionType.OCCUPATION};
//			missionDynamic.missionTypeArray = {SDRC_EMissionType.PATROL};
//			missionDynamic.missionTypeArray = {SDRC_EMissionType.ROADBLOCK};
//			missionDynamic.missionTypeArray = {SDRC_EMissionType.SQUATTERS};
//			missionDynamic.missionTypeArray = {SDRC_EMissionType.STASH};
//			missionDynamic.missionTypeArray = {SDRC_EMissionType.CONVOY, SDRC_EMissionType.CRASHSITE, SDRC_EMissionType.HUNTER, SDRC_EMissionType.HVTITEM, SDRC_EMissionType.HVTVIP, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.PATROL, SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.STASH, SDRC_EMissionType.CHOPPER};
//			missionDynamic.missionTypeArray = {SDRC_EMissionType.CRASHSITE, SDRC_EMissionType.HUNTER, SDRC_EMissionType.HVTITEM, SDRC_EMissionType.HVTVIP, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.PATROL, SDRC_EMissionType.SQUATTERS};
		
			missionStatic.missionTypeArray = {SDRC_EMissionType.CHOPPER};
//			missionStatic.missionTypeArray = {SDRC_EMissionType.CONVOY};
//			missionStatic.missionTypeArray = {SDRC_EMissionType.CRASHSITE};
//			missionStatic.missionTypeArray = {SDRC_EMissionType.HUNTER};
//			missionStatic.missionTypeArray = {SDRC_EMissionType.HVTITEM};
//			missionStatic.missionTypeArray = {SDRC_EMissionType.HVTVIP};
//			missionStatic.missionTypeArray = {SDRC_EMissionType.OCCUPATION};
//			missionStatic.missionTypeArray = {SDRC_EMissionType.PATROL};
//			missionStatic.missionTypeArray = {SDRC_EMissionType.ROADBLOCK};
//			missionStatic.missionTypeArray = {SDRC_EMissionType.SQUATTERS};
//			missionStatic.missionTypeArray = {SDRC_EMissionType.STASH};
//			missionStatic.missionTypeArray = {};
//			missionStatic.missionTypeArray = {SDRC_EMissionType.CONVOY, SDRC_EMissionType.CRASHSITE, SDRC_EMissionType.HUNTER, SDRC_EMissionType.HVTITEM, SDRC_EMissionType.HVTVIP, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.PATROL, SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.STASH, SDRC_EMissionType.CHOPPER};
//			missionStatic.missionTypeArray = {SDRC_EMissionType.HVTITEM, SDRC_EMissionType.HVTVIP, SDRC_EMissionType.OCCUPATION, SDRC_EMissionType.SQUATTERS, SDRC_EMissionType.STASH};
//			missionStatic.missionTypeArray = {SDRC_EMissionType.CONVOY, SDRC_EMissionType.CHOPPER};		
		
		#endif
	}
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
	ref array<float> aiCountCoef = 			{0.50, 0.60, 1.0, 1.40, 1.80};
	ref array<float> aiSkillCoef = 			{0.20, 0.60, 1.0, 1.30, 1.60};
	ref array<float> aiPerceptionCoef = 	{0.20, 0.60, 1.0, 1.30, 1.60};
	ref array<float> lootChanceCoef = 		{0.50, 0.60, 1.0, 1.10, 1.20};	
	ref array<float> lootCountCoef = 		{0.70, 0.90, 1.0, 1.50, 2.00};	
	ref array<float> rewardCoef = 			{0.50, 0.70, 1.0, 2.00, 3.00};	
	ref array<float> qrfChance = 			{0.80, 0.90, 1.0, 1.20, 1.30};
}