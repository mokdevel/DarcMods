//SDRC_ChopperCrewHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_ChopperCrewHelper
{
	//------------------------------------------------------------------------------------------------	
	// Crew functions
	//------------------------------------------------------------------------------------------------	
	static int SpawnCrew(IEntity owner, SDRC_EHeliCargoSeatFill cargoSeatFill, array<ref SCR_DefaultOccupantData> crewmember, string faction, EAISkill skill = EAISkill.REGULAR, float perceptionFactor = 1.0)
	{				
		const int CREW_SIZE = 3; 	//The size of a team to split the cargo passengers to. This will be the group size.
		
		if (!SDRC_Misc.IsMaster())
		{
			SDRC_Log.Add("[SDRC_ChopperCrewHelper:SpawnCrew] Client shall not spawn AI", LogLevel.DEBUG);
			return -1;
		}
		
		int pilotCount = SDRC_VehicleHelper.GetCompartmentCountOfType(owner, ECompartmentType.PILOT);
		int gunnerCount = SDRC_VehicleHelper.GetCompartmentCountOfType(owner, ECompartmentType.TURRET);
		int cargoCount = SDRC_VehicleHelper.GetCompartmentCountOfType(owner, ECompartmentType.CARGO);
		
		int crewCount = 0;		
		array<ResourceName> crewPrefabs = {}; 
		
		string factionReason = "";
		
		if ( SDRC_Misc.IsAddonLoaded("$DarcMissions:") || SDRC_Misc.IsAddonLoaded("$DarcMissionsDev:") )
		{
			//Use default faction for DarcMissions
			faction = SDRC_EnemyHelper.SelectEnemyFaction(faction);
			factionReason = "DarcMissions";
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
						factionReason = "Vehicle default";
					}
				}
				else
				{
					factionReason = "user defined on vehicle";
				}
			}
		}

		SDRC_Log.Add("[SDRC_ChopperCrewHelper:SpawnCrew] Faction used: " + faction + " - selected from: " + factionReason, LogLevel.DEBUG);
				
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
				case SDRC_EHeliCargoSeatFill.HIGH:
				{
					cargoCrewCount = SDRC_Misc.RandomInt(cargoCount/2, cargoCount);
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
		
		//The first AI to spawn, gets his own group. This is to fix the MI28 middle guys issue that just wants to sit there and get out.
		int crewInGroupCount = CREW_SIZE;
		
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
					SDRC_VehicleHelper.SpawnGroupInVehicle(prefab, owner, gPilot, faction, ECompartmentType.PILOT);
				}
				else if (i < (pilotCount + gunnerCount))
				{
					if (!gGunner)
					{
						gGunner = SDRC_AIHelper.GroupCreate(faction, pos);						
					}
					SDRC_VehicleHelper.SpawnGroupInVehicle(prefab, owner, gGunner, faction, ECompartmentType.TURRET);
				}
				else
				{
					if ( (!gCrew) || (crewInGroupCount > CREW_SIZE) )
					{
						//If have spawned a group earlier, then we can start the counter from zero. 
						//If not, then continue creating the first group to the max size from crewInGroupCount
						if (gCrew)
						{
							crewInGroupCount = 0;
						}

						gCrew = SDRC_AIHelper.GroupCreate(faction, pos);						
					}
					SDRC_VehicleHelper.SpawnGroupInVehicle(prefab, owner, gCrew, faction);
					crewInGroupCount++;
				}
				
				crewCount++;
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_ChopperCrewHelper:SpawnCrew] No crew defined.", LogLevel.DEBUG);
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
		const int GETOUT_DELAY = 20;	//(seconds) The delay between groups jumping out
		
		array<SCR_AIGroup> groups = {};
	
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return;
		}
		
		SDRC_VehicleHelper.GetOutVehicle(owner, groups);

		//Disable gunner and pilots. Otherwise they try to get out too immediately to engage the enemy. If in evac, everybody shall leave.
		if (chopperComp.GetBehaviour() != SDRC_EHeliBehaviour.EVAC)
		{
			SetPilotAndGunnerActive(owner, false);
			GetGame().GetCallqueue().CallLater(SetPilotAndGunnerActive, groups.Count() * GETOUT_DELAY * 1000 + 5000, false, owner, true);
		}

		foreach (int g, SCR_AIGroup group : groups)
		{
			if ( (g == 0) && (chopperComp.GetBehaviour() != SDRC_EHeliBehaviour.EVAC) )
			{
				//First group is pilots, ignore them .. unless in evac mode
				continue;
			}
			
			SDRC_Log.Add("[SDRC_ChopperCrewHelper:GetOut] Create waypoint for AI group: " + group, LogLevel.DEBUG);			
			
			vector pos = SDRC_Misc.RandomizePos(owner.GetOrigin(), 300);			
			GetGame().GetCallqueue().CallLater(SetWaypointDelayed, 1000 + GETOUT_DELAY * 1000 * (g - 1), false, group, pos);
			
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
	
	//------------------------------------------------------------------------------------------------
	static int CountCrew(IEntity owner)
	{
		SDRC_ChopperComp chopperComp = SDRC_ChopperComp.Cast(owner.FindComponent(SDRC_ChopperComp));
		if (!chopperComp)
		{
			return 0;
		}
		
		int crewCount = 0;
		foreach (AIGroup group : chopperComp.m_aGroups)
		{
			crewCount = crewCount + group.GetAgentsCount();
		}
		
		return crewCount;
	}
}