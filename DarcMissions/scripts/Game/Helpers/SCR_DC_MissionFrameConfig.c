//Helpers SCR_DC_MissionFrameConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

const int MISSION_LIFECYCLE_TIME_LIMIT = 10000;

//------------------------------------------------------------------------------------------------
class SCR_DC_MissionFrameConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Mission specific
	DC_LogLevel logLevel;	
	int missionCount;			//Maximum amount of missions to be active at the same time
	int missionLifeCycleTime;	//The cycle time to manage mission spawning, deletion etc...
	ref array<DC_EMissionType> missionTypeArray = {};	//Active missions list
	int minDistanceToMission;	//Distance to another mission. Two missions shall not be too close to each other.
	int minDistanceToPlayer;	//Mission shall not spawn too close a player.
	
	ref array<ref SCR_DC_NonValidArea> nonValidAreas = {};	
}

class SCR_DC_NonValidArea : Managed
{
	vector pos;
	float radius;
	string name;
	
	void Set(vector pos_, float radius_, string name_ = "")	
	{
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

		if (conf.missionLifeCycleTime < MISSION_LIFECYCLE_TIME_LIMIT)
		{
			SCR_DC_Log.Add("[SCR_DC_MissionConfig] missionLifeCycleTime is less than " + MISSION_LIFECYCLE_TIME_LIMIT + ". This could lead to performance issues.", LogLevel.WARNING);
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
		conf.missionCount = 4;
		conf.missionLifeCycleTime = MISSION_LIFECYCLE_TIME_LIMIT;			
//		conf.missionTypeArray = {DC_EMissionType.NONE, DC_EMissionType.HUNTER, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION};
//		conf.missionTypeArray = {DC_EMissionType.HUNTER, DC_EMissionType.OCCUPATION};		
		conf.missionTypeArray = {DC_EMissionType.OCCUPATION};		
//		conf.missionTypeArray = {DC_EMissionType.HUNTER};
		conf.minDistanceToMission = 500;
		conf.minDistanceToPlayer = 100;
		SCR_DC_NonValidArea area1 = new SCR_DC_NonValidArea;
		area1.Set("4780 0 11450", 1000, "Airport - for testing");
		conf.nonValidAreas.Insert(area1);
		SCR_DC_NonValidArea area2 = new SCR_DC_NonValidArea;
		area2.Set("9680 0 1560", 1000, "St. Pierre - for testing");
		conf.nonValidAreas.Insert(area2);
	}
}