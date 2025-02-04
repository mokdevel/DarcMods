//Mission SCR_DC_Mission_Occupation.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns groups to defend a location
*/

//------------------------------------------------------------------------------------------------

class SCR_DC_Mission_Occupation : SCR_DC_Mission
{
	private ref SCR_DC_OccupationJsonApi m_OccupationJsonApi = new SCR_DC_OccupationJsonApi;	
	private ref SCR_DC_OccupationConfig m_Config;
	
	protected ref SCR_DC_Occupation m_DC_Occupation;	//Occupation configuration in use
	protected ref array<IEntity> m_EntityList = {};		//Entities (e.g., tents) spawned
	protected ref array<SCR_AIGroup> m_Groups = {};		//Groups spawned
	
	private int m_SpawnIndex = 0;						//Counter for the item to spawn
	private float m_SpawnRotation = 0;					//Rotation of the camp for random locations.

	//------------------------------------------------------------------------------------------------
	void SCR_DC_Mission_Occupation()
	{
		SCR_DC_Log.Add("[SCR_DC_Mission_Occupation] Constructor", LogLevel.DEBUG);
				
		//Set some defaults
		SCR_DC_Mission();

		//Load config
		m_OccupationJsonApi.Load();
		m_Config = m_OccupationJsonApi.conf;
		
		if (m_Config.occupations.Count() == 0)
		{
			SCR_DC_Log.Add("[SCR_DC_Mission_Occupation] No occupations defined.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;
		}
		
		//Pick a configuration for mission
		if(m_Config.occupationList.Count() == 1)
		{
			//-1 = Pick any random value
			if(m_Config.occupationList[0] == -1)
			{
				m_DC_Occupation = m_Config.occupations.GetRandomElement();
			}
			else
			{
				//Single number = Use it as a index
				m_DC_Occupation = m_Config.occupations[m_Config.occupationList[0]];
			}
		}
		else
		{
			int occupationIdx = m_Config.occupationList.GetRandomElement();
			m_DC_Occupation = m_Config.occupations[occupationIdx];
		}

		//Set defaults
		vector pos = m_DC_Occupation.locationPos;
		string posName = m_DC_Occupation.locationName;
		IEntity location = null;
		
		//Find a location for the mission
		if (pos == "0 0 0")
		{
			location = SCR_DC_MissionHelper.FindMissionLocation(m_DC_Occupation.locationTypes);
			//Camps in random places are randomly rotated
			m_SpawnRotation = Math.RandomFloat(0, 360);
		}
		else
		{
			//Use a predefined location
			SCR_DC_Log.Add("[SCR_DC_Mission_Occupation] Predefined locations are not supported. Yet.", LogLevel.ERROR);
			SetState(DC_MissionState.EXIT);
			return;			
		}
		
		if (location)
		{	
			if (posName == "any")
			{
				posName = SCR_StringHelper.Translate(location.GetName());
			}			
			SetTitle(m_DC_Occupation.title + "" + posName);
			SetInfo(m_DC_Occupation.info);			
			SetPos(location.GetOrigin());
			SetPosName(posName);
			SetMarkerId(SCR_DC_MapMarkersUI.AddMarker(DC_ID_PREFIX, GetPos(), GetTitle()));

			SCR_DC_SpawnHelper.SetStructuresToOrigo(m_DC_Occupation.campItems);
				
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
				if (!IsActive())
				{
					SCR_DC_Log.Add("[SCR_DC_Mission_Occupation:MissionRun] All groups killed. Mission has ended.", LogLevel.NORMAL);
					SetState(DC_MissionState.END);
				}
			}
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionLifeCycleTime*1000);
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
		
		//Remove AI
		SCR_DC_Log.Add("[SCR_DC_Mission_Occupation:MissionEnd] Deleting AI groups", LogLevel.DEBUG);
		foreach(SCR_AIGroup group : m_Groups)
		{
			SCR_DC_AIHelper.GroupDelete(group);			
		}
		
		SCR_DC_Log.Add("[SCR_DC_Mission_Occupation:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);		
	}
			
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		IEntity entity;

		//Spawn entities one by one. Sets missions active once ready.		
		if ( (m_SpawnIndex < m_DC_Occupation.campItems.Count()) && (m_DC_Occupation.campItems.Count() > 0) )
		{			
			entity = SCR_DC_SpawnHelper.SpawnStructures(m_DC_Occupation.campItems, GetPos(), m_SpawnRotation, m_SpawnIndex);
			
			if (entity != NULL)
			{ 
				m_EntityList.Insert(entity);
			}
			else
			{
				SCR_DC_Log.Add("[SCR_DC_Mission_Occupation:MissionSpawn] Could not load: " + m_DC_Occupation.campItems[m_SpawnIndex], LogLevel.ERROR);				
			}
			
			m_SpawnIndex++;			
		}
		else
		{
			//Spawn mission AI 
			int groupCount = Math.RandomInt(m_DC_Occupation.groupCount[0], m_DC_Occupation.groupCount[1]);
			
			for (int i = 0; i < groupCount; i++)
			{
				SCR_AIGroup group = SCR_DC_MissionHelper.SpawnMissionAIGroup(m_DC_Occupation.groupTypes.GetRandomElement(), GetPos());
				if (group)
				{
					m_Groups.Insert(group);
					SCR_DC_MissionHelper.CreateMissionAIWaypoints(group, m_DC_Occupation.waypointRange[0], m_DC_Occupation.waypointRange[1], m_DC_Occupation.waypointMoveType, m_DC_Occupation.waypointType);
				}
				SCR_DC_Log.Add("[SCR_DC_Mission_Occupation:MissionSpawn] AI groups spawned: " + groupCount, LogLevel.DEBUG);								
			}
			
			//Put loot
			if (m_EntityList.Count() > 0)
			{
				IEntity entityLoot = m_EntityList[0];
				SCR_DC_SpawnHelper.SpawnItemsToStorage(entityLoot, m_DC_Occupation.itemNames, m_DC_Occupation.itemChance);			
				SCR_DC_Log.Add("[SCR_DC_Mission_Occupation:MissionSpawn] Loot added.", LogLevel.DEBUG);								
			}
			
			SetState(DC_MissionState.ACTIVE);
		}		
	}
}

