//Helpers SCR_DC_BuildingHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions related to buildings on map
*/

sealed class SCR_DC_BuildingHelper
{
	private static ref array<IEntity> m_BuildingsCache = {};
	private static ref array<IEntity> m_TmpBuildings = {};

	//------------------------------------------------------------------------------------------------
	/*!
	Find types of buildings
	\param buildings The list of found buildings
	\param filter Words used to keep an item in the list
	\param pos Position to start the search
	\param radius Search radius. If set to -1, cached data will be used. This is for fast full map search but will ignore pos.
	*/		
	static void FindBuildings(out array<IEntity>buildings, array<string>filter, vector pos = "0 0 0", float radius = -1)
	{	
		if (radius == -1)
		{
			//Search using cache
			FilterBuildingList(buildings, m_BuildingsCache, filter);
		}
		else
		{		
			m_TmpBuildings.Clear();
			GetGame().GetWorld().QueryEntitiesBySphere(pos, radius, FindBuildingCallback, null, EQueryEntitiesFlags.STATIC);		

			FilterBuildingList(buildings, m_TmpBuildings, filter);
		}
				
		//Print debug information
		foreach(IEntity building: buildings)
		{
			ResourceName res = building.GetPrefabData().GetPrefabName();
			SCR_DC_Log.Add("[SCR_DC_BuildingHelper:FindBuildings] Found: " + res + " at " + building.GetOrigin(), LogLevel.DEBUG);			
		}		
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Filter out an existing list with given filter
	\param buildings Final list
	\param buildingListSource List to filter
	\param filter Words used to keep an item in the list
	*/	
	static void FilterBuildingList(out array<IEntity>buildings, array<IEntity>buildingListSource, array<string>filter)
	{
		foreach(IEntity building: buildingListSource)
		{
			if(!building)
			{
				SCR_DC_Log.Add("[SCR_DC_BuildingHelper:FilterBuildingList] NULL - should never happen", LogLevel.WARNING);
				continue;
			}
			
			ResourceName buildingName = building.GetPrefabData().GetPrefabName();
			
			if (SCR_StringHelper.ContainsAny(buildingName, filter))
			{
				buildings.Insert(building);
				//SCR_DC_Log.Add("[SCR_DC_BuildingHelper:FilterBuildingList] Added: " + buildingName, LogLevel.DEBUG);
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Prepare an array with all houses on the map.
	NOTE: The list is not necessarily only buildings but anything you'd like to consider as a point for e.g. mission spawning. 
	*/		
	static void FillBuildingsCache(array<string>filter)
	{
		SCR_DC_Log.Add("[SCR_DC_BuildingHelper:FillBuildingsCache] Searching..", LogLevel.NORMAL);			
		
		m_BuildingsCache.Clear();
		float worldSize = SCR_DC_Misc.GetWorldSize();		
		vector pos = "0 0 0";
		pos[0] = worldSize/2;
		pos[1] = pos[0];
		float radius = worldSize; 
		
		m_TmpBuildings.Clear();
		GetGame().GetWorld().QueryEntitiesBySphere(pos, radius, FindBuildingCallback, null, EQueryEntitiesFlags.STATIC);		

		foreach(IEntity building: m_TmpBuildings)
		{
			ResourceName buildingName = building.GetPrefabData().GetPrefabName();
			
			if (SCR_StringHelper.ContainsAny(buildingName, filter))
			{
				continue;
			}
			m_BuildingsCache.Insert(building);
		}		
				
		//Print debug information
		#ifndef SCR_DC_RELEASE
			foreach(IEntity building: m_BuildingsCache)
			{
				ResourceName res = building.GetPrefabData().GetPrefabName();
				SCR_DC_Log.Add("[SCR_DC_BuildingHelper:FillBuildingsCache] Found: " + res + " at " + building.GetOrigin(), LogLevel.SPAM);			
			}		
		#endif
		
		SCR_DC_Log.Add("[SCR_DC_BuildingHelper:FillBuildingsCache] Found " + m_BuildingsCache.Count() + " items to building cache.", LogLevel.NORMAL);					
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Call back filter for FindBuilding
	*/		
	static bool FindBuildingCallback(IEntity entity)
	{
		if (entity.ClassName() == "SCR_DestructibleBuildingEntity")
		{
			ResourceName res = entity.GetPrefabData().GetPrefabName();
		  	if (res.IndexOf("_furniture") > -1 || res == "")
			{
				return true;
			}
			
			//SCR_DC_Log.Add("[SCR_DC_BuildingHelper:FindBuildingCallback] Found: " + res + " at " + entity.GetOrigin(), LogLevel.DEBUG);
			//EntityID id = entity.GetID();
			m_TmpBuildings.Insert(entity);
			return true;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find floors from a building entity.
	From the returned vector positions, the [0] item is the floor height. Roof has been removed.
	*/		
	static void FindBuildingFloors(out array<vector>floors, IEntity entity)
	{
		vector sums = SCR_DC_SpawnHelper.FindEntitySize(entity);
	
		vector pos;
		pos = entity.GetOrigin();
	
		float terrainY = SCR_TerrainHelper.GetTerrainY(entity.GetOrigin());
	
		//Trace from the ceiling down to terrain height
		vector posStart = entity.GetOrigin();
		posStart[1] = posStart[1] + sums[1];
		vector posEnd;
	
		TraceParam trace = new TraceParam();
		{
			trace.Start = posStart;
		
			posEnd = posStart;
			posEnd[1] = terrainY;
			trace.End = posEnd;

			SCR_DC_DebugHelper.AddDebugSphere(posStart, Color.GREEN, 2);
				
			//trace.Exclude = child;
			trace.TargetLayers = EPhysicsLayerDefs.Navmesh;
			trace.Flags = TraceFlags.ENTS | TraceFlags.WORLD;
		}		

		vector posCheck = posStart;
		int i = 0;
		while (posCheck[1] > terrainY && i < 10)	//Try a maximun of ten times. Just to avoid forever loop
		{
			vector floorpos;
			SCR_TerrainHelper.SnapToGeometry(floorpos, posCheck, {}, entity.GetWorld(), traceParam: trace);
			floors.Insert(floorpos);
			posCheck[1] = floorpos[1] - 1;
			trace.Start = posCheck;
			i++;
		}
		
		if (floors.Count() > 0)
		{
			floors.RemoveOrdered(0);					//Remove roof height
			SCR_ArrayHelperT<vector>.Reverse(floors);	//Change order to that [0] is the bottom floor
		}
		
		foreach (vector fpos: floors)
		{
			SCR_DC_DebugHelper.AddDebugSphere(fpos, Color.BLUE, 0.3);
		}
		
		SCR_DC_Log.Add("[SCR_DC_BuildingHelper:FindBuildingFloors] Found: " + floors.Count() + " floors from " + entity.GetPrefabData().GetPrefabName(), LogLevel.SPAM);
	}	
}