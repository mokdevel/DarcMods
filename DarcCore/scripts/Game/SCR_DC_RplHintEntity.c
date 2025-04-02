class SCR_DC_RplHintEntityClass: GenericEntityClass {}
SCR_DC_RplHintEntityClass g_SCR_DC_RplHintEntityClassInst;
 
class SCR_DC_RplHintEntity : GenericEntity
{
	protected static SCR_DC_RplHintEntity s_Instance;	

    void RplDCHint(IEntitySource src, IEntity parent)
    {
        this.SetEventMask(EntityEvent.FRAME);
    }
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
//		SCR_DC_DebugHelper.OnFrame(owner);
	}

	override void EOnActivate(IEntity owner)
	{
		SetEventMask(EntityEvent.FRAME | EntityEvent.POSTFRAME);
        SetFlags(EntityFlags.ACTIVE, true);
		Print("[SCR_DC_HintEntity] Running", LogLevel.NORMAL);
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
	static SCR_DC_RplHintEntity GetInstance()
	{
		return s_Instance;		
	}
};