//Helpers SDRC_NonValidAreaJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
class SDRC_NonValidArea : Managed
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
class SDRC_NonValidAreaConfig : SDRC_Config
{
	//Default information
	string author = "darc";
	//Mission specific
	ref array<ref SDRC_NonValidArea> m_NonValidAreas = {};		//List of areas where missions shall not spawn.

	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_NonValidAreaConfig data = SDRC_NonValidAreaConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		
		
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		
		#ifdef SDRC_CREATE_EXAMPLE_NONVALIDAREA
			//List of non valid areas where missions shall not spawn
			//Eden
			SDRC_NonValidArea areaE1 = new SDRC_NonValidArea();
			areaE1.Set("Eden", "4780 0 11450", 500, "Eden - Airport - for testing");
			m_NonValidAreas.Insert(areaE1);
			SDRC_NonValidArea areaE2 = new SDRC_NonValidArea();
			areaE2.Set("Eden", "9680 0 1560", 400, "Eden - St. Pierre - for testing");
			m_NonValidAreas.Insert(areaE2);
			SDRC_NonValidArea areaE3 = new SDRC_NonValidArea();
			areaE3.Set("Eden", "8800 0 3950", 800, "Eden - Quarry - for testing");
			m_NonValidAreas.Insert(areaE3);
			
			//Arland
			SDRC_NonValidArea areaA1 = new SDRC_NonValidArea();
			areaA1.Set("Arland", "1340 0 2320", 300, "Arland - Airport - for testing");
			m_NonValidAreas.Insert(areaA1);
			SDRC_NonValidArea areaA2 = new SDRC_NonValidArea();
			areaA2.Set("Arland", "1080 0 3300", 400, "Arland - Harbour - for testing");
			m_NonValidAreas.Insert(areaA2);
			SDRC_NonValidArea areaA3 = new SDRC_NonValidArea();
			areaA3.Set("Arland", "4500 0 10700", 300, "Arland - St. Philippe");
			m_NonValidAreas.Insert(areaA3);
	
			//Dummy for Arland, but as worldname is not defined, this will be valid for all worlds.
			SDRC_NonValidArea areaA10 = new SDRC_NonValidArea();
			areaA10.Set("", "900 0 1450", 300, "Dummy for Arland, but as worldname is not defined, this will be valid for all worlds.");
			m_NonValidAreas.Insert(areaA10);		
		
			//Just for testing - a huge blocker area			
/*			float worldSize = SDRC_Misc.GetWorldSize();
			SDRC_NonValidArea areaA11 = new SDRC_NonValidArea();
			vector pos = "0 0 0";
			pos[0] = worldSize/2;
			pos[2] = worldSize/2;
			areaA11.Set("", pos, worldSize/2, "Just a huge NonValidArea");
			m_NonValidAreas.Insert(areaA11);*/
		#endif
	}
	
	//------------------------------------------------------------------------------------------------
	void Populate(out array<ref SDRC_NonValidArea>nonValidAreaList)
	{
		string worldName = SDRC_Misc.GetWorldName(true);
		
		//Pick nonValidAreas for the current world
		foreach (SDRC_NonValidArea nonValidArea : m_NonValidAreas)
		{
			if (nonValidArea.worldName == worldName || nonValidArea.worldName == "")
			{
				nonValidAreaList.Insert(nonValidArea);
			}
		}
		SDRC_Log.Add("[SDRC_NonValidAreaJsonApi:Populate] Number of nonValidAreas defined: " + nonValidAreaList.Count(), LogLevel.NORMAL);			
	}		
}