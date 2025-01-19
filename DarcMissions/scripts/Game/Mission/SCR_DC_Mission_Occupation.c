//Mission SCR_DC_Mission_Occupation.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns groups to defend a location
*/

//------------------------------------------------------------------------------------------------

class SCR_DC_Mission_Occupation : SCR_DC_Mission
{
	private ref SCR_DC_MissionConfigOccupation m_Config = new SCR_DC_MissionConfigOccupation();	
	
	protected ref SCR_DC_Occupation m_DC_Occupation;	//Occupation configuration in use
	protected ref array<IEntity> m_EntityList = {};		//Entities (e.g., tents) spawned
	protected ref array<SCR_AIGroup> m_Groups = {};		//Groups spawned

	private const int DC_LOCATION_SEACRH_ITERATIONS = 5;	//How many different spots to try for a mission before giving up
	
	private int m_SpawnIndex = 0;

	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission_Occupation()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Occupation] Constructor", LogLevel.DEBUG);
				
		//Set some defaults
		SCR_DC_Mission();

		//Load config
		m_Config.Load();
		
		if (m_Config.occupations.Count() == 0)
		{
			SCR_DC_Log.Add("[SCR_DC_Mission_Occupation] No occupations defined.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;
		}
		
		//Pick a configuration for mission
		m_DC_Occupation = m_Config.occupations.GetRandomElement();

		//Set defaults
		vector pos = m_DC_Occupation.locationPos;
		string posName = m_DC_Occupation.locationName;
		bool positionFound = false;
		
		//Find a location for the mission
		if (pos == "0 0 0")
		{
			//Find a random location
			IEntity location = null;
			array<IEntity> locations = {};
			SCR_DC_Locations.GetLocations(locations, m_DC_Occupation.locationTypes);
			
			if (locations.Count() > 0)
			{
				for (int i = 0; i < DC_LOCATION_SEACRH_ITERATIONS; i++)
				{
					location = locations.GetRandomElement();
					pos = location.GetOrigin();
					
					if (SCR_DC_MissionHelper.IsValidMissionPos(pos))
					{				
						//Find an empty position at mission pos
						pos = SCR_DC_SpawnHelper.FindEmptyPos(pos, 200, 10);
						if (posName == "any")
						{
							posName = SCR_StringHelper.Translate(location.GetName());
						}
						positionFound = true;
					
						SCR_DC_Log.Add("[SCR_DC_Mission_Occupation] Location for spawn " + SCR_StringHelper.Translate(location.GetName()) + " " + location.GetOrigin(), LogLevel.DEBUG);
						break;
					}
					else
					{						
						SCR_DC_Log.Add("[SCR_DC_Mission_Occupation] Invalid mission position. Try " + (i + 1) + "/" + DC_LOCATION_SEACRH_ITERATIONS, LogLevel.SPAM);
					}
				}
			}
		}
		else
		{
			//Use a predefined location
			SCR_DC_Log.Add("[SCR_DC_Mission_Occupation] Predefined locations are not supported. Yet.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;			
		}
		
		if (positionFound)
		{	
			SetTitle(m_DC_Occupation.title + "" + posName);
			SetInfo(m_DC_Occupation.info);
			SetPos(pos);
			SetPosName(posName);
			SetMarkerId(SCR_DC_MapMarkersUI.AddMarker(DC_ID_PREFIX, GetPos(), GetTitle()));
	
			SetState(DC_MissionState.INIT);			
		}
		else
		{				
			//No suitable location found.
			SCR_DC_Log.Add("[SCR_DC_Mission_Occupation] Could not find suitable location.", LogLevel.ERROR);
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
				SCR_DC_Log.Add("[SCR_DC_Mission_Occupation:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
				SetState(DC_MissionState.END);
			}
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionLifeCycleTime);
	}

	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		//Remove spawned items
		foreach(IEntity entity : m_EntityList)
		{
			if (entity)
			{
				SCR_DC_Log.Add("[SCR_DC_Mission_Occupation:MissionEnd] Despawning: " + entity.GetPrefabData().GetPrefabName(), LogLevel.DEBUG);
				SCR_DC_SpawnHelper.DespawnItem(entity);
			}
		}
		//Remove marker from map
		SCR_DC_MapMarkersUI.DeleteMarker(GetMarkerId());
		//TBD: Delete all AI groups. This case is needed in case the mission is stopped by an external force (e.g., admin).		
		SCR_DC_Log.Add("[SCR_DC_Mission_Occupation:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);		
	}
			
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		IEntity entity;

		//Spawn entities one by one. Sets missions active once ready.		
		if (m_SpawnIndex < m_DC_Occupation.entityTypes.Count())
		{			
			entity = SCR_DC_SpawnHelper.SpawnItem(GetPos(), m_DC_Occupation.entityTypes[m_SpawnIndex]);
			
			if (entity != NULL)
			{ 
				m_EntityList.Insert(entity);
			}
			else
			{
				SCR_DC_Log.Add("[SCR_DC_Mission_Occupation:MissionSpawn] Could not load: " + m_DC_Occupation.entityTypes[m_SpawnIndex], LogLevel.ERROR);				
			}
			
			m_SpawnIndex++;			
		}
		else
		{
			//Spawn AI 
			int groupCount = Math.RandomInt(m_DC_Occupation.groupCount[0], m_DC_Occupation.groupCount[1]);
			
			for (int i = 0; i < groupCount; i++)
			{
				SpawnAI();
			}
			
			SetState(DC_MissionState.ACTIVE);
		}		
	}

	//------------------------------------------------------------------------------------------------
	private void SpawnAI()
	{
		//TBD: Support for multiple groups with groupCount
		string groupToSpawn = m_DC_Occupation.groupTypes.GetRandomElement();		
		vector posFixed = SCR_DC_SpawnHelper.FindEmptyPos(GetPos(), 100, 8);
		
		SCR_AIGroup group = SCR_DC_AIHelper.SpawnGroup(groupToSpawn, posFixed);
		
		if (group)
		{
			m_Groups.Insert(group);
			CreateWaypoints(group);
		}
	}			

	//------------------------------------------------------------------------------------------------
	//Create waypoint for AI m_Groups
	//This will randomize the type, speed and count of waypoints to create.
	
	private void CreateWaypoints(SCR_AIGroup group)
	{		
		if (group)
		{
			int rndCount = Math.RandomInt(4, 11);
			int rndRange = Math.RandomInt(m_DC_Occupation.waypointRange[0], m_DC_Occupation.waypointRange[1]);
			
			//Select the waypoint formation
			DC_EWaypointRndType waypointRndType = m_DC_Occupation.waypointType;
			if (waypointRndType == DC_EWaypointRndType.RANDOM)
			{
				array<DC_EWaypointRndType> waypointRndTypeArray = {DC_EWaypointRndType.SCATTERED, DC_EWaypointRndType.RADIUS, DC_EWaypointRndType.RADIUS}; //DC_EWaypointRndType.SLOTS
				waypointRndType = waypointRndTypeArray.GetRandomElement();
			}

			//Select the waypoint movement type
			DC_EWaypointMoveType waypointMoveType = m_DC_Occupation.waypointMoveType;
			if (waypointMoveType == DC_EWaypointMoveType.RANDOM)
			{
				array<DC_EWaypointMoveType> waypointMoveTypeArray = {DC_EWaypointMoveType.MOVECYCLE, DC_EWaypointMoveType.PATROLCYCLE, DC_EWaypointMoveType.PATROLCYCLE, DC_EWaypointMoveType.PATROLCYCLE};
				waypointMoveType = waypointMoveTypeArray.GetRandomElement();
			}
			
			SCR_DC_Log.Add("[SCR_DC_Mission_Occupation:CreateWaypoints] Adding " + rndCount + " waypoints (" + SCR_Enum.GetEnumName(DC_EWaypointRndType, waypointRndType) + ", " + SCR_Enum.GetEnumName(DC_EWaypointMoveType, waypointMoveType) + ")", LogLevel.DEBUG);

			SCR_DC_AIHelper.CreateWaypoints(group, rndCount, waypointMoveType, waypointRndType, rndRange, true);
		}
	}
}

