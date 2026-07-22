//Helpers SDRC_EnemyListConfig.c

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

	C_RANDOMIZED : This is a collection of C_RIFLEMAN + C_HEAVY + C_RECON. This is generated runtime.

	//Zombies and creatures
	G_ZOMBIE_SMALL
	G_ZOMBIE_MEDIUM
	G_ZOMBIE_LARGE
	C_ZOMBIE
	C_DEMON
	C_DEMON_BOSS	
*/

//------------------------------------------------------------------------------------------------
class SDRC_EnemyListConfig : SDRC_ListConfig
{
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_EnemyListConfig data = SDRC_EnemyListConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		
		m_modList = {};
		
		SDRC_Aka aka00 = new SDRC_Aka();
		aka00.names = {"RHS_USAF", "USAF_USMC"};
		m_akas.Insert(aka00);

		SDRC_Aka aka01 = new SDRC_Aka();
		aka01.names = {"RHS_AFRF", "RHS_RF"};
		m_akas.Insert(aka01);

		SDRC_Aka aka02 = new SDRC_Aka();
		aka02.names = {"USSR", "USSR", "TF_RF"};
		m_akas.Insert(aka02);

		SDRC_Aka aka03 = new SDRC_Aka();
		aka03.names = {"US", "US", "TF_US"};
		m_akas.Insert(aka03);

		SDRC_Aka aka04 = new SDRC_Aka();
		aka04.names = {"BACON_622120A5448725E3_FACTION", "Zombie", "Zombies"};
		m_akas.Insert(aka04);

		SDRC_Aka aka05 = new SDRC_Aka();
		aka05.names = {"BALLIEN_BC_FACTION", "Cultist", "Cultists"};
		m_akas.Insert(aka05);

		SDRC_Aka aka06 = new SDRC_Aka();
		aka06.names = {"ChinesePLA", "PLAGF", "PLAAF"};
		m_akas.Insert(aka06);

		SDRC_Aka aka07 = new SDRC_Aka();
		aka07.names = {"JSDF_RAS", "JGSDF"};
		m_akas.Insert(aka07);		

		SDRC_Aka aka08 = new SDRC_Aka();
		aka08.names = {"Takistan", "TA"};
		m_akas.Insert(aka08);	

		SDRC_Aka aka09 = new SDRC_Aka();
		aka09.names = {"AFOA", "Argentina"};
		m_akas.Insert(aka09);	
	
		SDRC_Aka aka10 = new SDRC_Aka();
		aka10.names = {"DSF", "SAS", "SBS"};
		m_akas.Insert(aka10);				
																				
		m_lists.Insert(enemyList00());
		m_lists.Insert(enemyList01());
		m_lists.Insert(enemyList02());
		m_lists.Insert(enemyList03());
		m_lists.Insert(enemyList04());
		m_lists.Insert(enemyList05());
		m_lists.Insert(enemyList06());
		m_lists.Insert(enemyList07());
		m_lists.Insert(enemyList08());
		m_lists.Insert(enemyList09());
		
		m_lists.Insert(enemyList20());
		m_lists.Insert(enemyList21());
		m_lists.Insert(enemyList22());
		m_lists.Insert(enemyList23());
		m_lists.Insert(enemyList24());
		m_lists.Insert(enemyList25());
		m_lists.Insert(enemyList26());
		m_lists.Insert(enemyList27());
		
		m_lists.Insert(enemyList50());		
		m_lists.Insert(enemyList51());		
		m_lists.Insert(enemyList52());		
		m_lists.Insert(enemyList53());		
		
