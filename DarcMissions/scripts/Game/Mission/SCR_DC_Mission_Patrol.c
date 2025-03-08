//Mission SCR_DC_Mission_Patrol.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns groups to defend a location
*/

//------------------------------------------------------------------------------------------------

class SCR_DC_Mission_Patrol : SCR_DC_Mission
{
	private ref SCR_DC_PatrolJsonApi m_PatrolJsonApi = new SCR_DC_PatrolJsonApi;	
	private ref SCR_DC_PatrolConfig m_Config;
	
	protected ref SCR_DC_Patrol m_DC_Patrol;	//Patrol configuration in use
	
	private int m_SpawnIndex = 0;						//Counter for the item to spawn

	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission_Patrol()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Patrol] Constructor", LogLevel.DEBUG);
				
		//Set some defaults
		SCR_DC_Mission();
		SetType(DC_EMissionType.PATROL);

		//Load config
		m_PatrolJsonApi.Load();
		m_Config = m_PatrolJsonApi.conf;
		
		if (m_Config.patrols.Count() == 0)
		{
			SCR_DC_Log.Add("[SCR_DC_Mission_Patrol] No patrols defined.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;
		}
		
		//Pick a configuration for mission
		if(m_Config.patrolList.Count() == 1)
		{
			//-1 = Pick any random value
			if(m_Config.patrolList[0] == -1)
			{
				m_DC_Patrol = m_Config.patrols.GetRandomElement();
			}
			else
			{
				//Single number = Use it as a index
				m_DC_Patrol = m_Config.patrols[m_Config.patrolList[0]];
			}
		}
		else
		{
			int patrolIdx = m_Config.patrolList.GetRandomElement();
			m_DC_Patrol = m_Config.patrols[patrolIdx];
		}

		//Set defaults
		vector pos = m_DC_Patrol.locationPos;
		string posName = m_DC_Patrol.locationName;
		IEntity location = null;
		
		//Find a location for the mission
		if (pos == "0 0 0")
		{
			location = SCR_DC_MissionHelper.FindMissionLocation(m_DC_Patrol.locationTypes);
		}
		else
		{
			//Use a predefined location
			SCR_DC_Log.Add("[SCR_DC_Mission_Patrol] Predefined locations are not supported. Yet.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;			
		}
		
		if (location)
		{	
			if (posName == "any")
			{
				posName = SCR_StringHelper.Translate(location.GetName());
			}			
			SetTitle(m_DC_Patrol.title + "" + posName);
			SetInfo(m_DC_Patrol.info);			
			SetPos(location.GetOrigin());
			SetPosName(posName);
			SetMarker(m_Config.showMarker, DC_EMissionIcon.MISSION);

			SetState(DC_MissionState.INIT);
		}
		else
		{				
			//No suitable location found.
			SCR_DC_Log.Add("[SCR_DC_Mission_Patrol] Could not find suitable location.", LogLevel.ERROR);
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
				if (!IsActive())
				{
					SCR_DC_Log.Add("[SCR_DC_Mission_Patrol:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
					SetState(DC_MissionState.END);
				}
			}
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionLifeCycleTime*1000);
	}

	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		super.MissionEnd();
		
		SCR_DC_Log.Add("[SCR_DC_Mission_Patrol:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);		
	}
			
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		IEntity entity;

		//Spawn mission AI 
		int groupCount = Math.RandomInt(m_DC_Patrol.groupCount[0], m_DC_Patrol.groupCount[1]);
		
		for (int i = 0; i < groupCount; i++)
		{
			SCR_AIGroup group = SCR_DC_MissionHelper.SpawnMissionAIGroup(m_DC_Patrol.groupTypes.GetRandomElement(), GetPos());
			if (group)
			{
				m_Groups.Insert(group);
				vector posFrom = "2776 0 1623";
				vector posTo = "3165 0 2800";
				SCR_DC_WPHelper.CreateMissionAIWaypoints(group, DC_EWaypointGenerationType.ROUTE, posFrom, posTo);
//				SCR_DC_WPHelper.CreateMissionAIWaypoints(group, m_DC_Patrol.waypointMoveType, posFrom, posTo);
//				SCR_DC_WPHelper.CreateMissionAIWaypoints(group, "0 0 0", "0 0 0", m_DC_Patrol.waypointRange[0], m_DC_Patrol.waypointRange[1], m_DC_Patrol.waypointMoveType, m_DC_Patrol.waypointType);
			}
			SCR_DC_Log.Add("[SCR_DC_Mission_Patrol:MissionSpawn] AI groups spawned: " + groupCount, LogLevel.DEBUG);								
		}
			
		SetState(DC_MissionState.ACTIVE);
	}
}

