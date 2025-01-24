//Helpers SCR_DC_MissionFrameJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

const int DC_MISSION_START_DELAY = 10;			//Time to wait before spawning the first mission (seconds)
const int DC_MISSION_ACTIVE_TIME = 180;			//Time to keep the mission active (seconds)
const int DC_MISSION_ACTIVE_DISTANCE = 300;		
const int DC_MISSION_LIFECYCLE_TIME_LIMIT = 10000;

//------------------------------------------------------------------------------------------------
class SCR_DC_MissionFrameConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Mission specific
	DC_LogLevel logLevel;
	//bool debugShowWaypoints = false;
	//bool debugShowMarks = false;
	int missionStartDelay;		//Time to wait before spawning the first mission (seconds)
	int missionCount;			//Maximum amount of missions to be active at the same time
	int missionLifeCycleTime;	//The cycle time to manage mission spawning, deletion etc...
	int missionActiveTime;		//Time to keep the mission active (seconds)
	int missionActiveDistance;	//The distance to a player to keep the mission active 
	ref array<DC_EMissionType> missionTypeArray = {};	//Active missions list
	int minDistanceToMission;	//Distance to another mission. Two missions shall not be too close to each other.
	int minDistanceToPlayer;	//Mission shall not spawn too close a player.
	
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

		if (conf.missionLifeCycleTime < DC_MISSION_LIFECYCLE_TIME_LIMIT)
		{
			SCR_DC_Log.Add("[SCR_DC_MissionConfig] missionLifeCycleTime is less than " + DC_MISSION_LIFECYCLE_TIME_LIMIT + ". This could lead to performance issues.", LogLevel.WARNING);
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
		conf.logLevel = DC_LogLevel.NORMAL;
		conf.missionStartDelay = DC_MISSION_START_DELAY;
		conf.missionCount = 4;
		conf.missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_LIMIT;
		conf.missionActiveTime = DC_MISSION_ACTIVE_TIME;
		conf.missionActiveDistance = DC_MISSION_ACTIVE_DISTANCE;
		
//		conf.missionTypeArray = {DC_EMissionType.NONE, DC_EMissionType.HUNTER, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION};
//		conf.missionTypeArray = {DC_EMissionType.HUNTER, DC_EMissionType.OCCUPATION};		
		conf.missionTypeArray = {DC_EMissionType.OCCUPATION};		
//		conf.missionTypeArray = {DC_EMissionType.HUNTER};
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