//------------------------------------------------------------------------------------------------
class SCR_DC_Occupation : Managed
{
	//Mission specific
	vector locationPos;
	string locationName;
	string title;
	string info;
	ref array<EMapDescriptorType> locationTypes;
	ref array<int> groupCount = {};		//min, max
	ref array<int> waypointRange = {};	//min, max
	DC_EWaypointRndType waypointType;
	DC_EWaypointMoveType waypointMoveType;
	ref array<string> groupTypes = {};	
	ref array<string> entityTypes = {};
	
	void Set(vector locationPos_, string locationName_, string title_, string info_, array<EMapDescriptorType> locationTypes_, array<int> groupCount_, array<int> waypointRange_, DC_EWaypointRndType waypointType_, DC_EWaypointMoveType _waypointMoveType, array<string> groupTypes_, array<string> entityTypes_)
	{
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
		entityTypes = entityTypes_;
	}
}

//------------------------------------------------------------------------------------------------
class SCR_DC_MissionConfigOccupation : SCR_DC_JsonConfig
{
	//Information stored in .json
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Occupation.json";
	int missionLifeCycleTime;			//How often the mission is run
	ref array<ref SCR_DC_Occupation> occupations = {};
	//----
		
	//------------------------------------------------------------------------------------------------
	void SCR_DC_MissionConfigOccupation()
	{
		version = 1;
		author = "darc";
	}
	
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

