//SDRC_ChopperEnemyHelper.c

//------------------------------------------------------------------------------------------------
enum SDRC_EHeliEnemySearchType
{
	NONE,
	ANY,				//Any target is accepted
	PLAYER,				//Only players are considered targets
	ANY_CHAR,			//Players and AI
//	VEHICLE,			//Any vehicle
//	VEHICLE_ARMORED,	//Only armored vehicles
}

//------------------------------------------------------------------------------------------------
class SDRC_ChopperEnemyHelper
{	
	//------------------------------------------------------------------------------------------------	
	// Enemy searching functionality
	//------------------------------------------------------------------------------------------------	

	//------------------------------------------------------------------------------------------------
	/*!
	Search for enemy and mark it. The knowledge will eventually be lost. 
	After a while, we may find another enemy to track.
	*/			
	static bool SearchForEnemy(IEntity owner)
	{
		bool found = false;
	
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return false;
		}
			
		if (chopperComp.m_EnemySearchType == SDRC_EHeliEnemySearchType.NONE)
		{
			return false;
		}
		
		if (chopperComp.m_iEnemyFoundTime > SDRC_Misc.GetCurrentTickTime())
		{
			return false;
		}
		
		if ( (SDRC_Misc.GetCurrentTickTime() > chopperComp.m_iEnemyFoundTime + chopperComp.m_iEnemyForgetTimeout) && (chopperComp.m_vEnemyPosition != "0 0 0") )
		{
			chopperComp.m_vEnemyPosition = "0 0 0";
			SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SearchForEnemy] Enemy position reset.", LogLevel.DEBUG);
		}
		
		chopperComp.m_vEnemyPosition = SDRC_ChopperEnemyHelper.DoEnemySearch(owner);
		if (chopperComp.m_vEnemyPosition != vector.Zero)
		{
			found = true;
			chopperComp.m_iEnemyFoundTime = SDRC_Misc.GetCurrentTickTime() + chopperComp.m_iEnemyFoundTimeout;
		}
		
		return found;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Search for enemy and return *first* enemy position found.
	
	//TBD: Extend to have a parameter where enemy needs to be in front of the heli
	
	\return position where enemy was found. vector.Zero returned if no enemies found.
	*/			
	static vector DoEnemySearch(IEntity owner)
	{
		vector enemyPosition = vector.Zero;
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return enemyPosition;
		}
		
		switch (chopperComp.m_EntityType)
		{
			case SDRC_EChopperType.HELICOPTER:
			case SDRC_EChopperType.FIXEDWING:			
			{
				enemyPosition = SearchEnemyWithAI(owner);
				break;
			}
			case SDRC_EChopperType.DRONE:
			{
				enemyPosition = SearchEnemyWithTrace(owner, chopperComp.params.rayLenEnemy);
				break;
			}
		}		
		
		return enemyPosition;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if we're to only search players as enemies
	*/	
	static bool IsSearchOnlyPlayer(IEntity owner)
	{
		bool searchOnlyPlayer = false;
		
		SDRC_ChopperComp cc = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		
		if (!cc)
		{
			SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SearchEnemyWithAI] SDRC_ChopperComp not found.", LogLevel.WARNING);			
		}
		
		//If searching only for players
		if ( (cc.m_EnemySearchType == SDRC_EHeliEnemySearchType.PLAYER) || 
		     (cc.m_EnemySearchType == SDRC_EHeliEnemySearchType.ANY_CHAR) || 
		     (cc.m_EnemySearchType == SDRC_EHeliEnemySearchType.ANY) 
		   )
		{
			searchOnlyPlayer = true;
		}
		
		return searchOnlyPlayer;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if we're to only search AI as enemies
	*/	
	static bool IsSearchOnlyAI(IEntity owner)
	{
		bool searchOnlyAI = false;
		
		SDRC_ChopperComp cc = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		
		if (!cc)
		{
			SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SearchEnemyWithAI] SDRC_ChopperComp not found.", LogLevel.WARNING);			
		}
		
		//If searching only for AI enemies
		if ( (cc.m_EnemySearchType == SDRC_EHeliEnemySearchType.ANY_CHAR) || 
		     (cc.m_EnemySearchType == SDRC_EHeliEnemySearchType.ANY) 
		   )
		{
			searchOnlyAI = true;
		}
		
		return searchOnlyAI;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Search for enemy and return *first* enemy position found. This will use the AIs vision functionality.
	
	//TBD: Extend to have a parameter where enemy needs to be in front of the heli
	*/	
	static vector SearchEnemyWithAI(IEntity owner)
	{
		vector enemyPosition = vector.Zero;
		
		//Enemy stuff		
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(owner.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		if (!scr_compartmentManager)
		{
			return vector.Zero;
		}
		
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
					
					if (IsSearchOnlyPlayer(owner))
					{
						if (EntityUtils.IsPlayer(target))
						{
							enemyPosition = target.GetOrigin();
							//SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SearchEnemyForHelicopter] Player enemy found at " + enemyPosition, LogLevel.DEBUG);
							break;
						}
					}
					else
					{
						if (target)	//Target may have been removed
						{
							enemyPosition = target.GetOrigin();
						}
						//SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SearchEnemyForHelicopter] Enemy found at " + enemyPosition, LogLevel.DEBUG);
						break;
					}
				}
			}
		}
		
		return enemyPosition;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Search for enemy and return *first* enemy position found. This will throw a ray towards players. 
	If there are obstacles, enemy is not seen.
	
	//TBD: Extend to have a parameter where enemy needs to be in front of the drone
	*/	
	static vector SearchEnemyWithTrace(IEntity owner, int rayLen = 200)
	{
		vector enemyPosition = vector.Zero;

		//Find the owner faction
		string chopperFaction = "";
		SDRC_ChopperComp cc = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		
		if (!cc)
		{
			SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SearchEnemyWithTrace] SDRC_ChopperComp not found.", LogLevel.ERROR);			
		}
		
		chopperFaction = cc.m_sFaction;
		
		//enemy arrays
		array<ref SDRC_PlayerPos> enemyPosArray = {};
		array<AIGroup> enemyGroups = {};
		
		//Search for player enemies
		if (IsSearchOnlyPlayer(owner))
		{
			SDRC_PlayerHelper.GetPlayersClosestToPosition(enemyPosArray, owner.GetOrigin(), rayLen);
		}

		//Search for AI enemies
		if (IsSearchOnlyAI(owner))
		{
			//Find all AI characters of near position			
			array<AIGroup> groups = {};
			
			SDRC_AIHelper.GroupFindAll(groups);
			
			foreach (int i, AIGroup group : groups)
			{
				if (vector.Distance(group.GetOrigin(), owner.GetOrigin()) < rayLen)
				{
					enemyGroups.Insert(group);
					i++;
					//Found
//					FactionKey factionKey = SDRC_AIHelper.GetGroupFactionKey(group);
					
				}
				
				//Find only 10 groups as max
				if (i > 10)
				{
					break;
				}
			}
		}
		
		//First search for players. These are priority enemy.
		if (!enemyPosArray.IsEmpty())
		{
			foreach (SDRC_PlayerPos enemyPos : enemyPosArray)
			{
				//TBD: Check that the target is an enemy from faction check.
				
				Faction targetFaction = SDRC_PlayerHelper.GetPlayerFaction(enemyPos.player);
				string targetFactionKey = targetFaction.GetFactionKey();
				
				if (SDRC_FactionHelper.IsEnemies(targetFactionKey, chopperFaction))
				{
					//Trace if there is an entity, like house, blocking. Trace will also stop on vehicles and other temporary obstacles.
					vector traceStartPos = owner.GetOrigin();
					vector traceEndPos = enemyPos.pos;
					if (SDRC_Math.IsTargetInLos(traceStartPos, traceEndPos, owner))
					{
						enemyPosition = enemyPos.pos;
					}
				}
			}
		}		
		
		//If no players found, search for enemy AI				
		if (enemyPosition == vector.Zero)
		{
			if (!enemyGroups.IsEmpty())
			{
				foreach (AIGroup group : enemyGroups)
				{
					Faction faction = SCR_AIGroup.Cast(group).GetFaction();
					string targetFactionKey = faction.GetFactionKey();
					if (SDRC_FactionHelper.IsEnemies(targetFactionKey, chopperFaction))
					{
						enemyPosition = group.GetOrigin();
						break;
					}					
				}
			}
		}
		
		return enemyPosition;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if there is an enemy in front of us. It needs to be within the sector defined for the chopper.
	
	If enemy is found, shoot a rocket.
	*/
	static void SearchEnemyForRocket(IEntity owner)
	{
		vector enemyPosition = DoEnemySearch(owner);	

		if (enemyPosition == vector.Zero)
		{
			return;
		}
		
		EnemyFoundForRocket(owner, enemyPosition);
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Enemy has been found. If we're aligned properly, shoot a rocket.
	*/
	static void EnemyFoundForRocket(IEntity owner, vector enemyPosition)
	{
		//SDRC_Log.Add("[SDRC_ChopperEnemyHelper:SearchEnemyForRocket] Enemy found.", LogLevel.DEBUG);

		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}
		
		vector fwd = owner.GetTransformAxis(2);
		fwd.Normalize();
		
//		if (SDRC_Math.IsTargetInSector(owner.GetOrigin(), fwd, enemyPosition, 35) )
		if (SDRC_Math.IsTargetInSector(owner.GetOrigin(), fwd, enemyPosition, chopperComp.m_RocketSector) )
		{
			ShootRocket(owner, enemyPosition);
		}		
	}
			
	//------------------------------------------------------------------------------------------------
	/*!
	Shoot the rocket. 
	
	This will spawn a rocket projectile towards target. The accuracy is dependent on AI skill defined for the chopper
	
	Rockets tested:
	-  {ECD8628EBF7E5F6B}Prefabs/Weapons/Ammo/Ammo_Rocket_Hydra70.et
	- {072A755D5CB85D47}Prefabs/Weapons/Ammo/Ammo_Rocket_Hydra70_HE_M229.et
	- {61AF60E0235DC3B1}Prefabs/Weapons/Ammo/Ammo_Rocket_Hydra70_HEDP_M247.et
	
	- {C9A1612DC5340613}Prefabs/Weapons/Ammo/Ammo_Rocket_S5.et
	- {EF17BED6DCEE4DE4}Prefabs/Weapons/Ammo/Ammo_Rocket_S5_FRAG_S5MO.et
	- {EE65544BA845C458}Prefabs/Weapons/Ammo/Ammo_Rocket_S5_HEDP_S5KO.et
	*/
	static void ShootRocket(IEntity owner, vector targetPos)
	{
		const float AI_ERROR = 30;
		
		if (!owner)
		{
			return;
		}
		
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}

		ResourceName rocketPrefab = chopperComp.m_RocketPrefab;
		if (rocketPrefab == "")
		{
			//No rocket selected
			return;
		}
		
		if (chopperComp.m_RocketCount == 0)
		{
			//Out of rockets
			return;
		}
		
		// Get ground height at target position
