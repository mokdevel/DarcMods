//Helpers SCR_DC_MissionFrameJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

#ifdef SCR_DC_RELEASE
	const int DC_MISSION_START_DELAY = 2*60;				//Time to wait before spawning the first mission (seconds)
	const int DC_MISSION_DELAY_BETWEEN_MISSIONS = 5*60;
	const int DC_MISSION_ACTIVE_TIME = 10*60;				//Time to keep the mission active (seconds)
	const int DC_MISSION_ACTIVE_DISTANCE = 300;		
	const int DC_MISSION_LIFECYCLE_TIME_LIMIT = 30;
	const int DC_MISSION_LIFECYCLE_TIME_DEFAULT = 30;
	const int DC_MISSIONFRAME_LIFECYCLE_TIME = 60;
#endif

#ifndef SCR_DC_RELEASE	//Development time options
	const int DC_MISSION_START_DELAY = 30;				//Time to wait before spawning the first mission (seconds)
	const int DC_MISSION_DELAY_BETWEEN_MISSIONS = 1*60;
	const int DC_MISSION_ACTIVE_TIME = 200;				//Time to keep the mission active (seconds)
	const int DC_MISSION_ACTIVE_DISTANCE = 300;		
	const int DC_MISSION_LIFECYCLE_TIME_LIMIT = 10;
	const int DC_MISSION_LIFECYCLE_TIME_DEFAULT = 10;
	const int DC_MISSIONFRAME_LIFECYCLE_TIME = 20;
#endif

//------------------------------------------------------------------------------------------------
class SCR_DC_MissionFrameConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Mission specific
	DC_LogLevel logLevel;
	bool debugShowWaypoints = true;
	bool debugShowMarks = true;
	int missionStartDelay;			//Time to wait before spawning the first mission (seconds)
	int missionDelayBetweeen;		//Time between missions
	int missionCount;				//Maximum amount of missions to be active at the same time
	int missionFrameLifeCycleTime;	//The cycle time to manage mission spawning, deletion etc...
	int missionActiveTime;			//Time to keep the mission active (seconds)
	int missionActiveDistance;		//The distance to a player to keep the mission active 
	int missionHintTime;			//Seconds to show mission hints to players. 0 disables hints.
	ref array<DC_EMissionType> missionTypeArray = {};	//Active missions list
	int minDistanceToMission;		//Distance to another mission. Two missions shall not be too close to each other.
	int minDistanceToPlayer;		//Mission shall not spawn too close to a player.
	
	ref array<ref SCR_DC_NonValidArea> nonValidAreas = {};	
}

class SCR_DC_NonValidArea : Managed
{
	string worldName;
	vector pos;
	float radius;
	string name;

	void Set(string worldName_, vector pos_, float radius_, string name_ = "")	
	{
		worldName = worldName_;
		pos = pos_;
		radius = radius_;
		name = name_;
	}		
}

//------------------------------------------------------------------------------------------------
class SCR_DC_MissionFrameJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig.json";
	ref SCR_DC_MissionFrameConfig conf = new SCR_DC_MissionFrameConfig;

	//------------------------------------------------------------------------------------------------
	void SCR_DC_MissionFrameJsonApi()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_MISSIONCONFIG_FILE);
		if(!loadContext)
		{
			SetDefaults();
			Save("");
			return;
		}
		
		loadContext.ReadValue("", conf);

		if (conf.missionFrameLifeCycleTime < DC_MISSION_LIFECYCLE_TIME_LIMIT)
		{
			SCR_DC_Log.Add("[SCR_DC_MissionFrameConfig] missionLifeCycleTime is less than " + DC_MISSION_LIFECYCLE_TIME_DEFAULT + ". This could lead to performance issues.", LogLevel.WARNING);
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
		conf.logLevel = DC_LogLevel.DEBUG;
		conf.missionStartDelay = DC_MISSION_START_DELAY;
		conf.missionDelayBetweeen = DC_MISSION_DELAY_BETWEEN_MISSIONS;
		conf.missionCount = 4;
		conf.missionFrameLifeCycleTime = DC_MISSIONFRAME_LIFECYCLE_TIME;
		conf.missionActiveTime = DC_MISSION_ACTIVE_TIME;
		conf.missionActiveDistance = DC_MISSION_ACTIVE_DISTANCE;
		conf.missionHintTime = 10;
		
//		conf.missionTypeArray = {DC_EMissionType.NONE, DC_EMissionType.HUNTER, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION};
//		conf.missionTypeArray = {DC_EMissionType.HUNTER, DC_EMissionType.OCCUPATION};
		conf.missionTypeArray = {DC_EMissionType.HUNTER, DC_EMissionType.CRASHSITE, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION};
//		conf.missionTypeArray = {DC_EMissionType.OCCUPATION};		
//		conf.missionTypeArray = {DC_EMissionType.HUNTER};
//		conf.missionTypeArray = {DC_EMissionType.CONVOY};		
//		conf.missionTypeArray = {DC_EMissionType.CRASHSITE};
//		conf.missionTypeArray = {DC_EMissionType.CHOPPER};
		conf.minDistanceToMission = 500;
		conf.minDistanceToPlayer = 100;
		
		//List of non valid areas where mission shall not spawn
		//Eden
		SCR_DC_NonValidArea areaE1 = new SCR_DC_NonValidArea;
		areaE1.Set("Eden", "4780 0 11450", 1000, "Airport - for testing");
		conf.nonValidAreas.Insert(areaE1);
		SCR_DC_NonValidArea areaE2 = new SCR_DC_NonValidArea;
		areaE2.Set("Eden", "9680 0 1560", 1000, "St. Pierre - for testing");
		conf.nonValidAreas.Insert(areaE2);
		
		//Arland
		SCR_DC_NonValidArea areaA1 = new SCR_DC_NonValidArea;
		areaA1.Set("Arland", "1340 0 2320", 200, "Airport - for testing");
		conf.nonValidAreas.Insert(areaA1);
		SCR_DC_NonValidArea areaA2 = new SCR_DC_NonValidArea;
		areaA2.Set("Arland", "1080 0 3300", 100, "Harbour - for testing");
		conf.nonValidAreas.Insert(areaA2);
	}
}