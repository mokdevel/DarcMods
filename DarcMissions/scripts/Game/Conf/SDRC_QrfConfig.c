//Helpers SDRC_QrfConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
class SDRC_QrfConfig : SDRC_Config
{
	string author = "darc";
	ref array<ref SDRC_Qrf> qrfs = {};	//List of different waves
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_QrfConfig data = SDRC_QrfConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		qrfs.Insert(Qrf00());
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
			"",
			SDRC_EDifficulty.RANDOM,
			0
		);
		qrf.ai.Set(
			{1, 1},
			{"G_LIGHT"},
			50, 1.0,
			{100, 200},
			SDRC_EWaypointGenerationType.RADIUS,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);
			
		return qrf;
	};
	
	//------------------------------------------------------------------------------------------------
	SDRC_Qrf GetQrf(int qrfIdx)
	{
		return qrfs[qrfIdx];
	}
}

class SDRC_Qrf : Managed
{
	int qrfIdx;									//Unique index for the qrf. 
	string comment;								//Generic comment to describe the mission. Not used in game.	
	string vehicle;								//Vehicle resourcename
	string info;								//Details for the hint shown for players
	SDRC_EDifficulty difficulty;				//Difficulty for specific mission
	int xp;										//Experience or other reward given
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();	

	void Set(int qrfIdx_, string comment_, string vehicle_, string info_, SDRC_EDifficulty difficulty_, int xp_)
	{
		qrfIdx = qrfIdx_;
		comment = comment_;
		vehicle = vehicle_;
		info = info_;
		difficulty = difficulty_;
		xp = xp_;
	}		
}