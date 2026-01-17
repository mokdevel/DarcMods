//Helpers SDRC_Core.c

//------------------------------------------------------------------------------------------------
/*!
File with random DEV time testing things.
*/

//------------------------------------------------------------------------------------------------
sealed class SDRC_DevHelper
{
	static void SDRC_DevDump()	
	{
//		GetGame().GetCallqueue().CallLater(ShowTestMessage, 2000, true);
		SDRC_MapMarkerHelper.CreateMapMarker("0300 0 1400", SDRC_EMissionIcon.GM_MISSION_HUNTER_MAP, "DMC_B", "Hunter");
		SDRC_MapMarkerHelper.CreateMapMarker("0300 0 1400", SDRC_EMissionIconDifficulty.ICON_DIFF_0, "", "*", markerTypeString : "DARC_MISSION_DIFFICULTY");
		SDRC_MapMarkerHelper.CreateMapMarker("0400 0 1400", SDRC_EMissionIcon.GM_MISSION_HUNTER_MAP, "DMC_B", "Hunter");
		SDRC_MapMarkerHelper.CreateMapMarker("0400 0 1400", SDRC_EMissionIconDifficulty.ICON_DIFF_1, "", "**", markerTypeString : "DARC_MISSION_DIFFICULTY");
		SDRC_MapMarkerHelper.CreateMapMarker("0500 0 1400", SDRC_EMissionIcon.GM_MISSION_HUNTER_MAP, "DMC_B", "Hunter");
		SDRC_MapMarkerHelper.CreateMapMarker("0500 0 1400", SDRC_EMissionIconDifficulty.ICON_DIFF_2, "", "***", markerTypeString : "DARC_MISSION_DIFFICULTY");
		SDRC_MapMarkerHelper.CreateMapMarker("0600 0 1400", SDRC_EMissionIcon.GM_MISSION_HUNTER_MAP, "DMC_B", "Hunter");
		SDRC_MapMarkerHelper.CreateMapMarker("0600 0 1400", SDRC_EMissionIconDifficulty.ICON_DIFF_3, "", "****", markerTypeString : "DARC_MISSION_DIFFICULTY");
		SDRC_MapMarkerHelper.CreateMapMarker("0700 0 1400", SDRC_EMissionIcon.GM_MISSION_HUNTER_MAP, "DMC_B", "Hunter");
		SDRC_MapMarkerHelper.CreateMapMarker("0700 0 1400", SDRC_EMissionIconDifficulty.ICON_DIFF_4, "", "****", markerTypeString : "DARC_MISSION_DIFFICULTY");
		
		SDRC_MapMarkerHelper.CreateMapMarker("0300 0 1200", SDRC_EMissionIcon.GM_MISSION_WIN_MAP, "DMC_B", "Win");
		SDRC_MapMarkerHelper.CreateMapMarker("0400 0 1200", SDRC_EMissionIcon.GM_MISSION_LOSE_MAP, "DMC_B", "Lose");
		SDRC_MapMarkerHelper.CreateMapMarker("0500 0 1200", SDRC_EMissionIcon.ICON_DEATHMARKER_MAP, "DMC_B", "Death");
		SDRC_MapMarkerHelper.CreateMapMarker("0600 0 1200", SDRC_EMissionIcon.ICON_DEATHMARKER_SMALL_MAP, "DMC_B", "Death sm");
		SDRC_MapMarkerHelper.CreateMapMarker("0700 0 1200", SDRC_EMissionIcon.ICON_DEATHMARKER_SMALL_RED_MAP, "DMC_B", "Death sm red");
		SDRC_MapMarkerHelper.CreateMapMarker("0800 0 1200", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, "DMC_B", "Plus sm");
		SDRC_MapMarkerHelper.CreateMapMarker("0900 0 1200", SDRC_EMissionIcon.ICON_EXCLAMATION_SMALL_MAP, "DMC_B", "Excl sm");
		SDRC_MapMarkerHelper.CreateMapMarker("1000 0 1200", SDRC_EMissionIcon.ICON_CRATE_SMALL_MAP, "DMC_B", "Crate sm");
						
		SDRC_MapMarkerHelper.CreateMapMarker("1000 0 1000", SDRC_EMissionIcon.GM_MISSION_X_MAP, "DMC_B", "X");
		SDRC_MapMarkerHelper.CreateMapMarker("1100 0 1000", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP, "DMC_B", "Squatters");
		SDRC_MapMarkerHelper.CreateMapMarker("1200 0 1000", SDRC_EMissionIcon.GM_MISSION_CRASHSITE_MAP, "DMC_B", "Crashsite");
		SDRC_MapMarkerHelper.CreateMapMarker("1300 0 1000", SDRC_EMissionIcon.GM_MISSION_OCCUPATION_MAP, "DMC_B", "Occupation");
		SDRC_MapMarkerHelper.CreateMapMarker("1400 0 1000", SDRC_EMissionIcon.GM_MISSION_CONVOY_MAP, "DMC_B", "Convoy");
		SDRC_MapMarkerHelper.CreateMapMarker("1500 0 1000", SDRC_EMissionIcon.GM_MISSION_HELICOPTER_MAP, "DMC_B", "Helicopter");
		SDRC_MapMarkerHelper.CreateMapMarker("1600 0 1000", SDRC_EMissionIcon.GM_MISSION_HUNTER_MAP, "DMC_B", "Hunter");
		SDRC_MapMarkerHelper.CreateMapMarker("1700 0 1000", SDRC_EMissionIcon.GM_MISSION_PATROL_MAP, "DMC_B", "Patrol");
		SDRC_MapMarkerHelper.CreateMapMarker("1800 0 1000", SDRC_EMissionIcon.GM_MISSION_RADIOACTIVE_MAP, "DMC_B", "Radioactive");
		SDRC_MapMarkerHelper.CreateMapMarker("1900 0 1000", SDRC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP, "DMC_B", "Roadblock");
		SDRC_MapMarkerHelper.CreateMapMarker("2000 0 1000", SDRC_EMissionIcon.GM_MISSION_HVTVIP_MAP, "DMC_B", "HVT VIP");
		SDRC_MapMarkerHelper.CreateMapMarker("2100 0 1000", SDRC_EMissionIcon.GM_MISSION_HVTITEM_MAP, "DMC_B", "HVT Item");
		SDRC_MapMarkerHelper.CreateMapMarker("2200 0 1000", SDRC_EMissionIcon.GM_MISSION_STASH_MAP, "DMC_B", "Stash");
		SDRC_MapMarkerHelper.CreateMapMarker("2300 0 1000", SDRC_EMissionIcon.GM_MISSION_BIOHAZARD_MAP, "DMC_B", "Biohazard");
		SDRC_MapMarkerHelper.CreateMapMarker("2400 0 1000", SDRC_EMissionIcon.GM_MISSION_GOLD_MAP, "DMC_B", "Gold");
		SDRC_MapMarkerHelper.CreateMapMarker("2500 0 1000", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP, "DMC_B", "Demon");
		SDRC_MapMarkerHelper.CreateMapMarker("2600 0 1000", SDRC_EMissionIcon.GM_MISSION_DIAMOND_MAP, "DMC_B", "Diamond");
		SDRC_MapMarkerHelper.CreateMapMarker("2700 0 1000", SDRC_EMissionIcon.GM_MISSION_ZOMBIE_MAP, "DMC_B", "Zombie");
		SDRC_MapMarkerHelper.CreateMapMarker("2800 0 1000", SDRC_EMissionIcon.GM_MISSION_CHOPPER_MAP, "DMC_B", "Chopper");
		SDRC_MapMarkerHelper.CreateMapMarker("2900 0 1000", SDRC_EMissionIcon.GM_MISSION_CAR_SIDE_MAP, "DMC_B", "Car side");
		SDRC_MapMarkerHelper.CreateMapMarker("3000 0 1000", SDRC_EMissionIcon.GM_MISSION_MEDAL_MAP, "DMC_B", "Medal");
		SDRC_MapMarkerHelper.CreateMapMarker("3100 0 1000", SDRC_EMissionIcon.GM_MISSION_PORTAL_MAP, "DMC_B", "Medal");
		
//			vector pos = "4600 0 6100";
//			SDRC_SpawnHelper.FindEmptyPos(pos, 300, 60);			
		
		/*
			vector pos;
			pos[0] = SDRC_Misc.GetWorldSize() / 2;
			pos[1] = 0;
			pos[2] = pos[0];
			pos = SDRC_SpawnHelper.FindEmptyPos(pos, 300, 5000);			*/
		
//			string wpnPrefab = "{FA5C25BF66A53DCF}Prefabs/Weapons/Rifles/AK74/Rifle_AK74.et";
//			string wpnPrefab = "{7A82FE978603F137}Prefabs/Weapons/Launchers/RPG7/Launcher_RPG7.et";
/*			string wpnPrefab = "{63E8322E2ADD4AA7}Prefabs/Weapons/Rifles/AK74/Rifle_AK74_GP25.et";
			string mag = SDRC_AmmoHelper.GetCompatibleMagazineForPrefab(wpnPrefab);

			IEntity wpn = SDRC_SpawnHelper.SpawnItem("0 0 0", "{FA5C25BF66A53DCF}Prefabs/Weapons/Rifles/AK74/Rifle_AK74.et", emptyPosRadius: -1);		
			if (wpn)
			{
				string magazine = SDRC_AmmoHelper.GetCompatibleMagazine(wpn);
			}*/		
	
//		SpawnTestMission();	
	}

