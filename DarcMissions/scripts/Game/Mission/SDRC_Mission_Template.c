//Mission SDRC_Mission_Template.c
//
//An example of a mission file with simple set. No real functionality.
//Do a "replace all" for the word "Template" with your mission name.

//------------------------------------------------------------------------------------------------
/*!

*/

const string DC_MISSIONCONFIG_FILE_TEMPLATE = "dc_missionConfig_Template.json";

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Template : SDRC_Mission
{
	private ref SDRC_TemplateJsonApi m_TemplateJsonApi = new SDRC_TemplateJsonApi(DC_MISSIONCONFIG_FILE_TEMPLATE);	
	private ref SDRC_TemplateConfig m_Config = new SDRC_TemplateConfig();
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Template(SDRC_EMissionType missionType, SDRC_MissionRequested request)
	{
		//Set some defaults
		//For new mission rtpw, define your own ENUM in SDRC_Mission.c and change here. *DO NOT* use modded enum as the index numbers are to kept.
		vector pos = GetPos();
				
		//Load config
		m_TemplateJsonApi.Load();
		m_Config = m_TemplateJsonApi.conf;
		
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
	//Mission specific Variables here
}

//------------------------------------------------------------------------------------------------
class SDRC_TemplateJsonApi : SDRC_JsonApi
{
	ref SDRC_TemplateConfig conf = new SDRC_TemplateConfig();
		
	//------------------------------------------------------------------------------------------------
	void SDRC_TemplateJsonApi(string fileName)
	{
		SetFileName(fileName);
	}
			
	//------------------------------------------------------------------------------------------------
	bool Load(bool createMissingFiles = true)
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(createMissingFiles);		
		if (!loadContext)
		{
			if (!createMissingFiles)
			{
				return false;
			}
			SetDefaults();
			Save();
			return true;
		}
		
		loadContext.ReadValue("", conf);
		return true;
	}	
	
	//------------------------------------------------------------------------------------------------
	void Save()
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen();
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}	
		
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		//Default
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		conf.general.markerIcon = SDRC_EMissionIcon.GM_MISSION_X_MAP;
		//Mission specific		
		conf.general.pos[0] = "0 0 0";
		conf.general.posName = "any";
		conf.general.title = "Template mission";
		conf.general.info = "Some additional information for players";
	}	
}