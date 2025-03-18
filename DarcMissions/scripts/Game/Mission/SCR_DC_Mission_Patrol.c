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
	
	protected ref SCR_DC_Patrol m_DC_Patrol;		//Patrol configuration in use
	
	private vector m_PosDestination = "0 0 0";

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
		
		//Pick a configuration for mission
		int idx = SCR_DC_MissionHelper.SelectMissionIndex(m_Config.patrolList);
		if(idx == -1)
		{
			SCR_DC_Log.Add("[SCR_DC_Mission_Convoy] No occupations defined.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;
		}
		m_DC_Patrol = m_Config.patrols[idx];

		//Set defaults
		vector pos = m_DC_Patrol.posStart;
		m_PosDestination = m_DC_Patrol.posDestination;
		string posName = m_DC_Patrol.locationName;
		IEntity location = null;
		IEntity locationDestination = null;
		
		//Find a location for the mission
		if (pos == "0 0 0")
		{
			location = SCR_DC_MissionHelper.FindMissionLocation(m_DC_Patrol.locationTypes);
			if(location)
			{			
				pos = location.GetOrigin();
			}
			else
			{
				pos = "0 0 0";				
			}
		}

		//Find a location for the destination
		if (m_PosDestination == "0 0 0" && pos != "0 0 0")
		{
			locationDestination = SCR_DC_MissionHelper.FindMissionLocation(m_DC_Patrol.locationTypes);
			if(location)
			{
				m_PosDestination = locationDestination.GetOrigin();
				SCR_DC_Log.Add("[SCR_DC_Mission_Patrol] Patrol destination: " + locationDestination.GetName(), LogLevel.DEBUG);
			}
			else
			{
				SCR_DC_Log.Add("[SCR_DC_Mission_Patrol] Could not find destination location for ROUTE.", LogLevel.WARNING);
			}
		}

		//If all is ok, let's finalize the mission creation				
		if (pos != "0 0 0" && m_PosDestination != "0 0 0")
		{	
			SetPos(pos);
			SetPosName(SCR_DC_Locations.CreateName(location, posName));
			SetTitle(m_DC_Patrol.title + "" + GetPosName());
			SetInfo(m_DC_Patrol.info);			
			SetMarker(m_Config.showMarker, DC_EMissionIcon.MISSION);
			SetShowHint(m_Config.showHint);			
			SetActiveDistance(m_Config.distanceToPlayer);				//Change the m_ActiveDistance to a mission specific one.

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
			//Move the position as the first patrol is moving. This way check for player distance works properly.
			if(m_Groups[0])
			{
				SetPos(m_Groups[0].GetOrigin());
				SCR_DC_DebugHelper.MoveDebugPos(GetId(), GetPos());
			}
			
			if (SCR_DC_AIHelper.AreAllGroupsDead(m_Groups))
			{
				if (!IsActive())
				{
					SCR_DC_Log.Add("[SCR_DC_Mission_Patrol:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
					SetState(DC_MissionState.END);
				}
			}
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionCycleTime*1000);
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
				SCR_DC_AIHelper.SetAIGroupSkill(group, m_DC_Patrol.AISkill, m_DC_Patrol.AIperception);					
				m_Groups.Insert(group);
				if (m_DC_Patrol.waypointGenType == DC_EWaypointGenerationType.ROUTE)
				{
					SCR_DC_WPHelper.CreateMissionAIWaypoints(group, m_DC_Patrol.waypointGenType, GetPos(), m_PosDestination, m_DC_Patrol.waypointMoveType);
				}
				else
				{
					SCR_DC_WPHelper.CreateMissionAIWaypoints(group, m_DC_Patrol.waypointGenType, "0 0 0", "0 0 0", m_DC_Patrol.waypointMoveType, m_DC_Patrol.waypointRange[0], m_DC_Patrol.waypointRange[1]);
				}
			}
			SCR_DC_Log.Add("[SCR_DC_Mission_Patrol:MissionSpawn] AI groups spawned: " + groupCount, LogLevel.DEBUG);								
		}
			
		SetState(DC_MissionState.ACTIVE);
	}
}

//------------------------------------------------------------------------------------------------
class SCR_DC_PatrolConfig : SCR_DC_MissionConfig
{
	//Mission specific	
	int patrolingTime;								//Time to patrol, in seconds
	int distanceToPlayer;							//If no players this close to any players and patrolingTime has passed, despawn mission.
	ref array<ref int> patrolList = {};				//The indexes of patrols.
	ref array<ref SCR_DC_Patrol> patrols = {};		//List of patrols
}

