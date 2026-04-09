//------------------------------------------------------------------------------------------------
/*!
This is the chopper frame file.
*/

//------------------------------------------------------------------------------------------------

const string DC_CHOPPERCONFIG_FILE_FRAME = "dc_chopperConfig.json";
const int 	 DC_CHOPPERCONFIG_FILE_FRAME_JSONVER = 1;

//------------------------------------------------------------------------------------------------
class SDRC_ChopperFrame
{
	protected static SDRC_ChopperFrame s_Instance;		
	private ref array<IEntity> m_aChoppers = {};
	
	//------------------------------------------------------------------------------------------------
	void SDRC_ChopperFrame()
	{
		SDRC_Log.Add("[SDRC_ChopperFrame] Starting SDRC_ChopperFrame", LogLevel.NORMAL);
		s_Instance = this;
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