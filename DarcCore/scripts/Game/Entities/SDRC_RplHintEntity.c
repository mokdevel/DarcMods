//------------------------------------------------------------------------------------------------
/*!
This is the hint entity class.

This is the entity with the RPL component attached and uses SDRC_RplHintComp for the communication.
The entity is not streamed so everyone gets the hint regardless on where they are on the map.
*/

//------------------------------------------------------------------------------------------------
class SDRC_RplHintEntityClass: GenericEntityClass {}
 
class SDRC_RplHintEntity : GenericEntity
{
	protected static SDRC_RplHintEntity s_Instance;	

	override void EOnActivate(IEntity owner)
	{
		SetEventMask(EntityEvent.FRAME | EntityEvent.POSTFRAME);
        SetFlags(EntityFlags.ACTIVE, true);
		s_Instance = this;
		Print("[SDRC_HintEntity] Running", LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to class
	*/
	static SDRC_RplHintEntity GetInstance()
	{
		return s_Instance;		
	}
};