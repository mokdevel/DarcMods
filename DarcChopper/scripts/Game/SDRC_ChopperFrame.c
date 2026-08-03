//------------------------------------------------------------------------------------------------
/*!
This is the chopper frame file.
*/

//------------------------------------------------------------------------------------------------
class SDRC_ChopperFrame
{
	private const string DC_CHOPPERCONFIG_FILE = "dc_chopperConfig.json";
	private const int 	 DC_CHOPPERCONFIG_FILE_JSONVER = 1;
		
	private ref SDRC_JsonApi2 m_ChopperFrameJsonApi = new SDRC_JsonApi2(DC_CHOPPERCONFIG_FILE);	
	ref SDRC_ChopperCompConfig m_Config = new SDRC_ChopperCompConfig();		

	protected static SDRC_ChopperFrame s_Instance;		
	private ref array<IEntity> m_aChoppers = {};
	
	//------------------------------------------------------------------------------------------------
	void SDRC_ChopperFrame()
	{
		SDRC_Log.Add("[SDRC_ChopperFrame] Starting SDRC_ChopperFrame", LogLevel.NORMAL);
		s_Instance = this;
		
		//Load configuration from file
		bool success = m_ChopperFrameJsonApi.Load(m_Config, SDRC_Config.Cast(m_Config), DC_CHOPPERCONFIG_FILE_JSONVER, safeUpdate: true);
		
		if (!success)
		{
			SDRC_Log.Add("[SDRC_Chopper_BaseGameMode] Could not load configuration. Using defaults.", LogLevel.ERROR);
		}			
	}
	
	//------------------------------------------------------------------------------------------------
	void AddChopperToList(IEntity chopper)
	{
		m_aChoppers.Insert(chopper);
	}

	//------------------------------------------------------------------------------------------------
	void RemoveChopperFromList(IEntity chopper)
	{
		int idx = m_aChoppers.Find(chopper);
		if (idx > -1)
		{
			m_aChoppers.RemoveOrdered(idx);
		}
	}
		
	//------------------------------------------------------------------------------------------------
	int GetChopperCount()
	{
		//Check that the choppers still exist
		for (int i = 0; i < m_aChoppers.Count() - 1; i++)
		{
			if (!m_aChoppers[i])
			{
				m_aChoppers.RemoveOrdered(i);
				i--;
			}
		}
		
		return m_aChoppers.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity GetChopperEntity(int index)
	{
		if (m_aChoppers.IsEmpty())
		{
			return null;
		}
		
		return m_aChoppers[index];
	}	
	
	//------------------------------------------------------------------------------------------------
	void GetAllChopperEntity(out array<IEntity> choppers)
	{
		choppers.Copy(m_aChoppers);		
	}
}


//------------------------------------------------------------------------------------------------

class SDRC_ChopperCompConfig_Drone
{
	//Drone specific
	float dropChance = 0.5;
	int dropDistanceToPlayer = 10;
	int searchDistance = 300;
	int searchTime = 600;
}

class SDRC_ChopperCompConfig_Helicopter
{
	//Drone specific
	float justATest = 0;
}

class SDRC_ChopperCompConfig : SDRC_Config
{
	//Default information
	string author = "darc";
	
	ref SDRC_ChopperCompConfig_Drone drone = new SDRC_ChopperCompConfig_Drone();
	ref SDRC_ChopperCompConfig_Helicopter helicopter = new SDRC_ChopperCompConfig_Helicopter();
	
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_ChopperCompConfig data = SDRC_ChopperCompConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
	}
}