/**********************************************************************************************************************
 ************************************************* FREEDOM FIGHTERS ***************************************************
 **********************************************************************************************************************
 * This file is part of the Freedom Fighters project, published under modified APL-ND license. You are free to adapt  *
 * (i.e. modify, rework or update) and share (i.e. copy, distribute or transmit) the material under the following     *
 * conditions:                                                                                                        *
 * - Attribution - You must attribute the material in the manner specified by the author or licensor (but not in any  *
 *   way that suggests that they endorse you or your use of the material).                                            *
 * - Noncommercial - You may not use this material for any commercial purposes.                                       *
 * - Arma Only - You may not convert or adapt this material to be used in other games than Arma.                      *
 * - No Derivatives - If you remix, transform, or build upon the material, you may not distribute the modified        *
 *   material.                                                                                                        *
 * The above list is only a highlight and is NOT an exhaustive list of restrictions. Please visit the following URL   *
 * to view the full text of the license:                                                                              *
 *    https://www.johnnykerner.dev/FreedomFighters/project-license/20250916/                                          *
 **********************************************************************************************************************
 * For more info or to contact the author, visit the project website:                                                 *
 *    https://www.johnnykerner.dev/FreedomFighters/                                                                   *
 ******************************************************************************************************************** */

//------------------------------------------------------------------------------------------------
// This component handles chopper attacks during (town) battles.
//
// This is attached to the battle controller entity.
//------------------------------------------------------------------------------------------------

[ComponentEditorProps(category: "JWK/FreedomFighters", description: "")]
class SDRC_JWK_ChopperBattleControllerComponentClass: JWK_EntityComponentClass
{
}

class SDRC_JWK_ChopperBattleControllerComponent: JWK_EntityComponent
{
	static const int REFRESH_INTERVAL_MS = 20000;
	
	private ref SDRC_JsonApi2 m_JsonApi = new SDRC_JsonApi2(DC_COMPATCONFIG_CHOPPER_FILE);	
	private ref SDRC_ChopperCompatFFConfig m_Config = new SDRC_ChopperCompatFFConfig();
	private ref SDRC_ChopperCompatFF m_DC_Attack = new SDRC_ChopperCompatFF();	
	
	private string m_sFaction;
	private ref array<SCR_AIGroup> m_Groups = {};		//Groups spawned
	private int m_iChopperCount;
	private vector m_vPosOrigin;						//Spawn position
	private vector m_vPosDestination;					//Battle position
	private ref array<IEntity> m_aVehicles = {};		//Vehicles spawned
	
	// --------------------------------------------------------------------------------------
	
	protected JWK_BattleControllerEntity m_BattleController;
	
	// --------------------------------------------------------------------------------------
	
