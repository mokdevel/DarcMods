//Helpers SDRC_MissionPosHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for position check for valid areas. 
*/

//------------------------------------------------------------------------------------------------
class SDRC_MissionPosHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Check if given pos is a valid position for mission. 

	\param distanceToMission Distance to another mission. Two missions shall not be too close to each other. -1 will use the default from MissionFrame.
	\param distanceToPlayer Mission shall not spawn too close to a player. -1 will use the default from MissionFrame.
	
	The position shall have ... 
	- no players nearby
	- no missions nearby
	- not in water	
	*/	
	static SDRC_EMissionError IsValidMissionPos(vector pos, float distanceToMission = -1, float distanceToPlayer = -1)
	{	
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		if (distanceToMission == -1)
			distanceToMission = baseGameMode.missionFrame.m_Config.minDistanceToMission;

		if (distanceToPlayer == -1)
			distanceToPlayer = baseGameMode.missionFrame.m_Config.minDistanceToPlayer;

		if (SDRC_Misc.IsPosInWater(pos))
		{
			SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.POSITION_IN_WATER), LogLevel.SPAM);
			return SDRC_EMissionError.POSITION_IN_WATER;
		}
				
		if (SDRC_Misc.IsPosUnderMap(pos))
		{
			SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.POSITION_UNDER_MAP), LogLevel.SPAM);
			return SDRC_EMissionError.POSITION_UNDER_MAP;
		}
				
		if (SDRC_PlayerHelper.IsAnyPlayerCloseToPos(pos, distanceToPlayer))
		{
			SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.PLAYER_TOO_CLOSE), LogLevel.SPAM);
			return SDRC_EMissionError.PLAYER_TOO_CLOSE;
		}

		if (IsAnyMissionCloseToPos(pos, distanceToMission))
		{
			SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.MISSION_TOO_CLOSE), LogLevel.SPAM);
			return SDRC_EMissionError.MISSION_TOO_CLOSE;
		}

		if (IsPosInNonValidArea(pos))
		{
			SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.IN_NON_VALID_AREA), LogLevel.SPAM);
			return SDRC_EMissionError.IN_NON_VALID_AREA;
		}
		
/*		#ifdef FREEDOM_FIGHTERS
			JWK_EFactionRole factionRole = JWK.GetTerritoryControl().GetControllingRoleAt(pos);
			SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.IN_FREEDOM_FIGHTERS_SAFEZONE), LogLevel.ERROR);
			
			if ( (factionRole == JWK_EFactionRole.PLAYER) || (factionRole == JWK_EFactionRole.SUPPORTING) )
			{
				SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.IN_FREEDOM_FIGHTERS_SAFEZONE), LogLevel.SPAM);
				return SDRC_EMissionError.IN_FREEDOM_FIGHTERS_SAFEZONE;
			}		
		#endif*/
		
		return SDRC_EMissionError.NONE;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Check if any mission is close to given position. Returns true if another mission is within radius
	*/	
	static bool IsAnyMissionCloseToPos(vector positionToCheck, int radiusToCheck = 1000)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		foreach (int i, SDRC_Mission mission: baseGameMode.missionFrame.m_MissionList)
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
	
		foreach (SDRC_NonValidArea nonValidArea : baseGameMode.missionFrame.m_aNonValidAreas)
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
}