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

	//-----------------------------------------------------------------------------------------------
	/*!
	Check if locations are needed to be cached. If not, startup is much faster
	*/
	static bool IsBuildingCacheNeeded()
	{
		array<string> addonList = {};
		
		SDRC_Misc.GetAddonList(addonList, false);
		
		if ( addonList.Contains("DarcMissions") || addonList.Contains("DarcSpawner") )
		{
			return true;
		}
		
		SDRC_Log.Add("[SDRC_Locations:IsBuildingCacheNeeded] Buildings cache not needed.", LogLevel.WARNING);
		return false;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find types of buildings
	\param buildings The list of found buildings
	\param filter Words used to keep an item in the list
	\param pos Position to start the search
	\param radius Search radius. If set to -1, cached data will be used. This is for fast full map search but will ignore pos.
	*/		
	static void FindBuildings(out array<IEntity> buildings, array<string> filter, vector pos = "0 0 0", float radius = -1)
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
		foreach (IEntity building : buildings)
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
	static void FilterBuildingList(out array<IEntity> buildings, array<IEntity> buildingListSource, array<string> filter)
	{
		foreach (IEntity building : buildingListSource)
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
	static void FillBuildingsCache(array<string> filter)
	{
		SDRC_Log.Add("[SDRC_BuildingHelper:FillBuildingsCache] Searching..", LogLevel.NORMAL);			
		
		m_BuildingsCache.Clear();
/*		float worldSize = SDRC_Misc.GetWorldSize();		
		vector pos = "0 0 0";
		pos[0] = worldSize/2;
		pos[2] = pos[0];
		float radius = worldSize;*/
		
		m_TmpBuildings.Clear();
		GetGame().GetWorld().QueryEntitiesBySphere(SDRC_Misc.GetWorldCenter(), SDRC_Misc.GetWorldSizeRadius(), FindBuildingCallback, null, EQueryEntitiesFlags.STATIC);		

		foreach (IEntity building : m_TmpBuildings)
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
			foreach (IEntity building : m_BuildingsCache)
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
	
	static void FindBuildingFloors(out array<vector> floors, IEntity building)
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
		vector pos = building.GetOrigin();
	
		//Trace from the ceiling terrain height up to roof.
		//We do multiple traces in slightly different positions. This way we can find traces that hit for example furniture.
		//All trace values are collected to floorsTmp
		vector posStartOrig = building.GetOrigin();
		posStartOrig[1] = SCR_TerrainHelper.GetTerrainY(posStartOrig);
		//Trace from terrain to building size
		vector posStart = posStartOrig;				
		posStart[1] = posStartOrig[1] + sums[1];
		
		DoFloorTrace(floorsTmp, building, posStart);
		
		for (int j = 0; j < 1; j++)
		{
			float mulValX = SDRC_Misc.RandomFloat(0.05, 0.30);
			float mulValY = SDRC_Misc.RandomFloat(0.05, 0.30);
			posStart[0] = posStartOrig[0] + (sums[0] * mulValX);
			posStart[2] = posStartOrig[2] + (sums[2] * mulValY);
			DoFloorTrace(floorsTmp, building, posStart);
			
			mulValX = SDRC_Misc.RandomFloat(0.05, 0.30);
			mulValY = SDRC_Misc.RandomFloat(0.05, 0.30);
			posStart[0] = posStartOrig[0] + (sums[0] * mulValX);
			posStart[2] = posStartOrig[2] - (sums[2] * mulValY);
			DoFloorTrace(floorsTmp, building, posStart);
	
			mulValX = SDRC_Misc.RandomFloat(0.05, 0.30);
			mulValY = SDRC_Misc.RandomFloat(0.05, 0.30);
			posStart[0] = posStartOrig[0] - (sums[0] * mulValX);
			posStart[2] = posStartOrig[2] + (sums[2] * mulValY);
			DoFloorTrace(floorsTmp, building, posStart);
	
			mulValX = SDRC_Misc.RandomFloat(0.05, 0.30);
			mulValY = SDRC_Misc.RandomFloat(0.05, 0.30);
			posStart[0] = posStartOrig[0] - (sums[0] * mulValX);
			posStart[2] = posStartOrig[2] - (sums[2] * mulValY);
			DoFloorTrace(floorsTmp, building, posStart);
		}
				
		//Get heights of all scans and sort from lowest to highest.
		//These are potential floor heights. This will be a long list of duplicates.
		array<float> floorHeight = {};		
		const int ROUNDER = 40;
		
		foreach (vector posTmp : floorsTmp)
		{
			int ival = posTmp[1] * ROUNDER;		//Round value
			float fval = ival / ROUNDER;		
			floorHeight.Insert(fval);		
		}
		floorHeight.Sort();
		#ifndef SDRC_RELEASE
			//floorHeight.Debug();
		#endif
		
		//Take the highest point found and subtract the the ground level. This an approximate of building height above ground.
		float highestHeight = floorHeight[floorHeight.Count() - 1];
		float groundHeight = GetGame().GetWorld().GetSurfaceY(pos[0], pos[2]);
		float buildingHeight = highestHeight - groundHeight;
				
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

		#ifndef SDRC_RELEASE
//			floors.Debug();
		#endif
		
		//TBD: Houses with issues:
		//- House_Village_E_1L03t.et		
		
		//NOTE: The floors order is from lowest to highest		
		if (floors.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_BuildingHelper:FindBuildingFloors] No floors found in building: " + building.GetPrefabData().GetPrefabName(), LogLevel.ERROR);				
			return;
		}

		//Remove the highest floor as rood/attic
		if (floors.Count() > 1)
		{
			SDRC_DebugHelper.AddDebugPos(floors[floors.Count() - 1], ARGB(20, 128, 128, 128), 12, "", 0.3, false);		//Gray for removed floor
			floors.RemoveOrdered(floors.Count() - 1);			
		}

		//If only two floors left and the upper floor is in the middle of the house, remove it.
		if (floors.Count() == 2)
		{
			float midBuildingHeight = buildingHeight / 2;
			if ( (floors[1][1] < midBuildingHeight + 0.5) && (floors[1][1] > midBuildingHeight - 0.5) )
			{
				SDRC_DebugHelper.AddDebugPos(floors[floors.Count() - 1], ARGB(20, 128, 128, 128), 12, "", 0.3, false);		//Gray for removed floor
				floors.RemoveOrdered(1);
			}
		}
		
		//If the two bottom floors are close to each other, we consider the lower one to be on the ground.
		if (floors.Count() > 1)
		{
			if ( (floors[1][1] - floors[0][1]) < 1.6)
			{
				SDRC_DebugHelper.AddDebugPos(floors[0], ARGB(20, 128, 128, 128), 12, "", 0.3, false);		//Gray for removed floor
				floors.RemoveOrdered(0);
			}
		}
		
		//Cache values if needed again
		buildingIDfloorCache.Copy(floors);
		
		#ifndef SDRC_RELEASE
			//Print(floorHeight);
			//Print(floors);
			foreach (vector fpos : floors)
			{
				SDRC_DebugHelper.AddDebugPos(fpos, ARGB(20, 255, 255, 0), 12, "", 0.3, false);		//Yellow
			}				
		#endif 
				
		SDRC_Log.Add("[SDRC_BuildingHelper:FindBuildingFloors] Found: " + floors.Count() + " floors from " + building.GetPrefabData().GetPrefabName(), LogLevel.SPAM);		//REMOVE
	}
	
	//------------------------------------------------------------------------------------------------
	static void DoFloorTrace(out array<vector>floors, IEntity entity, vector posEnd)
	{
		float terrainY = SCR_TerrainHelper.GetTerrainY(posEnd);
		
		vector posStart;
		posStart = posEnd;
		posStart[1] = terrainY - 0.5;					//Start under ground
	
		TraceParam traceUp = new TraceParam();
		{
			traceUp.Start = posStart;
			traceUp.End = posEnd;
			traceUp.TargetLayers = EPhysicsLayerDefs.Navmesh;
			traceUp.Flags = TraceFlags.ENTS | TraceFlags.WORLD;
		}
		
		TraceParam traceDown = new TraceParam();
		{
			traceDown.Start = "0 0 0";
			traceDown.End = "0 0 0";
			traceDown.TargetLayers = EPhysicsLayerDefs.Navmesh;
			traceDown.Flags = TraceFlags.ENTS | TraceFlags.WORLD;
		}		

		//Print("SDRC SnapToGeometry Start ");
		
		vector posCheck = posStart;
		int i = 0;		
		while (i < 10)	//Try a maximun of ten times. Just to avoid forever loop
		{
			vector floorpos = "0 0 0";

			SCR_TerrainHelper.SnapToGeometry(floorpos, posCheck, {}, entity.GetWorld(), traceParam: traceUp);

			//Print("SDRC SnapToGeometry : " + (posEnd[1] - posCheck[1]) + "/" + posEnd[1] + "/" + posCheck[1] + "/" + floorpos[1]);
			
			//If we reached the highest level, stop 5 meters before
			if (posEnd[1] == floorpos[1])
			{
				break;
			}
			
			//Make the trace start above the floor and go through it.
			vector floorposAbove = floorpos;
			floorposAbove[1] = floorpos[1] + 1.0;
			floorpos[1] = floorpos[1] - 0.5;
						
			#ifndef SDRC_RELEASE
				vector move = "0.05 0 0";
				SDRC_DebugHelper.AddDebugSphere(floorposAbove, ARGB(50, 255, 255, 255), 0.15);				//white
				SDRC_DebugHelper.AddDebugSphere(floorpos + move, ARGB(50, 128, 255, 128), 0.15);			//light green
			#endif 
			
			traceDown.Start = floorposAbove;
			traceDown.End = floorpos;
			SCR_TerrainHelper.SnapToGeometry(floorpos, floorposAbove, {}, entity.GetWorld(), traceParam: traceDown);			

			floorpos[1] = floorpos[1] + 0.1;	//Put the floor 10cm above the found position - just in case
			floors.Insert(floorpos);
			posCheck[1] = floorpos[1] + 1;		//Move 1m up from found floor
			traceUp.Start = posCheck;
			
			i++;
		}

		#ifndef SDRC_RELEASE
			//Green debug sphere to show where trace starts and ends
			vector add10 = "0 0 0";	//Move the sphere up so that we don't trace it.
			SDRC_DebugHelper.AddDebugSphere(posStart, Color.GREEN, 0.2);
			SDRC_DebugHelper.AddDebugSphere(posEnd + add10, Color.GREEN, 0.2);
		#endif
				
		#ifndef SDRC_RELEASE
			foreach (vector fpos : floors)
			{
				SDRC_DebugHelper.AddDebugSphere(fpos, ARGB(50, 0, 0, 255), 0.06);	//Blue ball for the found floor
			}				
		#endif 
	}	
}