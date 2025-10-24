//Helpers SDRC_SecondWaveJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
class SDRC_SecondWaveConfig : Managed
{
	int version = 1;
	string author = "darc";
	ref array<ref SDRC_SecondWave> waves = {};	//List of different waves
}

class SDRC_SecondWave : Managed
{
	int subIdx;								//Unique index for the wave. 
	string comment;							//Generic comment to describe the mission. Not used in game.	
	string vehicle;							//Vehicle resourcename
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();	

	void Set(int subIdx_, string comment_, string vehicle_)
	{
		subIdx = subIdx_;
		comment = comment_;
		vehicle = vehicle_;
	}		
}

//------------------------------------------------------------------------------------------------
class SDRC_SecondWaveJsonApi : SDRC_JsonApi
{
	ref SDRC_SecondWaveConfig conf = new SDRC_SecondWaveConfig();

	//------------------------------------------------------------------------------------------------
	void SDRC_SecondWaveJsonApi(string fileName)
	{
		SetFileName(fileName);
	}
			
	//------------------------------------------------------------------------------------------------
	void Populate()
	{
		SDRC_Log.Add("[SDRC_SecondWaveJsonApi:Populate] Number of secondWaves defined: " + conf.waves.Count(), LogLevel.NORMAL);
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
		//----------------------------------------------------
		conf.waves.Insert(Wave0());
	}
	
	//----------------------------------------------------
	SDRC_SecondWave Wave0()
	{
		ref SDRC_SecondWave wave = new SDRC_SecondWave();
		wave.Set(
			0, "index 0: Light group",
			"",
		);
		wave.ai.Set(
			{1, 1},
			{"G_LIGHT"},
			50, 1.0,
			{100, 200},
			SDRC_EWaypointGenerationType.RADIUS,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
			
		return wave;
	};
	
	//------------------------------------------------------------------------------------------------
	SDRC_SecondWave GetWave(int subIdx)
	{
		return conf.waves[subIdx];
	}	
}