//------------------------------------------------------------------------------------------------
// SECOND WAVE
//------------------------------------------------------------------------------------------------

#ifndef NEW_VERSION_WIP
class SDRC_MissionConfigSecondWave : Managed
{
	ref array<int> subIdx = {};									//subIdx from which to choose
	SDRC_EMissionSuccess activation = SDRC_EMissionSuccess.WIN;	//Which success activates the second wave
	ref array<int> delay = {};									//(seconds) Delay min-max before spawning second wave
	ref array<int> distance = {};								//min-max distance for the enemy spawn
	string info;												//Details for the hint shown for players
	SDRC_EDifficulty difficulty;								//Difficulty for specific mission
	int xp;														//Experience given	
}
#endif

#ifdef NEW_VERSION_WIP
class SDRC_MissionConfigSecondWave : Managed
{
	ref array<int> subIdx = {};									//subIdx from which to choose
	SDRC_EMissionSuccess activation = SDRC_EMissionSuccess.WIN;	//Which success activates the second wave
	float chance;
	ref array<int> delay = {};									//(seconds) Delay min-max before spawning second wave
	string info;												//Details for the hint shown for players
	SDRC_EMissionDifficulty difficulty;							//Difficulty for specific mission
	int xp;														//Experience given

	void Set(array<int> subIdx_, SDRC_EMissionSuccess activation_, float chance_, array<int> delay_, string info_, SDRC_EMissionDifficulty difficulty_, int xp_)	
	{
		subIdx = subIdx_;
		activation = activation_;
		chance = chance_;
		delay = delay_;
		info = info_;
		difficulty = difficulty_;
		xp = xp_;
	}	
}
#endif