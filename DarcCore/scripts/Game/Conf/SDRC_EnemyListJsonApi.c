//Helpers SDRC_EnemyListJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
// Types defined by default
/*
	//Groups
	G_LIGHT : Small arms units like rifle men.
	G_HEAVY : Units with bigger guns for example machine guns
	G_SNIPER : Units with rifles and typically with scopes
	G_LAUNCHER : Units with launchers
	G_ADMIN : Officers and similar higher ranking units
	G_MEDICAL : Medical units
	G_RECON : Units defined as recon units
	G_SPECIAL : Special Forces units.
	G_SMALL	: Small groups with two units. Mixed arms.

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

	//Zombies and creatures
	G_ZOMBIE_SMALL
	G_ZOMBIE_MEDIUM
	G_ZOMBIE_LARGE
	C_ZOMBIE
	C_DEMON
	C_DEMON_BOSS	
*/

//------------------------------------------------------------------------------------------------
class SDRC_EnemyListJsonApi : SDRC_JsonApi
{
	ref SDRC_ListConfig conf = new SDRC_ListConfig();

	//------------------------------------------------------------------------------------------------
	void SDRC_EnemyListJsonApi(string fileName)
	{
		SetFileName(fileName);
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
		conf.modList = {};
		
		SDRC_Aka aka00 = new SDRC_Aka();
		aka00.names = {"RHS_USAF", "USAF_USMC"};
		conf.akas.Insert(aka00);

		SDRC_Aka aka01 = new SDRC_Aka();
		aka01.names = {"RHS_AFRF", "RHS_RF"};
		conf.akas.Insert(aka01);

		SDRC_Aka aka02 = new SDRC_Aka();
		aka02.names = {"USSR", "USSR", "TF_RF"};
		conf.akas.Insert(aka02);

		SDRC_Aka aka03 = new SDRC_Aka();
		aka03.names = {"US", "US", "TF_US"};
		conf.akas.Insert(aka03);
						
		conf.lists.Insert(enemyList00());
		conf.lists.Insert(enemyList01());
		conf.lists.Insert(enemyList02());
		conf.lists.Insert(enemyList03());
		conf.lists.Insert(enemyList04());
		conf.lists.Insert(enemyList05());
		conf.lists.Insert(enemyList06());
		conf.lists.Insert(enemyList07());
		conf.lists.Insert(enemyList08());
		conf.lists.Insert(enemyList09());
		
		conf.lists.Insert(enemyList20());
		conf.lists.Insert(enemyList21());
		conf.lists.Insert(enemyList22());
		conf.lists.Insert(enemyList23());
		conf.lists.Insert(enemyList24());
		conf.lists.Insert(enemyList25());
		conf.lists.Insert(enemyList26());
		conf.lists.Insert(enemyList27());
		
		conf.lists.Insert(enemyList50());		
		conf.lists.Insert(enemyList51());		
		conf.lists.Insert(enemyList52());		
		conf.lists.Insert(enemyList53());		
		
		conf.lists.Insert(enemyList60());		
		conf.lists.Insert(enemyList61());				
		conf.lists.Insert(enemyList62());				
	}
				
