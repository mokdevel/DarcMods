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
	\param onlyBasicChecks Check only basic things
	
	Checks are done for positions:
	- Basic checks:
		- not in water	
		- not under map
	- Full checks:	
		- not in a nonValidArea
		- no players nearby
		- no players far away
		- no missions nearby
	*/	
	static SDRC_EMissionError IsValidMissionPos(vector pos, float distanceToMission = -1, float distanceToPlayer = -1, bool onlyBasicChecks = false, bool ignoreNonValidArea = false)
	{	
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		
		// --- Basic checks ---
		if (SDRC_Misc.IsPosInWater(pos))
		{
			SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.POSITION_IN_WATER), LogLevel.SPAM);
			return SDRC_EMissionError.POSITION_IN_WATER;
		}
				
		if (SDRC_Misc.IsPosUnderMap(pos))
		{
			SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.POSITION_UNDER_MAP), LogLevel.SPAM);
			return SDRC_EMissionError.POSITION_UNDER_MAP;
		}

		if (!ignoreNonValidArea)
		{
			if (IsPosInNonValidArea(pos))
			{
				SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.IN_NON_VALID_AREA), LogLevel.SPAM);
				return SDRC_EMissionError.IN_NON_VALID_AREA;
			}
		}
		
		//If we only check for basic things, return		
		if (onlyBasicChecks)
		{
			return SDRC_EMissionError.NONE;
		}
								
		// --- Full checks ---
		if (distanceToMission == -1)
			distanceToMission = baseGameMode.missionFrame.m_Config.minDistanceToMission;

		if (distanceToPlayer == -1)
			distanceToPlayer = baseGameMode.missionFrame.m_Config.minDistanceToPlayer;
		
		//If no players in game, ignore this
		if (SDRC_PlayerHelper.PlayerCount() > 0)
		{
			//Check that players are not too close
			if (SDRC_PlayerHelper.IsAnyPlayerCloseToPos(pos, distanceToPlayer))
			{
				SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.PLAYER_TOO_CLOSE), LogLevel.SPAM);
				return SDRC_EMissionError.PLAYER_TOO_CLOSE;
			}

			//Check that players are not too far. 
			//This check is not done for static missions.
			if (baseGameMode.missionFrame.m_Config.maxDistanceToPlayer > -1)
			{
				if (!SDRC_PlayerHelper.IsAnyPlayerCloseToPos(pos, baseGameMode.missionFrame.m_Config.maxDistanceToPlayer, distanceToPlayer))
				{
					SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.PLAYER_TOO_FAR), LogLevel.SPAM);
					return SDRC_EMissionError.PLAYER_TOO_FAR;
				}
			}
		}
		
		if (IsAnyMissionCloseToPos(pos, distanceToMission))
		{
			SDRC_Log.Add("[SDRC_MissionPosHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.MISSION_TOO_CLOSE), LogLevel.SPAM);
			return SDRC_EMissionError.MISSION_TOO_CLOSE;
		}

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
	
		foreach (SDRC_NonValidArea nonValidArea : baseGameMode.missionFrame.m_ConfigNonValidArea.m_NonValidAreas)
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