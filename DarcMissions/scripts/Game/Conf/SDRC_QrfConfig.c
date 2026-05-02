//Helpers SDRC_QrfConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

/*
Pre defined groups:
	0, "index 0: Light group",
	1, "index 1: Snipers",
	2, "index 2: Heavy team",
	3, "index 3: Spec ops",
	4, "index 4: MG team",
	5, "index 5: Multiple small groups",
	6, "index 6: Deadly recon team",
	7, "index 7: Not so sharp snipers",
	30, "index 30: Group in a car",
	31, "index 31: Armored vehicle",
	32, "index 32: Armed vehicle",
	60, "index 60: Armed chopper",
*/

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
	/*!
	Return a QRF conf.
	\param index The subIdx to search. -1 will pick a random one from all QRFs.
	*/	
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
		
		SDRC_Log.Add("[SDRC_QrfConfig:GetQrf] Wrong index requested: " + index, LogLevel.ERROR);
		
		return null;
	}
		
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		//Walking
		qrfs.Insert(Qrf00());
		qrfs.Insert(Qrf01());
		qrfs.Insert(Qrf02());
		qrfs.Insert(Qrf03());
		qrfs.Insert(Qrf04());
		qrfs.Insert(Qrf05());
		qrfs.Insert(Qrf06());
		qrfs.Insert(Qrf07());
		//Wheeled
		qrfs.Insert(Qrf30());
		qrfs.Insert(Qrf31());
		qrfs.Insert(Qrf32());
		//Chopper
		qrfs.Insert(Qrf60());
	}
	
	//----------------------------------------------------
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
			{250, 350},
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
			{250, 350},
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
			{250, 350},
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
	SDRC_Qrf Qrf03()
	{
		ref SDRC_Qrf qrf = new SDRC_Qrf();
		qrf.Set(
			3, "index 3: Spec ops",
			"",
			{250, 350},
			"", 0, 
			{SDRC_EDifficulty.RANDOM},
			0,
		    {}
		);
		qrf.ai.Set(
			{1, 2},
			{"G_SPECIAL", "G_RECON", },
			80, 1.3,
			{100, 200},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
			
		return qrf;
	};		
	
	//----------------------------------------------------
	SDRC_Qrf Qrf04()
	{
		ref SDRC_Qrf qrf = new SDRC_Qrf();
		qrf.Set(
			4, "index 4: MG team",
			"",
			{250, 350},
			"", 0, 
			{SDRC_EDifficulty.RANDOM},
			0,
		    {}
		);
		qrf.ai.Set(
			{1, 2},
			{"G_HEAVY", "G_LAUNCHER", },
			80, 1.3,
			{100, 200},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
			
		return qrf;
	};		
	
	//----------------------------------------------------
	SDRC_Qrf Qrf05()
	{
		ref SDRC_Qrf qrf = new SDRC_Qrf();
		qrf.Set(
			5, "index 5: Multiple small groups",
			"",
			{250, 350},
			"", 0, 
			{SDRC_EDifficulty.RANDOM},
			0,
		    {}
		);
		qrf.ai.Set(
			{2, 4},
			{"G_SMALL", },
			80, 1.3,
			{100, 200},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
			
		return qrf;
	};		
	
	//----------------------------------------------------
	SDRC_Qrf Qrf06()
	{
		ref SDRC_Qrf qrf = new SDRC_Qrf();
		qrf.Set(
			6, "index 6: Deadly recon team",
			"",
			{250, 350},
			"", 0, 
			{SDRC_EDifficulty.TOUGH, SDRC_EDifficulty.HARD},
			0,
		    {}
		);
		qrf.ai.Set(
			{1, 1},
			{"G_RECON", },
			100, 2.0,
			{100, 200},
			SDRC_EWaypointGenerationType.ROUTE,
			SDRC_EWaypointMoveType.MOVE,
		);
			
		return qrf;
	};	
		
	//----------------------------------------------------
	SDRC_Qrf Qrf07()
	{
		ref SDRC_Qrf qrf = new SDRC_Qrf();
		qrf.Set(
			7, "index 7: Not so sharp snipers",
			"",
			{250, 350},
			"", 0, 
			{SDRC_EDifficulty.RANDOM},
			0,
		    {}
		);
		qrf.ai.Set(
			{2, 2},
			{"G_SNIPER"},
			40, 1.8,
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
			{350, 450},
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
			{350, 450},
			"VEHICLE_WHEELED_ARMOR", 30,
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
			{350, 450},
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
	
	//----------------------------------------------------
	SDRC_Qrf Qrf60()
	{
		ref SDRC_Qrf qrf = new SDRC_Qrf();
		qrf.Set(
			60, "index 60: Armed chopper",
			"",
			{200, 250},
			"VEHICLE_CHOPPER_ARMED", 0,
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
	ref array<int> distance = {};					//Spawn distance min/max for qrf
	string vehicle;									//Vehicle resourcename
	int speed;										//Vehicle cruising speed
	ref array<SDRC_EDifficulty> difficulty = {}; 	//Difficulty options for qrf
	int xp;											//Experience or other reward given
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();
	ref array<string> modList = {};					//List of mods needed for this mission

	void Set(int qrfIdx_, string comment_, string info_, array<int> distance_, string vehicle_, int speed_, array<SDRC_EDifficulty> difficulty_, int xp_, array<string> modList_)
	{
		qrfIdx = qrfIdx_;
		comment = comment_;
		info = info_;
		distance = distance_;
		vehicle = vehicle_;
		speed = speed_;
		difficulty = difficulty_;
		xp = xp_;
		modList = modList_;
	}		
}