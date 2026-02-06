//SDRC_ChopperRocketHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_ChopperRocketHelper
{

	//------------------------------------------------------------------------------------------------
	// Spawn a rocket projectile (fallback method if weapon racks don't work)
	static void ShootRocket(IEntity owner, vector targetPos)
	{
		if (!owner)
			return;
		
		// Calculate target position with spread
		BaseWorld world = GetGame().GetWorld();
		if (!world)
			return;
		
		// Get ground height at target position
		float groundY = SDRC_Misc.GetSurfaceYWithWater(targetPos);

		vector rocketSpawnPos = SDRC_ChopperHelper.GetDestinationForward(owner, 10);
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.Transform[3] = rocketSpawnPos;
		params.TransformMode = ETransformMode.WORLD;
		
		ResourceName rocketPrefab = "{EE65544BA845C458}Prefabs/Weapons/Ammo/Ammo_Rocket_S5_HEDP_S5KO.et";

		IEntity rocket = GetGame().SpawnEntityPrefab(Resource.Load(rocketPrefab), world, params);
		if (rocket)
		{
			rocket.SetFlags(EntityFlags.VISIBLE, true);
			rocket.SetFlags(EntityFlags.ACTIVE, true);
			
			MissileMoveComponent missileMoveComp = MissileMoveComponent.Cast(rocket.GetRootParent().FindComponent(MissileMoveComponent));
			if (missileMoveComp)
			{
				SDRC_Log.Add("[SDRC_ChopperHelper:SetHealth] missileMoveComp found!", LogLevel.DEBUG);
//				missileMoveComp.AddForce("0 100 0");
//				missileMoveComp.SetVelocity("0 200 0");
//				missileMoveComp.SetEventMask(rocket, EntityEvent.FRAME);
				missileMoveComp.Launch(vector.Zero, vector.Zero, 0, rocket, null, null, null, null);							
			}
			
			SDRC_Log.Add("[SDRC_ChopperHelper:SetHealth] Rocket: " + rocketPrefab + " shot to: " + rocketSpawnPos, LogLevel.DEBUG);
		}
	}
}