	//void SDRC_JWK_ChopperBattleControllerComponent(IEntityComponentSource src, IEntity ent, IEntity parent) {}
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);	
		
		m_BattleController = JWK_BattleControllerEntity.Cast(owner);
		if (!m_BattleController) {
			JWK_Log.Log(this, "Must be attached to JWK_BattleControllerEntity!", LogLevel.ERROR);
			return;
		}
		
		//Load config
		if (!m_JsonApi.Load(m_Config, SDRC_ChopperCompatFFConfig.Cast(m_Config), DC_COMPATCONFIG_CHOPPER_FILE_JSONVER))
		{
			//Error
			return;
		}
		
		//Do a random check to see if choppers spawn with a chance of progress + spawnChance.
		float chance = JWK.GameSettingsCache().GetBalanceProgress() + m_Config.spawnChance;
		if (SDRC_Misc.RandomFloat(0, 1) > chance)
		{
			SDRC_Log.Add("[SDRC_JWK_ChopperBattleControllerComponent:OnPostInit] No choppers assigned to battle. Chance: " + chance * 100 + "%.", LogLevel.NORMAL);
			return;
		}
		
		m_iChopperCount = m_Config.chopperCount.GetRandomElement();
		
		if (m_iChopperCount == 0)
		{
			SDRC_Log.Add("[SDRC_JWK_ChopperBattleControllerComponent:OnPostInit] No choppers assigned to battle.", LogLevel.NORMAL);
			return;
		}
		
		m_vPosDestination = owner.GetOrigin();
		
		SDRC_Log.Add("[SDRC_JWK_ChopperBattleControllerComponent:OnPostInit] Choppers joining the battle: " + m_iChopperCount, LogLevel.NORMAL);
		
		for (int i = 0; i < m_iChopperCount; i++)
		{
			int delay = SDRC_Misc.RandomInt(m_Config.spawnDelay[0], m_Config.spawnDelay[1]);
			GetGame().GetCallqueue().CallLater(SpawnChopper, (delay * 60 * 1000) + 100, false);
			SDRC_Log.Add("[SDRC_JWK_ChopperBattleControllerComponent:OnPostInit] Chopper joining in " + delay + " minutes.", LogLevel.DEBUG);
		}

		m_BattleController.GetOnBattleStarted_S().Insert(OnBattleStarted_S);
		m_BattleController.GetOnWiped_S().Insert(OnWiped_S);
	}
	
	override void OnDelete(IEntity owner)
	{
		super.OnDelete(owner);
		
		if (GetFramework())
			GetFramework().GetCallQueue().Remove(Refresh_S);

		//Do despawn		
	}
	
	protected void OnBattleStarted_S(
		JWK_BattleControllerEntity controller,
		JWK_BattleSubjectComponent subject,
	) {
		GetFramework().GetCallQueue().CallLater(Refresh_S, REFRESH_INTERVAL_MS, true);
		Refresh_S();
	}
	
	protected void OnWiped_S(JWK_BattleControllerEntity controller)
	{
		//Despawn
	}	
	
	protected void Refresh_S()
	{
		
	}
	
	// ----------------------------------------------------------------------------------------------	
	private void SpawnChopper()
	{
		//Pick a configuration for attack
		int idx = m_Config.attackList.GetRandomElement();
		if (idx > m_Config.attackList.Count() - 1)
		{
			//Error
			SDRC_Log.Add("[SDRC_JWK_ChopperBattleControllerComponent:SpawnChopper] Incorrect index in attackList: " + idx, LogLevel.ERROR);
			return;
		}
		m_DC_Attack = m_Config.attacks[idx];

		//Select faction
		m_sFaction = m_Config.factions.GetRandomElement();
		if (m_sFaction == "FF")
		{
			JWK_Faction enemyFaction = JWK_Faction.GetByRole(JWK_EFactionRole.ENEMY);
			m_sFaction = enemyFaction.GetKey();
		}
		
		if (m_sFaction == "")
		{
			//Could not spawn vehicle
			SDRC_Log.Add("[SDRC_JWK_ChopperBattleControllerComponent:SpawnChopper] Could not select faction.", LogLevel.ERROR);
			return;			
		}
		
		string resourceName	= SDRC_SpawnHelper.SelectResourceName(m_DC_Attack.heliList);		
		if (resourceName[0] != "{")
		{
			resourceName = SDRC_VehicleListHelper.FindVehicleItem(resourceName, m_sFaction);
		}
		
		//Set to initial position, rotation and spawn
		m_vPosOrigin = SDRC_Misc.GetCoordinatesOnCircle(m_vPosDestination, 500, SDRC_Misc.RandomInt(0, 360));
		m_vPosOrigin[1] = SDRC_Misc.RandomFloat(m_DC_Attack.flyHeight[0], m_DC_Attack.flyHeight[1]) + SDRC_Misc.GetSurfaceYWithWater(m_vPosOrigin);		
		
		ref IEntity vehicle = SDRC_SpawnHelper.SpawnItem(m_vPosOrigin, resourceName, 0, -1, false);

		if (!vehicle)
		{
			//Could not spawn vehicle
			SDRC_Log.Add("[SDRC_JWK_ChopperBattleControllerComponent:SpawnChopper] Could not spawn vehicle: " + resourceName, LogLevel.ERROR);
			return;			
		}
				
		SDRC_ChopperComp vehicle_c = SDRC_ChopperComp.Cast(vehicle.FindComponent(SDRC_ChopperComp));
		
		if (!vehicle_c)
		{
			//Could not spawn vehicle
			SDRC_Log.Add("[SDRC_JWK_ChopperBattleControllerComponent:SpawnChopper] SDRC_ChopperComp not available in: " + resourceName, LogLevel.ERROR);
			return;			
		}
		
		vehicle_c.SetAutostart(false);
		vehicle_c.SetHeli(m_DC_Attack.speed[0], m_DC_Attack.speed[1], m_DC_Attack.flyHeight[0], m_DC_Attack.flyHeight[1], m_DC_Attack.flyDistance[0], m_DC_Attack.flyDistance[1]);
		vehicle_c.SetEnemySearchType(m_DC_Attack.enemyType);
		vehicle_c.Setup(vehicle);
		SDRC_Math.TurnEntityTowardsXZ(vehicle, m_vPosDestination);
		
		int attackTime = SDRC_Misc.RandomInt(m_Config.attackTime[0], m_Config.attackTime[1]);
		vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_SEARCH_DESTROY, m_vPosDestination, (attackTime * 60) );
		//vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_SEARCH_DESTROY, m_vPosDestination, 10 );
		vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_FLY_AWAY);		
		vehicle_c.AddDestination(SDRC_EFlyWayPointType.WP_DESPAWN);		
		
		m_aVehicles.Insert(vehicle);		
		
		GetGame().GetCallqueue().CallLater(SpawnCrew, 2 * 1000, false, vehicle);
		
		return;
	}

	// ----------------------------------------------------------------------------------------------		
	private void SpawnCrew(IEntity vehicle)
	{
		//Select pilots
		ResourceName pilot = SDRC_EnemyHelper.SelectEnemy("C_CREW", m_sFaction);

		if (pilot == "")
		{
			//If pilots not available, let's spawn a regular rifleman as a pilot
			pilot = SDRC_EnemyHelper.SelectEnemy("C_RIFLEMAN", m_sFaction);
		}		

		//Spawn pilots if such is available 
		if (pilot != "")
		{
			SCR_AIGroup group = SDRC_AIHelper.GroupCreate(m_sFaction, m_vPosOrigin);
			
			for (int i = 0; i < 2; i++)
			{		
				SDRC_VehicleHelper.SpawnGroupInVehicle(pilot, vehicle, group, m_sFaction);
				
				if (group)
				{			
					//SDRC_AIHelper.SetAIGroupSettings(group, m_DC_Attack.ai.GetSkill(GetDifficulty()), m_DC_Attack.ai.GetPerception(GetDifficulty()));
					m_Groups.Insert(group);					
				}
			}
		}
		
		//Spawn mission AI
		int aiCount = SDRC_Misc.RandomInt(m_DC_Attack.ai.count[0], m_DC_Attack.ai.count[1]); 
		
		for (int i = 0; i < aiCount; i++)
		{		
			string groupToSpawn = m_DC_Attack.ai.types.GetRandomElement();
			ResourceName aiType = SDRC_EnemyHelper.SelectEnemy(groupToSpawn, m_sFaction);
			
			SCR_AIGroup group = SDRC_AIHelper.GroupCreate(m_sFaction, m_vPosOrigin);
			SDRC_VehicleHelper.SpawnGroupInVehicle(aiType, vehicle, group, m_sFaction);
			
			if (group)
			{			
				SDRC_AIHelper.SetAIGroupSettings(group, m_DC_Attack.ai.skill, m_DC_Attack.ai.perception);
				m_Groups.Insert(group);					
			}
		}		
		
		//All done, activate
		SDRC_ChopperComp vehicle_c = SDRC_ChopperComp.Cast(vehicle.FindComponent(SDRC_ChopperComp));		
		vehicle_c.Ready(vehicle);
		
		SDRC_Log.Add("[SDRC_JWK_ChopperBattleControllerComponent] Chopper joined the battle: " + vehicle, LogLevel.NORMAL);
	}
}
