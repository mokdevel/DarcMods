//Helpers SDRC_MissionEnumHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for missions. 
*/

//------------------------------------------------------------------------------------------------
enum SDRC_EMissionType
{
	NONE = 0,
	HUNTER,
	OCCUPATION,
	CONVOY,
	CRASHSITE,
	PATROL,
	SQUATTERS,
	ROADBLOCK,
	HVTVIP,
	HVTITEM,
	STASH,
	CHOPPER,	//Mission not ready or working
	
	REQUESTED = 100
};

//------------------------------------------------------------------------------------------------
enum SDRC_EMissionDifficulty
{
	EASY, 
	MODERATE, 
	NORMAL, 
	TOUGH, 
	HARD
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
		delete convoyJsonApi;
		
		SDRC_CrashsiteJsonApi crashsiteJsonApi = new SDRC_CrashsiteJsonApi(DC_MISSIONCONFIG_FILE_CRASHSITE);	
		crashsiteJsonApi.Load();		
		delete crashsiteJsonApi;
		
		SDRC_HunterJsonApi hunterJsonApi = new SDRC_HunterJsonApi(DC_MISSIONCONFIG_FILE_HUNTER);				
		hunterJsonApi.Load();								
		delete hunterJsonApi;
		
		SDRC_OccupationJsonApi occupationJsonApi = new SDRC_OccupationJsonApi(DC_MISSIONCONFIG_FILE_OCCUPATION);	
		occupationJsonApi.Load();		
		delete occupationJsonApi;
		
		SDRC_PatrolJsonApi patrolJsonApi = new SDRC_PatrolJsonApi(DC_MISSIONCONFIG_FILE_PATROL);	
		patrolJsonApi.Load();
		delete patrolJsonApi;
		
		SDRC_SquatterJsonApi squatterJsonApi = new SDRC_SquatterJsonApi(DC_MISSIONCONFIG_FILE_SQUATTER);	
		squatterJsonApi.Load();
		delete squatterJsonApi;		
		
		SDRC_RoadblockJsonApi roadblockJsonApi = new SDRC_RoadblockJsonApi(DC_MISSIONCONFIG_FILE_ROADBLOCK);	
		roadblockJsonApi.Load();
		delete roadblockJsonApi;		
		
		SDRC_HvtVipJsonApi hvtVipJsonApi = new SDRC_HvtVipJsonApi(DC_MISSIONCONFIG_FILE_HVTVIP);	
		hvtVipJsonApi.Load();
		delete hvtVipJsonApi;				
		
		SDRC_HvtItemJsonApi hvtItemJsonApi = new SDRC_HvtItemJsonApi(DC_MISSIONCONFIG_FILE_HVTITEM);	
		hvtItemJsonApi.Load();
		delete hvtItemJsonApi;				
		
		SDRC_StashJsonApi stashJsonApi = new SDRC_StashJsonApi(DC_MISSIONCONFIG_FILE_STASH);	
		stashJsonApi.Load();
		delete stashJsonApi;						
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
			/*			case SDRC_EMissionType.CHOPPER:
			{
				resourceName = "";
				break;
			}*/
			default:
				SDRC_Log.Add("[SDRC_MissionEnumHelper:GetMissionPrefab] Incorrect mission type: " + missionType, LogLevel.ERROR);
		}
		
		return resourceName;
	}	
}