//Mission SDRC_Mission_Template.c
//
//An example of a mission file with simple set. No real functionality.
//Do a "replace all" for the word "Template" with your mission name.

//------------------------------------------------------------------------------------------------
/*!

*/

const string DC_MISSIONCONFIG_FILE_TEMPLATE = "dc_missionConfig_Template.json";
const int DC_MISSIONCONFIG_FILE_TEMPLATE_JSONVER = 2;

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Template : SDRC_Mission
{
	private ref SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_TEMPLATE);	
	private ref SDRC_TemplateConfig m_Config = new SDRC_TemplateConfig();
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Template(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Set some defaults
		//For new mission rtpw, define your own ENUM in SDRC_Mission.c and change here. *DO NOT* use modded enum as the index numbers are to kept.
		vector pos = GetPos();
				
		//Load config
		if (!m_JsonApi.Load(m_Config, SDRC_MissionConfig.Cast(m_Config), DC_MISSIONCONFIG_FILE_TEMPLATE_JSONVER))
		{
			SetState(SDRC_EMissionState.FAILED, SDRC_EMissionError.ERROR_LOADING_JSON);
			return;
		}
		m_Config.LoadMissionFiles(DC_MISSIONCONFIG_FILE_TEMPLATE_JSONVER);
		
		//Check if GM requested mission
		if (!IsRequested())
		{		
			pos = m_Config.general.pos[0];
		}
		else
		{
			//The provided pos is accepted
		}
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(GetPos(), m_Config.general.posName));
		SetVisibility(m_Config.showMarker, m_Config.showHint, m_Config.showMessage);
		UpdateGeneral(m_Config.general);		
/*		SetHint(m_Config.showHint, m_Config.general.title, m_Config.general.info);		
		SetMessages(m_Config.showMessage, m_Config.general.winMessage, m_Config.general.loseMessage);						
		SetMarker(m_Config.showMarker, m_Config.general.markerIcon, m_Config.general.markerType);
		SetWinCondition(m_Config.general.winCondition);*/
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == SDRC_EMissionState.SPAWN)
		{
			MissionSpawn();
			SetState(SDRC_EMissionState.ACTIVE);
		}

		if (GetState() == SDRC_EMissionState.END)
		{
			MissionEnd();
			SetState(SDRC_EMissionState.EXIT);
		}	
				
		if (GetState() == SDRC_EMissionState.ACTIVE)
		{			
			//Add code for runtime
			
/*			//Eventually when mission is to be ended do this:
			//SetState(SDRC_EMissionState.END);

			//For example:
			if (!IsActive())
			{
				SetState(SDRC_EMissionState.END);
			}
*/
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionCycleTime*1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		super.MissionEnd();	
		
		//The rest of your clean up code.
	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		//Code for whatever you need for spawning things.
	}
}
	
//------------------------------------------------------------------------------------------------
class SDRC_TemplateConfig : SDRC_MissionConfig
{
	//Common for all
	ref SDRC_MissionConfigGeneral general = new SDRC_MissionConfigGeneral();
	ref SDRC_MissionConfigAi ai = new SDRC_MissionConfigAi();
	ref SDRC_MissionConfigQrf qrf = null;
	//Mission specific variables here
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_TemplateConfig data = SDRC_TemplateConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		missionType = SDRC_EMissionType.NONE;
		
		//Default
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		general.markerIcon = SDRC_EMissionIcon.GM_MISSION_X_MAP;
		//Mission specific		
		general.pos[0] = "0 0 0";
		general.posName = "any";
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();		
		message.Set("Template mission",
			"Some additional information for players",
			"Win",
			"Lose",);
		general.messages.Insert(message);
	}
}