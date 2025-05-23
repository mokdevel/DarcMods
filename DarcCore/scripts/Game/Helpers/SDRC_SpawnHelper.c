//Helpers SDRC_SpawnHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for spawning entities (items) on map. 
*/

//------------------------------------------------------------------------------------------------
sealed class SDRC_SpawnHelper
{
	const float SIZEDIV = 1.8;
	const float EMPTY_POS_RADIUS = 50;	//How far from the center the spawned position can be.
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Spawn a resource on the map.
	\param pos Location on map
	\param resourceName Name of the resource to spawn
	\param rotation The angle (0-360) to rotate the item
	\param emptyPosRadius How far from the center the spawned position can be. If set to -1, spawns to exact position
	\param snap Snap the object to ground when spawning
	*/
	static IEntity SpawnItem(vector pos, string resourceName, float rotation = 0, float emptyPosRadius = EMPTY_POS_RADIUS, bool snap = true)
	{
		IEntity entity = NULL;
		vector posFixed;
        EntitySpawnParams params = EntitySpawnParams();
		
		vector sums = FindPrefabSize(resourceName);
		if (sums == "0 0 0")
			return null;
		
		Resource resource = Resource.Load(resourceName);
		if (!resource.IsValid())
			return null;
			
		if (emptyPosRadius > -1)
		{
			//Spawn the resource to a free spot close to pos
			posFixed = FindEmptyPos(pos, emptyPosRadius, (SDRC_Misc.FindMaxValue(sums)/SIZEDIV));
			if (posFixed != "0 0 0")
			{
				vector transform[4];
				GetTransformFromPosAndRot(transform, posFixed, rotation, snap);
				params.Transform = transform;
				
				entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
				RebuildNavmesh(entity);
				
				SDRC_Log.Add("[SDRC_SpawnHelper:SpawnItem] Entity spawned to: " + posFixed, LogLevel.DEBUG);
			}
			else
			{
				SDRC_Log.Add("[SDRC_SpawnHelper:SpawnItem] Could not find an empty spot for " + resourceName, LogLevel.ERROR);			
			}
		}
		else
		{
			//Spawn the resource exactly to pos
			vector transform[4];
			GetTransformFromPosAndRot(transform, pos, rotation, snap);
	        params.TransformMode = ETransformMode.WORLD;			
	        params.Transform = transform;
			entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
			RebuildNavmesh(entity);
			
			SDRC_Log.Add("[SDRC_SpawnHelper:SpawnItem] Entity spawned to exact position: " + pos, LogLevel.DEBUG);
		}
		return entity;
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Spawn an item inside a building.
	
	The spawn point is chosen randomly on the floors. Sometimes the randomess sets them outside.
	
			|------------------------------------------| House size
		               |---------||---------|            Random spot is 1/6 of house size from the center
		          |----*----|                            Radius to search for a spot is 1/5 of house size
	
	*/
	static IEntity SpawnItemInBuilding(IEntity building, string item, float rotation = 0, float emptyPosRadius = EMPTY_POS_RADIUS, bool snap = true)
	{
		IEntity entity = NULL;
		array<vector> floors = {};
		vector pos, floorpos;
		
		SDRC_BuildingHelper.FindBuildingFloors(floors, building);

		//Find the building size. The bigger X or Y value will be used as the radius
		vector sums = SDRC_SpawnHelper.FindEntitySize(building);
		//Pick the radius to be the bigger one from X/Y
		float radius = sums[0];
		if (sums[0] < sums[2])
		{
			radius = sums[2];
		}
	
		float empty_radius = 1.2;

		if (!floors.IsEmpty())
		{
			floorpos = floors.GetRandomElement();
		}
		else
		{
			floorpos = building.GetOrigin();
			ResourceName res = building.GetPrefabData().GetPrefabName();
			SDRC_Log.Add("[SDRC_SpawnHelper:SpawnItemInBuilding] No floors found from: " + res + " . Spawn will be interesting...", LogLevel.SPAM);
		}
		pos = SDRC_Misc.RandomizePos(floorpos, radius/6);
		pos = SDRC_SpawnHelper.FindEmptyPos(pos, radius/5, emptyPosRadius);
		pos[1] = pos[1] + 0.1;			
		SDRC_DebugHelper.AddDebugSphere(pos, Color.PINK, empty_radius);
		entity = SpawnItem(pos, item, rotation, emptyPosRadius, snap);
		
		return entity;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Despawn an entity
	*/
	static void DespawnItem(IEntity entity)
	{
		SCR_EntityHelper.DeleteEntityAndChildren(entity);
		//RebuildNavmesh(entity);	//TBD: Not sure if this is needed
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Find size of a prefab
	*/
	static vector FindPrefabSize(ResourceName resourceName)
	{
		IEntity entity = NULL;
//		vector posFixed;
		vector posNone = {0,1000,0};
		vector mins, maxs, sums;
        EntitySpawnParams params = EntitySpawnParams();
		
		Resource resource = Resource.Load(resourceName);
		if (!resource.IsValid())
			return "0 0 0";

		//Find the proper size of the resource to spawn. 
        params.TransformMode = ETransformMode.WORLD;			
        params.Transform[3] = posNone;
		entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		entity.GetWorldBounds(mins, maxs);
		sums = maxs - mins;		

		SDRC_Log.Add(string.Format("[SDRC_SpawnHelper:FindPrefabSize] Itemsize: %1, X: %2, Y: %3, Z: %4, S: %5", SDRC_Misc.FindMaxValue(sums), maxs[0]-mins[0], maxs[1]-mins[1], maxs[2]-mins[2], sums), LogLevel.SPAM);							
				
		//Delete the unnecessary resource that we used for getting the bounding box. Wait for it to initialize properly.
		GetGame().GetCallqueue().CallLater(DespawnItem, 10000, false, entity);
		
		return sums;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Find size of an entity
	*/
	static vector FindEntitySize(IEntity entity)
	{
		vector mins, maxs, sums;
		entity.GetWorldBounds(mins, maxs);
		sums = maxs - mins;
		
		return sums;
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Spawns a structure or structures, for example a building on the map.
	\param structures Array of structures to spawn
	\param pos Location on map. 
	\param rotation Rotation of the object around origo
	\param index Index for the individual item to spawn. -1 will spawn all.
	\param emptyPosRadius How far from the center the spawned position can be. If set to -1, spawns to exact position. 
	*/
	static IEntity SpawnStructures(array<ref SDRC_Structure> structures, vector pos = "0 0 0", float rotation = 0, int index = -1, float emptyPosRadius = -1)
	{
		IEntity entity;
		
		if (index == -1)
		{
			foreach (SDRC_Structure structure : structures)
			{
				vector newPos = RotatePosAroundPivot(structure.GetPosition(), "0 0 0", rotation);
				
				entity = SpawnItem(newPos + pos, structure.GetResource(), structure.GetRotationY() + rotation, emptyPosRadius);
				if (!entity)
				{
					SDRC_Log.Add("[SDRC_SpawnHelper:SpawnStructures] Could not spawn: " + structure.GetResource(), LogLevel.ERROR);			
				}
			}
			return null;	//Return null as we spawned multiple structures
		}
		else
		{
			vector newPos = RotatePosAroundPivot(structures[index].GetPosition(), "0 0 0", rotation);
		
			entity = SpawnItem(newPos + pos, structures[index].GetResource(), structures[index].GetRotationY() + rotation, emptyPosRadius);
//			entity = SpawnItem(structures[index].GetPosition() + pos, structures[index].GetResource(), 0, emptyPosRadius);
			if (!entity)
			{
				SDRC_Log.Add("[SDRC_SpawnHelper:SpawnStructures] Could not spawn: " + structures[index].GetResource(), LogLevel.ERROR);			
			}
			return entity;	//Return entity of spawned individual structure
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Resets coordinaters of structures around "0 0 0" (or around given pos)
	\param structures Array of structures to spawn
	\param pos The location to move to.
	*/
	static void SetStructuresToOrigo(array<ref SDRC_Structure> structures, vector pos = "0 0 0")
	{
		vector avgPos = "0 0 0";
		int i = 0;
		
		if (structures.IsEmpty())
		{
			return;
		}	
		
		foreach (SDRC_Structure structure : structures)		
		{
			avgPos = avgPos + structure.GetPosition();
			i++;
		}
		avgPos[0] = avgPos[0]/i;
		avgPos[1] = avgPos[1]/i;
		avgPos[2] = avgPos[2]/i;
		SDRC_Log.Add("[SDRC_SpawnHelper:SetStructuresToOrigo] avgPos = " + avgPos, LogLevel.SPAM);		
		
		foreach (SDRC_Structure structure : structures)		
		{
			structure.SetPosition(structure.GetPosition() - avgPos + pos);
			SDRC_Log.Add("[SDRC_SpawnHelper:SetStructuresToOrigo] pos = " + structure.GetPosition(), LogLevel.SPAM);		
		}
	}
			
	//------------------------------------------------------------------------------------------------
	/*!
	Rebuilds the navmesh around the entity
	*/
	static void RebuildNavmesh(IEntity entity)
	{
		SCR_AIWorld aiWorld = SCR_AIWorld.Cast(GetGame().GetAIWorld());
		if (aiWorld)
		{
			aiWorld.RequestNavmeshRebuildEntity(entity);
		}
		else
		{
			SDRC_Log.Add("[SDRC_SpawnHelper:RebuildNavmesh] Could not create navmesh.", LogLevel.DEBUG);
		}
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Get transform from the given position and rotation in XZ plane
	*/
	static void GetTransformFromPosAndRot(out vector transform[4], vector pos, float rotation, bool snap = true)
	{
		Math3D.MatrixIdentity3(transform);
		Math3D.AnglesToMatrix(Vector(rotation, 0, 0), transform);
		transform[3] = pos;
		if (snap)
		{
			SCR_TerrainHelper.SnapAndOrientToTerrain(transform);
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Rotate a position around a pivot point
	*/
	static vector RotatePosAroundPivot(vector pos, vector pivot, float rotation)
	{
		vector transform[4];
		vector newtransform[4];

		Math3D.MatrixIdentity3(transform);
		transform[3] = pos;
						
		SCR_Math3D.RotateAround(transform, pivot, "0 1 0", SDRC_Misc.AngleToRadians(rotation), newtransform);			
		
		return newtransform[3];		
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find an empty spot within areaRadius to fit emptySize
	*/
	static vector FindEmptyPos(vector pos, float areaRadius, float emptySize)
	{		
		vector posFixed;
		
		if (SCR_WorldTools().FindEmptyTerrainPosition(posFixed, pos, areaRadius, emptySize, 2, TraceFlags.ENTS|TraceFlags.WORLD|TraceFlags.OCEAN))
		{
			SDRC_Log.Add("[SDRC_SpawnHelper:FindEmptyPos] Found: " + posFixed, LogLevel.SPAM);			
			return posFixed;
		}
		
		SDRC_Log.Add("[SDRC_SpawnHelper:FindEmptyPos] Empty spot not found. Using original.", LogLevel.DEBUG);			
		return pos;
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	*/
	static void DisableVehicleArsenal(IEntity entity, ResourceName resourceName, bool disable)
	{	
		//Disable arsenal
		string tempEntity = resourceName;
		tempEntity.ToLower();
		if (disable && (tempEntity.Contains("vehicle")) )
		{
			SDRC_SpawnHelper.DisableArsenal(entity);
		}		
	}
		
	//------------------------------------------------------------------------------------------------
	/*! 
	Disable arsenal on an entity
	
	TBD: This works in mysterious ways. If you disable the Arsenal in SlotManagerComponent of the prefan, it is not visible in the game. 
	I have not found a way to do the same via script. So, we disable the arsenal. The content are cleared, but you can still see the empty arsenal.
	*/	
	static void DisableArsenal(IEntity entity)
	{	
		SCR_ArsenalComponent arsenalComponent;
			
		// Disable arsenal
		arsenalComponent = SCR_ArsenalComponent.FindArsenalComponent(entity);
		if (arsenalComponent)
		{				
			arsenalComponent.SetArsenalEnabled(false, false);
//			arsenalComponent.SetSupportedArsenalItemModes(0);
				
			SDRC_Log.Add("[SDRC_LootHelper:DisableArsenal] Disabling arsenal. Type: " + arsenalComponent.GetArsenalType(), LogLevel.SPAM);
		}			

		// Disable virtual arsenals					
		SlotManagerComponent slotManager = SlotManagerComponent.Cast(entity.FindComponent(SlotManagerComponent));
		if (slotManager)
		{
			array<EntitySlotInfo> slots = {};
			slotManager.GetSlotInfos(slots);			

			foreach (EntitySlotInfo slot : slots)
			{
				if (!slot)
					continue;

				//Print out the slot names
				//string str = slot.GetSourceName();
				//SDRC_Log.Add("[SDRC_LootHelper:DisableArsenal] Slot name: " + str, LogLevel.DEBUG);
				IEntity arsenal;
				arsenal = slot.GetAttachedEntity();

				if (!arsenal)
					continue;

//				arsenalComponent = SCR_ArsenalComponent.Cast(arsenal.FindComponent(SCR_ArsenalComponent));
				arsenalComponent = SCR_ArsenalComponent.FindArsenalComponent(arsenal);
				if (arsenalComponent)
				{
					arsenalComponent.SetArsenalEnabled(false, false);
//					arsenalComponent.SetSupportedArsenalItemModes(0);
					SDRC_Log.Add("[SDRC_LootHelper:DisableArsenal] Disabling virtual arsenal. Type: " + arsenalComponent.GetArsenalType(), LogLevel.SPAM);
				}
			}
		}		
/*		
		// TBD: Did not change the behaviour
		SCR_ResourceComponent resourceComponent;
		
		resourceComponent = SCR_ResourceComponent.FindResourceComponent(entity);
		if (resourceComponent)
		{
			resourceComponent.SetResourceTypeEnabled(false);
			resourceComponent.SetIsVisible(false);
			SCR_ResourceEncapsulator enc = resourceComponent.GetEncapsulator(EResourceType.SUPPLIES);
			if (enc)
			{
				enc.SetResourceRights(EResourceRights.NONE);
			}
		}
		
		SCR_ResourceContainer resourceContainer;			
		resourceContainer = SCR_ResourceContainer.Cast(entity.FindComponent(SCR_ResourceContainer));
		if (resourceContainer)
		{
			resourceContainer.SetResourceValue(0, true);
			resourceContainer.SetResourceValueUnsafe(0, true);
			resourceContainer.SetMaxResourceValue(0, true);
			resourceContainer.SetOnEmptyBehavior(EResourceContainerOnEmptyBehavior.DELETE);
		}*/
	}
}