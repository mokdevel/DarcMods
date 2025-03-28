//Helpers SCR_DC_NonValidAreaJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
class SCR_DC_NonValidAreaConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	//Mission specific
	ref array<ref SCR_DC_NonValidArea> nonValidAreas = {};		//List of areas where missions shall not spawn.
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
class SCR_DC_NonValidAreaJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_nonValidArea.json";
		
	ref SCR_DC_NonValidAreaConfig conf = new SCR_DC_NonValidAreaConfig;

	//------------------------------------------------------------------------------------------------
	void SCR_DC_NonValidAreaJsonApi()
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
		//List of non valid areas where mission shall not spawn
		//Eden
		SCR_DC_NonValidArea areaE1 = new SCR_DC_NonValidArea;
		areaE1.Set("Eden", "4780 0 11450", 500, "Airport - for testing");
		conf.nonValidAreas.Insert(areaE1);
		SCR_DC_NonValidArea areaE2 = new SCR_DC_NonValidArea;
		areaE2.Set("Eden", "9680 0 1560", 400, "St. Pierre - for testing");
		conf.nonValidAreas.Insert(areaE2);
		
		//Arland
		SCR_DC_NonValidArea areaA1 = new SCR_DC_NonValidArea;
		areaA1.Set("Arland", "1340 0 2320", 300, "Airport - for testing");
		conf.nonValidAreas.Insert(areaA1);
		SCR_DC_NonValidArea areaA2 = new SCR_DC_NonValidArea;
		areaA2.Set("Arland", "1080 0 3300", 200, "Harbour - for testing");
		conf.nonValidAreas.Insert(areaA2);
	}
}