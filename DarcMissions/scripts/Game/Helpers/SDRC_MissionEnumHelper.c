//Helpers SDRC_MissionEnumHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for missions. 
*/

//------------------------------------------------------------------------------------------------
enum DC_EMissionType
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
	CHOPPER,	//Mission not ready or working
	
	REQUESTED = 100
};

//------------------------------------------------------------------------------------------------
enum DC_EMissionFailReason
{
	NONE,
	POS_IN_WATER,
	PLAYER_TOO_CLOSE,
	MISSION_TOO_CLOSE,
	IN_NON_VALID_AREA
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
		SDRC_NonValidAreaJsonApi nonValidAreaJsonApi = new SDRC_NonValidAreaJsonApi();
		nonValidAreaJsonApi.Load();
		delete nonValidAreaJsonApi;		
		
		//Create a default mission configs
		SDRC_ConvoyJsonApi convoyJsonApi = new SDRC_ConvoyJsonApi();	
		convoyJsonApi.Load();
		delete convoyJsonApi;
		
		SDRC_CrashsiteJsonApi crashsiteJsonApi = new SDRC_CrashsiteJsonApi();	
		crashsiteJsonApi.Load();		
		delete crashsiteJsonApi;
		
		SDRC_HunterJsonApi hunterJsonApi = new SDRC_HunterJsonApi();				
		hunterJsonApi.Load();								
		delete hunterJsonApi;
		
		SDRC_OccupationJsonApi occupationJsonApi = new SDRC_OccupationJsonApi();	
		occupationJsonApi.Load();		
		delete occupationJsonApi;
		
		SDRC_PatrolJsonApi patrolJsonApi = new SDRC_PatrolJsonApi();	
		patrolJsonApi.Load();
		delete patrolJsonApi;
		
		SDRC_SquatterJsonApi squatterJsonApi = new SDRC_SquatterJsonApi();	
		squatterJsonApi.Load();
		delete squatterJsonApi;		
		
		SDRC_RoadblockJsonApi roadblockJsonApi = new SDRC_RoadblockJsonApi();	
		roadblockJsonApi.Load();
		delete roadblockJsonApi;		
		
		SDRC_HvtVipJsonApi hvtVipJsonApi = new SDRC_HvtVipJsonApi();	
		hvtVipJsonApi.Load();
		delete hvtVipJsonApi;				
		
