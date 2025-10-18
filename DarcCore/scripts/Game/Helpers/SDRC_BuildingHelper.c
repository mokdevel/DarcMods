//Helpers SDRC_BuildingHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions related to buildings on map

NOTE: In order to use the caching, FillBuildingsCache has to be run at startup.
*/

sealed class SDRC_BuildingHelper
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
		foreach (IEntity building: buildings)
		{
			ResourceName res = building.GetPrefabData().GetPrefabName();
			SDRC_Log.Add("[SDRC_BuildingHelper:FindBuildings] Found: " + res + " at " + building.GetOrigin(), LogLevel.SPAM);			
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
		foreach (IEntity building: buildingListSource)
		{
			if (!building)
			{
				SDRC_Log.Add("[SDRC_BuildingHelper:FilterBuildingList] NULL - should never happen", LogLevel.WARNING);
				continue;
			}
			
			ResourceName buildingName = building.GetPrefabData().GetPrefabName();
			
			if (SCR_StringHelper.ContainsAny(buildingName, filter))
			{
				buildings.Insert(building);
				//SDRC_Log.Add("[SDRC_BuildingHelper:FilterBuildingList] Added: " + buildingName, LogLevel.DEBUG);
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
		SDRC_Log.Add("[SDRC_BuildingHelper:FillBuildingsCache] Searching..", LogLevel.NORMAL);			
		
		m_BuildingsCache.Clear();
		float worldSize = SDRC_Misc.GetWorldSize();		
		vector pos = "0 0 0";
		pos[0] = worldSize/2;
		pos[1] = pos[0];
		float radius = worldSize; 
		
		m_TmpBuildings.Clear();
		GetGame().GetWorld().QueryEntitiesBySphere(pos, radius, FindBuildingCallback, null, EQueryEntitiesFlags.STATIC);		

		foreach (IEntity building: m_TmpBuildings)
		{
			ResourceName buildingName = building.GetPrefabData().GetPrefabName();
			
			if (SCR_StringHelper.ContainsAny(buildingName, filter))
			{
				continue;
			}
			m_BuildingsCache.Insert(building);
		}		
				
		//Print debug information
		#ifndef SDRC_RELEASE
			foreach (IEntity building: m_BuildingsCache)
			{
				ResourceName res = building.GetPrefabData().GetPrefabName();
				SDRC_Log.Add("[SDRC_BuildingHelper:FillBuildingsCache] Found: " + res + " at " + building.GetOrigin(), LogLevel.DEBUG);			
			}		
		#endif
		
		SDRC_Log.Add("[SDRC_BuildingHelper:FillBuildingsCache] Found " + m_BuildingsCache.Count() + " items to building cache.", LogLevel.NORMAL);					
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
			
			//SDRC_Log.Add("[SDRC_BuildingHelper:FindBuildingCallback] Found: " + res + " at " + entity.GetOrigin(), LogLevel.DEBUG);
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
	static EntityID buildingID = null;	
	static ref array<vector> buildingIDfloorCache = {};	
	
	static void FindBuildingFloors(out array<vector>floors, IEntity building)
	{
		array<vector>floorsTmp = {};
		floors = {};
		
		//Is this the same building as used before?
		if (buildingID == building.GetID())
		{
			SDRC_Log.Add("[SDRC_BuildingHelper:FindBuildingFloors] Same building - using cached floor values.", LogLevel.DEBUG);
			floors.Copy(buildingIDfloorCache);
			return;
		}

		//Clear cache		
		buildingID = building.GetID();
		buildingIDfloorCache.Clear();
		
		//Finding the building size
		vector sums = SDRC_SpawnHelper.FindEntitySize(building);

		//Used for debugging		
		vector size = sums;
		size[1] = 0;
	 	float floorMarkerSize = SDRC_Misc.FindMaxValue(sums) / 2;
	
		vector pos;
		pos = building.GetOrigin();
	
		//Trace from the ceiling down to terrain height.
		//We do multiple traces in slightly different positions. This way we can find traces that hit for example furniture.
		//All trace values are collected to floorsTmp
		vector posStartOrig = building.GetOrigin();		
		vector posStart = posStartOrig;
				
		posStart[1] = posStartOrig[1] + sums[1];
		DoFloorTrace(floorsTmp, building, posStart);
		
		for (int j = 0; j < 1; j++)
		{
			float mulValX = Math.RandomFloat(0.05, 0.30);
			float mulValY = Math.RandomFloat(0.05, 0.30);
			posStart[0] = posStartOrig[0] + (sums[0] * mulValX);
			posStart[2] = posStartOrig[2] + (sums[2] * mulValY);
			DoFloorTrace(floorsTmp, building, posStart);
			
			mulValX = Math.RandomFloat(0.05, 0.30);
			mulValY = Math.RandomFloat(0.05, 0.30);
			posStart[0] = posStartOrig[0] + (sums[0] * mulValX);
			posStart[2] = posStartOrig[2] - (sums[2] * mulValY);
			DoFloorTrace(floorsTmp, building, posStart);
	
			mulValX = Math.RandomFloat(0.05, 0.30);
			mulValY = Math.RandomFloat(0.05, 0.30);
			posStart[0] = posStartOrig[0] - (sums[0] * mulValX);
			posStart[2] = posStartOrig[2] + (sums[2] * mulValY);
			DoFloorTrace(floorsTmp, building, posStart);
	
			mulValX = Math.RandomFloat(0.05, 0.30);
			mulValY = Math.RandomFloat(0.05, 0.30);
			posStart[0] = posStartOrig[0] - (sums[0] * mulValX);
			posStart[2] = posStartOrig[2] - (sums[2] * mulValY);
			DoFloorTrace(floorsTmp, building, posStart);
		}
				
		//Get heights of all scans and sort from lowest to highest.
		//These are potential floor heights.
		array<float> floorHeight = {};		
		const int ROUNDER = 40;
		
		foreach (vector posTmp : floorsTmp)
		{
			int ival = posTmp[1] * ROUNDER;		//Round value
			float fval = ival / ROUNDER;		
			floorHeight.Insert(fval);		
		}
		floorHeight.Sort();
		
		float highestHeight = floorHeight[floorHeight.Count() - 1];
		
		//Those that are single values, are most likely not proper floor heights. We hit a furniture or some other part of the building.
		//Clean the list and leave only those with multiples.
		//For example: {11,15,15,15,19,19,19,22,22} -> {15, 19, 22}
		
		float checkValue = floorHeight[0];
		float addedValue = 0;
		for (int i = 1; i < floorHeight.Count(); i++)
		{			
			if ( (checkValue == floorHeight[i]) && (checkValue != addedValue) )
			{
				vector newPos = posStartOrig;
				newPos[1] = floorHeight[i];
				addedValue = floorHeight[i];
				floors.Insert(newPos);
			}
			else
			{
				checkValue = floorHeight[i];
			}		
		}

		//TBD: Houses with issues:
		//- House_Village_E_1L03t.et		
		
		//NOTE: The floors order is from lowest to highest		
		
		//Ground check : If the two last floors are too close to each other, the lowest floor is considered ground
		if (floors.Count() > 1)
		{
			float diff = floors[1][1] - floors[0][1];
			if ( diff < 1.3 )	//Was 1.7
			{
				floors.RemoveOrdered(0);				//Remove ground as a floor
			}
		}		

		//Start from lowest floor and start to come upwards. See that the next floor is not too far away as it's most likely roof or attic.
		if (floors.Count() > 1)
		{
			array<vector> tmpFloors = {};
			tmpFloors.Insert(floors[0]);
			
			for (int i = 1; i < floors.Count() - 1; i++)
			{
				float distance = floors[i + 1][1] - floors[i][1];				
				SDRC_Log.Add("[SDRC_BuildingHelper:FindBuildingFloors] Distance between floors: " + distance + " in building: " + building.GetPrefabData().GetPrefabName(), LogLevel.WARNING);				
				
				if ( (distance < 2.4) && (distance > 0.8) )
				{
					tmpFloors.Insert(floors[i]);
				}
			}
			floors.Clear();
			floors.Copy(tmpFloors);
		}
		
				
/*		
		//Roof check : If we found multiple floors, the highest one is with high probability the roof. Remove it.
		if (floors.Count() > 1)
		{
			float roofHeight = floorHeight[floors.Count() - 1];
			vector highPoint = floors[0];
			highPoint[1] = highestHeight;
			
			SDRC_DebugHelper.AddDebugPos(highPoint, ARGB(20, 255, 0, 0), floorMarkerSize, "", 0.3, false);								//Highest point Red
			SDRC_DebugHelper.AddDebugPos(floors[floors.Count() - 1], ARGB(20, 255, 0, 255), floorMarkerSize * 0.8, "", 0.3, false);		//Roof Purple
			
			//Remove roof as a floor
			floors.RemoveOrdered(floors.Count() - 1);

			if (floors.Count() > 1)
			{
				//Check if the next floor is close to roof. If yes, it's most likely attic
				float atticHeight = floorHeight[floors.Count() - 1];
				//float atticDistance = roofHeight - atticHeight;
				float atticDistance = highestHeight - atticHeight;

				SDRC_DebugHelper.AddDebugPos(floors[floors.Count() - 1], ARGB(20, 0, 255, 0), floorMarkerSize * 0.8, "", 0.3, false);		//Attic Green
								
				SDRC_Log.Add("[SDRC_BuildingHelper:FindBuildingFloors] Attic distance from roof: " + atticDistance + " in building: " + building.GetPrefabData().GetPrefabName(), LogLevel.WARNING);
				
				//We expect the floor distance to be ~2m. From roof to first floor is ~4m. Anything between would be attic. We try with 3m.
				if ( atticDistance < 3.0)
				{
					//Remove attic as a floor
					floors.RemoveOrdered(floors.Count() - 1);	
					SDRC_Log.Add("[SDRC_BuildingHelper:FindBuildingFloors] Removed attic in building: " + building.GetPrefabData().GetPrefabName(), LogLevel.WARNING);
				}
			}
		}
*/		
		//Cache values if needed again
		buildingIDfloorCache.Copy(floors);
		
		#ifndef SDRC_RELEASE
			//Print(floorHeight);
			//Print(floors);
			foreach (vector fpos: floors)
			{
				SDRC_DebugHelper.AddDebugPos(fpos, ARGB(20, 255, 255, 0), floorMarkerSize, "", 0.3, false);		//Yellow
			}				
		#endif 
				
		SDRC_Log.Add("[SDRC_BuildingHelper:FindBuildingFloors] Found: " + floors.Count() + " floors from " + building.GetPrefabData().GetPrefabName(), LogLevel.SPAM);		//REMOVE
	}
	
	//------------------------------------------------------------------------------------------------
	static void DoFloorTrace(out array<vector>floors, IEntity entity, vector posStart)
	{
		float terrainY = SCR_TerrainHelper.GetTerrainY(posStart);
		
		vector posEnd;
		posEnd = posStart;
		posEnd[1] = terrainY + 0.1;					//Stop 10cm above ground
	
		TraceParam trace = new TraceParam();
		{
			trace.Start = posStart;
			trace.End = posEnd;

			#ifndef SDRC_RELEASE
				//Green debug sphere to show where trace starts
				SDRC_DebugHelper.AddDebugSphere(posStart, Color.GREEN, 0.1);
			#endif
				
			//trace.Exclude = child;
			trace.TargetLayers = EPhysicsLayerDefs.Navmesh;
			trace.Flags = TraceFlags.ENTS | TraceFlags.WORLD;
		}		

		vector posCheck = posStart;
		int i = 0;		
		while ( (posCheck[1] > terrainY) && (i < 10) )	//Try a maximun of ten times. Just to avoid forever loop
		{
			vector floorpos;
			SCR_TerrainHelper.SnapToGeometry(floorpos, posCheck, {}, entity.GetWorld(), traceParam: trace);
			
			#ifndef SDRC_RELEASE
//				vector move = "0.15 0 0.0";
//				SDRC_DebugHelper.AddDebugSphere(posCheck, ARGB(50, 255, 0, 255), 0.05);				
//				SDRC_DebugHelper.AddDebugSphere(floorpos + move, ARGB(50, 255, 255, 255), 0.05);				
			#endif 
			
			floors.Insert(floorpos);
			posCheck[1] = floorpos[1] - 1;		//Move 1m down from found floor
			trace.Start = posCheck;
			
			i++;
		}
		
		#ifndef SDRC_RELEASE
			foreach (vector fpos: floors)
			{
				SDRC_DebugHelper.AddDebugSphere(fpos, ARGB(50, 0, 0, 255), 0.06);	//Blue ball for the found floor
			}				
		#endif 
	}	
}