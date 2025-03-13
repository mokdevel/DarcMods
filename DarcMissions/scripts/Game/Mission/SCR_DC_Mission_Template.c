//Mission SCR_DC_Mission_Template.c
//
//An example of a mission file with simple set. No real functionality.
//Do a "replace all" for the word "Template" with your mission name.

class SCR_DC_Mission_Template : SCR_DC_Mission
{
	private ref SCR_DC_TemplateJsonApi m_TemplateJsonApi = new SCR_DC_TemplateJsonApi();	
	private ref SCR_DC_TemplateConfig m_Config;
	
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission_Template()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Template] Constructor", LogLevel.DEBUG);
				
		//Set some defaults
		SCR_DC_Mission();
		SetType(DC_EMissionType.NONE);	//REMEMBER: Define your own ENUM in SCR_DC_Mission.c and change here. Don't use modded enum.

		//Load config
		m_TemplateJsonApi.Load();
		m_Config = m_TemplateJsonApi.conf;
		
		SetPos(m_Config.pos);
		SetPosName(m_Config.posName);
		SetTitle(m_Config.title);
		SetInfo(m_Config.info);
		SetMarker(m_Config.showMarker, DC_EMissionIcon.MISSION);

		SetState(DC_MissionState.INIT);			
	}	
	
	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		if (GetState() == DC_MissionState.INIT)
		{
			MissionSpawn();
			SetState(DC_MissionState.ACTIVE);
		}

		if (GetState() == DC_MissionState.END)
		{
			MissionEnd();
			SetState(DC_MissionState.EXIT);
		}	
				
		if (GetState() == DC_MissionState.ACTIVE)
		{			
			//Add code for runtime
			
/*			//Eventually when mission is to be ended do this:
			//SetState(DC_MissionState.END);

			//For example:			
			if (SCR_DC_AIHelper.AreAllGroupsDead(m_Groups))
			{
				SCR_DC_Log.Add("[SCR_DC_Mission_Template:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
				SetState(DC_MissionState.END);
			}*/			
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionLifeCycleTime*1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		super.MissionEnd();	
		
		//The rest of your clean up code.
		
		SCR_DC_Log.Add("[SCR_DC_Mission_Template:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		//Code for whatever you need for spawning things.
	}
}
	
//------------------------------------------------------------------------------------------------
class SCR_DC_TemplateConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	int missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_DEFAULT;	//How often the mission is run
	bool showMarker;
	
	//Mission specific
	vector pos;
	string posName;
	string title;
	string info;
	
	//Variables here
}

//------------------------------------------------------------------------------------------------
class SCR_DC_TemplateJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Template.json";
	ref SCR_DC_TemplateConfig conf = new SCR_DC_TemplateConfig;
		
	//------------------------------------------------------------------------------------------------
	void Load()
	{	
		SCR_JsonLoadContext loadContext = LoadConfig(DC_MISSIONCONFIG_FILE);
		
		if(!loadContext)
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
		conf.missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_DEFAULT;
		conf.showMarker = true;		
		//Mission specific		
		conf.pos = "0 0 0";
		conf.posName = "A location name";
		conf.title = "Template mission";
		conf.info = "Some additional information for players";
	}	
}