	//------------------------------------------------------------------------------------------------
	static void TestMissionPositions()
	{
		SDRC_MapMarkerHelper.DeleteMarker("DEV_HELPER");
		
		for (int i = 0;i < 250; i++)
		{
			vector pos = SDRC_Misc.GetRandomWorldPos();
			SDRC_SpawnHelper.FindEmptyPos(pos, 300, 10);			
			
			if (pos != "0 0 0")
			{
				//If pos has been set, we blindly accept it. Do basic checking for pos.
				if (SDRC_MissionPosHelper.IsValidMissionPos(pos) == SDRC_EMissionError.NONE)
				{
					SDRC_MapMarkerHelper.CreateMapMarker(pos, SDRC_EMissionIcon.ICON_DEATHMARKER_SMALL_MAP, "DEV_HELPER");					
				}
			}			
		}	
	}

	//------------------------------------------------------------------------------------------------
	static void ShowTestMessage()
	{
		SDRC_MissionHintHelper.Show("This is a hint!", "This is some text!");
	}
	
	//------------------------------------------------------------------------------------------------
	static void SpawnTestMission()
	{
		string resourceName = SDRC_MissionEnumHelper.GetMissionPrefab(SDRC_EMissionType.OCCUPATION);
		if (resourceName == "")
		{
			//ERROR
			return;
		}
		vector pos = "2000 0 2000";
		IEntity missionEntity = SDRC_SpawnHelper.SpawnItem(pos, resourceName, 0, -1);
		if (!missionEntity)
		{		
			//ERROR
			return;
		}
		GetGame().GetCallqueue().CallLater(SetMissionParameters_Delayed, 2000, false, missionEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	static void SetMissionParameters_Delayed(IEntity missionEntity)
	{
		SDRC_DarcMissionGM ent = SDRC_DarcMissionGM.Cast(missionEntity);
		if (ent)
		{
			SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
			
			SDRC_DarcMissionEditableRequestComp requestComp = SDRC_DarcMissionEditableRequestComp.Cast(ent.FindComponent(SDRC_DarcMissionEditableRequestComp));
			requestComp.general = general;
			requestComp.SetRequestId(1234);
			requestComp.SetMissionType(SDRC_EMissionType.OCCUPATION);
			requestComp.SetSubIdx(0);
		}	
		else
		{
			//ERROR
		}	
	}						
}
