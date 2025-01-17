//Helpers SCR_DC_MissionHelper.c

sealed class SCR_DC_MissionHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Find an empty spot where to spawn a mission. 
	*/	
	static vector FindMissionPos(float distanceToMission = -1, float distanceToPlayer = -1)
	{	
		vector pos = SCR_DC_Misc.GetRandomWorldPos();
		if (!IsValidMissionPos(pos, distanceToMission, distanceToPlayer))
		{
			return "0 0 0";
		}
		
		return pos;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if given pos is a valid position for mission. 
	
	The position shall have ... 
	- no players nearby
	- no missions nearby
	- not in water	
	*/	
	static bool IsValidMissionPos(vector pos, float distanceToMission = -1, float distanceToPlayer = -1)
	{
		float waterHeight = GetGame().GetWorld().GetOceanHeight(pos[0], pos[2]);
	
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		if (distanceToMission == -1)
			distanceToMission = baseGameMode.missionFrame.m_DC_MissionFrameConfig.frame.minDistanceToMission;

		if (distanceToPlayer == -1)
			distanceToPlayer = baseGameMode.missionFrame.m_DC_MissionFrameConfig.frame.minDistanceToPlayer;
				
		if (waterHeight != 0)
		{			
			return false;
		}
		
		if (SCR_DC_PlayerHelper.IsAnyPlayerCloseToPos(pos, distanceToPlayer))
		{
			return false;
		}

		if (IsAnyMissionCloseToPos(pos, distanceToMission))
		{
			return false;
		}

		if (IsPosInNonValidArea(pos))
		{
			return false;
		}
										
		if (!SCR_DC_Core.RELEASE)
		{
			SCR_DC_MapMarkersUI.AddMarker("DUMMY_", pos, "", DC_EMarkerType.DEBUG);
		}
		
		return true;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Check if any mission is close to given position. Returns true if another mission is within radius
	*/	
	static bool IsAnyMissionCloseToPos(vector positionToCheck, int radiusToCheck = 1000)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		foreach(int i, SCR_DC_Mission mission: baseGameMode.missionFrame.m_MissionList)
		{
			float distance = vector.Distance(mission.GetPos(), positionToCheck);
		
			if (distance < radiusToCheck)
			{
				return true;
				break;
			}
		}

		return false;
	}		

	//------------------------------------------------------------------------------------------------
	/*!
	Check if pos is within nonValidArea
	*/	
	static bool IsPosInNonValidArea(vector pos)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
	
		foreach (SCR_DC_NonValidArea nonValidArea : baseGameMode.missionFrame.m_DC_MissionFrameConfig.nonValidAreas)
		{
			float distance = vector.Distance(pos, nonValidArea.pos);
		
			if (distance < nonValidArea.radius)
			{
				return true;
				break;
			}
		}

		return false;
	}			

	//------------------------------------------------------------------------------------------------
	/*!
	Test mission positions on map. Only for debugging.
	*/	
	static void DebugTestMissionPos(float distanceToMission = -1, float distanceToPlayer = -1)
	{	
		vector pos;

		for (int i = 0; i < 500; i++)
		{		
			SCR_DC_MissionHelper.FindMissionPos();
		}		
	}
				
/*
	https://github.com/Kexanone/KexScenarioCore_AR/blob/32c0f538d8d00a6583a51e35efed66302700adb9/scripts/Game/KSC/Global/KSC_GameTools.c#L26

	//------------------------------------------------------------------------------------------------
	//! Get transform from the given position and rotation in XZ plane
	protected static void GetTransformFromPosAndRot(out vector transform[4], vector pos, float rotation)
	{
		Math3D.MatrixIdentity3(transform);
		Math3D.AnglesToMatrix(Vector(rotation, 0, 0), transform);
		transform[3] = pos;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Spawn prefab at the given position and with the given rotation in XZ plane
	static IEntity SpawnPrefab(ResourceName name, vector pos, float rotation = 0)
	{
		vector transform[4];
		GetTransformFromPosAndRot(transform, pos, rotation);
		return SpawnPrefab(name, transform);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Spawn a structure, align it to the terrain surface and rebuild navmesh
	static IEntity SpawnStructurePrefab(ResourceName name, vector pos,  float rotation = 0)
	{
		vector transform[4];
		GetTransformFromPosAndRot(transform, pos, rotation);
		SCR_TerrainHelper.SnapAndOrientToTerrain(transform);
		IEntity entity = SpawnPrefab(name, transform);
		
		SCR_AIWorld aiWorld = SCR_AIWorld.Cast(GetGame().GetAIWorld());
		if (aiWorld)
			aiWorld.RequestNavmeshRebuildEntity(entity);
		
		return entity;
	}
*/	

			
}