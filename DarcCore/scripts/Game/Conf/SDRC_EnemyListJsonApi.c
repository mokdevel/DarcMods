//Helpers SDRC_EnemyListJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
// Types defined by default
/*
	//Groups
	G_LIGHT
	G_HEAVY
	G_SNIPER
	G_LAUNCHER
	G_ADMIN
	G_MEDICAL
	G_RECON
	G_SPECIAL
	G_SMALL		//A small group of 2 

	//Characters
	C_RIFLEMAN
	C_HEAVY
	C_RECON
	C_OFFICER
	C_CREW
	C_SNIPER
	C_LAUNCHER
	C_MEDIC
	C_SPECIAL
*/

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
			"C_RIFLEMAN",
			"/Characters/Factions",			
			{"Rifleman", "Sapper", "_PL", "_SL"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed"}
		);
		conf.lists.Insert(enemyList20);
		
		SDRC_List enemyList21 = new SDRC_List();
		enemyList21.Set(
			"C_HEAVY",
			"/Characters/Factions",			
			{"_GL", "_MG", "Grenadier", "_Ammo"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed"}
		);
		conf.lists.Insert(enemyList21);
				
		SDRC_List enemyList22 = new SDRC_List();
		enemyList22.Set(
			"C_RECON",
			"/Characters/Factions",			
			{"Scout", "Spotter", "_RTO", "_SL", "_GL"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed"}
		);
		conf.lists.Insert(enemyList22);
				
		SDRC_List enemyList23 = new SDRC_List();
		enemyList23.Set(
			"C_OFFICER",
			"/Characters/Factions",			
			{"_Officer", "Sergeant", "_AC"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed"}
		);
		conf.lists.Insert(enemyList23);
		
		SDRC_List enemyList24 = new SDRC_List();
		enemyList24.Set(
			"C_CREW",
			"/Characters/Factions",			
			{"Crew", "Pilot", "_CC"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed"}
		);
		conf.lists.Insert(enemyList24);
		
		SDRC_List enemyList25 = new SDRC_List();
		enemyList25.Set(
			"C_SNIPER",
			"/Characters/Factions",			
			{"Sniper", "Sharpshooter"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed"}
		);
		conf.lists.Insert(enemyList25);	
		
		SDRC_List enemyList26 = new SDRC_List();
		enemyList26.Set(
			"C_LAUNCHER",
			"/Characters/Factions",			
			{"_AAT", "_AT", "_AT", "_LAT"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed"}
		);
		conf.lists.Insert(enemyList26);
		
		SDRC_List enemyList27 = new SDRC_List();
		enemyList27.Set(
			"C_MEDIC",
			"/Characters/Factions",			
			{"Medic"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed"}
		);
		conf.lists.Insert(enemyList27);
		
		SDRC_List enemyList28 = new SDRC_List();
		enemyList28.Set(
			"C_SPECIAL",
			"/Characters/Factions",			
			{"_SF", "FIA_AC"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed"}
		);
		conf.lists.Insert(enemyList28);
	}
}