//------------------------------------------------------------------------------------------------
/*!
This is the debug entity class. 
*/

//------------------------------------------------------------------------------------------------
class SCR_DC_DebugEntityClass: GenericEntityClass {};

class SCR_DC_DebugEntity: GenericEntity 
{
	protected static SCR_DC_DebugEntity s_Instance;	

	override void EOnFrame(IEntity owner, float timeSlice)
	{
		SCR_DC_DebugHelper.OnFrame(owner);
	}

	override void EOnActivate(IEntity owner)
	{
		SCR_DC_Log.Add("[SCR_DC_DebugEntity] Starting SCR_DC_DebugEntity", LogLevel.NORMAL);
		SCR_DC_DebugHelper.Setup();
		SetEventMask(EntityEvent.FRAME | EntityEvent.POSTFRAME);
        SetFlags(EntityFlags.ACTIVE, true);
		s_Instance = this;
		SCR_DC_Log.Add("[SCR_DC_DebugEntity] Running", LogLevel.DEBUG);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Get instance of the SCR_DC_DebugEntity.
	\return SCR_DC_DebugEntity

	Example:
	\code
		SCR_DC_DebugEntity debug = SCR_DC_DebugEntity.GetInstance();
	\endcode
	*/
	static SCR_DC_DebugEntity GetInstance()
	{
		return s_Instance;		
	}
};