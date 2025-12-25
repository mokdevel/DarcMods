//Helpers SDRC_MissionRequestHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_MissionRequested : Managed
{
	EntityID entityID = null;
	int requestId = -1;
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	SDRC_EMissionType missionType = SDRC_EMissionType.NONE;
}

//------------------------------------------------------------------------------------------------
class SDRC_MissionRequestHelper
{							
	//------------------------------------------------------------------------------------------------
	/*!	
	Fill mission details from a mission request
	*/	
	static void FillMissionRequest()
	{

	}
	
	//------------------------------------------------------------------------------------------------	
	/*!
	Remove deleted missions from the list
	*/		
	static void CleanMissionsRequestedArray()
	{

	}

	//------------------------------------------------------------------------------------------------
	/*!
	Dump information of GM spawned missions
	*/			
	static void dumpMissionRequested()
	{
	
	}				
}
