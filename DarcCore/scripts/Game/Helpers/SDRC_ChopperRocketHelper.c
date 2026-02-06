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
		
		// Get ground height at target position
		float groundY = SDRC_Misc.GetSurfaceYWithWater(targetPos);
		targetPos[1] = groundY + 10;
		
		vector rocketSpawnPos = SDRC_ChopperHelper.GetDestinationForward(owner, 10);

//		vector rotation = SDRC_Math.GetRotationVector(rocketSpawnPos, targetPos);
		
		ResourceName rocketPrefab = "{EE65544BA845C458}Prefabs/Weapons/Ammo/Ammo_Rocket_S5_HEDP_S5KO.et";
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.Transform[3] = rocketSpawnPos;
		params.TransformMode = ETransformMode.WORLD;
		
		IEntity rocket = GetGame().SpawnEntityPrefab(Resource.Load(rocketPrefab), GetGame().GetWorld(), params);
		SDRC_Math.TurnEntityTowards(rocket, targetPos + "0 10 0");
		
		vector launchDirection = vector.Direction(rocketSpawnPos, targetPos);
//		vector launchDirection = vector.Direction(rocketSpawnPos, targetPos + "0 200 0");
		launchDirection.Normalize();
		
/*		vector p0 = rocket.GetOrigin();
		vector angles = vector.Direction(p0, targetPos);
		angles.Normalize();
		angles = angles.VectorToAngles();
		rocket.SetYawPitchRoll(angles);			*/
		
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
				missileMoveComp.Launch(launchDirection, vector.Zero, 0, rocket, null, null, null, null);							
//				missileMoveComp.Launch(direction, vector.Zero, 0, rocket, null, null, null, null);							
			}
			
			SDRC_Log.Add("[SDRC_ChopperHelper:SetHealth] Rocket: " + rocketPrefab + " shot to: " + rocketSpawnPos, LogLevel.DEBUG);
		}
	}
}