		SDRC_HvtItemJsonApi hvtItemJsonApi = new SDRC_HvtItemJsonApi();	
		hvtItemJsonApi.Load();
		delete hvtItemJsonApi;				
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!
	Return the prefab for mission requests
	*/	
	static string GetMissionPrefab(DC_EMissionType missionType)
	{
		string resourceName = "";
		
		switch (missionType)
		{
/*			case DC_EMissionType.NONE:
			{
				resourceName = "";
				break;
			}*/
			case DC_EMissionType.HUNTER:
			{
				resourceName = "{907DCF8F8818652B}Prefabs/Systems/DarcMissionHunters.et"; 
				break;
			}
			case DC_EMissionType.OCCUPATION:
			{
				resourceName = "{922D29EAFA4BB4F1}Prefabs/Systems/DarcMissionOccupation.et"; 
				break;
			}
			case DC_EMissionType.CONVOY:
			{
				resourceName = "{CE7D07A8484A1DE9}Prefabs/Systems/DarcMissionConvoy.et"; 
				break;
			}
			case DC_EMissionType.CRASHSITE:
			{
				resourceName = "{003521A6AD7BEB3C}Prefabs/Systems/DarcMissionCrashsite.et"; 
				break;
			}
			case DC_EMissionType.PATROL:
			{
				resourceName = "{E8489D032B77CD71}Prefabs/Systems/DarcMissionPatrol.et"; 
				break;
			}
			case DC_EMissionType.SQUATTERS:
			{
				resourceName = "{0178274B1C19A219}Prefabs/Systems/DarcMissionSquatters.et"; 
				break;
			}
			case DC_EMissionType.ROADBLOCK:
			{
				resourceName = "{F1BE2D46F0949F6D}Prefabs/Systems/DarcMissionRoadblock.et"; 
				break;
			}			
			case DC_EMissionType.HVTVIP:
			{
				resourceName = "{6A859F436655EEFA}Prefabs/Systems/DarcMissionHvtVip.et"; 
				break;
			}			
			case DC_EMissionType.HVTITEM:
			{
				resourceName = "{B9D6798E6A963E0C}Prefabs/Systems/DarcMissionHvtItem.et"; 
				break;
			}			
/*			case DC_EMissionType.CHOPPER:
			{
				resourceName = "";
				break;
			}*/
			default:
				SDRC_Log.Add("[SDRC_MissionEnumHelper:GetMissionPrefab] Incorrect mission type: " + missionType, LogLevel.ERROR);
		}
		
		return resourceName;
	}			
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find marker texture file (*.edds)
	\param icon Icon to find the texture 
	*/
	static string GetMarkerTexture(DC_EMissionIcon icon)
	{
		string texture = "";
		
		switch (icon)
		{						
			case DC_EMissionIcon.NONE:
			{
				texture = "{885DA992DEA7C7BC}UI/Textures/Icons/icon_empty_round.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_X_MAP:
			{
				texture = "{D2202D0BD28F1595}UI/Textures/Icons/gm_mission_X_map.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_SQUATTERS_MAP:
			{
				texture = "{80C94D83C82B22B0}UI/Textures/Icons/gm_mission_Squatters_map.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_CRASHSITE_MAP:
			{
				texture = "{D717A926DD1BF821}UI/Textures/Icons/gm_mission_Crashsite_map.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_OCCUPATION_MAP:
			{
				texture = "{B039E934071DB071}UI/Textures/Icons/gm_mission_Occupation_map.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_CONVOY_MAP:
			{
				texture = "{8F0F7AD0EF00FCDB}UI/Textures/Icons/gm_mission_Convoy_map.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_HELICOPTER_MAP:
			{
				texture = "{E9B24656D68043DB}UI/Textures/Icons/gm_mission_Helicopter_map.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_HUNTER_MAP:
			{
				texture = "{B7B4981CD0DFF5BE}UI/Textures/Icons/gm_mission_Hunter_map.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_PATROL_MAP:
			{
				texture = "{614156B627595CA2}UI/Textures/Icons/gm_mission_Patrol_map.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_RADIOACTIVE_MAP:
			{
				texture = "{B9B3508F01BFEF7D}UI/Textures/Icons/gm_mission_RadioActive_map.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP:		
			{
				texture = "{A72A9123C15DABDF}UI/Textures/Icons/gm_mission_Roadblock_map.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_HVTVIP_MAP:		
			{
				texture = "{4E8D7F3F80D40037}UI/Textures/Icons/gm_mission_HvtVip_map.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_HVTITEM_MAP:		
			{
				texture = "{14450944913A8F8D}UI/Textures/Icons/gm_mission_HvtItem_map.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_WIN_MAP:		
			{
				texture = "{8C37D9AEF869E351}UI/Textures/Icons/gm_mission_Win_map.edds";
				break;
			}
			case DC_EMissionIcon.GM_MISSION_LOSE_MAP:		
			{
				texture = "{063A043A682AE7F2}UI/Textures/Icons/gm_mission_Lose_map.edds";
				break;
			}
			
			//NOTE: The ones below are not usable as markers
			case DC_EMissionIcon.ICON_WIN_ROUND:		
			{
				texture = "{E3E45CDDDBB68FD4}UI/Textures/Icons/gm_mission_Win_round.edds";
				break;
			}
			case DC_EMissionIcon.ICON_LOSE_ROUND:		
			{
				texture = "{784AED49188FB189}UI/Textures/Icons/gm_mission_Lose_round.edds";
				break;
			}
			default:
				SDRC_Log.Add("[SDRC_MissionEnumHelper:GetMarkerTexture] Incorrect icon type.", LogLevel.ERROR);			
		}		
		return texture;
	}		
}