//Mission SDRC_Mission_Occupation.c

//------------------------------------------------------------------------------------------------
/*!
This mission spawns groups to defend a location
*/

//------------------------------------------------------------------------------------------------
class SDRC_Mission_Occupation : SDRC_Mission
{
	private ref SDRC_OccupationJsonApi m_OccupationJsonApi = new SDRC_OccupationJsonApi();	
	private ref SDRC_OccupationConfig m_Config;
	
	protected ref SDRC_Occupation m_DC_Occupation;	//Occupation configuration in use
	
	private int m_iSpawnIndex = 0;						//Counter for the item to spawn
	private float m_fSpawnRotation = 0;					//Rotation of the camp for random locations.

	//------------------------------------------------------------------------------------------------
	void SDRC_Mission_Occupation()
	{
		SDRC_Log.Add("[SDRC_Mission_Occupation] Constructor", LogLevel.SPAM);
				
		//Set some defaults
		SetType(DC_EMissionType.OCCUPATION);

		//Load config
		m_OccupationJsonApi.Load();
		m_Config = m_OccupationJsonApi.conf;
		
		//Pick a configuration for mission
		int idx = SDRC_MissionHelper.SelectMissionIndex(m_Config.occupationList);
		if (idx == -1)
		{
			SDRC_Log.Add("[SDRC_Mission_Occupation] No occupations defined.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}
		m_DC_Occupation = m_Config.occupations[idx];
				
		//Set defaults
		vector pos = m_DC_Occupation.pos;
		string posName = m_DC_Occupation.posName;
		
		//Find a location for the mission
		if (pos == "0 0 0")
		{
			pos = SDRC_MissionHelper.FindMissionPos(m_DC_Occupation.locationTypes, m_Config.emptySize);
			//Camps in random places are randomly rotated
			m_fSpawnRotation = Math.RandomFloat(0, 360);
		}
		else
		{
			if (!SDRC_MissionHelper.IsValidMissionPos(pos))
			{
				pos = "0 0 0";	//Mission position is not valid
			}
		}
		
		if (pos == "0 0 0")	//No suitable location found.
		{				
			SDRC_Log.Add("[SDRC_Mission_Occupation] Could not find suitable location.", LogLevel.ERROR);
			SetState(DC_EMissionState.FAILED);
			return;
		}	
		
		SetPos(pos);
		SetPosName(SDRC_Locations.CreateName(pos, posName));
		SetTitle(m_DC_Occupation.title + "" + GetPosName());
		SetInfo(m_DC_Occupation.info);			
		SetMarker(m_Config.showMarker, DC_EMissionIcon.N_FENCE);
		SetShowHint(m_Config.showHint);

		SDRC_SpawnHelper.SetStructuresToOrigo(m_DC_Occupation.campItems);
			
		SetState(DC_EMissionState.INIT);
	}

	//------------------------------------------------------------------------------------------------
	override void MissionRun()
	{
		super.MissionRun();
		
		if (GetState() == DC_EMissionState.INIT)
		{
			MissionSpawn();
			GetGame().GetCallqueue().CallLater(MissionRun, 2*1000);		//Spawn stuff every two seconds
			return;
		}

		if (GetState() == DC_EMissionState.END)
		{
			MissionEnd();
			SetState(DC_EMissionState.EXIT);
		}	
				
		if (GetState() == DC_EMissionState.ACTIVE)
		{	
			if (!IsActive())
			{
				SDRC_Log.Add("[SDRC_Mission_Occupation:MissionRun] Mission over.", LogLevel.NORMAL);
				SetState(DC_EMissionState.END);
			}
		}
		
		GetGame().GetCallqueue().CallLater(MissionRun, m_Config.missionCycleTime*1000);
	}

	//------------------------------------------------------------------------------------------------
	override void MissionEnd()
	{			
		super.MissionEnd();
		
		SDRC_Log.Add("[SDRC_Mission_Occupation:MissionEnd] Mission cleared for deletion.", LogLevel.NORMAL);		
	}
			
	//------------------------------------------------------------------------------------------------
	private void MissionSpawn()
	{					
		IEntity entity;

		//Spawn entities one by one. Sets missions active once ready.
		if ( (m_iSpawnIndex < m_DC_Occupation.campItems.Count()) && (m_DC_Occupation.campItems.Count() > 0) )
		{			
			entity = SDRC_SpawnHelper.SpawnStructures(m_DC_Occupation.campItems, GetPos(), m_fSpawnRotation, m_iSpawnIndex);
			
			if (entity != NULL)
			{ 
				m_EntityList.Insert(entity);
				//Disable arsenal
				string resourceName = m_DC_Occupation.campItems[m_iSpawnIndex].GetResource();
				SDRC_SpawnHelper.DisableVehicleArsenal(entity, resourceName, m_Config.disableArsenal);				
			}
			else
			{
				SDRC_Log.Add("[SDRC_Mission_Occupation:MissionSpawn] Could not load: " + m_DC_Occupation.campItems[m_iSpawnIndex], LogLevel.ERROR);				
			}
			
			m_iSpawnIndex++;			
		}
		else
		{
			//Spawn mission AI 
			int groupCount = Math.RandomInt(m_DC_Occupation.groupCount[0], m_DC_Occupation.groupCount[1]);
			
			for (int i = 0; i < groupCount; i++)
			{
				SCR_AIGroup group = SDRC_MissionHelper.SpawnMissionAIGroup(m_DC_Occupation.groupTypes.GetRandomElement(), GetPos(), GetFaction());
				if (group)
				{
					SDRC_AIHelper.SetAIGroupSkill(group, m_DC_Occupation.aiSkill, m_DC_Occupation.aiPerception);					
					m_Groups.Insert(group);
					
					int minRange = m_DC_Occupation.waypointRange[0];
					int maxRange = m_DC_Occupation.waypointRange[1];
					
					//If there are more than one group and loot, spawn one to protect the loot. 
					//For the first group, waypointRange is ignored.
					if ((m_DC_Occupation.loot) && i == 0)
					{
						minRange = 5;
						maxRange = 30;					
					}
					
					SDRC_WPHelper.CreateMissionAIWaypoints(group, m_DC_Occupation.waypointGenType, GetPos(), "0 0 0", m_DC_Occupation.waypointMoveType, minRange, maxRange);
//					SDRC_WPHelper.CreateMissionAIWaypoints(group, DC_EWaypointGenerationType.LOITER, GetPos(), "0 0 0", DC_EWaypointMoveType.LOITER, m_DC_Occupation.waypointRange[0], m_DC_Occupation.waypointRange[1]);
				}
				SDRC_Log.Add("[SDRC_Mission_Occupation:MissionSpawn] AI groups spawned: " + groupCount, LogLevel.DEBUG);								
			}
			
			//Put loot
			if (m_DC_Occupation.loot)			
			{
				m_DC_Occupation.loot.box = m_EntityList[0];
				SDRC_LootHelper.SpawnItemsToStorage(m_DC_Occupation.loot.box, m_DC_Occupation.loot.items, m_DC_Occupation.loot.itemChance);
				SDRC_Log.Add("[SDRC_Mission_Occupation:MissionSpawn] Loot added.", LogLevel.DEBUG);								
			}
			
			SetState(DC_EMissionState.ACTIVE);
		}		
	}
}

//------------------------------------------------------------------------------------------------
class SDRC_OccupationConfig : SDRC_MissionConfig
{
	//Mission specific	
	int emptySize = 7;										//The size (radius) of the empty space to found to decide on a mission position.
	bool disableArsenal;									//Disable arsenal for vehicles so that no other items are found
	ref array<ref int> occupationList = {};					//The indexes of occupations.
	ref array<ref SDRC_Occupation> occupations = {};		//List of occupations
}

//------------------------------------------------------------------------------------------------
class SDRC_Occupation : Managed
{
	//Occupation specific
	string comment;							//Generic comment to describe the mission. Not used in game.
	vector pos;								//Position for mission. "0 0 0" used for random location chosen from locationTypes.
	string posName;							//Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes 
	string title;							//Title for the hint shown for players
	string info;							//Details for the hint shown for players
	ref array<EMapDescriptorType> locationTypes = {};	
	ref array<int> groupCount = {};			//min, max
	ref array<int> waypointRange = {};		//min, max
	DC_EWaypointGenerationType waypointGenType;
	DC_EWaypointMoveType waypointMoveType;
	ref array<string> groupTypes = {};
	int aiSkill;
	float aiPerception	
	
	//Optional settings
	ref SDRC_Loot loot = null;
	ref array<ref SDRC_Structure> campItems = {};
	
	void Set(string comment_, vector pos_, string posName_, string title_, string info_, array<EMapDescriptorType> locationTypes_, array<int> groupCount_, array<int> waypointRange_, DC_EWaypointGenerationType waypointGenType_, DC_EWaypointMoveType waypointMoveType_, array<string> groupTypes_, int aiSkill_, float aiPerception_)
	{
		comment = comment_;
		pos = pos_;
		posName = posName_;
		title = title_;
		info = info_;
		locationTypes = locationTypes_;
		groupCount = groupCount_;
		waypointRange = waypointRange_;
		waypointGenType = waypointGenType_;
		waypointMoveType = waypointMoveType_;
		groupTypes = groupTypes_;
		aiSkill = aiSkill_;
		aiPerception = aiPerception_;		
	}
}		

//------------------------------------------------------------------------------------------------
class SDRC_OccupationJsonApi : SDRC_JsonApi
{
	const string DC_MISSIONCONFIG_FILE = "dc_missionConfig_Occupation.json";
	ref SDRC_OccupationConfig conf = new SDRC_OccupationConfig();
	
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
		array<string> lootItems = {};
		
		//Default		
		conf.missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		conf.showMarker = true;
		//Mission specific		
		conf.occupationList = {0,0,0,1,1,2,2,2,3,4};

		//----------------------------------------------------
		SDRC_Occupation occupation0 = new SDRC_Occupation();
		occupation0.Set
		(
			"index 0: Mission in villages and local areas.",
			"0 0 0",
			"any",
			"Guards patroling near ",
			"Avoid the location. Loot has already been lost.",
			{
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL
			},
			{1, 2},
			{50, 300},
			DC_EWaypointGenerationType.RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_RECON", "G_LIGHT"
			},
			50, 1.0		
		);
		conf.occupations.Insert(occupation0);
		
		//----------------------------------------------------
		SDRC_Occupation occupation1 = new SDRC_Occupation();
		occupation1.Set(
			"index 1: Bandit camp spawning to non city areas",
			"0 0 0",
			"any",
			"Bandit camp near ",
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
			{25, 100},
			DC_EWaypointGenerationType.SCATTERED,//RANDOM,
			DC_EWaypointMoveType.PATROLCYCLE,
			{
				"G_ADMIN", "G_LIGHT", "G_LIGHT"
			},
			50, 1.0		
		);
		conf.occupations.Insert(occupation1);
		
		SDRC_Loot occupation1loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"WEAPON_ATTACHMENT",
				"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			};
		occupation1loot.Set(0.7, lootItems);
		occupation1.loot = occupation1loot;
		
		SDRC_Structure ocu1item0 = new SDRC_Structure();
		ocu1item0.Set(
			"{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
			"1032.038 39 2478.923"
		);
		occupation1.campItems.Insert(ocu1item0);
		SDRC_Structure ocu1item1 = new SDRC_Structure();
		ocu1item1.Set(
			"{39C308BBB5945B85}Prefabs/Props/Military/Furniture/ChairMilitary_US_02.et",
			"1028.55 39 2478.16",
			"0 119.334 0"
		);
		occupation1.campItems.Insert(ocu1item1);
		SDRC_Structure ocu1item2 = new SDRC_Structure();
		ocu1item2.Set(
			"{D9842C11742C00CF}Prefabs/Props/Civilian/Fireplace_01.et",
			"1029.9 39 2477.44"
		);
		occupation1.campItems.Insert(ocu1item2);					
		SDRC_Structure ocu1item3 = new SDRC_Structure();
		ocu1item3.Set(
			"{0511E95F422061BB}Prefabs/Props/Recreation/Camp/TentSmall_02/TentSmall_02_blue.et",
			"1029.974 39 2480.114"
		);
		occupation1.campItems.Insert(ocu1item3);
		
		//----------------------------------------------------
		SDRC_Occupation occupation2 = new SDRC_Occupation();
		occupation2.Set(
			"index 2: Occupation that will spawn mainly to cities and towns.",
			"0 0 0",
			"any",
			"Occupation in ",
			"City is being occupied.",
			{
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_CITY,
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_TOWN, 
				EMapDescriptorType.MDT_AIRPORT,
			},
			{2, 4},
			{30, 200},
			DC_EWaypointGenerationType.RADIUS,
			DC_EWaypointMoveType.RANDOM,		
			{
				"G_LIGHT", "G_LIGHT", "G_LIGHT", 
				"G_ADMIN", "G_HEAVY", "G_LAUNCHER", "G_MEDICAL"
			},
			50, 1.0		
		);
		conf.occupations.Insert(occupation2);

		SDRC_Loot occupation2loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"WEAPON_ATTACHMENT",
				"WEAPON_OPTICS",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et",		//This item from Escapists
				"{377BE4876BC891A1}Prefabs/Items/Medicine/EpinephrineInjection_01.et"		//This item from Escapists
			};
		occupation2loot.Set(0.9, lootItems);
		occupation2.loot = occupation2loot;
		
