//SDRC_ChopperEnemyHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_ChopperEnemyHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Search for enemy and return first enemy position found.
	
	\return position where enemy was found. vector.Zero returned if no enemies found.
	*/			
	static vector SearchEnemy(IEntity owner, bool SearchOnlyPlayer = false)
	{
		vector enemyPosition = vector.Zero;
		
		//Enemy stuff		
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(owner.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		array<IEntity> occupants = {};
		scr_compartmentManager.GetOccupants(occupants);

		foreach (IEntity occupant : occupants)
		{
			SCR_AICombatComponent aicc = SCR_AICombatComponent.Cast(occupant.FindComponent(SCR_AICombatComponent));
			if (aicc)
			{
				BaseTarget bt = aicc.GetCurrentTarget();
				if (bt)
				{
					IEntity target = bt.GetTargetEntity();					
					
					if (SearchOnlyPlayer)
					{
						if (EntityUtils.IsPlayer(target))
						{
							enemyPosition = target.GetOrigin();
							SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SearchForEnemy] Player enemy found at " + enemyPosition, LogLevel.DEBUG);
							break;
						}
					}
					else
					{
						enemyPosition = target.GetOrigin();
						SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SearchForEnemy] Enemy found at " + enemyPosition, LogLevel.DEBUG);
						break;
					}
				}
			}
		}
		
		return enemyPosition;
	}

	//------------------------------------------------------------------------------------------------
	// Is there an enemy in front of us. If it's within sector, shoot
		
	static void SearchEnemyForRocket(IEntity owner)
	{
		vector enemyPosition = SearchEnemy(owner);

		if (enemyPosition == vector.Zero)
		{
			return;
		}
		
		//SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SearchEnemyForRocket] Enemy found.", LogLevel.DEBUG);

		vector fwd = owner.GetTransformAxis(2);
		fwd.Normalize();
		
		if (SDRC_Math.IsTargetInSector(owner.GetOrigin(), fwd, enemyPosition, 15) )
		{
			ShootRocket(owner, enemyPosition);
		}
	}	
		
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
		
		ResourceName rocketPrefab = "{EE65544BA845C458}Prefabs/Weapons/Ammo/Ammo_Rocket_S5_HEDP_S5KO.et";
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.Transform[3] = rocketSpawnPos;
		params.TransformMode = ETransformMode.WORLD;
		
		IEntity rocket = GetGame().SpawnEntityPrefab(Resource.Load(rocketPrefab), GetGame().GetWorld(), params);
		SDRC_Math.TurnEntityTowards(rocket, targetPos + "0 2 0");
		
		vector launchDirection = vector.Direction(rocketSpawnPos, targetPos);
		launchDirection.Normalize();
		
		if (rocket)
		{
			rocket.SetFlags(EntityFlags.VISIBLE, true);
			rocket.SetFlags(EntityFlags.ACTIVE, true);
			
			MissileMoveComponent missileMoveComp = MissileMoveComponent.Cast(rocket.GetRootParent().FindComponent(MissileMoveComponent));
			if (missileMoveComp)
			{
				SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SetHealth] missileMoveComp found!", LogLevel.DEBUG);
				missileMoveComp.Launch(launchDirection, vector.Zero, 0, rocket, null, null, null, null);							
			}
			
			SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SetHealth] Rocket: " + rocketPrefab + " shot to: " + rocketSpawnPos, LogLevel.DEBUG);
		}
	}
}