//SDRC_ChopperCrewHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_ChopperCrewHelper
{
	//------------------------------------------------------------------------------------------------	
	// Crew functions
	//------------------------------------------------------------------------------------------------	
	static int SpawnCrew(IEntity owner, SDRC_EHeliCargoSeatFill cargoSeatFill, array<ref SCR_DefaultOccupantData> crewmember, string faction, EAISkill skill = EAISkill.REGULAR, float perceptionFactor = 1.0)
	{				
		if (!SDRC_Misc.IsMaster())
		{
			SDRC_Log.Add("[SDRC_ChopperCrewHelper:SpawnCrew] Client shall not spawn AI", LogLevel.DEBUG);
			return -1;
		}
		
		int pilotCount = SDRC_VehicleHelper.GetCompartmentCountOfType(owner, ECompartmentType.PILOT);
		int gunnerCount = SDRC_VehicleHelper.GetCompartmentCountOfType(owner, ECompartmentType.TURRET);
		int cargoCount = SDRC_VehicleHelper.GetCompartmentCountOfType(owner, ECompartmentType.CARGO);
		
		//Add one additional pilot and remove him from cargo
		if (cargoCount > 0)
		{					
			//The reason to do this is that MI28 has the guy sitting between pilots. He typically refuses to leave the helicopter.
			pilotCount++;		
			cargoCount--;
		}
		
		int crewCount = 0;		
		array<ResourceName> crewPrefabs = {}; 
		
		if ( SDRC_Misc.IsAddonLoaded("$DarcMissions:") || SDRC_Misc.IsAddonLoaded("$DarcMissionsDEV:") )
		{
			//Use default faction for DarcMissions
			faction = SDRC_EnemyHelper.SelectEnemyFaction(faction);
		}
		else
		{
			//If no faction defined, find the default vehichle faction and use that
			if (faction == "")
			{			
				Vehicle veh = Vehicle.Cast(owner);
				
				if (veh)
				{
					Faction veh_faction = veh.GetDefaultFaction();
					if (veh_faction)
					{
						faction = veh_faction.GetFactionKey();
					}
				}
			}
		}

		SDRC_Log.Add("[SDRC_ChopperHelper:SpawnCrew] Faction used: " + faction, LogLevel.DEBUG);
				
		//Select crew	
		if (crewmember.IsEmpty())
		{
			//Add pilots			
			for (int i = 0; i < pilotCount; i++)
			{
				ResourceName member = SDRC_EnemyHelper.SelectEnemy("C_CREW", faction);
				if (member == "")
				{
					member = "{472F2B06FF9BF37D}Prefabs/Characters/Factions/CIV/Dockworker/Character_CIV_Dockworker_4.et";
				}
				crewPrefabs.Insert(member);
			}
			
			//Add gunners to turrets			
			for (int i = 0; i < gunnerCount; i++)
			{
				ResourceName member = SDRC_EnemyHelper.SelectEnemy("C_RIFLEMAN", faction);
				if (member == "")
				{
					member = "{A2B367FFF37E6416}Prefabs/Characters/Factions/CIV/Dockworker/Character_CIV_Dockworker_5.et";
				}
				crewPrefabs.Insert(member);
			}			
			
			int cargoCrewCount = 0;		//case: SDRC_EHeliCargoSeatFill.NONE
			switch (cargoSeatFill)
			{
				case SDRC_EHeliCargoSeatFill.RANDOM:
				{
					cargoCrewCount = SDRC_Misc.RandomInt(1, cargoCount);
					break;
				}
				case SDRC_EHeliCargoSeatFill.LOW:
				{
					cargoCrewCount = SDRC_Misc.RandomInt(1, cargoCount/2);
					break;
				}
				case SDRC_EHeliCargoSeatFill.HALF:
				{
					cargoCrewCount = cargoCount / 2;
					break;
				}
				case SDRC_EHeliCargoSeatFill.FULL:
				{
					cargoCrewCount = cargoCount;
					break;
				}
			}
			
			//cargoCrewCount = 3;				//REMOVE
			
			//Add random additional riflemen
			for (int i = 0; i < cargoCrewCount; i++)
			{
				ResourceName member = SDRC_EnemyHelper.SelectEnemy("C_RIFLEMAN", faction);
				if (member == "")
				{
					member = "{CEE7531F4FBAEB38}Prefabs/Characters/Factions/CIV/Dockworker/Character_CIV_Dockworker_6.et";
				}
				crewPrefabs.Insert(member);
			}
		}
		else
		{
			//Use the provided crew
			foreach (int i, SCR_DefaultOccupantData member : crewmember)
			{
				crewPrefabs.Insert(member.GetDefaultOccupantPrefab());
			}
		}
		
		SCR_AIGroup	gPilot = null;
		SCR_AIGroup	gGunner = null;
		SCR_AIGroup	gCrew = null;
		
		int crewInGroupCount = 0;
		
		//Add the crew
		if (crewPrefabs.Count() > 0)
		{			
			foreach (int i, ResourceName prefab : crewPrefabs)
			{
				//Skip empty ones
				if (prefab == "")
				{
					continue;
				}
				
				//Spawn pilots if such is available 
				vector pos = owner.GetOrigin();
				pos = pos + "30 0 30";
				
				if (i < pilotCount)
				{
					if (!gPilot)
					{
						gPilot = SDRC_AIHelper.GroupCreate(faction, pos);						
					}
					SDRC_VehicleHelper.SpawnGroupInVehicle(prefab, owner, gPilot);
					//NOTE: We could enforce ECompartmentType.PILOT, but for MI28, we may add one additional pilot for the middle seat.
				}
				else if (i < (pilotCount + gunnerCount))
				{
					if (!gGunner)
					{
						gGunner = SDRC_AIHelper.GroupCreate(faction, pos);						
					}
					SDRC_VehicleHelper.SpawnGroupInVehicle(prefab, owner, gGunner, ECompartmentType.TURRET);
				}
				else
				{
					if ( (!gCrew) || (crewInGroupCount > 3) )
//					if (!gCrew)
					{
						gCrew = SDRC_AIHelper.GroupCreate(faction, pos);						
						crewInGroupCount = 0;
					}
					SDRC_VehicleHelper.SpawnGroupInVehicle(prefab, owner, gCrew);
					crewInGroupCount++;
				}
				
				crewCount++;
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_ChopperHelper:SpawnCrew] No crew defined. Without pilots, we will crash.", LogLevel.WARNING);
		}

		//Set AI skill
		if (gPilot) 
		{
			SDRC_AIHelper.SetAIGroupSettings(gPilot, skill, perceptionFactor);
		}
		if (gGunner)
		{
			SDRC_AIHelper.SetAIGroupSettings(gGunner, skill, perceptionFactor);
		}
		if (gCrew)
		{
			SDRC_AIHelper.SetAIGroupSettings(gCrew, skill, perceptionFactor);
		}
						
		return crewCount;
	}

	//------------------------------------------------------------------------------------------------
	static void GetOut(IEntity owner)
	{
		const int DEACTIVE_TIME = 30;	//How long pilot and gunner are deactivated
		
		array<SCR_AIGroup> groups = {};
	
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}
		
		SetPilotAndGunnerActive(owner, false);
		GetGame().GetCallqueue().CallLater(SetPilotAndGunnerActive, DEACTIVE_TIME * 1000, false, owner, true);

		SDRC_VehicleHelper.GetOutVehicle(owner, groups);

		foreach (int g, SCR_AIGroup group : groups)
		{
			SDRC_Log.Add("[SDRC_ChopperComp:HandleState] Create waypoint for AI group: " + group, LogLevel.DEBUG);			
			
			vector pos = SDRC_Misc.RandomizePos(owner.GetOrigin(), 300);			
			GetGame().GetCallqueue().CallLater(SetWaypointDelayed, 1000 + 8000 * g, false, group, pos);
			
			int index = 0;
			while (index != -1)
			{
				index = chopperComp.m_aGroups.Find(group);
				if (index > -1)
				{
					chopperComp.m_aGroups.RemoveOrdered(index);
				}
			}
		}
	}

	static void SetWaypointDelayed(SCR_AIGroup group, vector pos)
	{
		SDRC_WPHelper.CreateWaypoint(group, pos, SDRC_EWaypointMoveType.GETOUT);
		SDRC_WPHelper.CreateWaypoint(group, pos, SDRC_EWaypointMoveType.MOVE);
		SDRC_DebugHelper.AddDebugPos(pos);
	}
		
	//------------------------------------------------------------------------------------------------
	static void SetPilotAndGunnerActive(IEntity owner, bool activate)
	{
		if (!owner)
		{
			//Heli has been removed before this action happens
			return;
		}
		
		//Find pilots
		SCR_BaseCompartmentManagerComponent scr_compartmentManager = SCR_BaseCompartmentManagerComponent.Cast(owner.FindComponent(SCR_BaseCompartmentManagerComponent));
		
		array<IEntity> occupants = {};
		array<AIGroup> groups = {};
		scr_compartmentManager.GetOccupantsOfType(occupants, ECompartmentType.PILOT);
		scr_compartmentManager.GetOccupantsOfType(occupants, ECompartmentType.TURRET);
		
		//Collect groups in the helicopter 
		foreach (IEntity occupant : occupants)
		{
			AIGroup group = SDRC_AIHelper.GetAIGroup(occupant);
			
			if (group != null)
			{
				if (!groups.Contains(group))
				{
					groups.Insert(group);
				}
			}
		}		
		
		//Enable/disable group
		foreach (AIGroup group : groups)
		{
			SCR_AIGroup AIgroup = SCR_AIGroup.Cast(group);
			SDRC_AIHelper.SetAIGroupEnableDelayed(AIgroup, activate);			
		}	
	}	
}