		SDRC_Structure ocu2item0 = new SDRC_Structure();
		ocu2item0.Set(
			//"{4A9E0C3D18D5A1B7}Prefabs/Props/Crates/LootCrateWooden_01.et",
	        "{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et",
        	"1018.164 39 2501.937"
		);
		occupation2.campItems.Insert(ocu2item0);
		
		SDRC_Structure ocu2item1 = new SDRC_Structure();
		ocu2item1.Set(
	        "{2CB4BFA62C2D9C12}Prefabs/Structures/Military/CamoNets/CamoNet_Small_Top_01_base.et",
	        "1017.857 39 2501.721"
		);
		occupation2.campItems.Insert(ocu2item1);

		SDRC_Structure ocu2item2 = new SDRC_Structure();
		ocu2item2.Set(
	        "{B6307C189CCCA0B9}Prefabs/Props/Military/Sandbags/Sandbag_01_round_high_plastic.et",
			"1018.146 39 2504.881"
		);
		occupation2.campItems.Insert(ocu2item2);
	
		SDRC_Structure ocu2item3 = new SDRC_Structure();
		ocu2item3.Set(
	        "{B6307C189CCCA0B9}Prefabs/Props/Military/Sandbags/Sandbag_01_round_high_plastic.et",
			"1020.456 39 2500.896",
			"0 114.569 0"
		);
		occupation2.campItems.Insert(ocu2item3);
		
