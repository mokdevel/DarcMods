//SDRC_RPLGMComp_MissionData.c

//------------------------------------------------------------------------------------------------
/*!
RPL component to provide GM map information to clients.

NOTE: This has been added to {}}Prefabs/Characters/DarcMissions/DefaultPlayerController.et
*/

//------------------------------------------------------------------------------------------------
modded class SDRC_RplGMComp
{
	static ref array<ref SDRC_GMMissionData> m_MissionData = {};

	//------------------------------------------------------------------------------------------------
	//Values needed for runtime SDRC_MissionSubIdxEditorAttributeBaseValues modifying. Ugly hack!
	//AR does not provide proper runtime editing of the editable values so trick the game. We use 
	//a global value that can be set and read in SDRC_MissionSubIdxEditorAttributeBaseValues
	//The table is filled in SDRC_MissionEnumHelper:CreateAllConfigs
	
	//The largest index found in the missionListMission. The array is filled SDRC_MissionEnumHelper:CreateAllConfigs at startup.
	ref array<int> missionIndexMaxList = {0};					//First item is prefilled to reflect SDRC_EMissionType.NONE
	
	//Values are filled in SCR_BaseEditorAttributeVar ReadVariable() when opening the mission for editing in GM.
	int m_MissionIndexMax = 0;									//The max value for the slider. This is the biggest mission index found.
	SDRC_EMissionType m_MissionType = SDRC_EMissionType.NONE;	//Mission type we're editing.
	
	//------------------------------------------------------------------------------------------------	
	// Mission data stuff
	//------------------------------------------------------------------------------------------------	
	
	override void InitMissionData()
	{
		//Add an empty stub for missionType.NONE as the first one
		ref SDRC_GMMissionData missionData = new SDRC_GMMissionData();
		m_MissionData.Insert(missionData);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Store mission data in the component
	*/	
	
	void AddMissionData(SDRC_EMissionType type, int maxIndex, array<string> names)
	{
		ref SDRC_GMMissionData missionData = new SDRC_GMMissionData();
		missionData.type = type;
		missionData.maxIndex = maxIndex;
		foreach (string name : names)
		{
			missionData.names.Insert(name);
		}
		m_MissionData.Insert(missionData);
	}
}