//Helpers SCR_DC_SpawnHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for spawning entities (items) on map. 
*/

sealed class SCR_DC_SpawnHelper
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
	//pos
	*/
//	static IEntity SpawnItem(vector pos, string resourceName, float rotation, float emptyPosRadius)
	static IEntity SpawnItem(vector pos, string resourceName, float rotation = 0, float emptyPosRadius = EMPTY_POS_RADIUS)
	{
		IEntity entity = NULL;
		vector posFixed;
		vector posNone = {0,1000,0};
		vector mins, maxs, sums;
        EntitySpawnParams params = EntitySpawnParams();
		
		Resource resource = Resource.Load(resourceName);
		if (!resource.IsValid())
			return null;

		//Find the proper size of the resource to spawn. 
        params.TransformMode = ETransformMode.WORLD;			
        params.Transform[3] = posNone;
		entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		entity.GetWorldBounds(mins, maxs);
		sums = maxs - mins;
		
		//Delete the unnecessary resource that we used for getting the bounding box. Wait for it to initialize properly.
		GetGame().GetCallqueue().CallLater(DespawnItem, 10000, false, entity);
		
		if (emptyPosRadius > -1)
		{
			//Spawn the resource to a free spot close to pos
			SCR_DC_Log.Add(string.Format("[SCR_DC_MissionHelper:SpawnItem] Itemsize: %1, X: %2, Y: %3, Z: %4, S: %5", SCR_DC_Misc.FindMaxValue(sums), maxs[0]-mins[0], maxs[1]-mins[1], maxs[2]-mins[2], sums), LogLevel.DEBUG);							
			
			posFixed = FindEmptyPos(pos, EMPTY_POS_RADIUS, (SCR_DC_Misc.FindMaxValue(sums)/SIZEDIV));
			if(posFixed != "0 0 0")
			{
				//New code
				vector transform[4];
				GetTransformFromPosAndRot(transform, posFixed, rotation);
				params.Transform = transform;
				//New end

				//Old code				
//		        params.TransformMode = ETransformMode.WORLD;			
//		        params.Transform[3] = posFixed;
				//Old end
				entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
				
				SCR_DC_Log.Add("[SCR_DC_MissionHelper:SpawnItem] Entity spawned to: " + posFixed, LogLevel.DEBUG);
			}
			else
			{
				SCR_DC_Log.Add("[SCR_DC_MissionHelper:SpawnItem] Could not find an empty spot for " + resourceName, LogLevel.ERROR);			
			}
		}
		else
		{
			//Spawn the resource exactly to pos
			vector transform[4];
			GetTransformFromPosAndRot(transform, pos, rotation);
	        params.TransformMode = ETransformMode.WORLD;			
	        params.Transform = transform;
			entity = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
			RebuildNavmesh(entity);	
			
			SCR_DC_Log.Add("[SCR_DC_MissionHelper:SpawnItem] Entity spawned to exact position: " + pos, LogLevel.DEBUG);
		}
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
	Spawns a structure, for example a building 
	*/
	static IEntity SpawnStructure(vector pos, array<ref SCR_DC_Structure> structures, int index)
	{
		if (index == -1)
		{
			foreach(SCR_DC_Structure structure : structures)
			{
				SpawnItem(structure.GetPosition(), structure.GetResource(), 0, true);
			}
			return null;
		}
		else
		{
			IEntity entity;
			entity = SpawnItem(structures[index].GetPosition(), structures[index].GetResource(), 0, true);		
			return entity;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Try to add an item to a storage of an entity
	*/	
	static bool AddToStorage(IEntity entity, ResourceName item)
	{	
		//NOTE: The below Resource.Load will result in an error if the ResourceName is not available. For example from 
		//if (FileIO.FileExists("Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et")) ... 
				
		Resource resource = Resource.Load(item);
		if (!resource.IsValid())
			return null;		
		
		ScriptedInventoryStorageManagerComponent storageManager = ScriptedInventoryStorageManagerComponent.Cast(entity.FindComponent(ScriptedInventoryStorageManagerComponent));			
					
		return storageManager.TrySpawnPrefabToStorage(item);
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
			SCR_DC_Log.Add("[SCR_DC_MissionHelper:RebuildNavmesh] Could not create navmesh.", LogLevel.DEBUG);
		}
	}	

	//------------------------------------------------------------------------------------------------
	/*!
	Get transform from the given position and rotation in XZ plane
	*/
	static void GetTransformFromPosAndRot(out vector transform[4], vector pos, float rotation)
	{
		Math3D.MatrixIdentity3(transform);
		Math3D.AnglesToMatrix(Vector(rotation, 0, 0), transform);
		transform[3] = pos;
		SCR_TerrainHelper.SnapAndOrientToTerrain(transform);
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Find an empty spot within areaRadius to fit emptySize
	*/
	static vector FindEmptyPos(vector pos, float areaRadius, float emptySize)
	{		
		vector posFixed;
		
		if(SCR_WorldTools().FindEmptyTerrainPosition(posFixed, pos, areaRadius, emptySize))
		{
			SCR_DC_Log.Add("[SCR_DC_MissionHelper:FindEmptyPos] Found.", LogLevel.SPAM);			
			return posFixed;
		}
		
		SCR_DC_Log.Add("[SCR_DC_MissionHelper:FindEmptyPos] Empty spot not found. Using original.", LogLevel.DEBUG);			
		return pos;
	}
}