//Mission SCR_DC_Mission_Squatter.c
//
//An example of a mission file with simple set. No real functionality.
//Do a "replace all" for the word "Squatter" with your mission name.

class SCR_DC_Mission_Squatter : SCR_DC_Mission
{
	private ref SCR_DC_SquatterJsonApi m_SquatterJsonApi = new SCR_DC_SquatterJsonApi();	
	private ref SCR_DC_SquatterConfig m_Config;

	protected ref SCR_DC_Squatter m_DC_Squatter;	//Squatter configuration in use
		
	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission_Squatter()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Squatter] Constructor", LogLevel.DEBUG);
				
		//Set some defaults
		SCR_DC_Mission();
		SetType(DC_EMissionType.SQUATTER);

		//Load config
		m_SquatterJsonApi.Load();
		m_Config = m_SquatterJsonApi.conf;
		
		//Pick a configuration for mission
		int idx = SCR_DC_MissionHelper.SelectMissionIndex(m_Config.squatterList);
		if(idx == -1)
		{
			SCR_DC_Log.Add("[SCR_DC_Mission_Squatter] No squatters defined.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;
		}
		m_DC_Squatter = m_Config.squatters[idx];
		
		//Set defaults
		vector pos = m_DC_Squatter.pos;
		string posName = m_DC_Squatter.posName;
		
		//Find a location for the mission
		if (pos == "0 0 0")
		{
			//TBD: Find location and building.
			//pos = SCR_DC_MissionHelper.FindMissionPos(m_DC_Squatter.locationTypes, m_Config.emptySize);
		}
		
		if (pos != "0 0 0")
		{					
			SetPos(pos);
			SetPosName(SCR_DC_Locations.CreateName(pos, posName));
			SetTitle(m_DC_Squatter.title + "" + GetPosName());
			SetInfo(m_DC_Squatter.info);			
			SetMarker(m_Config.showMarker, DC_EMissionIcon.MISSION);
			SetShowHint(m_Config.showHint);
				
			SetState(DC_MissionState.INIT);			
		}
		else
		{				
			//No suitable location found.
			SCR_DC_Log.Add("[SCR_DC_Mission_Squatter] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;
		}	
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
				SCR_DC_Log.Add("[SCR_DC_Mission_Squatter:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
				SetState(DC_MissionState.END);
			}*/			
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionCycleTime*1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		super.MissionEnd();	
		
		//The rest of your clean up code.
		
		SCR_DC_Log.Add("[SCR_DC_Mission_Squatter:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		//Code for whatever you need for spawning things.
	}
}
	
//------------------------------------------------------------------------------------------------
class SCR_DC_SquatterConfig : SCR_DC_MissionConfig
{
	//Mission specific
	
	//Variables here
	ref array<ref int> squatterList = {};				//The indexes of squatters.
	ref array<ref SCR_DC_Squatter> squatters = {};		//List of squatters
}

//------------------------------------------------------------------------------------------------
class SCR_DC_Squatter : Managed
{
	//Occupation specific
	string comment;							//Generic comment to describe the mission. Not used in game.
	vector pos;								//Position for mission. "0 0 0" used for random location chosen from locationTypes.
	string posName;							//Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes 
	string title;							//Title for the hint shown for players
	string info;							//Details for the hint shown for players
	ref array<EMapDescriptorType> locationTypes = {};
	ref array<int> AICount = {};			//min, max
	ref array<string> AITypes = {};
	int AISkill;
	float AIperception;
	ref array<string> buildingNames = {};
	
	void Set(string comment_, vector pos_, string posName_, string title_, string info_, array<EMapDescriptorType> locationTypes_, array<int> AICount_, array<string> AITypes_, int AISkill_, float AIperception_, array<string> buildingNames_)
	{
		comment = comment_;
		pos = pos_;
		posName = posName_;
		title = title_;
		info = info_;
		locationTypes = locationTypes_;
		AICount = AICount_;
		AITypes = AITypes_;
		AISkill = AISkill_;
		AIperception = AIperception_;	
		buildingNames = buildingNames_;	
	}
	
}

//------------------------------------------------------------------------------------------------
class SCR_DC_SquatterJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Squatter.json";
	ref SCR_DC_SquatterConfig conf = new SCR_DC_SquatterConfig;
		
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
		conf.missionCycleTime = DC_MISSION_CYCLE_TIME_DEFAULT;
		conf.showMarker = true;		
		//Mission specific
		conf.squatterList = {0};		
	}	
}