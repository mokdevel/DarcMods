//Helpers SCR_DC_MissionFrameConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

const int MISSION_LIFECYCLE_TIME_LIMIT = 10000;

//------------------------------------------------------------------------------------------------
class SCR_DC_Frame : Managed
{
	//Mission specific
	DC_LogLevel logLevel;	
	[Attribute(defvalue: "1", UIWidgets.Slider, "Search radius in meters", "0 10")]	
	int missionCount;			//Maximum amount of missions to be active at the same time
	int missionLifeCycleTime;	//The cycle time to manage mission spawning, deletion etc...
	ref array<DC_EMissionType> missionTypeArray = {};	//Active missions list
	int minDistanceToMission;	//Distance to another mission. Two missions shall not be too close to each other.
	int minDistanceToPlayer;	//Mission shall not spawn too close a player.
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
class SCR_DC_MissionFrameConfig : SCR_DC_JsonConfig
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig.json";
	ref SCR_DC_Frame frame = new SCR_DC_Frame;
	ref array<ref SCR_DC_NonValidArea> nonValidAreas = {};

	//------------------------------------------------------------------------------------------------
	void SCR_DC_MissionFrameConfig()
	{
		version = 1;
		author = "darc";
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
		
		loadContext.ReadValue("frame", frame);
		loadContext.ReadValue("nonValidAreas", nonValidAreas);

		if (frame.missionLifeCycleTime < MISSION_LIFECYCLE_TIME_LIMIT)
		{
			SCR_DC_Log.Add("[SCR_DC_MissionConfig] missionLifeCycleTime is less than " + MISSION_LIFECYCLE_TIME_LIMIT + ". This could lead to performance issues.", LogLevel.WARNING);
		}
	}	

	//------------------------------------------------------------------------------------------------
	void Save(string data)
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen(DC_MISSIONCONFIG_FILE);
		saveContext.WriteValue("frame", frame);
		saveContext.WriteValue("nonValidAreas", nonValidAreas);
		SaveConfigClose(saveContext);
	}	
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		frame.logLevel = DC_LogLevel.NORMAL;
		frame.missionCount = 4;
		frame.missionLifeCycleTime = MISSION_LIFECYCLE_TIME_LIMIT;			
//		frame.missionTypeArray = {DC_EMissionType.NONE, DC_EMissionType.HUNTER, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION, DC_EMissionType.OCCUPATION};
//		frame.missionTypeArray = {DC_EMissionType.HUNTER, DC_EMissionType.OCCUPATION};		
		frame.missionTypeArray = {DC_EMissionType.OCCUPATION};		
//		frame.missionTypeArray = {DC_EMissionType.HUNTER};
		frame.minDistanceToMission = 500;
		frame.minDistanceToPlayer = 100;
		SCR_DC_NonValidArea area1 = new SCR_DC_NonValidArea;
		area1.Set("4780 0 11450", 1000, "Airport - for testing");
		nonValidAreas.Insert(area1);
		SCR_DC_NonValidArea area2 = new SCR_DC_NonValidArea;
		area2.Set("9680 0 1560", 1000, "St. Pierre - for testing");
		nonValidAreas.Insert(area2);
	}
}