//Helpers SDRC_MissionEnumHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for missions. 
*/

//------------------------------------------------------------------------------------------------
enum SDRC_EMissionType
{
	ERROR = -1,
	NONE = 0,
	HUNTER,			//1
	OCCUPATION,		//2
	CONVOY,			//3
	CRASHSITE,		//4
	PATROL,			//5
	SQUATTERS,		//6
	ROADBLOCK,		//7
	HVTVIP,			//8
	HVTITEM,		//9
	STASH,			//10
	CHOPPER,		//11
	
	REQUESTED = 100
};

//------------------------------------------------------------------------------------------------
enum SDRC_EMissionDifficulty
{
	RANDOM = -1,
	EASY = 0, 
	MODERATE, 
	NORMAL, 
	TOUGH, 
	HARD,
};

//------------------------------------------------------------------------------------------------
sealed class SDRC_MissionEnumHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Creates config files. To be run at first run of the mod. Will not overwrite existing confs.
	*/	
	static void CreateAllConfigs()
	{		
		//Create a default nonValidArea config
		SDRC_NonValidAreaJsonApi nonValidAreaJsonApi = new SDRC_NonValidAreaJsonApi(DC_MISSIONCONFIG_FILE_NONVALIDAREA);
		nonValidAreaJsonApi.Load();
		delete nonValidAreaJsonApi;		
		
		//Create a default mission configs
		SDRC_ConvoyJsonApi convoyJsonApi = new SDRC_ConvoyJsonApi(DC_MISSIONCONFIG_FILE_CONVOY);	
		convoyJsonApi.Load();
		convoyJsonApi.CreateMissionFiles();
		delete convoyJsonApi;
		
		SDRC_CrashsiteJsonApi crashsiteJsonApi = new SDRC_CrashsiteJsonApi(DC_MISSIONCONFIG_FILE_CRASHSITE);	
		crashsiteJsonApi.Load();		
		crashsiteJsonApi.CreateMissionFiles();
		delete crashsiteJsonApi;
		
		SDRC_HunterJsonApi hunterJsonApi = new SDRC_HunterJsonApi(DC_MISSIONCONFIG_FILE_HUNTER);				
		hunterJsonApi.Load();								
		hunterJsonApi.CreateMissionFiles();
		delete hunterJsonApi;
		
		SDRC_OccupationJsonApi occupationJsonApi = new SDRC_OccupationJsonApi(DC_MISSIONCONFIG_FILE_OCCUPATION);	
		occupationJsonApi.Load();		
		occupationJsonApi.CreateMissionFiles();
		delete occupationJsonApi;
		
		SDRC_PatrolJsonApi patrolJsonApi = new SDRC_PatrolJsonApi(DC_MISSIONCONFIG_FILE_PATROL);	
		patrolJsonApi.Load();
		patrolJsonApi.CreateMissionFiles();
		delete patrolJsonApi;
		
		SDRC_SquatterJsonApi squatterJsonApi = new SDRC_SquatterJsonApi(DC_MISSIONCONFIG_FILE_SQUATTER);	
		squatterJsonApi.Load();
		squatterJsonApi.CreateMissionFiles();
		delete squatterJsonApi;		
		
		SDRC_RoadblockJsonApi roadblockJsonApi = new SDRC_RoadblockJsonApi(DC_MISSIONCONFIG_FILE_ROADBLOCK);	
		roadblockJsonApi.Load();
		roadblockJsonApi.CreateMissionFiles();
		delete roadblockJsonApi;		
		
		SDRC_HvtVipJsonApi hvtVipJsonApi = new SDRC_HvtVipJsonApi(DC_MISSIONCONFIG_FILE_HVTVIP);	
		hvtVipJsonApi.Load();
		hvtVipJsonApi.CreateMissionFiles();
		delete hvtVipJsonApi;				
		
		SDRC_HvtItemJsonApi hvtItemJsonApi = new SDRC_HvtItemJsonApi(DC_MISSIONCONFIG_FILE_HVTITEM);	
		hvtItemJsonApi.Load();
		hvtItemJsonApi.CreateMissionFiles();
		delete hvtItemJsonApi;				
		
		SDRC_JsonApi2 stashJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_STASH);	
		SDRC_StashConfig confStash = new SDRC_StashConfig();		
		stashJsonApi.Load(confStash);
		confStash.CreateMissionFiles();
		delete stashJsonApi;
		
		SDRC_JsonApi2 chopperJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_CHOPPER);	
		SDRC_ChopperConfig confChopper = new SDRC_ChopperConfig();		
		chopperJsonApi.Load(confChopper);
		confChopper.CreateMissionFiles();
		delete chopperJsonApi;
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!
	Return the prefab for mission requests
	*/	
	static string GetMissionPrefab(SDRC_EMissionType missionType)
	{
		string resourceName = "";
		
		switch (missionType)
		{
/*			case SDRC_EMissionType.NONE:
			{
				resourceName = "";
				break;
			}*/
			case SDRC_EMissionType.HUNTER:
			{
				resourceName = "{907DCF8F8818652B}Prefabs/Systems/DarcMissionHunters.et"; 
				break;
			}
			case SDRC_EMissionType.OCCUPATION:
			{
				resourceName = "{922D29EAFA4BB4F1}Prefabs/Systems/DarcMissionOccupation.et"; 
				break;
			}
			case SDRC_EMissionType.CONVOY:
			{
				resourceName = "{CE7D07A8484A1DE9}Prefabs/Systems/DarcMissionConvoy.et"; 
				break;
			}
			case SDRC_EMissionType.CRASHSITE:
			{
				resourceName = "{003521A6AD7BEB3C}Prefabs/Systems/DarcMissionCrashsite.et"; 
				break;
			}
			case SDRC_EMissionType.PATROL:
			{
				resourceName = "{E8489D032B77CD71}Prefabs/Systems/DarcMissionPatrol.et"; 
				break;
			}
			case SDRC_EMissionType.SQUATTERS:
			{
				resourceName = "{0178274B1C19A219}Prefabs/Systems/DarcMissionSquatters.et"; 
				break;
			}
			case SDRC_EMissionType.ROADBLOCK:
			{
				resourceName = "{F1BE2D46F0949F6D}Prefabs/Systems/DarcMissionRoadblock.et"; 
				break;
			}			
			case SDRC_EMissionType.HVTVIP:
			{
				resourceName = "{6A859F436655EEFA}Prefabs/Systems/DarcMissionHvtVip.et"; 
				break;
			}			
			case SDRC_EMissionType.HVTITEM:
			{
				resourceName = "{B9D6798E6A963E0C}Prefabs/Systems/DarcMissionHvtItem.et"; 
				break;
			}			
			case SDRC_EMissionType.STASH:
			{
				resourceName = "{631842AD30537026}Prefabs/Systems/DarcMissionStash.et"; 
				break;
			}	
			case SDRC_EMissionType.CHOPPER:
			{
				resourceName = "{D934E89EF2C4F392}Prefabs/Systems/DarcMissionChopper.et";
				break;
			}
			default:
				SDRC_Log.Add("[SDRC_MissionEnumHelper:GetMissionPrefab] Incorrect mission type: " + missionType, LogLevel.ERROR);
		}
		
		return resourceName;
	}	
}