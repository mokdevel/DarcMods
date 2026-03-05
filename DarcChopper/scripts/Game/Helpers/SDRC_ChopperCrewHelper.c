//SDRC_ChopperCrewHelper.c

//------------------------------------------------------------------------------------------------
class SDRC_ChopperCrewHelper
{
	//------------------------------------------------------------------------------------------------	
	// Crew functions
	//------------------------------------------------------------------------------------------------	
	static int SpawnCrew(IEntity owner, array<ref SCR_DefaultOccupantData> crewmember, string faction, EAISkill skill = EAISkill.REGULAR, float perceptionFactor = 1.0)
	{		
		int pilotCount = SDRC_VehicleHelper.GetCompartmentCountOfType(owner, ECompartmentType.PILOT);
		int crewCount = 0;
		
		array<ResourceName> crewPrefabs = {}; 
		
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
					SDRC_Log.Add("[SDRC_ChopperHelper:SpawnCrew] Vehicle faction: " + faction, LogLevel.DEBUG);
				}
			}
		}
		
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
			
			//Add 1-4 random additional riflemen
			for (int i = 0; i < SDRC_Misc.RandomInt(4, 8); i++)
			{
				ResourceName member = SDRC_EnemyHelper.SelectEnemy("C_RIFLEMAN", faction);
				if (member == "")
				{
					member = "{472F2B06FF9BF37D}Prefabs/Characters/Factions/CIV/Dockworker/Character_CIV_Dockworker_4.et";
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
						
		//Add the crew
		if (crewPrefabs.Count() > 0)
		{			
			SCR_AIGroup	gPilot;
			SCR_AIGroup	gCrew;
			
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
				}
				else
				{
					if (!gCrew)
					{
						gCrew = SDRC_AIHelper.GroupCreate(faction, pos);						
					}
					SDRC_VehicleHelper.SpawnGroupInVehicle(prefab, owner, gCrew);
				}
				
				crewCount++;
			}
			
			//Set AI skill
			if (gPilot)
			{
				SDRC_AIHelper.SetAIGroupSettings(gPilot, skill, perceptionFactor);
			}
			if (gCrew)
			{
				SDRC_AIHelper.SetAIGroupSettings(gPilot, skill, perceptionFactor);
			}
			
		}
		else
		{
			SDRC_Log.Add("[SDRC_ChopperHelper:SpawnCrew] No crew defined. Without pilots, we will crash.", LogLevel.WARNING);
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

		foreach (SCR_AIGroup group : groups)
		{
			SDRC_Log.Add("[SDRC_ChopperComp:HandleState] Create waypoint for AI group: " + group, LogLevel.DEBUG);			
			
			vector pos = SDRC_Misc.RandomizePos(owner.GetOrigin(), 75);			
			SDRC_DebugHelper.AddDebugPos(pos);
			SDRC_WPHelper.CreateWaypoint(group, pos, SDRC_EWaypointMoveType.MOVE);
			
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
	
	//------------------------------------------------------------------------------------------------
	static void SetPilotAndGunnerActive(IEntity owner, bool activate)
	{
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