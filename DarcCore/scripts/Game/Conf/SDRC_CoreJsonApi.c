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
	bool debugShowWaypoints = true;
	bool debugShowMarks = true;	
	bool debugShowSpheres = true;
	string fallbackEnemyFaction = "USSR";
	bool showOnGMMapNonValidArea = true;	
	bool showOnGMMapMissionMarker = true;	
	ref array<string> buildingExcludeFilter = {};
	ref SDRC_EmptyPos emptyPos = new SDRC_EmptyPos();
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
			"FuelTank_"
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
			"Boulder_", "BeachStone_", 
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
	}
};