		m_lists.Insert(enemyList60());		
		m_lists.Insert(enemyList61());				
		m_lists.Insert(enemyList62());				
	}
				
	//------------------------------------------------------------------------------------------------
	// Group lists
	//------------------------------------------------------------------------------------------------
	
	SDRC_List enemyList00()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_LIGHT",
			{"Prefabs/Groups", "622120A5448725E3/Prefabs/Groups"},	
			{"LightFire", "FireTeam", "FireGroup", "RifleSquad", "SentryTeam", 
			 "Regulars_Brick", "InfantrySection",						//British Forces
			 "Group_Zombies_CIV", "Group_Zombies_FIA", "Group_Zombies_US", "Group_Zombies_USSR", //Bacon Zombies
//			 "CIV_Small", "FIA_Small", "USSR_Small", "Tier1_Small", 	//Bacon Zombies
			 "Cultists_Light", 											//Ballien Cultists
			 "CloseProtectionTeam",										//RHS_ION
			 "DSF_1983_SBS_Patrol",										//1st RM DSF			
			},	
			{"_Base", "_NotSpawned", "_Remnants", "_Random", "_Heavy"},
			{}
		);	
		return enemyList;
	}
	
	SDRC_List enemyList01()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_SNIPER",
			{"Prefabs/Groups", "622120A5448725E3/Prefabs/Groups"},	
			{"Sniper", "SharpShooter", "USSR_Spetsnaz_SentryTeam", "VKPO_S_SentryTeam.et", 
			 "KS05_SF_GROUP_A", "KS05_SF_GROUP_C", 						//Omega gropus
			 "Group_Zombies_CIV", "Group_Zombies_FIA", "Group_Zombies_US", "Group_Zombies_USSR", //Bacon Zombies
//			 "CIV_Small", "FIA_Small", "USSR_Small", "Tier1_Small", 	//Bacon Zombies
			 "Cultists_Light", 											//Ballien Cultists
			 "PLAGF_SentryTeam",										//PLA: Proper Sniper Group is missing
			 "JGSDF_2008_SentryTeam",									//JSFD: Proper Group is missing
			 "CloseProtectionTeam",										//RHS_ION
			 "TA_RifleSquad", 											//Takistan
			 "Argentina_Sentry",										//1st RM AFOA
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
			{"Prefabs/Groups", "622120A5448725E3/Prefabs/Groups"},	
			{"Team_AT", "Team_GL", "Team_LAT",
			 "Group_Zombies_High",	//Bacon Zombies
//			 "CIV_Medium", "FIA_Medium", "USSR_Medium", "Tier1_Medium",	//Bacon Zombies
			 "Cultists_Medium", 										//Ballien Cultists
			 "PLASTICBANDIT_FireTeam_Heavy",							//Bandit faction
			 "JGSDF_2008_SentryTeam",									//JSFD: Proper Group is missing			
			 "CloseProtectionTeam",										//RHS_ION
			 "Argentina_AT",											//1st RM AFOA
			 "DSF_1983_SAS_Patrol",										//1st RM DSF
			},
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
			{"Prefabs/Groups", "622120A5448725E3/Prefabs/Groups"},	
			{"_Platoon",
			 "Group_Zombies_High",	//Bacon Zombies
//			 "CIV_Small", "FIA_Small", "USSR_Small", "Tier1_Small", 	//Bacon Zombies			
			 "Cultists_Light", 											//Ballien Cultists
			 "PLASTICBANDIT_Squad",										//Bandit faction random team			
			 "JGSDF_2008_SentryTeam",									//JSFD: Proper Group is missing			
			 "StaticSecurityTeam",										//RHS_ION						
			 "TA_SentryTeam",											//Takistan
			 "Argentina_Sentry",										//1st RM AFOA
			 "DSF_1983_SAS_SpecialProjectsTeam",						//1st RM DSF
			},
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
			{"Prefabs/Groups", "622120A5448725E3/Prefabs/Groups"},	
			{"_Medical",
			 "Group_Zombies_High",	//Bacon Zombies
//			 "CIV_Medium", "FIA_Medium", "USSR_Medium", "Tier1_Medium",	//Bacon Zombies	
			 "Cultists_Light", 											//Ballien Cultists
			 "PLASTICBANDIT_Squad",										//Bandit faction random team
			 "JGSDF_2008_LightFireTeam",								//JSFD: Proper Group is missing
			 "StaticSecurityTeam",										//RHS_ION			
			 "Argentina_Sentry",										//1st RM AFOA
			 "DSF_1983_SAS_Patrol",										//1st RM DSF			
			},
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
			{"Prefabs/Groups", "622120A5448725E3/Prefabs/Groups"},	
			{"_Recon", "_Maneuver", "_RecceTeam",
			 "Group_Zombies_CIV", "Group_Zombies_FIA", "Group_Zombies_US", "Group_Zombies_USSR", //Bacon Zombies
//			 "CIV_Small", "FIA_Small", "USSR_Small", "Tier1_Small", 	//Bacon Zombies			
			 "Cultists_Medium", 										//Ballien Cultists
			 "PLASTICBANDIT_Squad",										//Bandit faction random team		
			 "JGSDF_2008_LightFireTeam",								//JSFD: Proper Group is missing
			 "CloseProtectionTeam",										//RHS_ION
			 "TA_Team_Suppress",										//Takistan
			 "Argentina_Sentry",										//1st RM AFOA
			 "DSF_1983_SAS_Patrol",										//1st RM DSF			
			},
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
			{"Prefabs/Groups", "622120A5448725E3/Prefabs/Groups"},	
			{"MachineGunTeam", "FireTeam", "FireGroup", "Suppress", "Spetsnaz_Squad",
			 "GunGroup",												//British Forces
			 "Group_Zombies_Horde", "Group_Zombies_CIV_Large",			//Bacon Zombies
//			 "CIV_Large", "FIA_Large", "USSR_Large", "Tier1_Large", 	//Bacon Zombies			
//			 "AmbientPatrol_Horde", "Zombies_Horde",					//Bacon Zombies	Hordes
			 "Cultists_Horde", 											//Ballien Cultists
			 "JGSDF_2008_SentryTeam",									//JSFD: Proper Group is missing
			 "QuickReactionForce",										//RHS_ION
			 "Recce",													//1st RM DSF
			},	
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
			{"Prefabs/Groups", "622120A5448725E3/Prefabs/Groups"},	
			{"GreenBeret", "Sapper", "Sentry", "Spetsnaz", "SpecialForces",
			 "KS05_SF_GROUP_B", 		//Omega groups
			 "Group_Zombies_Horde",		//Bacon Zombies
//			 "CIV_Large", "FIA_Large", "USSR_Large", "Tier1_Large", 	//Bacon Zombies			
			 "Cultists_Medium", 										//Ballien Cultists
			 "PLASTICBANDIT_Squad",										//Bandit faction
			 "MobileSecurityTeam",										//RHS_ION
			 "SAS_SpecialProjectsTeam",									//1st RM DSF
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
			{"Prefabs/Groups", "622120A5448725E3/Prefabs/Groups"},	
			{"Spetsnaz_SentryTeam", "GreenBeret_SentryTeam", "SentryTeam",
			 "SharpshooterTeam", "MedicalSection", "SapperTeam", 
			 "Group_Zombies_CIV", "Group_Zombies_FIA", "Group_Zombies_US", "Group_Zombies_USSR", //Bacon Zombies
//			 "CIV_Small", "FIA_Small", "USSR_Small", "Tier1_Small", 	//Bacon Zombies			
			 "Cultists_Light", 											//Ballien Cultists
			 "PLASTICBANDIT_Patrol",									//Bandit faction
			 "JGSDF_2008_LightFireTeam",								//JSFD: Proper Group is missing
			 "CloseProtectionTeam",										//RHS_ION
			 "Argentina_Sentry",										//1st RM AFOA			
			 "DSF_1983_SAS_Patrol",										//1st RM DSF
			},
			{"_Base", "_NotSpawned", "_Remnants", "_Random"},
			{}
		);			
		return enemyList;
	}
	
	//------------------------------------------------------------------------------------------------
	// Character lists		
	//------------------------------------------------------------------------------------------------
	
	SDRC_List enemyList09()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_RIFLEMAN",
			{"Prefabs/Characters/Factions", "622120A5448725E3/Prefabs/Characters", },
			{"Rifleman", "Sapper", "_PL", "_SL",
			 "Zombie_CIV", "Zombie_FIA", "Zombie_US", "Zombie_USSR", 	//Bacon Zombies
			 "Scav_Rookie", "Stalker_Rookie",							//Bandit faction
			 "Civilian_Cultist", 										//Ballien Cultists
			 "{DFC792E39288F721}Prefabs/Characters/Civilian_Cultist_1.et",	//Ballien Cultists
			 "Marksman",												//RHS_ION
			 "1983_SBS_RTO", "1983_SBS_SL", 							//1st RM DSF
			},
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
			{"Prefabs/Characters/Factions", "622120A5448725E3/Prefabs/Characters", },
			{"_GL", "_MG", "Grenadier", "_Ammo", "_LMG",
			 "Exploder", "Heavy_Tank", 						//Bacon Zombies			
			 "Scav_Gopnik", "Stalker_Rookie", "Scav_Brute", //Bandit faction			
			 "Civilian_Cultist", 							//Ballien Cultists
			 "{DFC792E39288F721}Prefabs/Characters/Civilian_Cultist_2.et",	//Ballien Cultists
			 "JGSDF_2008_Rifleman",							//JSFD: Proper Character is missing
			 "ION_Scout",									//RHS_ION
			 "1983_SASRTO", "1983_SAS_SL", 					//1st RM DSF
			},
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
			{"Prefabs/Characters/Factions", "622120A5448725E3/Prefabs/Characters", },
			{"Scout", "Spotter", "_RTO", "_SL", "_GL",
			 "_AG1", //MEI		
			 "Zombie_Military_ALL", "Military_Pistol_ALL",	//Bacon Zombies
			 "Scav_Veteran", "Stalker_Veteran",				//Bandit faction			
			 "Civilian_Cultist", 							//Ballien Cultists
			 "{DFC792E39288F721}Prefabs/Characters/Civilian_Cultist_3.et",	//Ballien Cultists
			 "JGSDF_2008_TL",								//JSFD: Proper Character is missing
			 "Argentina_NCO",								//1st RM AFOA			
			 "SAS_Trooper", 								//1st RM DSF
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
			{"Prefabs/Characters/Factions", "622120A5448725E3/Prefabs/Characters", },
			{"_Officer", "Sergeant", "_AC", 
			 "_Leader", //MEI
			 "Exploder", "Heavy_Tank", 						//Bacon Zombies			
			 "Scav_Veteran", "Stalker_Veteran",				//Bandit faction			
			 "Warrior_Cultist", 							//Ballien Cultists
			 "{E8C16E36B946D92D}Prefabs/Characters/Warrior_Cultist.et",	//Ballien Cultists
			 "JGSDF_2008_CC",								//JSFD: Proper Character is missing
			 "ION_Scout",									//RHS_ION			
			 "TA_AR",										//Takistan
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
			{"Prefabs/Characters/Factions", "622120A5448725E3/Prefabs/Characters", },
			{"Crew", "Pilot", "_CC",
			 "MEI_Leader", //MEI
			 "Exploder", "Heavy_Tank", 						//Bacon Zombies			
			 "Scav_Veteran", "Stalker_Veteran",				//Bandit faction			
			 "Civilian_Cultist", 							//Ballien Cultists
			 "{DFC792E39288F721}Prefabs/Characters/Civilian_Cultist_3.et",	//Ballien Cultists
			 "ION_Scout",									//RHS_ION			
			 "TA_AR",										//Takistan			
			 "SP_Trooper", 									//1st RM DSF
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
			{"Prefabs/Characters/Factions", "622120A5448725E3/Prefabs/Characters", },
			{"Sniper", "Sharpshooter",
			 "Exploder", 									//Bacon Zombies			
			 "Stalker_Shadow", "Stalker_Legend",			//Bandit faction			
			 "Kamikaze_Cultist",							//Ballien Cultists
			 "{B7CA600660E7743B}Prefabs/Characters/Kamikaze_Cultist_1.et",	//Ballien Cultists
			 "ION_SL",										//RHS_ION			
			 "Argentina_NCO",								//1st RM AFOA			
			},
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
			{"Prefabs/Characters/Factions", "622120A5448725E3/Prefabs/Characters", },
			{"_AAT", "_AT", "_AT", "_LAT",
			 "Exploder", 									//Bacon Zombies			
			 "Scav_Brute", 									//Bandit faction			
			 "Warrior_Cultist", 							//Ballien Cultists
			 "{E8C16E36B946D92D}Prefabs/Characters/Warrior_Cultist.et",	//Ballien Cultists
			 "JGSDF_2008_TL",								//JSFD: Proper Character is missing
			 "ION_SL",										//RHS_ION			
			},
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
			{"Prefabs/Characters/Factions", "622120A5448725E3/Prefabs/Characters", },
			{"Medic",
			 "Zombie_Military_ALL", "Military_Pistol_ALL",	//Bacon Zombies
			 "Scav_Veteran", "Stalker_Veteran",				//Bandit faction						
			 "Civilian_Cultist", 							//Ballien Cultists
			 "{DFC792E39288F721}Prefabs/Characters/Civilian_Cultist_2.et",	//Ballien Cultists
			 "JGSDF_2008_CC",								//JSFD: Proper Character is missing
			},
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
			{"Prefabs/Characters/Factions", "622120A5448725E3/Prefabs/Characters"},
			{"_SF", "_SR", "FIA_AC", 
			 "_Bomb", 										//MEI
			 "MEI_Rifleman1", 								//MEI
			 "Zombie_Military_ALL", "Military_Pistol_ALL",	//Bacon Zombies			
			 "Exploder", "Heavy_Tank", 						//Bacon Zombies			
			 "Stalker_Shadow", "Stalker_Legend",			//Bandit faction						
			 "PLAGF_Scout_RTO",								//PLA
			 "Kamikaze_Cultist",							//Ballien Cultists
			 "{B7CA600660E7743B}Prefabs/Characters/Kamikaze_Cultist_1.et",	//Ballien Cultists
			 "JGSDF_2008_Sniper",							//JSFD: Proper Character is missing
			 "ION_Scout",									//RHS_ION			
			 "TA_SL",										//Takistan			
			 "Argentina_MAT",								//1st RM AFOA
			 "SP_Trooper", "SP_Sapper",	"SAS_Demolitionist", //1st RM DSF			
			},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);					
		return enemyList;
	}
	
	//------------------------------------------------------------------------------------------------
	// Monster - zombies
	//------------------------------------------------------------------------------------------------

	SDRC_List enemyList50()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"G_ZOMBIE_SMALL",
			{"622120A5448725E3/Prefabs/Groups"},
			{
			 "Group_Zombies_CIV", "Group_Zombies_FIA", "Group_Zombies_US", "Group_Zombies_USSR",
			},
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
			{
			 "Group_Zombies_CIV_Large", "Group_Zombies_High",
			},
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
			{
			 "Group_Zombies_Horde",
			},
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
		
	//------------------------------------------------------------------------------------------------
	// Monster - creatures
	//------------------------------------------------------------------------------------------------
	
	SDRC_List enemyList60()
	{
		ref SDRC_List enemyList = new SDRC_List();
		enemyList.Set(
			"C_DEMON",
			{"Prefabs/Characters", "Beasts"},				//NOTE: Here we have upper level Prefabs/Characters as filter
			{
			 "ShadowTwin", "Whisper", 
			 "Kamikaze_Cultist",							//Ballien Cultists
			},
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
			{"Prefabs/Characters", "Beasts"},				//NOTE: Here we have upper level Prefabs/Characters as filter
			{
			 "Stalker Heavy", "Zombie_Heavy",
			 "character_115_dupe", "character_115_pump", "character_115_skel.",
			 "Warrior_Cultist", 							//Ballien Cultists
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
			{"Prefabs/Characters", "Beasts"},				//NOTE: Here we have upper level Prefabs/Characters as filter
			{
			 "GhoulHeavy", "PumpkinHead", "Skeleton", 		//"TreeDemon",
			 "Whisper", 
			 "Warrior_Cultist", 							//Ballien Cultists
			},
			{"_Base", "_NotSpawned", "_Remnants", "/CIV/", "_Randomized", "Variant", "Suppressed", "Unarmed", "_Random"},
			{}
		);					
		return enemyList;
	}	
}
