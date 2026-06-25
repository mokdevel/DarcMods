//Helpers SDRC_CoreConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
class SDRC_CoreConfig : SDRC_Config
{
	//Default information
	string author = "darc";
	DC_LogLevel logLevel;
	string subDir;						//Directory specifying a certain conf for play. For example "Escapists"	
	bool debugShowWaypoints = SDRC_Conf.SHOW_DEBUG_INFO;
	bool debugShowMarks = SDRC_Conf.SHOW_DEBUG_INFO;	
	bool debugShowSpheres = SDRC_Conf.SHOW_DEBUG_INFO;
	bool debugShowLines = SDRC_Conf.SHOW_DEBUG_INFO;
	bool debugShowInfo = SDRC_Conf.SHOW_DEBUG_INFO;
	string fallbackEnemyFaction = "USSR";
	bool showOnGMMapNonValidArea = true;	
	bool showOnGMMapMissionMarker = true;
	ref array<string> buildingExcludeFilter = {};
	ref SDRC_EmptyPos emptyPos = new SDRC_EmptyPos();
	ref array<ref SDRC_LocationAka> locationAkas = {};
	ref array<ref SDRC_LocationAka> buildingAkas = {};
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_CoreConfig data = SDRC_CoreConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		
//		logLevel = DC_LogLevel.DEBUG;	
		logLevel = SDRC_Conf.DEFAULT_LOGLEVEL;
		subDir = SDRC_Conf.DEFAULT_DIR;		
		buildingExcludeFilter = {
			"BrickPile", "WoodPile", "Hotbed", "Henhouse", "PhoneBooth",
			"AmmoDump", "ElectricCabinet", "ControlBox110kV", "LightBeacon",
			"PierConcrete", "PierWooden", "Pier_", "SeaBollard", 
			"ConcreteRetainingWall", "StoneRetainingWall", "ConcreteBridge",
			"Calvary", "Castle_", "Davit_",
			"GraveStone", "Grave_", "GraveFence",
			"MooringDolphin",
			"HouseRuin",
			//These from Kunar map
			"Wall_2", "Wall_5", "militarywall", "fort_rampart_", "CNCBlock", "MEStairs",
			//Gogland
			"Gogland_trench", "Gogland_short_wall",
			//Anizay
			"tem_toilet", "tem_buildingwall", "TEM_bigwall", "tem_wall", 
			"tem_buildingtower", "tem_column", "tem_arab", "tem_cover", 
			"tem_buildingbridge", "tem_arch", "tem_potmarket", "tem_oil_pump", 
			"tem_brokengate", 
			"FuelTank_",
			//Kunar
			"BigHBarrier", "Wall_E", "Wall_ATC", 
		};
		emptyPos.limit = 5;
		emptyPos.ignoreFilter = {
			"ParticleEffectEntity",
		};
		emptyPos.stopFilter = {
			"RiverPartEntity", "LakeGeneratorEntity", 
			"SCR_DestructibleBuildingEntity", 			//Class: Building
			"GraniteCliff_", "GraniteRock_", 			//Large rocks
		};		
		emptyPos.classFilter = {			
			"SCR_DestructibleEntity",					//Fences etc
			"SCR_IndestructibleEnvironmentalEntity",	//Rocks
			"StaticModelEntity",						
		 	"Tree",
		};
		emptyPos.objectFilter = {
			//Rocks etc
			"Boulder_", "BeachStone_", "GraniteCliff_",
			//Misc
			"BlastCover_",
			//Building parts
			"BrickWall", 
			//Trees
			"t_pinus_sylvestris_3",
			"t_picea_abies_3",
			"t_carpinus_betulus_3",
			"t_betula_pendula_3",
			"t_piceaabies_2", "t_piceaabies_3",
			//Anizay
			//"tem_wall",
			"tem_wall_L_",
			"tem_concrete_wall",
			"tem_cave", 
			"tem_big", "rocktower", "rockwall",
			"tem_r_tk_rock", "tem_rock",
			//Trees
			"tem_palm2",
		};
		
		// Location akas
		ref SDRC_LocationAka aka00 = new SDRC_LocationAka();
		aka00.Set(EMapDescriptorType.MDT_BASE, {"military"});
		locationAkas.Insert(aka00);

		ref SDRC_LocationAka aka01 = new SDRC_LocationAka();
		aka01.Set(EMapDescriptorType.MDT_AIRPORT, {"airport"});
		locationAkas.Insert(aka01);

		ref SDRC_LocationAka aka02 = new SDRC_LocationAka();
		aka02.Set(EMapDescriptorType.MDT_PORT, {"harbour", "harbor"});
		locationAkas.Insert(aka02);						

		// Building akas
		ref SDRC_LocationAka b_aka00 = new SDRC_LocationAka();
		b_aka00.Set(EMapDescriptorType.MDT_CHURCH, {"Church", "Mosque_", "Minaret", });
		buildingAkas.Insert(b_aka00);
		
		ref SDRC_LocationAka b_aka01 = new SDRC_LocationAka();
		b_aka01.Set(EMapDescriptorType.MDT_POLICE, {"_Police"});
		buildingAkas.Insert(b_aka01);
	}		
}