//------------------------------------------------------------------------------------------------
class SCR_DC_OccupationConfig : Managed
{
	//Default information
	int version = 1;
	string author = "darc";
	int missionLifeCycleTime = DC_MISSION_LIFECYCLE_TIME_DEFAULT;	//How often the mission is run
	
	//Mission specific	
	ref array<ref int> occupationList = {};							//Which occupations to use. If first one is -1, any random one will be chosen from occupations. A single value will work as index.
	ref array<ref SCR_DC_Occupation> occupations = {};				//List of occupations
}

//------------------------------------------------------------------------------------------------
class SCR_DC_Occupation : Managed
{
	//Mission specific
	string comment;							//Generic comment to describe the mission. Not used in game.
	vector locationPos;						//Position for mission. "0 0 0" used for random location chosen from locationTypes.
	string locationName;					//Name for the mission location (like "bandit camp"). "any" uses location name found from locationTypes 
	string title;
	string info;
	ref array<EMapDescriptorType> locationTypes = {};
	ref array<int> groupCount = {};			//min, max
	ref array<int> waypointRange = {};		//min, max
	DC_EWaypointRndType waypointType;
	DC_EWaypointMoveType waypointMoveType;
	ref array<string> groupTypes = {};
	ref array<string> itemNames = {};		//Items to add to each spawnNames
	float itemChance;						//The chance to spawn and item from itemNames. 0.5 = 50% chance
	
	ref array<ref SCR_DC_Structure> campItems = {};
	
	void Set(string comment_, vector locationPos_, string locationName_, string title_, string info_, array<EMapDescriptorType> locationTypes_, array<int> groupCount_, array<int> waypointRange_, DC_EWaypointRndType waypointType_, DC_EWaypointMoveType _waypointMoveType, array<string> groupTypes_,  array<string> itemNames_, float itemChance_)
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
		itemNames = itemNames_;
		itemChance = itemChance_;
	}
}		

