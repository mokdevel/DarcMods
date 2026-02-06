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
		targetPos[1] = groundY;
		
		vector rocketSpawnPos = SDRC_ChopperHelper.GetDestinationForward(owner, 10);

		vector direction = vector.Direction(rocketSpawnPos, targetPos);
//		direction.Normalize();
/*
		EntitySpawnParams params = new EntitySpawnParams();
		
		vector transform[4];
//		Math3D.MatrixIdentity3(transform);
		Math3D.AnglesToMatrix(direction, transform);
		
		transform[3] = rocketSpawnPos;		
        params.TransformMode = ETransformMode.WORLD;			
        params.Transform = transform;
		
/*		EntitySpawnParams params = new EntitySpawnParams();
		params.Transform[3] = rocketSpawnPos;
		params.TransformMode = ETransformMode.WORLD;*/
		
		EntitySpawnParams params = EntitySpawnParams();
		owner.GetWorldTransform(params.Transform);
		params.TransformMode = ETransformMode.WORLD;
		Math3D.AnglesToMatrix(direction, params.Transform);
		params.Transform[3] = rocketSpawnPos;
		
		
		ResourceName rocketPrefab = "{EE65544BA845C458}Prefabs/Weapons/Ammo/Ammo_Rocket_S5_HEDP_S5KO.et";
		
		IEntity rocket = SDRC_SpawnHelper.SpawnItem(rocketSpawnPos, rocketPrefab, 90, -1, false);
		
//		IEntity rocket = GetGame().SpawnEntityPrefab(Resource.Load(rocketPrefab), world, params);
//		SDRC_Math.TurnEntityTowards(rocket, targetPos);
		
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
//				missileMoveComp.Launch(direction, vector.Zero, 0, rocket, null, null, null, null);							
			}
			
			SDRC_Log.Add("[SDRC_ChopperHelper:SetHealth] Rocket: " + rocketPrefab + " shot to: " + rocketSpawnPos, LogLevel.DEBUG);
		}
	}
}