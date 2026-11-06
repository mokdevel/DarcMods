//SDRC_RPLGMComp.c

//------------------------------------------------------------------------------------------------
/*!
RPL component to provide GM map information to clients.

The component is split accross multiple additional files:
- SDRC_RPLGMComp_NonValidArea.c
- SDRC_RPLGMComp_Symbols.c
- SDRC_RPLGMComp_MissionData.c

NOTE: This has been added to {}}Prefabs/Characters/DarcMissions/DefaultPlayerController.et
*/
[ComponentEditorProps(category: "GameScripted/DarcMissions", description: "DarcMissions GM Helper Component")]
class SDRC_RplGMCompClass : ScriptComponentClass { }
SDRC_RplGMCompClass g_RplGMCompClass;

//------------------------------------------------------------------------------------------------
class SDRC_RplGMComp : ScriptComponent
{
	private static SDRC_RplGMComp s_Instance;	
	
	[RplProp(onRplName: "OnUpdateTimeLeft")]
	bool m_ShowMissionTimeLeft = false;

	//------------------------------------------------------------------------------------------------
    override void OnPostInit(IEntity owner)
    {
		Init_MissionData();
		
		//Store instance
		s_Instance = this;				
    }
 
	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to component
	*/
	static SDRC_RplGMComp GetInstance()
	{
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get instance
	//TBD: This might be exactly the same GetInstance() above
	static SDRC_RplGMComp FindInstance()
	{
		SDRC_RplGMEntity gmEnt = SDRC_RplGMEntity.GetInstance();
		
		if (!gmEnt)
		{
			return null;
		}
		
		return SDRC_RplGMComp.Cast(gmEnt.FindComponent(SDRC_RplGMComp));
	}
	
	//------------------------------------------------------------------------------------------------	
	// Time left stuff
	//------------------------------------------------------------------------------------------------	
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Update time left attribute
	*/
	protected void OnUpdateTimeLeft()
	{
		SDRC_Log.Add("[SDRC_RplStoryComp:OnUpdateTimeLeft] TimeLeft updated.", LogLevel.NORMAL);
	}	
	
	//------------------------------------------------------------------------------------------------
	void UpdateTimeLeft(bool timeLeft)
	{
		RpcDo_UpdateTimeLeft(timeLeft);
		Rpc(RpcDo_UpdateTimeLeft, timeLeft);
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	RPL: Time left attribute update
	*/
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcDo_UpdateTimeLeft(bool timeLeft)
	{
		m_ShowMissionTimeLeft = timeLeft;

		Replication.BumpMe();
	}		
	
	//------------------------------------------------------------------------------------------------	
	// Mission stuff
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Request for a mission deletion.
	*/
 	void DoDeleteMission(int playerID, string missionId)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:DoDeleteMission] Deletion of " + missionId + " requested by " + playerID, LogLevel.SPAM);	
		DeleteMission(missionId);
//		SyncMapSymbols(playerID);
	}
		
	//------------------------------------------------------------------------------------------------
	/*!	
	Request for a mission ending. Used by DarcStories
	*/
 	void DoEndMission(int playerID, string missionId)
	{
		SDRC_Log.Add("[SDRC_RplGMComp:DoEndMission] Ending of " + missionId + " requested by " + playerID, LogLevel.SPAM);	
		EndMission(missionId, SDRC_EMissionSuccess.LOSE);
//		SyncMapSymbols(playerID);
	}
	
	//------------------------------------------------------------------------------------------------	
	// Mission stuff
	//------------------------------------------------------------------------------------------------	
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Delete a mission with specific mission ID (DCMxxxx).
	*/	
	void DeleteMission(string id, SDRC_EMissionSuccess success = SDRC_EMissionSuccess.DELETED)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!baseGameMode)
		{
			SDRC_Log.Add("[SDRC_RplGMComp:DeleteMission] SCR_BaseGameMode not found.", LogLevel.ERROR);
		}
		
		int idx = baseGameMode.missionFrame.FindMissionWithId(id);
		if (idx != -1)
		{
			baseGameMode.missionFrame.m_MissionList[idx].SetState(SDRC_EMissionState.END);
			baseGameMode.missionFrame.m_MissionList[idx].SetSuccess(success);
			SDRC_Log.Add("[SDRC_RplGMComp:DeleteMission] Ending mission: " + id + " - " + baseGameMode.missionFrame.m_MissionList[idx].GetTitle(), LogLevel.DEBUG);				
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!	
	Delete a mission with specific mission ID (DCMxxxx).
	*/	
	void EndMission(string id, SDRC_EMissionSuccess success)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());			
		if (!baseGameMode)
		{
			SDRC_Log.Add("[SDRC_RplGMComp:EndMission] SCR_BaseGameMode not found.", LogLevel.ERROR);
		}
		
		int idx = baseGameMode.missionFrame.FindMissionWithId(id);
		if (idx != -1)
		{
			if (success == SDRC_EMissionSuccess.LOSE)
			{
				baseGameMode.missionFrame.m_MissionList[idx].DoLose();
			}
			if (success == SDRC_EMissionSuccess.WIN)
			{
				baseGameMode.missionFrame.m_MissionList[idx].DoWin();
			}
			SDRC_Log.Add("[SDRC_RplGMComp:DeleteMission] Ending mission: " + id + " - (" + SCR_Enum.GetEnumName(SDRC_EMissionSuccess, success) + ")" + baseGameMode.missionFrame.m_MissionList[idx].GetTitle(), LogLevel.DEBUG);				
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	// Modded class references
	//
	// Functions in modded class to sort the stuff in files per topic. Reference are here for compilation reasons.
	//------------------------------------------------------------------------------------------------	
	
 	void Init_MissionData() {}
	void Sync_Symbols(int playerID) {}
	
	//------------------------------------------------------------------------------------------------
}