//------------------------------------------------------------------------------------------------
class SCR_DC_OccupationJsonApi : SCR_DC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Occupation.json";
	ref SCR_DC_OccupationConfig conf = new SCR_DC_OccupationConfig;
	
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
		conf.occupationList = {0,0,0,1,1,1,2};		//Set -1 in the first entry to get a random occupation. Single number will be used as index.

		//----------------------------------------------------
		SCR_DC_Occupation occupation0 = new SCR_DC_Occupation;
		occupation0.Set
		(
			"Gogland: Mission to be used with Escapists",
			"0 0 0",
			"any",
			"Patrol in ",
			"Avoid the location",
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL
			},
			{1, 2},
			{50, 300},
			DC_EWaypointRndType.RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"{4C44B4D8F2820F25}Prefabs/Groups/OPFOR/Spetsnaz/Group_USSR_Spetsnaz_SentryTeam.et",
				"{8EDE6E160E71ABB4}Prefabs/Groups/OPFOR/KLMK/Group_USSR_SapperTeam_KLMK.et",
				"{8E29E7581DE832CC}Prefabs/Groups/OPFOR/KLMK/Group_USSR_MedicalSection_KLMK.et"
			},
			{},
			1.0		
		);
		conf.occupations.Insert(occupation0);
		
		//----------------------------------------------------
		SCR_DC_Occupation occupation1 = new SCR_DC_Occupation;
		occupation1.Set
		(
			"Bandit camp spawning to non city areas",
			"0 0 0",
			"any",
			"Bandit camp at ",
			"Bandits are protecting their valuable loot.",
			{
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_PORT,
				EMapDescriptorType.MDT_AIRPORT,
				EMapDescriptorType.MDT_FORTRESS
			},
			{1, 2},
			{10, 100},
			DC_EWaypointRndType.RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"{4C44B4D8F2820F25}Prefabs/Groups/OPFOR/Spetsnaz/Group_USSR_Spetsnaz_SentryTeam.et",
				"{4D3BBEC1A955626A}Prefabs/Groups/OPFOR/Spetsnaz/Group_USSR_Spetsnaz_Squad.et",
				"{8EDE6E160E71ABB4}Prefabs/Groups/OPFOR/KLMK/Group_USSR_SapperTeam_KLMK.et",
				"{8E29E7581DE832CC}Prefabs/Groups/OPFOR/KLMK/Group_USSR_MedicalSection_KLMK.et"				
			},
			{
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			},
			0.9		
		);
		conf.occupations.Insert(occupation1);
		
		SCR_DC_Structure ocu1item0 = new SCR_DC_Structure;
		ocu1item0.Set
		(
			"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
			"1032.038 39 2478.923",
			"0 0 0"
		);
		occupation1.campItems.Insert(ocu1item0);
		SCR_DC_Structure ocu1item1 = new SCR_DC_Structure;
		ocu1item1.Set
		(
			"{39C308BBB5945B85}Prefabs/Props/Military/Furniture/ChairMilitary_US_02.et",
			"1028.55 39 2478.16",
			"0 119.334 0"
		);
		occupation1.campItems.Insert(ocu1item1);
		SCR_DC_Structure ocu1item2 = new SCR_DC_Structure;
		ocu1item2.Set
		(
			"{D9842C11742C00CF}Prefabs/Props/Civilian/Fireplace_01.et",
			"1029.9 39 2477.44",
			"0 0 0"
		);
		occupation1.campItems.Insert(ocu1item2);					
		SCR_DC_Structure ocu1item3 = new SCR_DC_Structure;
		ocu1item3.Set
		(
			"{0511E95F422061BB}Prefabs/Props/Recreation/Camp/TentSmall_02/TentSmall_02_blue.et",
			"1029.974 39 2480.114",
			"0 0 0"
		);
		occupation1.campItems.Insert(ocu1item3);
		
		//----------------------------------------------------
		SCR_DC_Occupation occupation2 = new SCR_DC_Occupation;
		occupation2.Set
		(
			"Occupation that will spawn mainly to cities and towns with USSR forces.",
			"0 0 0",
			"any",
			"Occupation at ",
			"City is being occupied.",
			{
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_NAME_VILLAGE,
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
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{00E36F41CA310E2A}Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et",
				"{0D9A5DCF89AE7AA9}Prefabs/Items/Medicine/MorphineInjection_01/MorphineInjection_01.et",
				"{13772C903CB5E4F7}Prefabs/Items/Equipment/Maps/PaperMap_01_folded.et",
				"{C819E0B7454461F2}Prefabs/Items/Equipment/Compass/Compass_Adrianov_Map.et",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			},
			0.9		
		);
		conf.occupations.Insert(occupation2);

		SCR_DC_Structure ocu2item0 = new SCR_DC_Structure;
		ocu2item0.Set
		(
			//"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
	        "{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
        	"1018.164 39 2501.937",
			"0 0 0"
		);
		occupation2.campItems.Insert(ocu2item0);
		
		SCR_DC_Structure ocu2item1 = new SCR_DC_Structure;
		ocu2item1.Set
		(
	        "{2CB4BFA62C2D9C12}Prefabs/Structures/Military/CamoNets/CamoNet_Small_Top_01_base.et",
	        "1017.857 39 2501.721",
			"0 0 0"
		);
		occupation2.campItems.Insert(ocu2item1);

		SCR_DC_Structure ocu2item2 = new SCR_DC_Structure;
		ocu2item2.Set
		(
	        "{B6307C189CCCA0B9}Prefabs/Props/Military/Sandbags/Sandbag_01_round_high_plastic.et",
			"1018.146 39 2504.881",
			"0 0 0"
		);
		occupation2.campItems.Insert(ocu2item2);
	
		SCR_DC_Structure ocu2item3 = new SCR_DC_Structure;
		ocu2item3.Set
		(
	        "{B6307C189CCCA0B9}Prefabs/Props/Military/Sandbags/Sandbag_01_round_high_plastic.et",
			"1020.456 39 2500.896",
			"0 114.569 0"
		);
		occupation2.campItems.Insert(ocu2item3);
	}
}