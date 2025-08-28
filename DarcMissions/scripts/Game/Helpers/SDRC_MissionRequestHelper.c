//Helpers SDRC_MissionRequestHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_MissionRequested : Managed
{
	EntityID entityID = null;
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	DC_EMissionType type = DC_EMissionType.NONE;
	int subIdx = -1;	
}

//------------------------------------------------------------------------------------------------
class SDRC_MissionRequestHelper
{							
	//------------------------------------------------------------------------------------------------
	/*!	
	Fill mission details from a mission request
	*/	
	static SDRC_MissionRequested FillMissionRequest()
	{
		//Clean the array before searching for a mission. It could be that user has deleted the mission in the array.
		SDRC_MissionRequestHelper.CleanMissionsRequestedArray();

		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		
		//Is the list empty?		
		if (baseGameMode.missionFrame.m_missionsRequested.IsEmpty())
		{
			return null;
		}
		
		//Take the first mission requested from the table (=[0]).
		SDRC_MissionRequested missionRequest = baseGameMode.missionFrame.m_missionsRequested[0];
		
		//Collect information from the mission entity
		IEntity missionEntity = GetGame().GetWorld().FindEntityByID(missionRequest.entityID);
		SDRC_DarcMissionRequestComp requestComp = SDRC_DarcMissionRequestComp.Cast(missionEntity.FindComponent(SDRC_DarcMissionRequestComp));
		missionRequest.type = requestComp.GetMissionType();
		missionRequest.subIdx = requestComp.GetMissionSubIdx();
		missionRequest.general = requestComp.general;
		
		//Entity may have been moved so we read the pos before spawning mission.
		vector pos = missionEntity.GetOrigin();
		//If it is in the default "0 0 0", all is good. Otherwise, snap to ground
		if (pos[0] != 0)
		{
			pos[1] = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]);
			missionRequest.general.pos[0] = pos;
			
/*			missionRequest.general.pos[0] = missionEntity.GetOrigin();		//Real position
			//If position was "0 0 0", it's possible it has moved due to gravity.
			if (missionRequest.general.pos[0][0] == 0 && missionRequest.general.pos[0][2] == 0)
			{
				missionRequest.general.pos[0][1] == 0;
			}*/
		}
				
		SDRC_SpawnHelper.DespawnItem(missionEntity);				

		//Clean the array as we removed an entry. Others could have been removed at the same time.
		SDRC_MissionRequestHelper.CleanMissionsRequestedArray();						
				
		return missionRequest;
	}
	
	//------------------------------------------------------------------------------------------------	
	/*!
	Remove deleted missions from the list
	*/		
	static void CleanMissionsRequestedArray()
	{
		int i = 0;
		
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		
		while (i < baseGameMode.missionFrame.m_missionsRequested.Count())
		{
			IEntity entity = GetGame().GetWorld().FindEntityByID(baseGameMode.missionFrame.m_missionsRequested[i].entityID);
			if (!entity)
			{
				baseGameMode.missionFrame.m_missionsRequested.Remove(i);
			}
			else
			{
				i++;
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Dump information of GM spawned missions
	*/			
	static void dumpMissionRequested()
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());	
		
		foreach (SDRC_MissionRequested mission : baseGameMode.missionFrame.m_missionsRequested)
		{
			IEntity entity = GetGame().GetWorld().FindEntityByID(mission.entityID);
			
			if (entity)
			{
				SDRC_Log.Add("[SDRC_MissionRequestHelper:dumpGMSpawnedMissions] " + entity + " at " + entity.GetOrigin(), LogLevel.DEBUG);
			}
			else
			{
				SDRC_Log.Add("[SDRC_MissionRequestHelper:dumpGMSpawnedMissions] Deleted at: " + entity.GetOrigin(), LogLevel.DEBUG);
			}
			
		}		
	}				
}
