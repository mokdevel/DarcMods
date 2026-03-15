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
		//SDRC_Spline3D.TestSpline();		
		
		SDRC_Log.Add("[SDRC_ChopperFrame] Starting SDRC_ChopperFrame", LogLevel.NORMAL);
		s_Instance = this;
	}
	
	void AddChopperToList(IEntity chopper)
	{
		m_aChoppers.Insert(chopper);
	}
}