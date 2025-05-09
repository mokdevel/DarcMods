//Helpers SDRC_EnemyListJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
// Types defined by default
/*
	G_LIGHT
	G_HEAVY
	G_SNIPER
	G_LAUNCHER
	G_ADMIN
	G_MEDICAL
	G_RECON
	G_SPECIAL
	G_SMALL		//A small group of 2 
*/

/*class SDRC_EnemyListConfig : SDRC_ListConfig
{
}*/

//------------------------------------------------------------------------------------------------
class SDRC_EnemyListJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_enemyList.json";
		
	ref SDRC_ListConfig conf = new SDRC_ListConfig();

	//------------------------------------------------------------------------------------------------
	void SDRC_EnemyListJsonApi()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_MISSIONCONFIG_FILE);
		if (!loadContext)
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
		
		conf.modList = {};
		/*conf.modList = {"$ArmaReforger:Prefabs",
						"$WCS_Armaments:Prefabs",
						"$M110MarksmanRifle:Prefabs",
						"$RISLaserAttachments:Prefabs"
		};*/
		
		//Group lists
		SDRC_List enemyList01 = new SDRC_List();
		enemyList01.Set(
			"G_LIGHT",
			"/Groups",
			{"LightFire", "FireTeam", "FireGroup", "RifleSquad", "SentryTeam"},
			{"_Base", "_NotSpawned", "_Remnants"}
		);
		conf.lists.Insert(enemyList01);
		
		SDRC_List enemyList02 = new SDRC_List();
		enemyList02.Set(
			"G_SNIPER",
			"/Groups",
			{"Sniper", "SharpShooter"},
			{"_Base", "_NotSpawned", "_Remnants"}
		);
		conf.lists.Insert(enemyList02);
		
		SDRC_List enemyList03 = new SDRC_List();
		enemyList03.Set(
			"G_LAUNCHER",
			"/Groups",
			{"Team_AT", "Team_GL", "Team_LAT"},
			{"_Base", "_NotSpawned", "_Remnants"}
		);
		conf.lists.Insert(enemyList03);
		
		SDRC_List enemyList04 = new SDRC_List();
		enemyList04.Set(
			"G_ADMIN",
			"/Groups",
			{"_Platoon"},
			{"_Base", "_NotSpawned", "_Remnants"}
		);
		conf.lists.Insert(enemyList04);
		
		SDRC_List enemyList05 = new SDRC_List();
		enemyList05.Set(
			"G_MEDICAL",
			"/Groups",
			{"_Medical"},
			{"_Base", "_NotSpawned", "_Remnants"}
		);
		conf.lists.Insert(enemyList05);
		
		SDRC_List enemyList06 = new SDRC_List();
		enemyList06.Set(
			"G_RECON",
			"/Groups",
			{"_Recon", "_Maneuver"},
			{"_Base", "_NotSpawned", "_Remnants"}
		);
		conf.lists.Insert(enemyList06);
		
		SDRC_List enemyList07 = new SDRC_List();
		enemyList07.Set(
			"G_HEAVY",
			"/Groups",
			{"MachineGunTeam", "FireTeam", "FireGroup", "Suppress", "Spetsnaz_Squad"},
			{"_Base", "_NotSpawned", "_Remnants"}
		);
		conf.lists.Insert(enemyList07);
		
		SDRC_List enemyList08 = new SDRC_List();
		enemyList08.Set(
			"G_SPECIAL",
			"/Groups",
			{"GreenBeret", "Sapper", "Sentry", "Spetsnaz"},
			{"_Base", "_NotSpawned", "_Remnants"}
		);
		conf.lists.Insert(enemyList08);
		
		SDRC_List enemyList09 = new SDRC_List();
		enemyList09.Set(
			"G_SMALL",
			"/Groups",			
			{"Spetsnaz_SentryTeam", "GreenBeret_SentryTeam", "SharpshooterTeam", "MedicalSection", "SapperTeam", "SentryTeam"},
			{"_Base", "_NotSpawned", "_Remnants"}
		);
		conf.lists.Insert(enemyList09);		

		//Character lists		
		SDRC_List enemyList20 = new SDRC_List();
		enemyList20.Set(
			"G_SMALL",
			"/Characters",			
			{""},
			{"_Base", "_NotSpawned", "_Remnants"}
		);
		conf.lists.Insert(enemyList20);		
		
	}
}