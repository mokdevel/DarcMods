//------------------------------------------------------------------------------------------------
// Quick Reaction Force
//------------------------------------------------------------------------------------------------
class SDRC_MissionConfigQrf : Managed
{
	ref array<int> subIdx = {};									//subIdx from which to choose
	SDRC_EMissionSuccess activation = SDRC_EMissionSuccess.WIN;	//Which success activates the second wave
	float chance;
	ref array<int> delay = {};									//(seconds) Delay min-max before spawning second wave

	void Set(array<int> subIdx_, SDRC_EMissionSuccess activation_, float chance_, array<int> delay_)	
	{
		subIdx = subIdx_;
		activation = activation_;
		chance = chance_;
		delay = delay_;
	}	
}