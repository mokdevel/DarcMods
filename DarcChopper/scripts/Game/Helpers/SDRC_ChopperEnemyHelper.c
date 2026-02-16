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
							//SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SearchForEnemy] Player enemy found at " + enemyPosition, LogLevel.DEBUG);
							break;
						}
					}
					else
					{
						enemyPosition = target.GetOrigin();
						//SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SearchForEnemy] Enemy found at " + enemyPosition, LogLevel.DEBUG);
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
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}
		
		//chopperComp.m_RocketSector;
		
		if (SDRC_Math.IsTargetInSector(owner.GetOrigin(), fwd, enemyPosition, 35) )
		{
			ShootRocket(owner, enemyPosition);
		}
	}	
		
	//------------------------------------------------------------------------------------------------
	// Spawn a rocket projectile
	
	//Rockets tested:
	//{ECD8628EBF7E5F6B}Prefabs/Weapons/Ammo/Ammo_Rocket_Hydra70.et
	//{072A755D5CB85D47}Prefabs/Weapons/Ammo/Ammo_Rocket_Hydra70_HE_M229.et
	//{61AF60E0235DC3B1}Prefabs/Weapons/Ammo/Ammo_Rocket_Hydra70_HEDP_M247.et
	
	//{C9A1612DC5340613}Prefabs/Weapons/Ammo/Ammo_Rocket_S5.et
	//{EF17BED6DCEE4DE4}Prefabs/Weapons/Ammo/Ammo_Rocket_S5_FRAG_S5MO.et
	//{EE65544BA845C458}Prefabs/Weapons/Ammo/Ammo_Rocket_S5_HEDP_S5KO.et
	
	static void ShootRocket(IEntity owner, vector targetPos)
	{
		if (!owner)
		{
			return;
		}
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}

		ResourceName rocketPrefab = chopperComp.m_RocketPrefab;	//"{EE65544BA845C458}Prefabs/Weapons/Ammo/Ammo_Rocket_S5_HEDP_S5KO.et";
		if (rocketPrefab == "")
		{
			return;
		}
		
		// Get ground height at target position
		float groundY = SDRC_Misc.GetSurfaceYWithWater(targetPos);
		targetPos[1] = groundY + 10;
								
//		vector rocketSpawnPos = SDRC_ChopperHelper.GetDestinationForward(owner, 10);
		vector rocketSpawnPos = SDRC_ChopperHelper.GetDestinationForward(owner, chopperComp.m_RocketPosition[0]);
		
		
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
				//SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SetHealth] missileMoveComp found!", LogLevel.DEBUG);
				missileMoveComp.Launch(launchDirection, vector.Zero, 10, rocket, null, null, null, null);							
			}
			else
			{
				SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SetHealth] missileMoveComp not found!", LogLevel.DEBUG);
			}
			
			//SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SetHealth] Rocket: " + rocketPrefab + " shot to: " + rocketSpawnPos, LogLevel.DEBUG);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// Find weapons
	//
	// TBD: This currently does not return anything usable
	
	static void GetWeapons(IEntity owner)
	{
		array<WeaponSlotComponent> weaponSlots = {};
		array<BaseWeaponComponent> weaponComponents = {};
		array<IEntity> weapons = {};
		
		BaseWeaponManagerComponent weaponManager = BaseWeaponManagerComponent.Cast(owner.FindComponent(BaseWeaponManagerComponent));
		
		if ( !weaponManager )
		{
			return ;
		}
			
		weaponManager.GetWeaponsSlots(weaponSlots);
		weaponManager.GetWeapons(weaponComponents);
		weaponManager.GetWeaponsList(weapons);
		
		string weaponSlotType;
		
		// If there is an empty slot of the same type as the weapon on the ground we are able to equip the weapon without replacing anything
		foreach ( WeaponSlotComponent weaponSlot : weaponSlots )
		{
			weaponSlotType = weaponSlot.GetWeaponSlotType();
			SDRC_Log.Add("[SDRC_ChopperEnemyHelper:GetWeapons] weaponSlotType: " + weaponSlotType, LogLevel.DEBUG);
			
/*			if ( !weaponSlot.GetWeaponEntity() )
				return true;*/
		}		
				
	}
}