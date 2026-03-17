//------------------------------------------------------------------------------------------------
/*!
This is the GM entity class. This handles the requested missions.
*/

//------------------------------------------------------------------------------------------------
class SDRC_RplGMEntityClass: GenericEntityClass {}
 
class SDRC_RplGMEntity : GenericEntity
{
	protected static SDRC_RplGMEntity s_Instance;	

	//------------------------------------------------------------------------------------------------
	override void EOnActivate(IEntity owner)
	{
		SetEventMask(EntityEvent.FRAME | EntityEvent.POSTFRAME);
        SetFlags(EntityFlags.ACTIVE, true);
		s_Instance = this;
		Print("[SDRC_RplGMEntity] Running", LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to class
	*/
	static SDRC_RplGMEntity GetInstance()
	{
		return s_Instance;		
	}
};