class RplDCHintClass: GenericEntityClass {}
RplDCHintClass g_RplDCHintClassInst;
 
class RplDCHint : GenericEntity
{
	protected static RplDCHint s_Instance;	

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
	static RplDCHint GetInstance()
	{
		return s_Instance;		
	}
};