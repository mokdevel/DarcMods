//Helpers SDRC_Core.c

//------------------------------------------------------------------------------------------------
/*!
File with random DEV time testing things.
*/

//#define SDRC_RELEASE

sealed class SDRC_DevHelper
{
	static void SDRC_DevDump()	
	{
		/*SDRC_MapMarkerHelper.CreateMapMarker("1000 0 1000", SDRC_EMissionIcon.GM_MISSION_X_MAP, "DMC_B", "Here is a text");
		SDRC_MapMarkerHelper.CreateMapMarker("1100 0 1000", SDRC_EMissionIcon.GM_MISSION_SQUATTERS_MAP, "DMC_B", "Darc_SK");
		SDRC_MapMarkerHelper.CreateMapMarker("1200 0 1000", SDRC_EMissionIcon.GM_MISSION_CRASHSITE_MAP, "DMC_B", "Darc_SK");
		SDRC_MapMarkerHelper.CreateMapMarker("1300 0 1000", SDRC_EMissionIcon.GM_MISSION_OCCUPATION_MAP, "DMC_B", "Darc_SK");
		SDRC_MapMarkerHelper.CreateMapMarker("1400 0 1000", SDRC_EMissionIcon.GM_MISSION_CONVOY_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("1500 0 1000", SDRC_EMissionIcon.GM_MISSION_HELICOPTER_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("1600 0 1000", SDRC_EMissionIcon.GM_MISSION_HUNTER_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("1700 0 1000", SDRC_EMissionIcon.GM_MISSION_PATROL_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("1800 0 1000", SDRC_EMissionIcon.GM_MISSION_RADIOACTIVE_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("1900 0 1000", SDRC_EMissionIcon.GM_MISSION_ROADBLOCK_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("2000 0 1000", SDRC_EMissionIcon.GM_MISSION_HVTVIP_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("2100 0 1000", SDRC_EMissionIcon.GM_MISSION_HVTITEM_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("2200 0 1000", SDRC_EMissionIcon.GM_MISSION_STASH_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("2300 0 1000", SDRC_EMissionIcon.GM_MISSION_BIOHAZARD_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("2400 0 1000", SDRC_EMissionIcon.GM_MISSION_CRATE_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("2500 0 1000", SDRC_EMissionIcon.GM_MISSION_DEMON_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("2600 0 1000", SDRC_EMissionIcon.GM_MISSION_DIAMOND_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("2700 0 1000", SDRC_EMissionIcon.GM_MISSION_ZOMBIE_MAP, "DMC_B", "This is a description for a mission");
	
		SDRC_MapMarkerHelper.CreateMapMarker("1000 0 1300", SDRC_EMissionIcon.GM_MISSION_WIN_MAP, "DMC_B", "This is a description for a mission");
		SDRC_MapMarkerHelper.CreateMapMarker("1100 0 1300", SDRC_EMissionIcon.GM_MISSION_LOSE_MAP, "DMC_B", "This is a description for a mission");

		SDRC_MapMarkerHelper.CreateMapMarker("1000 0 1600", SDRC_EMissionIcon.ICON_DEATHMARKER_MAP, "DMC_B", "Here is a text");
		SDRC_MapMarkerHelper.CreateMapMarker("1100 0 1600", SDRC_EMissionIcon.ICON_DEATHMARKER_SMALL_MAP, "DMC_B", "Here is a text");
		SDRC_MapMarkerHelper.CreateMapMarker("1200 0 1600", SDRC_EMissionIcon.ICON_DEATHMARKER_SMALL_RED_MAP, "DMC_B", "Here is a text");
		SDRC_MapMarkerHelper.CreateMapMarker("1300 0 1600", SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, "DMC_B", "Here is a text");
		SDRC_MapMarkerHelper.CreateMapMarker("1400 0 1600", SDRC_EMissionIcon.ICON_EXCLAMATION_SMALL_MAP, "DMC_B", "Here is a text");
		SDRC_MapMarkerHelper.CreateMapMarker("1500 0 1600", SDRC_EMissionIcon.ICON_CRATE_SMALL_MAP, "DMC_B", "Here is a text");*/
				
/*			for (int i = 0;i < 250; i++)
			{
				vector pos = SDRC_Misc.GetRandomWorldPos();
				SDRC_SpawnHelper.FindEmptyPos(pos, 300, 50);			
			}*/
		
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
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	DEBUG: Test mission positions on map. Only for debugging.
	This will create a map marker for each position tested.
	*/	
	static void DebugTestMissionPos()
	{	
		#ifndef SDRC_RELEASE
			vector pos;
	
			for (int i = 0; i < 400; i++)
			{		
				pos = SDRC_MissionHelper.FindMissionPos();
				if (pos != "0 0 0")
				{
					SDRC_MapMarkerHelper.CreateMapMarker(pos, SDRC_EMissionIcon.ICON_PLUS_SMALL_MAP, "DUMMY_");	//TBD: Create some other debug marker
				}
			}		
		#endif
	}

	//------------------------------------------------------------------------------------------------
	/*!
	DEBUG: Delete the test mission positions on map. Only for debugging.
	*/	
	static void DeleteDebugTestMissionPos()
	{	
		if (!SDRC_Conf.RELEASE)
		{
			SDRC_MapMarkerHelper.DeleteMarker("DUMMY_");
		}
	}	
}