	//Group lists
	SDRC_List enemyList00()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_LIGHT",
			{"Prefabs/Groups"},
			{"LightFire", "FireTeam", "FireGroup", "RifleSquad", "SentryTeam", 
			 "Regulars_Brick", "InfantrySection"},	//British Forces
			{"_Base", "_NotSpawned", "_Remnants", "_Random"},
			{}
		);	
		return enemyList;
	}
	
	SDRC_List enemyList01()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_SNIPER",
			{"Prefabs/Groups"},
			{"Sniper", "SharpShooter", "USSR_Spetsnaz_SentryTeam", "VKPO_S_SentryTeam.et", 
			"KS05_SF_GROUP_A", "KS05_SF_GROUP_C", 	//Omega gropus
			},
			{"_Base", "_NotSpawned", "_Remnants", "_Random"},
			{}
		);		
		return enemyList;
	}
	
	SDRC_List enemyList02()
	{		
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_LAUNCHER",
			{"Prefabs/Groups"},
			{"Team_AT", "Team_GL", "Team_LAT"},
			{"_Base", "_NotSpawned", "_Remnants", "_Random"},
			{}
		);		
		return enemyList;
	}
	
	SDRC_List enemyList03()
	{			
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_ADMIN",
			{"Prefabs/Groups"},
			{"_Platoon"},
			{"_Base", "_NotSpawned", "_Remnants", "_Random"},
			{}
		);		
		return enemyList;
	}
	
	SDRC_List enemyList04()
	{			
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_MEDICAL",
			{"Prefabs/Groups"},
			{"_Medical"},
			{"_Base", "_NotSpawned", "_Remnants", "_Random"},
			{}
		);				
		return enemyList;
	}
	
	SDRC_List enemyList05()
	{	
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_RECON",
			{"Prefabs/Groups"},
			{"_Recon", "_Maneuver", "_RecceTeam"},
			{"_Base", "_NotSpawned", "_Remnants", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	SDRC_List enemyList06()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_HEAVY",
			{"Prefabs/Groups"},
			{"MachineGunTeam", "FireTeam", "FireGroup", "Suppress", "Spetsnaz_Squad",
			 "GunGroup"},	//British Forces
			{"_Base", "_NotSpawned", "_Remnants", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	SDRC_List enemyList07()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_SPECIAL",
			{"Prefabs/Groups"},
			{"GreenBeret", "Sapper", "Sentry", "Spetsnaz", "SpecialForces",
			"KS05_SF_GROUP_B", //Omega gropus
			},
			{"_Base", "_NotSpawned", "_Remnants", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	SDRC_List enemyList08()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_SMALL",
			{"Prefabs/Groups"},			
			{"Spetsnaz_SentryTeam", "GreenBeret_SentryTeam", "SharpshooterTeam", "MedicalSection", "SapperTeam", "SentryTeam"},
			{"_Base", "_NotSpawned", "_Remnants", "_Random"},
			{}
		);			
		return enemyList;
	}
	
	SDRC_List enemyList09()
	{
		//Character lists		
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_RIFLEMAN",
			{"Prefabs/Characters/Factions"},
			{"Rifleman", "Sapper", "_PL", "_SL"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	SDRC_List enemyList20()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_HEAVY",
			{"Prefabs/Characters/Factions"},			
			{"_GL", "_MG", "Grenadier", "_Ammo", "_LMG"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);						
		return enemyList;
	}
	
	SDRC_List enemyList21()
	{	
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_RECON",
			{"Prefabs/Characters/Factions"},
			{"Scout", "Spotter", "_RTO", "_SL", "_GL",
				"_AG1", //MEI		
			},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	SDRC_List enemyList22()
	{		
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_OFFICER",
			{"Prefabs/Characters/Factions"},
			{"_Officer", "Sergeant", "_AC", 
				"_Leader", //MEI
			},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	SDRC_List enemyList23()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_CREW",
			{"Prefabs/Characters/Factions"},
			{"Crew", "Pilot", "_CC",
				"MEI_Leader", //MEI
			},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	SDRC_List enemyList24()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_SNIPER",
			{"Prefabs/Characters/Factions"},
			{"Sniper", "Sharpshooter"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	SDRC_List enemyList25()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_LAUNCHER",
			{"Prefabs/Characters/Factions"},
			{"_AAT", "_AT", "_AT", "_LAT"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	SDRC_List enemyList26()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_MEDIC",
			{"Prefabs/Characters/Factions"},
			{"Medic"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	SDRC_List enemyList27()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_SPECIAL",
			{"Prefabs/Characters/Factions"},
			{"_SF", "_SR", "FIA_AC", 
				"_Bomb", //MEI
				"MEI_Rifleman1", //MEI
			},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	//Monster - zombies and creatures
	SDRC_List enemyList50()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_ZOMBIE_SMALL",
			{"622120A5448725E3/Prefabs/Groups"},
			{"Group_Zombies"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random", "_Huge", "_Large", "_Medium"},
			{}
		);			
		return enemyList;
	}
	
	SDRC_List enemyList51()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_ZOMBIE_MEDIUM",
			{"622120A5448725E3/Prefabs/Groups"},
			{"_Medium"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);			
		return enemyList;
	}
	
	SDRC_List enemyList52()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_ZOMBIE_LARGE",
			{"622120A5448725E3/Prefabs/Groups"},
			{"_Huge", "_Large", "_Horde"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);			
		return enemyList;
	}
	
	SDRC_List enemyList53()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_ZOMBIE",
			{"622120A5448725E3/Prefabs/Characters"},
			{"Zombie_", "Character_", "Variant_"},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random", "Zombie_Heavy"},
			{}
		);			
		return enemyList;
	}	
		
	SDRC_List enemyList60()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_DEMON",
			{"Prefabs/Characters", "Beasts"},
			{"ShadowTwin", "Whisper", },
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	SDRC_List enemyList61()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_DEMON_BOSS",
			{"Prefabs/Characters", "Beasts"},
			{"Stalker Heavy", "Zombie_Heavy",
			"character_115_dupe", "character_115_pump", "character_115_skel.",
			},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	SDRC_List enemyList62()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_BEASTS",
			{"Prefabs/Characters"},		
			{"GhoulHeavy", "PumpkinHead", "Skeleton", //"TreeDemon",
			},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);					
		return enemyList;
	}	
}
