//Helpers SDRC_IconHelper.c

//------------------------------------------------------------------------------------------------
/*!
File with helper functions related to icons
*/

sealed class SDRC_IconHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Find marker texture file (*.edds)
	\param icon Icon to find the texture 
	*/
	static string GetMarkerTexture(SDRC_EMissionIcon icon)
	{
		string texture = "";
		
		switch (icon)
		{						
			case SDRC_EMissionIcon.NONE:
			{
				texture = "{885DA992DEA7C7BC}UI/Textures/Icons/icon_empty_round.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_X_MAP:
			{
				texture = "{D2202D0BD28F1595}UI/Textures/Icons/gm_mission_X_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP:
			{
				texture = "{80C94D83C82B22B0}UI/Textures/Icons/gm_mission_Squatters_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_CRASHSITE_MAP:
			{
				texture = "{D717A926DD1BF821}UI/Textures/Icons/gm_mission_Crashsite_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_OCCUPATION_MAP:
			{
				texture = "{B039E934071DB071}UI/Textures/Icons/gm_mission_Occupation_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_CONVOY_MAP:
			{
				texture = "{8F0F7AD0EF00FCDB}UI/Textures/Icons/gm_mission_Convoy_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_HELICOPTER_MAP:
			{
				texture = "{E9B24656D68043DB}UI/Textures/Icons/gm_mission_Helicopter_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_HUNTER_MAP:
			{
				texture = "{B7B4981CD0DFF5BE}UI/Textures/Icons/gm_mission_Hunter_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_PATROL_MAP:
			{
				texture = "{614156B627595CA2}UI/Textures/Icons/gm_mission_Patrol_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_RADIOACTIVE_MAP:
			{
				texture = "{B9B3508F01BFEF7D}UI/Textures/Icons/gm_mission_RadioActive_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP:		
			{
				texture = "{A72A9123C15DABDF}UI/Textures/Icons/gm_mission_Roadblock_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_HVTVIP_MAP:		
			{
				texture = "{4E8D7F3F80D40037}UI/Textures/Icons/gm_mission_HvtVip_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_HVTITEM_MAP:		
			{
				texture = "{14450944913A8F8D}UI/Textures/Icons/gm_mission_HvtItem_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_STASH_MAP:
			{
				texture = "{FD181DB538595893}UI/Textures/Icons/gm_mission_Stash_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_CHOPPER_MAP:
			{				
				texture = "{384CFFB2945C5D54}UI/Textures/Icons/gm_mission_Chopper_map.edds";
				break;
			}
			
			case SDRC_EMissionIcon.GM_MISSION_BIOHAZARD_MAP:
			{
				texture = "{6C698741C70CAC62}UI/Textures/Icons/gm_mission_Biohazard_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_GOLD_MAP:
			{
				texture = "{9945B5A978A43DE3}UI/Textures/Icons/gm_mission_Gold_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_DEMON_MAP:
			{
				texture = "{3D3C82FFBF12DCEB}UI/Textures/Icons/gm_mission_Demon_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_DIAMOND_MAP:
			{
				texture = "{01FC7796E43EEC59}UI/Textures/Icons/gm_mission_Diamond_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_ZOMBIE_MAP:
			{
				texture = "{C2289A3C07D1846A}UI/Textures/Icons/gm_mission_Zombie_map.edds";
				break;
			}
						
			//Win / Lose markers
			case SDRC_EMissionIcon.GM_MISSION_WIN_MAP:		
			{
				texture = "{8C37D9AEF869E351}UI/Textures/Icons/gm_mission_Win_map.edds";
				break;
			}
			case SDRC_EMissionIcon.GM_MISSION_LOSE_MAP:		
			{
				texture = "{063A043A682AE7F2}UI/Textures/Icons/gm_mission_Lose_map.edds";
				break;
			}
			
			//NOTE: The ones below are not usable as markers
			case SDRC_EMissionIcon.ICON_WIN_ROUND:		
			{
				texture = "{E3E45CDDDBB68FD4}UI/Textures/Icons/gm_mission_Win_round.edds";
				break;
			}
			case SDRC_EMissionIcon.ICON_LOSE_ROUND:		
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