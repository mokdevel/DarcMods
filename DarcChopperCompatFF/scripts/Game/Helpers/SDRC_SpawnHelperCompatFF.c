//Helpers SDRC_SpawnHelper.c

//------------------------------------------------------------------------------------------------
/*!
Compatibility for Freedom Fighters
*/

//------------------------------------------------------------------------------------------------
modded class SDRC_SpawnHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Fix persistence for vehicle. For example for EPF.
	*/
	override static void SetPersistence(IEntity entity, bool persistence = true)	
	{	
		if (SDRC_VehicleHelper.IsVehicle(entity))
		{
			//Avoid despawning of vehicles
			auto JWK_persistence = JWK_CompTU<EPF_PersistenceComponent>.FindIn(entity);
			if (JWK_persistence) 
			{
				JWK_persistence.JWK_SetEnabled(persistence);
				if (persistence)
				{
					SDRC_Log.Add("[SDRC_SpawnHelperCompatFF:SetPersistence] Persistence enabled for: " + entity.GetPrefabData().GetPrefabName(), LogLevel.DEBUG);
				}
				else
				{
					SDRC_Log.Add("[SDRC_SpawnHelperCompatFF:SetPersistence] Persistence disabled for: " + entity.GetPrefabData().GetPrefabName(), LogLevel.DEBUG);
				}
			}
			//FF despawns vehicles beyond certain distance from players and spawns them back in when they get near for performance, this will disable that.
			auto streamable = JWK_CompTU<JWK_StreamableVehicleComponent>.FindIn(entity);
			if (streamable) streamable.SetStreamingEnabled_S(false);
			
			SDRC_Log.Add("[SDRC_SpawnHelperCompatFF:SetPersistence] Streaming disabled for: " + entity.GetPrefabData().GetPrefabName(), LogLevel.DEBUG);
		}
						
		super.SetPersistence(entity, persistence);
	}	
}
