//------------------------------------------------------------------------------------------------
/*!
This is the line draw helper entity class.
*/

//------------------------------------------------------------------------------------------------
class SDRC_RplLineDrawEntityClass: GenericEntityClass {}
 
class SDRC_RplLineDrawEntity : GenericEntity
{
	protected static SDRC_RplLineDrawEntity s_Instance;	
	
	//------------------------------------------------------------------------------------------------
	override void EOnActivate(IEntity owner)
	{
		SetEventMask(EntityEvent.FRAME | EntityEvent.POSTFRAME);
        SetFlags(EntityFlags.ACTIVE, true);
		s_Instance = this;
		Print("[SDRC_RplLineDrawEntity] Running", LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to class
	*/
	static SDRC_RplLineDrawEntity GetInstance()
	{
		return s_Instance;		
	}
}