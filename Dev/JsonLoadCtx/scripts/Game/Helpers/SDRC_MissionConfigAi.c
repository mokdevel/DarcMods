//------------------------------------------------------------------------------------------------
// AI CONFIG
//------------------------------------------------------------------------------------------------
class SDRC_MissionConfigAi : Managed
{
	private ref array<int> count = {};
	ref array<string> types = {}; 			//The prefab names of AI groups or characters. The AI is randomly picked from this list.
	private int skill;						//Skill for AI (0-100). See SCR_AICombatComponent and EAISkill
	private float perception;
	ref array<int> waypointRange = {};		//min, max
	SDRC_EWaypointGenerationType waypointGenType;
	SDRC_EWaypointMoveType waypointMoveType;
	
	void Set(array<int> count_, array<string> types_, int skill_, float perception_, array<int> waypointRange_, SDRC_EWaypointGenerationType waypointGenType_, SDRC_EWaypointMoveType waypointMoveType_)
	{
		count = count_;
		types = types_;
		skill = skill_;
		perception = perception_;		
		waypointRange = waypointRange_;
		waypointGenType = waypointGenType_;
		waypointMoveType = waypointMoveType_;
	}
	
	//------------------------------------------------------------------------------------------------
	/*! Return the count of AIs for the mission 
	
	NOTE: mission and missionFrame difficulty affects the outcome	
	*/	
	int GetCount(SDRC_EMissionDifficulty difficulty = SDRC_EMissionDifficulty.NORMAL)
	{
		int cnt = SDRC_Misc.RandomInt(count[0], count[1]);
		
		SCR_BaseGameMode m_BaseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (m_BaseGameMode)
		{
		}
		
		return cnt;
	}
	
	//------------------------------------------------------------------------------------------------
	/*! Return a proper skill value that matches EAISkill. 
	
	NOTE: mission and missionFrame difficulty affects the outcome	
	*/	
	int GetSkill(SDRC_EMissionDifficulty difficulty = SDRC_EMissionDifficulty.NORMAL)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	/*! Return a proper perception value. 
	
	NOTE: mission and missionFrame difficulty affects the outcome	
	TBD: I'm a little unsure how this in total affects, but I presume the higher is better.
	*/
	float GetPerception(SDRC_EMissionDifficulty difficulty = SDRC_EMissionDifficulty.NORMAL)
	{
	}			
}