//------------------------------------------------------------------------------------------------
class SCR_DC_Patrol : Managed
{
	//Patrol specific
	string comment;							//Generic comment to describe the mission. Not used in game.
	vector posStart;						//Position for mission. "0 0 0" used for random location chosen from locationTypes.
	vector posDestination;					//Destination for the patrol to go to
	string locationName;					//Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes 
	string title;							//Title for the hint shown for players
	string info;							//Details for the hint shown for players
	ref array<EMapDescriptorType> locationTypes = {};
	ref array<int> groupCount = {};			//min, max	
	ref array<int> waypointRange = {};		//min, max
	DC_EWaypointGenerationType waypointGenType;
	DC_EWaypointMoveType waypointMoveType;
	ref array<string> groupTypes = {};	
	int AISkill;
	float AIperception	
	
	void Set(string comment_, vector posStart_, vector posDestination_, string locationName_, string title_, string info_, array<EMapDescriptorType> locationTypes_, array<int> groupCount_, array<int> waypointRange_, DC_EWaypointGenerationType waypointGenType_, DC_EWaypointMoveType waypointMoveType_, array<string> groupTypes_, int AISkill_, float AIperception_)
	{
		comment = comment_;
		posStart = posStart_;
		posDestination = posDestination_;
		locationName = locationName_;
		title = title_;
		info = info_;
		locationTypes = locationTypes_;
		groupCount = groupCount_;
		waypointRange = waypointRange_;
		waypointGenType = waypointGenType_;
		waypointMoveType = waypointMoveType_;
		groupTypes = groupTypes_;
		AISkill = AISkill_;
		AIperception = AIperception_;				
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
		conf.missionCycleTime = DC_MISSION_CYCLE_TIME_DEFAULT * 3;
		conf.showMarker = true;
		//Mission specific
		conf.patrolList = {0};//,1,2};
		conf.distanceToPlayer = 500;

		//----------------------------------------------------
		SCR_DC_Patrol patrol0 = new SCR_DC_Patrol;
		patrol0.Set
		(
			"USSR patrols going between two points hopefully following roads",
			"0 0 0",
			"0 0 0",
			"any",
			"Patrol seen close to ",
			"Beware",
			{
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_VALLEY,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_RIDGE
			},
			{1, 1},
			{0, 0},	//Not used with ROUTE
			DC_EWaypointGenerationType.ROUTE,
			DC_EWaypointMoveType.MOVE,
			{
				"{4C44B4D8F2820F25}Prefabs/Groups/OPFOR/Spetsnaz/Group_USSR_Spetsnaz_SentryTeam.et",
				"{8EDE6E160E71ABB4}Prefabs/Groups/OPFOR/KLMK/Group_USSR_SapperTeam_KLMK.et",
				"{8E29E7581DE832CC}Prefabs/Groups/OPFOR/KLMK/Group_USSR_MedicalSection_KLMK.et"
			},
			50, 1.0
		);
		conf.patrols.Insert(patrol0);
		
		//----------------------------------------------------
		SCR_DC_Patrol patrol1 = new SCR_DC_Patrol;
		patrol1.Set(
			"USSR patrols",
			"0 0 0",
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
			DC_EWaypointGenerationType.RADIUS,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"{4C44B4D8F2820F25}Prefabs/Groups/OPFOR/Spetsnaz/Group_USSR_Spetsnaz_SentryTeam.et",
				"{4D3BBEC1A955626A}Prefabs/Groups/OPFOR/Spetsnaz/Group_USSR_Spetsnaz_Squad.et",
				"{8EDE6E160E71ABB4}Prefabs/Groups/OPFOR/KLMK/Group_USSR_SapperTeam_KLMK.et",
				"{8E29E7581DE832CC}Prefabs/Groups/OPFOR/KLMK/Group_USSR_MedicalSection_KLMK.et"				
			},
			50, 1.0
		);
		conf.patrols.Insert(patrol1);

		//----------------------------------------------------
		SCR_DC_Patrol patrol2 = new SCR_DC_Patrol;
		patrol2.Set
		(
			"USSR patrols",
			"0 0 0",
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
			DC_EWaypointGenerationType.RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"{4C44B4D8F2820F25}Prefabs/Groups/OPFOR/Spetsnaz/Group_USSR_Spetsnaz_SentryTeam.et",
				"{8EDE6E160E71ABB4}Prefabs/Groups/OPFOR/KLMK/Group_USSR_SapperTeam_KLMK.et",
				"{8E29E7581DE832CC}Prefabs/Groups/OPFOR/KLMK/Group_USSR_MedicalSection_KLMK.et"
			},
			50, 1.0
		);
		conf.patrols.Insert(patrol2);				
	}
}