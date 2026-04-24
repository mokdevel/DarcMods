//Helpers SDRC_QrfConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
class SDRC_QrfConfig : SDRC_Config
{
	int version = 1;
	string author = "darc";
	string comment = "";
	ref array<ref SDRC_Qrf> qrfs = {};	//List of different waves
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_QrfConfig data = SDRC_QrfConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------
	SDRC_Qrf GetQrf(int index = -1)
	{
		//If random request, find a random one
		if (index == -1)
		{
			SDRC_Qrf qrf = qrfs.GetRandomElement();
			index = qrf.qrfIdx;
		}
		
		foreach (SDRC_Qrf qrf : qrfs)
		{
			if (qrf.qrfIdx == index)
			{
				return qrf;
			}
		}
		
		return null;
	}
		
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		//Walking
		qrfs.Insert(Qrf00());
		qrfs.Insert(Qrf01());
		qrfs.Insert(Qrf02());
		//Wheeled
		qrfs.Insert(Qrf30());
		qrfs.Insert(Qrf31());
		qrfs.Insert(Qrf32());
	}
	
	void Populate()
	{
		SDRC_Log.Add("[SDRC_QrfConfig:Populate] Number of qrfs defined: " + qrfs.Count(), LogLevel.NORMAL);
	}	
	
	//----------------------------------------------------
	SDRC_Qrf Qrf00()
	{
		ref SDRC_Qrf qrf = new SDRC_Qrf();
		qrf.Set(
			0, "index 0: Light group",
			"",
			"", 0, 
			{SDRC_EDifficulty.RANDOM},
			0,
		    {}
		);
		qrf.ai.Set(
			{1, 1},
			{"G_LIGHT"},
			50, 1.0,
			{100, 200},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
			
		return qrf;
	};
	
	//----------------------------------------------------
	SDRC_Qrf Qrf01()
	{
		ref SDRC_Qrf qrf = new SDRC_Qrf();
		qrf.Set(
			1, "index 1: Snipers",
			"",
			"", 0, 
			{SDRC_EDifficulty.RANDOM},
			0,
		    {}
		);
		qrf.ai.Set(
			{1, 1},
			{"G_SNIPER"},
			80, 1.5,
			{100, 200},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
			
		return qrf;
	};	
	
	//----------------------------------------------------
	SDRC_Qrf Qrf02()
	{
		ref SDRC_Qrf qrf = new SDRC_Qrf();
		qrf.Set(
			2, "index 2: Heavy team",
			"",
			"", 0, 
			{SDRC_EDifficulty.RANDOM},
			0,
		    {}
		);
		qrf.ai.Set(
			{1, 1},
			{"G_HEAVY"},
			80, 1.3,
			{100, 200},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
			
		return qrf;
	};	
	
	//----------------------------------------------------
	SDRC_Qrf Qrf30()
	{
		ref SDRC_Qrf qrf = new SDRC_Qrf();
		qrf.Set(
			30, "index 30: Group in a car",
			"",
			"VEHICLE_WHEELED_MILITARY_CAR", 30,
			{SDRC_EDifficulty.RANDOM},
			0,
		    {}
		);
		qrf.ai.Set(
			{1, 1},
			{"G_LIGHT"},
			50, 1.0,
			{100, 200},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
			
		return qrf;
	};
	
	//----------------------------------------------------
	SDRC_Qrf Qrf31()
	{
		ref SDRC_Qrf qrf = new SDRC_Qrf();
		qrf.Set(
			31, "index 31: Armored vehicle",
			"",
			"VEHICLE_WHEELED_ARMED", 30,
			{SDRC_EDifficulty.RANDOM},
			0,
		    {}
		);
		qrf.ai.Set(
			{1, 2},
			{"G_LIGHT"},
			50, 1.0,
			{100, 200},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
			
		return qrf;
	};			
	
	//----------------------------------------------------
	SDRC_Qrf Qrf32()
	{
		ref SDRC_Qrf qrf = new SDRC_Qrf();
		qrf.Set(
			32, "index 32: Armed vehicle",
			"",
			"VEHICLE_WHEELED_ARMED", 40,
			{SDRC_EDifficulty.RANDOM},
			0,
		    {}
		);
		qrf.ai.Set(
			{1, 2},
			{"G_LIGHT"},
			50, 1.0,
			{100, 200},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
			
		return qrf;
	};	
}

//------------------------------------------------------------------------------------------------
class SDRC_Qrf : Managed
{
	int qrfIdx;										//Unique index for the qrf. 
	string comment;									//Generic comment to describe the mission. Not used in game.	
	string info;									//Details for the hint shown for players
	string vehicle;									//Vehicle resourcename
	int speed;										//Vehicle cruising speed
	ref array<SDRC_EDifficulty> difficulty = {}; 	//Difficulty options for qrf
	int xp;											//Experience or other reward given
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();	
	ref array<string> modList = {};					//List of mods needed for this mission

	void Set(int qrfIdx_, string comment_, string info_, string vehicle_, int speed_, array<SDRC_EDifficulty> difficulty_, int xp_, array<string> modList_)
	{
		qrfIdx = qrfIdx_;
		comment = comment_;
		info = info_;
		vehicle = vehicle_;
		speed = speed_;
		difficulty = difficulty_;
		xp = xp_;
		modList = modList_;
	}		
}