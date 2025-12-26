//Helpers SDRC_LoadCtx

//------------------------------------------------------------------------------------------------
/*!
Compatibility for Freedom Fighters
*/

//------------------------------------------------------------------------------------------------
class SDRC_LoadCtx
{	
	const string DC_COMPATCONFIG_FILE = "dc_ctx.json";
	const int DC_COMPAT_WAIT_FOR_PLAYERS_TIME = 15;
	const int DC_COMPAT_CLEAN_WAIT_TIME = 15;
	ref SDRC_JsonApi2 m_DC_CompatJsonApi = null;
	ref SCR_JsonLoadContext loadContext;
	ref SDRC_CtxConfig conf = new SDRC_CtxConfig();
	
	//------------------------------------------------------------------------------------------------
	/*!
	Add you init code here.
	*/
	bool Init()
	{		
		SDRC_Log.Add("[SDRC_LoadCtx] Initializing..", LogLevel.NORMAL);
		
		m_DC_CompatJsonApi = new SDRC_JsonApi2(DC_COMPATCONFIG_FILE);		
		m_DC_CompatJsonApi.Load(conf);
		
		SDRC_Log.Add("[SDRC_LoadCtx] Author: " + conf.author, LogLevel.NORMAL);

		return true;
	}
}

//------------------------------------------------------------------------------------------------
//class SDRC_CtxConfig : SDRC_Config
class SDRC_CtxConfig : SDRC_MissionConfig2
{
	//Mission specific
	string comment;
	//Specific
	int hideOutSafeZoneDistance = 300;
	float spawnRateForGreenZones = 0.15; 
	bool setEnemyFactionAutomatically = true;	//Automatically set enemy faction from . (WIP)
	bool rewardPerUser = false;					//Shall reward be set per user or for a group
	int rewardDefault = 500;					//Default reward unless specific reward has been set in a mission. (WIP)
	ref array<string> names = {};
	ref array<string> pos = {};
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{	
		super.SetDefaults();
		names = {"Janne", "Jorma"};
		pos = {"1.0 1.0 1.0", "1.1 1.1 1.1"};
	}
	
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_CtxConfig data = SDRC_CtxConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}	
}