//#define TESTING

static const string DC_COMPATCONFIG_FILE = "dc_compatFFConfigChopper.json";
static const int DC_COMPATCONFIG_FILE_JSONVER = 1;

//------------------------------------------------------------------------------------------------
class SDRC_ChopperCompatFF
{
	//Mission specific
	ref array<string> heliList = {};
	ref array<int> flyHeight = {};						//min, max - Spawn helicopter between these values.
	ref array<int> speed = {};							//min, max - 
	ref array<float> flyDistance = {};					//min, max - Distance for finding new positions
	SDRC_EHeliWaypointGenerationType wpType;
	SDRC_EHeliEnemySearchType enemyType;
	ref SDRC_ChopperConfigAi ai = new SDRC_ChopperConfigAi();	
			
	void Set(array<string> heliList_, array<int> flyHeight_, array<int> speed_, array<float> flyDistance_, SDRC_EHeliWaypointGenerationType wpType_, SDRC_EHeliEnemySearchType enemyType_)
	{
		heliList = heliList_;
		flyHeight = flyHeight_;
		speed = speed_;
		flyDistance = flyDistance_;
		wpType = wpType_;
		enemyType = enemyType_;
	}
}

//------------------------------------------------------------------------------------------------
// AI CONFIG
//------------------------------------------------------------------------------------------------
class SDRC_ChopperConfigAi : Managed
{
	ref array<int> count = {};
	ref array<string> types = {}; 		//The names of AI groups or characters. The AI is randomly picked from this list.
	int skill;							//Skill for AI (0-100). See SCR_AICombatComponent and EAISkill
	float perception;
	
	void Set(array<int> count_, array<string> types_, int skill_, float perception_)
	{
		count = count_;
		types = types_;
		skill = skill_;
		perception = perception_;		
	}
}
//------------------------------------------------------------------------------------------------
class SDRC_ChopperCompatFFConfig : SDRC_Config
{
	//Default information
	int version = 1;
	string author = "darc";
	string comment = "";
	ref array<string> factions = {};						//Factions to use for choppers
	float spawnChance;										//Chance % that is added to campaign progress
	ref array<int> spawnDistance = {};						//Distance min/max to spawn the attacking chopper	
	ref array<int> spawnDelay = {};							//(minutes) The delay before spawning the attacking chopper
	ref array<ref int> chopperCount = {};					//The amount choppers to spawn
	ref array<int> attackTime = {};							//(minutes) Time to attacks on area.
	ref array<ref int> attackList = {};						//The list of mission indexes.
	ref array<ref SDRC_ChopperCompatFF> attacks = {};		//List of sub attacks
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_ChopperCompatFFConfig data = SDRC_ChopperCompatFFConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		
	
	//------------------------------------------------------------------------------------------------
	//Loads the conf once to get it on file system
	static void LoadConfOnce()
	{
		SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_COMPATCONFIG_FILE);	
		SDRC_ChopperCompatFFConfig m_Config = new SDRC_ChopperCompatFFConfig();
		m_JsonApi.Load(m_Config, SDRC_ChopperCompatFFConfig.Cast(m_Config), DC_COMPATCONFIG_FILE_JSONVER);
	}
	
	//------------------------------------------------------------------------------------------------
	/*int GetSubMissionIdx(int subIdx)
	{
		int idx = -1;
		foreach (int i, SDRC_ChopperCompatFF attack : attacks)
		{
			if (attack.general.subIdx == subIdx)
			{
				idx = i;
				break;
			}
		}
		return idx;
	}*/
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		factions = {"FF"};
		#ifndef TESTING		
			spawnChance = 0.2;
			spawnDistance = {1500, 2500};
			spawnDelay = {5, 25};
			chopperCount = {0,1,1,2,2,2,2,2,2,3};
			attackTime = {20, 60};
			attackList = {0};
		#else
			spawnChance = 1.0;
			spawnDistance = {500, 700};
			spawnDelay = {0, 0};
			chopperCount = {2};
			attackTime = 1;
			attackList = {0};
		#endif		
		//----------------------------------------------------
		attacks.Insert(Attack0());
	};
	
	//----------------------------------------------------
	SDRC_ChopperCompatFF Attack0()
	{
		ref SDRC_ChopperCompatFF attack = new SDRC_ChopperCompatFF();		
		attack.Set
		(
			{
			 "VEHICLE_CHOPPER_ARMED",
			 "VEHICLE_CHOPPER_ARMED",
			 //From: https://reforger.armaplatform.com/workshop/672F2BB6523FBA29-WZHelisforDarcChopper
			 "{26436A51FE36D07C}Prefabs/Vehicles/Helicopters/Ka-137/Ka137_Patrol.et",			
 			 //From: https://reforger.armaplatform.com/workshop/68D2D498670F201F-MH-6MforDarcChopper
 			 "{87314096BD3C9D1A}Prefabs/Vehicles/Helicopters/AH6M/AH6M_M134_Patrol.et",
 			 "{C4590C7F97F99DB2}Prefabs/Vehicles/Helicopters/AH6M/MH6M_Patrol.et",
			 "{19022AB51719F2AD}Prefabs/Vehicles/Helicopters/AH6M/OPFOR/AH6M_OPFOR_M134_Patrol.et",
			 "{2B0F7648840C4F6E}Prefabs/Vehicles/Helicopters/AH6M/OPFOR/MH6M_OPFOR_Patrol.et",
			},
			{35, 70},
			{7, 25},
			{0.1, 0.3},
			SDRC_EHeliWaypointGenerationType.NONE,
			SDRC_EHeliEnemySearchType.ANY_CHAR,
		);
		attack.ai.Set
		(
			{1, 2},
			{"G_SMALL", "G_LIGHT"},
			60, 1.0,
		);
		
		return attack;
	}
}