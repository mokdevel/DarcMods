
class SCR_DC_Mission_Camp : SCR_DC_Mission
{
	private ref SCR_DC_CampJsonApi m_CampJsonApi = new SCR_DC_CampJsonApi();	
	private ref SCR_DC_CampConfig m_Config;
	
	protected ref SCR_DC_Camp m_DC_Camp;				//Camp configuration in use	

	private int m_SpawnIndex = 0;

	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission_Camp()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Camp] Constructor", LogLevel.DEBUG);
				
		//Set some defaults
		SCR_DC_Mission();

		//Load config
		m_CampJsonApi.Load();
		m_Config = m_CampJsonApi.conf;
		
		if (m_Config.camps.Count() == 0)
		{
			SCR_DC_Log.Add("[SCR_DC_Mission_Camp] No camps defined.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;
		}
		
		//Pick a configuration for mission
		if(m_Config.campIdx == -1)
		{
			m_DC_Camp = m_Config.camps.GetRandomElement();
		}
		else
		{
			m_DC_Camp = m_Config.camps[m_Config.campIdx];
		}
		
		string posName = "Unknown";
		vector pos = m_DC_Camp.pos;
		
		SetTitle(m_DC_Camp.title + "" + posName);
		SetInfo(m_DC_Camp.info);
		SetPos(pos);
		SetPosName(posName);
		SetMarkerId(SCR_DC_MapMarkersUI.AddMarker(DC_ID_PREFIX, GetPos(), GetTitle()));

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
			if (SCR_DC_AIHelper.AreAllGroupsDead(m_Groups))
			{
				SCR_DC_Log.Add("[SCR_DC_Mission_Occupation:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
				//SetState(DC_MissionState.END);
			}
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionLifeCycleTime*1000);
	}
	
	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		super.MissionEnd();
		
		SCR_DC_Log.Add("[SCR_DC_Mission_Camp:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);
	}
	
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		SpawnCamp();
	}	

	//------------------------------------------------------------------------------------------------
	private void SpawnCamp()
	{
		SCR_DC_SpawnHelper.SetStructuresToOrigo(m_DC_Camp.campItems);
		SCR_DC_SpawnHelper.SpawnStructures(m_DC_Camp.campItems, m_DC_Camp.pos + "00 0 00", 0, -1);
		SCR_DC_SpawnHelper.SpawnStructures(m_DC_Camp.campItems, m_DC_Camp.pos + "05 0 05", 45, -1);
		SCR_DC_SpawnHelper.SpawnStructures(m_DC_Camp.campItems, m_DC_Camp.pos + "10 0 10", 90, -1);
		SCR_DC_SpawnHelper.SpawnStructures(m_DC_Camp.campItems, m_DC_Camp.pos + "15 0 15", 135, -1);
	}			
}
	
//------------------------------------------------------------------------------------------------
class SCR_DC_CampConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	int missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_DEFAULT;	//How often the mission is run
	
	//Mission specific	
	ref array<ref SCR_DC_Camp> camps = {};
	int campIdx;													//Which camp to use. -1 for random	
}

//------------------------------------------------------------------------------------------------
class SCR_DC_Camp : Managed
{		
	string comment;						//Generic comment to describe the camp. Not used in game.	
	vector pos;
	vector rot;
	string title;
	string info;

	ref array<ref SCR_DC_Structure> campItems = {};
			
	void Set(string comment_, vector pos_, vector rot_, string title_, string info_,)
	{
		comment = comment;
		pos = pos_;
		rot = rot_;
		title = title_;
		info = info_;		
	}
}		

//------------------------------------------------------------------------------------------------
class SCR_DC_CampJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Camp.json";
	ref SCR_DC_CampConfig conf = new SCR_DC_CampConfig;
		
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
		//Mission specific
		conf.missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_DEFAULT;
		conf.campIdx = 0;

		SCR_DC_Camp camp0 = new SCR_DC_Camp;
		camp0.Set
		(
			"Comment",
			"1000 0 2480",
			"0 0 0",
			"Banditcamp at ",
			"Loot is for grabs."
		);
		conf.camps.Insert(camp0);
		
		SCR_DC_Structure camp0item0 = new SCR_DC_Structure;
		camp0item0.Set
		(
			"{0511E95F422061BB}Prefabs/Props/Recreation/Camp/TentSmall_02/TentSmall_02_blue.et",
			"1029.974 39 2487.454",
			"0 0 0"
		);
		camp0.campItems.Insert(camp0item0);
		SCR_DC_Structure camp0item1 = new SCR_DC_Structure;
		camp0item1.Set
		(
			"{39C308BBB5945B85}Prefabs/Props/Military/Furniture/ChairMilitary_US_02.et",
			"1028.55 39 2485.5",
			"0 119.334 0"
		);
		camp0.campItems.Insert(camp0item1);
		SCR_DC_Structure camp0item2 = new SCR_DC_Structure;
		camp0item2.Set
		(
			"{D9842C11742C00CF}Prefabs/Props/Civilian/Fireplace_01.et",
			"1029.9 39 2484.78",
			"0 0 0"
		);
		camp0.campItems.Insert(camp0item2);			
	}	
}