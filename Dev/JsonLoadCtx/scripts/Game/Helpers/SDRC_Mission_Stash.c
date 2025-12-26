//Mission SDRC_Mission_Stash.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns a stash for loot. This essentially the same as Occupation mission.if

The usage for Stash mission is mainly as a story ending when using DarcStories.
*/

const string DC_MISSIONCONFIG_FILE_STASH = "dc_missionConfig_Stash.json";

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Stash : SDRC_Mission
{
	private ref SDRC_JsonApi2 m_StashJsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_STASH);	
	private static ref SDRC_StashConfig m_Config = new SDRC_StashConfig();	
	private ref SDRC_Camp m_DC_Stash = new SDRC_Camp();
	
	private int m_iSpawnIndex = 0;						//Counter for the item to spawn
	private float m_fSpawnRotation = 0;					//Rotation of the camp for random locations.

	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Stash(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Load config
		m_StashJsonApi.CreateMissionFiles();
		m_StashJsonApi.Load(m_Config);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{

	}

	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
	}
			
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
	}
	
	//------------------------------------------------------------------------------------------------	
	override void DoWin()
	{	
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_StashConfig : SDRC_MissionConfig2
{
	//Mission specific	
	int activeDistance;							//Distance of winning and keeping the mission active
//	ref array<ref SDRC_Camp> subMissions = {};	//List of Stashs
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_StashConfig data = SDRC_StashConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------	
	int GetSubMissionIdx(int subIdx)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		//Default		
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		activeDistance = 50;
		missionList = {0,0,0};
		missionFiles.Insert("dc_missionConfig_Stash_010.json");
/*		#ifndef SDRC_RELEASE
			missionFiles.Insert("dc_missionConfig_Stash_01x.json");	//Just for testing that dummy files don't appear
		#endif*/
		//Mission specific		
		//----------------------------------------------------
	};
}