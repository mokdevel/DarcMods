//------------------------------------------------------------------------------------------------
/*!
This is the debug entity class. 
*/

//------------------------------------------------------------------------------------------------
class SCR_DC_DebugEntityClass: GenericEntityClass {};

class SCR_DC_DebugEntity: GenericEntity 
{

	protected static SCR_DC_DebugEntity s_Instance;	
	
/*	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		SCR_DC_Log.Add("[SCR_DC_DebugEntity:EOnInit] Created", LogLevel.DEBUG);
	}*/
	
/*	private void SCR_DC_DebugEntity()
	{
		SCR_DC_Log.Add("[SCR_DC_DebugEntity] Setup!", LogLevel.NORMAL);		
		SCR_DC_DebugHelper.Setup();
	}*/
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		SCR_DC_DebugHelper.OnFrame(owner);
	}

	override void EOnActivate(IEntity owner)
	{
		SCR_DC_Log.Add("[SCR_DC_DebugEntity] Starting..", LogLevel.DEBUG);
		SCR_DC_DebugHelper.Setup();
		SetEventMask(EntityEvent.FRAME | EntityEvent.POSTFRAME);
        SetFlags(EntityFlags.ACTIVE, true);
		SCR_DC_Log.Add("[SCR_DC_DebugEntity] Running", LogLevel.DEBUG);
	}

	static void Setup()	
	{
		SCR_DC_Log.Add("[SCR_DC_DebugEntity] Setup!", LogLevel.DEBUG);		
		SCR_DC_DebugHelper.Setup();
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