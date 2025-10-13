//Helpers SDRC_MissionHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for missions. 
*/

modded enum SDRC_EMissionError
{
	//Specific for mods
	IN_FREEDOM_FIGHTERS_SAFEZONE,
}

//------------------------------------------------------------------------------------------------
modded class SDRC_MissionHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Check if given pos is a valid position for mission. 

	This part for Freedom Fighters support.
	*/	
	override static SDRC_EMissionError IsValidMissionPos(vector pos, float distanceToMission = -1, float distanceToPlayer = -1)
	{	
		SDRC_EMissionError missionError = super.IsValidMissionPos(pos, distanceToMission, distanceToPlayer);
		
		if (missionError == SDRC_EMissionError.NONE)
		{		
			JWK_EFactionRole factionRole = JWK.GetTerritoryControl().GetControllingRoleAt(pos);
			
			if ( (factionRole == JWK_EFactionRole.PLAYER) || (factionRole == JWK_EFactionRole.SUPPORTING) )
			{
				SDRC_Log.Add("[SDRC_MissionHelper:IsValidMissionPos] Failed: " + SCR_Enum.GetEnumName(SDRC_EMissionError, SDRC_EMissionError.IN_FREEDOM_FIGHTERS_SAFEZONE), LogLevel.SPAM);
				return SDRC_EMissionError.IN_FREEDOM_FIGHTERS_SAFEZONE;
			}
		}
		
		return missionError;
	}
}