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
		SDRC_JsonApi2 nonValidAreaJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_NONVALIDAREA);	
		SDRC_NonValidAreaConfig confNonValidArea = new SDRC_NonValidAreaConfig();		
		nonValidAreaJsonApi.Load(confNonValidArea, SDRC_NonValidAreaConfig.Cast(confNonValidArea), DC_MISSIONCONFIG_FILE_NONVALIDAREA_VER);
		delete confNonValidArea;
		delete nonValidAreaJsonApi;
		
		//Create a default mission configs
		SDRC_JsonApi2 convoyJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_CONVOY);	
		SDRC_ConvoyConfig confConvoy = new SDRC_ConvoyConfig();		
		convoyJsonApi.Load(confConvoy, SDRC_MissionConfig.Cast(confConvoy), DC_MISSIONCONFIG_FILE_CONVOY_VER);
		confConvoy.CreateMissionFiles();
		delete confConvoy;
		delete convoyJsonApi;		
		
		SDRC_JsonApi2 crashsiteJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_CRASHSITE);	
		SDRC_CrashsiteConfig confCrashsite = new SDRC_CrashsiteConfig();		
		crashsiteJsonApi.Load(confCrashsite, SDRC_MissionConfig.Cast(confCrashsite), DC_MISSIONCONFIG_FILE_CRASHSITE_VER);		
		confCrashsite.CreateMissionFiles();
		delete confCrashsite;
		delete crashsiteJsonApi;
		
		SDRC_JsonApi2 hunterJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_HUNTER);	
		SDRC_HunterConfig confHunter = new SDRC_HunterConfig();		
		hunterJsonApi.Load(confHunter, SDRC_MissionConfig.Cast(confHunter), DC_MISSIONCONFIG_FILE_HUNTER_VER);		
		confHunter.CreateMissionFiles();
		delete confHunter;
		delete hunterJsonApi;
		
		SDRC_JsonApi2 occupationJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_OCCUPATION);	
		SDRC_OccupationConfig confOccupation = new SDRC_OccupationConfig();		
		occupationJsonApi.Load(confOccupation, SDRC_MissionConfig.Cast(confOccupation), DC_MISSIONCONFIG_FILE_OCCUPATION_VER);		
		confOccupation.CreateMissionFiles();
		delete confOccupation;
		delete occupationJsonApi;			
		
		SDRC_JsonApi2 patrolJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_PATROL);	
		SDRC_PatrolConfig confPatrol = new SDRC_PatrolConfig();		
		patrolJsonApi.Load(confPatrol, SDRC_MissionConfig.Cast(confPatrol), DC_MISSIONCONFIG_FILE_PATROL_VER);		
		confPatrol.CreateMissionFiles();
		delete confPatrol;
		delete patrolJsonApi;		
		
		SDRC_JsonApi2 squatterJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_SQUATTER);	
		SDRC_SquatterConfig confSquatter = new SDRC_SquatterConfig();		
		squatterJsonApi.Load(confSquatter, SDRC_MissionConfig.Cast(confSquatter), DC_MISSIONCONFIG_FILE_SQUATTER_VER);
		confSquatter.CreateMissionFiles();
		delete confSquatter;
		delete squatterJsonApi;			
		
		SDRC_JsonApi2 roadblockJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_ROADBLOCK);	
		SDRC_RoadblockConfig confRoadblock = new SDRC_RoadblockConfig();		
		roadblockJsonApi.Load(confRoadblock, SDRC_MissionConfig.Cast(confRoadblock), DC_MISSIONCONFIG_FILE_ROADBLOCK_VER);
		confRoadblock.CreateMissionFiles();
		delete confRoadblock;
		delete roadblockJsonApi;			
		
		SDRC_JsonApi2 hvtVipJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_HVTVIP);	
		SDRC_HvtVipConfig confHvtVip = new SDRC_HvtVipConfig();		
		hvtVipJsonApi.Load(confHvtVip, SDRC_MissionConfig.Cast(confHvtVip), DC_MISSIONCONFIG_FILE_HVTVIP_VER);
		confHvtVip.CreateMissionFiles();
		delete confHvtVip;
		delete hvtVipJsonApi;		
				
		SDRC_JsonApi2 hvtItemJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_HVTITEM);	
		SDRC_HvtItemConfig confHvtItem = new SDRC_HvtItemConfig();		
		hvtItemJsonApi.Load(confHvtItem, SDRC_MissionConfig.Cast(confHvtItem), DC_MISSIONCONFIG_FILE_HVTITEM_VER);
		confHvtItem.CreateMissionFiles();
		delete confHvtItem;
		delete hvtItemJsonApi;		
		
		SDRC_JsonApi2 stashJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_STASH);	
		SDRC_StashConfig confStash = new SDRC_StashConfig();		
		stashJsonApi.Load(confStash, SDRC_MissionConfig.Cast(confStash), DC_MISSIONCONFIG_FILE_STASH_VER);
		confStash.CreateMissionFiles();
		delete confStash;
		delete stashJsonApi;
		
		SDRC_JsonApi2 chopperJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_CHOPPER);	
		SDRC_ChopperConfig confChopper = new SDRC_ChopperConfig();		
		chopperJsonApi.Load(confChopper, SDRC_MissionConfig.Cast(confChopper), DC_MISSIONCONFIG_FILE_CHOPPER_VER);
		confChopper.CreateMissionFiles();
		delete confChopper;
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