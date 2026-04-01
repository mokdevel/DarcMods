//Helpers SDRC_MissionHelper.c

//------------------------------------------------------------------------------------------------
/*!
Compat for Freedom Fighters
*/

modded enum SDRC_EMissionError
{
	//Specific for mods
	FREEDOM_FIGHTERS_IN_SAFEZONE,
	FREEDOM_FIGHTERS_HIDEOUT_TOO_CLOSE,
}

//------------------------------------------------------------------------------------------------
modded class SDRC_MissionPosHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Check if given pos is a valid position for mission. 

	This part for Freedom Fighters support.
	*/	
	override static SDRC_EMissionError IsValidMissionPos(vector pos, float distanceToMission = -1, float distanceToPlayer = -1, bool onlyBasicChecks = false, bool ignoreNonValidArea = false)
	{	
		SDRC_EMissionError missionError = super.IsValidMissionPos(pos, distanceToMission, distanceToPlayer);
		//SDRC_EMissionError missionError = vanilla.IsValidMissionPos(pos, distanceToMission, distanceToPlayer);
		
		if (missionError == SDRC_EMissionError.NONE)
		{		
			JWK_EFactionRole factionRole = JWK.GetTerritoryControl().GetControllingRoleAt(pos);
			
			if (Math.RandomFloat(0, 1) > SDRC_Compat.m_Config.spawnRateForGreenZones)
			{			
				if ( (factionRole == JWK_EFactionRole.PLAYER) || (factionRole == JWK_EFactionRole.SUPPORTING) )
				{
					SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.FREEDOM_FIGHTERS_IN_SAFEZONE), LogLevel.SPAM);
					return SDRC_EMissionError.FREEDOM_FIGHTERS_IN_SAFEZONE;
				}
			}
						
			float distance = JWK_IndexSystem.Get().FindDistanceToNearestXZ(JWK_ResistanceHideoutEntity, pos);
			//SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Distance: " + distance, LogLevel.DEBUG);
//			if (distance < HIDEOUT_DISTANCE)
			if (distance < SDRC_Compat.m_Config.hideOutSafeZoneDistance)
			{
				SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.FREEDOM_FIGHTERS_HIDEOUT_TOO_CLOSE), LogLevel.SPAM);
				return SDRC_EMissionError.FREEDOM_FIGHTERS_HIDEOUT_TOO_CLOSE;
			}
			
		}
		return missionError;
	}
}