		loadContext.ReadValue("missionLifeCycleTime", missionLifeCycleTime);
		loadContext.ReadValue("occupations", occupations);
	}	
	
	//------------------------------------------------------------------------------------------------
	void Save(string data)
	{
		SCR_JsonSaveContext saveContext = SaveConfigOpen(DC_MISSIONCONFIG_FILE);
		saveContext.WriteValue("missionLifeCycleTime", missionLifeCycleTime);
		saveContext.WriteValue("occupations", occupations);
		SaveConfigClose(saveContext);
	}	
		
	//------------------------------------------------------------------------------------------------
	void SetDefaults()
	{
		SCR_DC_Occupation occupation = new SCR_DC_Occupation;
		//Mission specific
		missionLifeCycleTime = MISSION_LIFECYCLE_TIME_LIMIT;

		occupation.Set
		(
			"0 0 0",
			"any",
			"Bandit camp at ",
			"Bandits are protecting their valuable loot.",
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_FORTRESS
			},
			{1, 2},
			{50, 200},
			DC_EWaypointRndType.RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"{4D3BBEC1A955626A}Prefabs/Groups/OPFOR/Spetsnaz/Group_USSR_Spetsnaz_Squad.et",
			},
			{				
				"{76153B97A551AADE}Prefabs/Props/Military/Arsenal/AmmoBoxes/USSR/AmmoBoxArsenal_Explosives_USSR.et",
				"{B53350B377FE1864}Prefabs/Structures/Military/Camps/Canvas_Covers/CanvasCover_Small_A_Base.et",
			}
		);
		occupations.Insert(occupation);
	
		SCR_DC_Occupation occupation2 = new SCR_DC_Occupation;
		occupation2.Set
		(
			"0 0 0",
			"any",
			"Occupation at ",
			"City is being occupied.",
			{
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_NAME_TOWN, 
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_AIRPORT,
			},
			{2, 4},
			{50, 300},
			DC_EWaypointRndType.RADIUS,
			DC_EWaypointMoveType.RANDOM,		
			{
				"{30ED11AA4F0D41E5}Prefabs/Groups/OPFOR/Group_USSR_FireGroup.et",
				"{657590C1EC9E27D3}Prefabs/Groups/OPFOR/Group_USSR_LightFireTeam.et",
				"{96BAB56E6558788E}Prefabs/Groups/OPFOR/Group_USSR_Team_AT.et",
				"{43C7A28EEB660FF8}Prefabs/Groups/OPFOR/Group_USSR_Team_GL.et",
				"{1C0502B5729E7231}Prefabs/Groups/OPFOR/Group_USSR_Team_Suppress.et"
			},
			{				
				"{76153B97A551AADE}Prefabs/Props/Military/Arsenal/AmmoBoxes/USSR/AmmoBoxArsenal_Explosives_USSR.et",
				"{25C8B889A777399D}Prefabs/Props/Infrastructure/ConeTraffic/ConeTraffic_01_red.et",
				"{C8C36ADC9DFC5FBB}Prefabs/Props/Crates/Crate_01_red.et",
				"{C8C36ADC9DFC5FBB}Prefabs/Props/Crates/Crate_01_red.et",
				"{B53350B377FE1864}Prefabs/Structures/Military/Camps/Canvas_Covers/CanvasCover_Small_A_Base.et",
				"{C012BB3488BEA0C2}Prefabs/Vehicles/Wheeled/BTR70/BTR70.et",
			}
		);
		occupations.Insert(occupation2);
	}	
}


/*
	private static const ref array<string> m_SpawnEntities = 
	{
//		"{36D873BC68DC0215}Prefabs/Props/Military/MilitaryCrates/CrateStack_01/Dst/CrateStack_01_Dst_3_NoDecal.et",
		"{A9CF5E8C5D91C476}Prefabs/Compositions/Misc/SubCompositions/Decorations/Barrel_Group_Crate_01.et",
		"{25C8B889A777399D}Prefabs/Props/Infrastructure/ConeTraffic/ConeTraffic_01_red.et",
		"{C8C36ADC9DFC5FBB}Prefabs/Props/Crates/Crate_01_red.et",
		"{C8C36ADC9DFC5FBB}Prefabs/Props/Crates/Crate_01_red.et",
		"{B53350B377FE1864}Prefabs/Structures/Military/Camps/Canvas_Covers/CanvasCover_Small_A_Base.et",
		"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
		"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
		"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
		"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
//		"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
//		"{43C4AF1EEBD001CE}Prefabs/Vehicles/Wheeled/UAZ452/UAZ452_ambulance.et",
	};

*/