//		float groundY = SDRC_Misc.GetSurfaceYWithWater(targetPos);
//		targetPos[1] = groundY + 10;
					
		//Randomize the position depending on AI skill.			
		float targetError = ( (100 - chopperComp.m_AISkill)/100 ) * AI_ERROR;
		targetPos = SDRC_Misc.RandomizePos(targetPos, targetError);
		//Move rocket position up or down according to [1]
		targetPos[1] = targetPos[1] + chopperComp.m_RocketPosition[1];

		float distance = vector.Distance(owner.GetOrigin(), targetPos);
		
		if (distance > chopperComp.m_RocketRange)
		{
			//Too far away
			return;
		}
		
//		vector rocketSpawnPos = SDRC_ChopperHelper.GetDestinationForward(owner, 10);
		vector rocketSpawnPos = SDRC_ChopperHelper.GetDestinationForward(owner, chopperComp.m_RocketPosition[0]);
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.Transform[3] = rocketSpawnPos;
		params.TransformMode = ETransformMode.WORLD;
		
		IEntity rocket = GetGame().SpawnEntityPrefab(Resource.Load(rocketPrefab), GetGame().GetWorld(), params);
		SDRC_Math.TurnEntityTowards(rocket, targetPos);
		
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
				//One rocket shot
				chopperComp.m_RocketCount--;
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