//Mission SDRC_Mission_Template.c
//
//An example of a mission file with simple set. No real functionality.
//Do a "replace all" for the word "Template" with your mission name.

//------------------------------------------------------------------------------------------------
/*!

*/

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Template : SDRC_Mission
{
	private ref SDRC_TemplateJsonApi m_TemplateJsonApi = new SDRC_TemplateJsonApi();	
	private ref SDRC_TemplateConfig m_Config;
	
	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Template()
	{
		SDRC_Log.Add("[SDRC_Mission_Template] Constructor", LogLevel.SPAM);
				
		//Set some defaults
		SetType(DC_EMissionType.NONE);	//REMEMBER: Define your own ENUM in SDRC_Mission.c and change here. Don't use modded enum.

		//Load config
		m_TemplateJsonApi.Load();
		m_Config = m_TemplateJsonApi.conf;
		
		SetPos(m_Config.pos);
		SetPosName(SDRC_Locations.CreateName(GetPos(), m_Config.posName));
		SetTitle(m_Config.title + "" + GetPosName());
		SetInfo(m_Config.info);
		SetMarker(m_Config.showMarker, DC_EMissionIcon.N_MISSION);
		SetShowHint(m_Config.showHint);

		SetState(DC_EMissionState.INIT);			
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		if (GetState() == DC_EMissionState.INIT)
		{
			MissionSpawn();
			SetState(DC_EMissionState.ACTIVE);
		}

		if (GetState() == DC_EMissionState.END)
		{
			MissionEnd();
			SetState(DC_EMissionState.EXIT);
		}	
				
		if (GetState() == DC_EMissionState.ACTIVE)
		{			
			//Add code for runtime
			
/*			//Eventually when mission is to be ended do this:
			//SetState(DC_EMissionState.END);

			//For example:
			if (!IsActive())
			{
				SDRC_Log.Add("[SDRC_Mission_Template:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
				SetState(DC_EMissionState.END);
			}
			
//			if (SDRC_AIHelper.AreAllGroupsDead(m_Groups))
//			{
//				SDRC_Log.Add("[SDRC_Mission_Template:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
//				SetState(DC_EMissionState.END);
//			}*/			
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionCycleTime*1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		super.MissionEnd();	
		
		//The rest of your clean up code.
		
		SDRC_Log.Add("[SDRC_Mission_Template:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);
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
	//Mission specific
	vector pos;
	string posName;
	string title;
	string info;
	
	//Variables here
}

//------------------------------------------------------------------------------------------------
class SDRC_TemplateJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Template.json";
	ref SDRC_TemplateConfig conf = new SDRC_TemplateConfig();
		
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_MISSIONCONFIG_FILE);
		
		if (!loadContext)
		{
			SetDefaults();
			Save("");
			return;
		}

		loadContext.ReadValue("", conf);
	}	
	
	//------------------------------------------------------------------------------------------------
	void Save(string data)
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen(DC_MISSIONCONFIG_FILE);
		saveContext.WriteValue("", conf);
		SaveConfigClose(saveContext);
	}	
		
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		//Default
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		conf.showMarker = true;		
		//Mission specific		
		conf.pos = "0 0 0";
		conf.posName = "A location name";
		conf.title = "Template mission";
		conf.info = "Some additional information for players";
	}	
}