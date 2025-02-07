//Helpers SCR_DC_SpawnerConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J

//------------------------------------------------------------------------------------------------
class SCR_DC_Conf : Managed
{
	//Mission specific
	DC_LogLevel logLevel;
	int spawnSetID;			//-1 = random, other numbers are the index of spawnSet
	int spawnRndRadius;
	ref array<ref SCR_DC_Set> sets = {};
}

//------------------------------------------------------------------------------------------------
class SCR_DC_Set : Managed
{
	//Mission specific
	string comment;
	ref array<int> ints;
	ref array<string> strings;
	
	void Set(string comment_, array<int> ints_, array<string> strings_)
	{
		comment = comment_;
		ints = ints_;
		strings = strings_;
	}
}

//------------------------------------------------------------------------------------------------
class SCR_DC_Config : SCR_DC_JsonApi
{
	const string DC_CONFIG_FILE = "dummy.json";
	ref SCR_DC_Conf conf = new SCR_DC_Conf;

	//------------------------------------------------------------------------------------------------
	void Load()
	{	
//		SCR_JsonLoadContext loadContext = LoadConfig(DC_CONFIG_FILE);
		
//		if(!loadContext)
//		{
//			SetDefaults();
//			Save("");
//			return;
//		}
		
		SCR_JsonLoadContext loadContext = new SCR_JsonLoadContext();
		bool success = loadContext.LoadFromFile(DC_CONFIG_FILE);
		if (success)
		{
			SCR_DC_Conf data = new SCR_DC_Conf;
			loadContext.ReadValue("", data);
			SCR_DC_Log.Add("SCR_DC logLevel: " + data.logLevel);
			SCR_DC_Log.Add("SCR_DC comment: " + data.sets[0].comment);
			//loadContext.ReadValue("spawnSets", spawnSets);		
		}
	}	

	//------------------------------------------------------------------------------------------------
	void Save(string data)
	{
		SetFileName(DC_CONFIG_FILE);
		//SCR_JsonSaveContext saveContext = SaveConfigOpen(DC_CONFIG_FILE);
		SCR_JsonSaveContext saveContext = new SCR_JsonSaveContext();
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}	
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		conf.logLevel = DC_LogLevel.NORMAL;
		conf.spawnSetID = 1;
		conf.spawnRndRadius = 100;
		
		//Different spawner confs
		//---
		SCR_DC_Set spawnSet1 = new SCR_DC_Set;
		spawnSet1.Set
		(			
			"Used for Escapists. Spawns random ambulances with some gear",
			{ 11,22,33,44,55,66 },
			{"aa", "bb", "cc"}
		);
		conf.sets.Insert(spawnSet1);
		
		//---
		SCR_DC_Set spawnSet2 = new SCR_DC_Set;
		spawnSet2.Set
		(			
			"Used for Escapists. Spawns random ambulances with some gear",
			{ 111,222,333,444,555,666 },
			{"aaa", "bbb", "ccc"}
		);
		conf.sets.Insert(spawnSet2);		
	}
}