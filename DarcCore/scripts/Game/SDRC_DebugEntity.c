//------------------------------------------------------------------------------------------------
/*!
This is the debug entity class. 
*/

//------------------------------------------------------------------------------------------------
class SDRC_DebugEntityClass: GenericEntityClass {};

class SDRC_DebugEntity: GenericEntity 
{
	protected static SDRC_DebugEntity s_Instance;	

	override void EOnFrame(IEntity owner, float timeSlice)
	{
		SDRC_DebugHelper.OnFrame(owner);
	}

	override void EOnActivate(IEntity owner)
	{
		SDRC_Log.Add("[SDRC_DebugEntity] Starting SDRC_DebugEntity", LogLevel.NORMAL);
		SDRC_DebugHelper.Setup();
		SetEventMask(EntityEvent.FRAME | EntityEvent.POSTFRAME);
        SetFlags(EntityFlags.ACTIVE, true);
		s_Instance = this;
		SDRC_Log.Add("[SDRC_DebugEntity] Running", LogLevel.DEBUG);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Get instance of the SDRC_DebugEntity.
	\return SDRC_DebugEntity

	Example:
	\code
		SDRC_DebugEntity debug = SDRC_DebugEntity.GetInstance();
	\endcode
	*/
	static SDRC_DebugEntity GetInstance()
	{
		return s_Instance;		
	}
};