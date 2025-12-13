//Helpers SDRC_CoreJsonApi.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
class SDRC_CoreConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	DC_LogLevel logLevel;
	string subDir;						//Directory specifying a certain conf for play. For example "Escapists"	
	bool debugShowWaypoints = false;
	bool debugShowMarks = false;	
	bool debugShowSpheres = false;
	#ifdef NEW_VERSION_WIP	
		bool debugShowLines = false;
		bool debugShowInfo = false;
	#endif
	string fallbackEnemyFaction = "USSR";
	bool showOnGMMapNonValidArea = true;	
	bool showOnGMMapMissionMarker = true;
	ref array<string> buildingExcludeFilter = {};
	ref SDRC_EmptyPos emptyPos = new SDRC_EmptyPos();
	ref array<ref SDRC_LocationAka> locationAkas = {};
	#ifdef NEW_VERSION_WIP	
		ref array<ref SDRC_LocationAka> buildingAkas = {};
	#endif
}

//------------------------------------------------------------------------------------------------
class SDRC_CoreJsonApi : SDRC_JsonApi
{
	ref SDRC_CoreConfig conf = new SDRC_CoreConfig();

	//------------------------------------------------------------------------------------------------
	void SDRC_CoreJsonApi(string fileName)
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
//		conf.logLevel = DC_LogLevel.DEBUG;	
		conf.logLevel = SDRC_Conf.DEFAULT_LOGLEVEL;
		conf.subDir = SDRC_Conf.DEFAULT_DIR;		
		conf.buildingExcludeFilter = {
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
		conf.emptyPos.limit = 5;
		conf.emptyPos.ignoreFilter = {
			"ParticleEffectEntity",
		};
		conf.emptyPos.stopFilter = {
			"RiverPartEntity", "LakeGeneratorEntity", 
			"SCR_DestructibleBuildingEntity", 			//Class: Building
			"GraniteCliff_", "GraniteRock_", 			//Large rocks
		};		
		conf.emptyPos.classFilter = {			
			"SCR_DestructibleEntity",					//Fences etc
			"SCR_IndestructibleEnvironmentalEntity",	//Rocks
			"StaticModelEntity",						
		 	"Tree",
		};
		conf.emptyPos.objectFilter = {
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
		
		ref SDRC_LocationAka aka00 = new SDRC_LocationAka();
		aka00.Set(EMapDescriptorType.MDT_BASE, {"military"});
		conf.locationAkas.Insert(aka00);

		ref SDRC_LocationAka aka01 = new SDRC_LocationAka();
		aka01.Set(EMapDescriptorType.MDT_AIRPORT, {"airport"});
		conf.locationAkas.Insert(aka01);

		ref SDRC_LocationAka aka02 = new SDRC_LocationAka();
		aka02.Set(EMapDescriptorType.MDT_PORT, {"harbour", "harbor"});
		conf.locationAkas.Insert(aka02);						

	#ifdef NEW_VERSION_WIP
		ref SDRC_LocationAka b_aka00 = new SDRC_LocationAka();
		b_aka00.Set(EMapDescriptorType.MDT_CHURCH, {"Church", "Mosque_", "Minaret", });
		conf.buildingAkas.Insert(b_aka00);
		
		ref SDRC_LocationAka b_aka01 = new SDRC_LocationAka();
		b_aka01.Set(EMapDescriptorType.MDT_POLICE, {"_Police"});
		conf.buildingAkas.Insert(b_aka01);
	#endif
	}
};