		//----------------------------------------------------
		SDRC_Occupation occupation3 = new SDRC_Occupation();
		occupation3.Set(
			"index 3: Car crash in an unusual place",
			"0 0 0",
			"any",
			"Car crash near ",
			"Loot is up for grabs.",
			{
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_FUELSTATION,
				EMapDescriptorType.MDT_NAME_SETTLEMENT,
				EMapDescriptorType.MDT_PARKING,
				EMapDescriptorType.MDT_BASE,
				EMapDescriptorType.MDT_RADIO,
				EMapDescriptorType.MDT_CONSTRUCTION_SITE,
			},
			{1, 3},
			{10, 60},
			DC_EWaypointGenerationType.RANDOM,
			DC_EWaypointMoveType.RANDOM,		
			{
				"G_RECON"
			},
			50, 1.0		
		);
		conf.occupations.Insert(occupation3);

		SDRC_Loot occupation3loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",	"WEAPON_RIFLE",
				"WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"WEAPON_ATTACHMENT",
				"WEAPON_OPTICS",
				"WEAPON_LAUNCHER",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"
			};
		occupation3loot.Set(0.7, lootItems);
		occupation3.loot = occupation3loot;
		
		SDRC_Structure ocu3item0 = new SDRC_Structure();
		ocu3item0.Set(
	       "{F9CB8E28C2B3DF2B}Prefabs/Props/Crates/CrateWooden_02/LootCrateWooden_02_1x1x1.et",
        	"78.569 1 110.113",
	        "0 -34.136 0"
 		);
		occupation3.campItems.Insert(ocu3item0);
		
		SDRC_Structure ocu3item1 = new SDRC_Structure();
		ocu3item1.Set(
	        "{56FFF0C990358ED2}Prefabs/Props/Wrecks/UAZ452_wreck_static.et",
        	"78.8 1 113.211"
		);
		occupation3.campItems.Insert(ocu3item1);

		SDRC_Structure ocu3item2 = new SDRC_Structure();
		ocu3item2.Set(
	        "{6095B175AA9804DC}Prefabs/Props/VehicleParts/Tires/Tire_UAZ469.et",
    	    "77.252 1 114.03"
		);
		occupation3.campItems.Insert(ocu3item2);
	
		SDRC_Structure ocu3item3 = new SDRC_Structure();
		ocu3item3.Set(
	        "{6095B175AA9804DC}Prefabs/Props/VehicleParts/Tires/Tire_UAZ469.et",
        	"80.555 1 110.34"
		);
		occupation3.campItems.Insert(ocu3item3);
		
		SDRC_Structure ocu3item4 = new SDRC_Structure();
		ocu3item4.Set(
	        "{8BAF6C3ACF99388E}Prefabs/Props/Garbage/Cardboard/Cardboard_Pile_05.et",
	        "79.362 1 108.878"
		);
		occupation3.campItems.Insert(ocu3item4);		
		
		SDRC_Structure ocu3item5 = new SDRC_Structure();
		ocu3item5.Set(
	        "{530705FBB61026D2}Prefabs/Props/Garbage/Cardboard/Cardboard_Pile_03.et",
    	    "76.743 1 111.704"
		);
		occupation3.campItems.Insert(ocu3item5);		

		SDRC_Structure ocu3item6 = new SDRC_Structure();
		ocu3item6.Set(
	        "{2424EBB806A690D4}Prefabs/Props/Garbage/Medical/GarbageMedicalUS_02.et",
    	    "80.1 1 113.321"
		);
		occupation3.campItems.Insert(ocu3item6);
		
		//----------------------------------------------------
		SDRC_Occupation occupation4 = new SDRC_Occupation();
		occupation4.Set(
			"index 4: Campers with a car and a tent",
			"0 0 0",
			"any",
			"Campers near ",
			"Rob them before they leave.",
			{
				EMapDescriptorType.MDT_NAME_RIDGE,
				EMapDescriptorType.MDT_FORESTSQUARE,
				EMapDescriptorType.MDT_NAME_HILL,
				EMapDescriptorType.MDT_NAME_VALLEY			
			},
			{1, 3},
			{10, 90},
			DC_EWaypointGenerationType.RANDOM,
			DC_EWaypointMoveType.RANDOM,		
			{
				"G_SPECIAL", "G_HEAVY"
			},
			50, 1.0		
		);
		conf.occupations.Insert(occupation4);

		SDRC_Loot occupation4loot = new SDRC_Loot();
		lootItems = {
				"WEAPON_RIFLE",
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN",
				"WEAPON_GRENADE", "WEAPON_GRENADE", "WEAPON_GRENADE",
				"WEAPON_ATTACHMENT", "WEAPON_ATTACHMENT",
				"WEAPON_OPTICS",
				"ITEM_MEDICAL", "ITEM_MEDICAL",	"ITEM_MEDICAL",	"ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL"
			};
		occupation4loot.Set(0.4, lootItems);
		occupation4.loot = occupation4loot;		
		
 		SDRC_Structure ocu4item0 = new SDRC_Structure();
		ocu4item0.Set(
			"{E28501E93F8EFDC0}Prefabs/Vehicles/Wheeled/UAZ469/UAZ469_FIA_uncovered.et",
			"84.933 1 97.416",
			"0 -43.327 0"
		);
		occupation4.campItems.Insert(ocu4item0);
		
 		SDRC_Structure ocu4item1 = new SDRC_Structure();
		ocu4item1.Set(
			"{06FE4FE70907D486}Prefabs/Props/Military/Compositions/Dst/PersonnelService_Table_01/PersonnelService_Table_01_dst_01.et",
			"80.32 1.092 90.817",
			"0 -40.157 0"
		);
		occupation4.campItems.Insert(ocu4item1);
		
 		SDRC_Structure ocu4item2 = new SDRC_Structure();
		ocu4item2.Set(
			"{172DD50ACF177B9E}Prefabs/Props/Military/Furniture/ChairMilitary_USSR_01.et",
			"79.558 1.075 91.27",
			"0 -38.585 0"
		);
		occupation4.campItems.Insert(ocu4item2);
		
 		SDRC_Structure ocu4item3 = new SDRC_Structure();
		ocu4item3.Set(
			"{9CBBE8B23794214D}Prefabs/Props/Commercial/CabinetCardFile_01/Dst/CabinetCardFile_01_dst_green.et",
			"76.37 1.078 91.049"
		);
		occupation4.campItems.Insert(ocu4item3);
		
 		SDRC_Structure ocu4item4 = new SDRC_Structure();
		ocu4item4.Set(
			"{C768E842A6F11CEE}Prefabs/Structures/Military/Camps/TentUSSR_01/TentUSSR_01_camonet.et",
			"78.758 0 92.718"
		);
		occupation4.campItems.Insert(ocu4item4);
	}	
}