//------------------------------------------------------------------------------------------------
class SCR_DC_PatrolConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	int missionLifeCycleTime;						//How often the mission is run
	bool showMarker;
	
	//Mission specific	
	ref array<ref int> patrolList = {};				//Which patrols to use. If first one is -1, any random one will be chosen from occupations. A single value will work as index.
	ref array<ref SCR_DC_Patrol> patrols = {};		//List of patrols
	int patrolingTime;								//Time to patrol, is seconds
	int distanceToPlayer;							//If no players this close to any players and patrolingTime has passed, despawn mission.
}

//------------------------------------------------------------------------------------------------
class SCR_DC_Patrol : Managed
{
	//Patrol specific
	string comment;							//Generic comment to describe the mission. Not used in game.
	vector locationPos;						//Position for mission. "0 0 0" used for random location chosen from locationTypes.
	string locationName;					//Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes 
	string title;							//Title for the hint shown for players
	string info;							//Details for the hint shown for players
	ref array<EMapDescriptorType> locationTypes = {};
	ref array<int> groupCount = {};			//min, max	
	ref array<int> waypointRange = {};		//min, max
	DC_EWaypointGenerationType waypointType;
	DC_EWaypointMoveType waypointMoveType;
	ref array<string> groupTypes = {};	
	
	void Set(string comment_, vector locationPos_, string locationName_, string title_, string info_, array<EMapDescriptorType> locationTypes_, array<int> groupCount_, array<int> waypointRange_, DC_EWaypointGenerationType waypointType_, DC_EWaypointMoveType _waypointMoveType, array<string> groupTypes_)
	{
		comment = comment_;
		locationPos = locationPos_;
		locationName = locationName_;
		title = title_;
		info = info_;
		locationTypes = locationTypes_;
		groupCount = groupCount_;
		waypointRange = waypointRange_;
		waypointType = waypointType_;
		waypointMoveType = _waypointMoveType;
		groupTypes = groupTypes_;
	}
}		

//------------------------------------------------------------------------------------------------
class SCR_DC_PatrolJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Patrol.json";
	ref SCR_DC_PatrolConfig conf = new SCR_DC_PatrolConfig;
	
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
		array<string> lootItems = {};
		
		//Default
		conf.missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_DEFAULT * 3;
		conf.showMarker = true;
		//Mission specific
		conf.patrolList = {0};//}{0,0,0,1,1,1,2};		//Set -1 in the first entry to get a random occupation. Single number will be used as index.
		conf.distanceToPlayer = 500;

		//----------------------------------------------------
		SCR_DC_Patrol patrol0 = new SCR_DC_Patrol;
		patrol0.Set
		(
			"USSR patrols",
			"0 0 0",
			"any",
			"Patrol in ",
			"Beware",
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL
			},
			{1, 1},
			{300, 700},
			DC_EWaypointGenerationType.RADIUS,//.RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"{4C44B4D8F2820F25}Prefabs/Groups/OPFOR/Spetsnaz/Group_USSR_Spetsnaz_SentryTeam.et",
				"{8EDE6E160E71ABB4}Prefabs/Groups/OPFOR/KLMK/Group_USSR_SapperTeam_KLMK.et",
				"{8E29E7581DE832CC}Prefabs/Groups/OPFOR/KLMK/Group_USSR_MedicalSection_KLMK.et"
			}
		);
		conf.patrols.Insert(patrol0);
		
		//----------------------------------------------------
		SCR_DC_Patrol patrol1 = new SCR_DC_Patrol;
		patrol1.Set(
			"US patrols",
			"0 0 0",
			"any",
			"Patrol in ",
			"Beware",
			{
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_FORTRESS
			},
			{1, 1},
			{300, 1000},
			DC_EWaypointGenerationType.RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"{4C44B4D8F2820F25}Prefabs/Groups/OPFOR/Spetsnaz/Group_USSR_Spetsnaz_SentryTeam.et",
				"{4D3BBEC1A955626A}Prefabs/Groups/OPFOR/Spetsnaz/Group_USSR_Spetsnaz_Squad.et",
				"{8EDE6E160E71ABB4}Prefabs/Groups/OPFOR/KLMK/Group_USSR_SapperTeam_KLMK.et",
				"{8E29E7581DE832CC}Prefabs/Groups/OPFOR/KLMK/Group_USSR_MedicalSection_KLMK.et"				
			}
		);
		conf.patrols.Insert(patrol1);
	}
}