//------------------------------------------------------------------------------------------------
/*!
This is the hint entity class.

This is the entity with the RPL component attached and uses SDRC_RplStoryComp for the communication.
The entity is not streamed so everyone gets the hint regardless on where they are on the map.
*/

//------------------------------------------------------------------------------------------------
class SDRC_RplStoryEntityClass: GenericEntityClass {}
 
class SDRC_RplStoryEntity : GenericEntity
{
	protected static SDRC_RplStoryEntity s_Instance;	

	override void EOnActivate(IEntity owner)
	{
		SetEventMask(EntityEvent.FRAME | EntityEvent.POSTFRAME);
        SetFlags(EntityFlags.ACTIVE, true);
		s_Instance = this;
		Print("[SDRC_StoryEntity] Running", LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to class
	*/
	static SDRC_RplStoryEntity GetInstance()
	{
		return s_Instance;		
	}
};