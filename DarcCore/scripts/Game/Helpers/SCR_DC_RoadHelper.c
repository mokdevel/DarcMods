//Helpers SCR_DC_RoadHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for road handling. 
*/

//------------------------------------------------------------------------------------------------
class SCR_DC_RoadPos : Managed
{
	vector posOnRoad;
	float distanceToRoad;
	BaseRoad road;
}

//------------------------------------------------------------------------------------------------
sealed class SCR_DC_RoadHelper
{	
	//------------------------------------------------------------------------------------------------
	/*!
	Creates vector points from given two positions. 
	\param routePts Array to have the given points
	\param posFrom, posTo Positions for the route start and end
	\param stepDistance The distance between points to split the route
	*/
	static void CreateRoute(out array<vector> routePts, vector posFrom, vector posTo, int stepDistance = 700)
	{
		float distance = vector.DistanceXZ(posFrom, posTo);
		int ptCount = (distance/stepDistance) + 1;
		
		//Limit the ptCount
		if(ptCount > 4)
		{
			ptCount = 4;	//4 will limit the WPs to 16, 3 would limit to 9
		}
			
		vector posStart = posFrom;
		SCR_DC_RoadPos roadPos;
		routePts = {posFrom, posTo};

		for (int i = 0; i < ptCount; i++)
		{				
			SplitRoute(routePts);
		}
		
//		SCR_DC_Log.Add("[SCR_DC_RoadHelper:CreateRoute] Route pts created: " + routePts.Count(), LogLevel.SPAM);
		
		#ifndef SCR_DC_RELEASE
			//Draw debug markers for a straight line from start to end
			for (int i = 0; i < 20; i++)
			{
				vector ptPos = vector.Lerp(posStart, posTo, i*(1/ptCount));
				SCR_DC_DebugHelper.AddDebugPos(ptPos, Color.CYAN, 3, "ROADTEST", 2);
			}
		#endif		
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Splits the points and adds point on the road to the route
	Example: 
		Road from vector points A to B. Table starts as {A,B} and after split has an mid point a {A, a, B}. For readability, let's have it as {A, B, C}.
		Next split call will split it on AB and BC -> {A, a, B, b, C}. The positions will be on road if they are close to roads.	
	\param route Route to split
	*/	
	
	static void SplitRoute(out array<vector>route)
	{
		array<vector> tmpPos = {};
		tmpPos.InsertAll(route);
		
		//The distance tried for a road depends on the map size. For a 1024 size map ~150 is a good value -> 7 as divider
		float distanceToRoad = SCR_DC_Misc.GetWorldSize() / 27;
		
		SCR_DC_RoadPos roadPos = new SCR_DC_RoadPos;
		for (int i = 0; i < tmpPos.Count() - 1; i++)		
		{
			vector splitPos = vector.Lerp(tmpPos[i], tmpPos[i + 1], 0.5);
			vector pos = FindClosestRoadposToPos(roadPos, splitPos, distanceToRoad);			
			if (roadPos)
			{
				route.InsertAt(roadPos.posOnRoad, (i*2 + 1));
				#ifndef SCR_DC_RELEASE
					SCR_DC_DebugHelper.AddDebugPos(roadPos.posOnRoad, Color.RED, 5, "ROADTEST", 1.5);
				#endif
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find the closest position on a road close to given position. 
	\param roadPos The structure to have move details of returned position
	\param pos Position from where to check. If a better pos is found, this will be overwritten with it
	\param maxDistanceToRoad Limit on how far the road may be from pos. 
	\return Position on a road close the given position
	*/	
	static vector FindClosestRoadposToPos(out SCR_DC_RoadPos roadPos, vector pos, float maxDistanceToRoad = 10000)
	{
		BaseRoad road = null;
		array<vector> roadPts = {};
			
		SCR_AIWorld aiWorld = SCR_AIWorld.Cast(GetGame().GetAIWorld());
		RoadNetworkManager rnManager = null;
		if (aiWorld)
		{
			rnManager = aiWorld.GetRoadNetworkManager();
		}
		
		if (rnManager)
		{
			rnManager.GetClosestRoad(pos, roadPos.road, roadPos.distanceToRoad);			
			if(roadPos.distanceToRoad < maxDistanceToRoad)
			{
				FindRoadPts(roadPts, roadPos.road);
				int i = 0;
				
				pos[1] = 0;		//Distance calculation done on plane at 0 height
				foreach(vector pt: roadPts)
				{	
					pt[1] = 0;	//Distance calculation done on plane at 0 height
					if(SCR_DC_Misc.IsPosNearPos(pos, pt, (roadPos.distanceToRoad + 10)))
					{
						pt[1] = GetGame().GetWorld().GetSurfaceY(pt[0], pt[2]);
						roadPos.posOnRoad = pt;
						SCR_DC_Log.Add("[SCR_DC_RoadHelper:FindClosestRoadposToPos] Road point found. Iterations: " + i, LogLevel.SPAM);
						return pt;
					}
					i++;
				}
				
				return "0 0 0";
			}
		}
		
		return "0 0 0";
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Find the closest road to a position. 
	\param pos Position from where to check
	\param maxDistanceToRoad Limit on how far the road may be from pos. 
	*/
	static BaseRoad FindClosestRoad(vector pos, float maxDistanceToRoad = 10000)
	{
		BaseRoad road = null;
	
		SCR_AIWorld aiWorld = SCR_AIWorld.Cast(GetGame().GetAIWorld());
		RoadNetworkManager rnManager = null;
		if (aiWorld)
		{
			rnManager = aiWorld.GetRoadNetworkManager();
		}
		
		if (rnManager)
		{
			float distanceToRoad;
			rnManager.GetClosestRoad(pos, road, distanceToRoad);
			if(distanceToRoad > maxDistanceToRoad)
			{
				return null;
			}
		}
		
		return road;
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Find points on the road
	\param roadPts The array where to store the road points.
	\param road Known road to map.
	*/	
	static void FindRoadPts(out array<vector> roadPts, BaseRoad road)
	{
		roadPts = {};
		
		if(road)
		{
			road.GetPoints(roadPts);
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Combine road segments.
	In order to combine them in right order, we find out the first point of the road to continue. 
	We want to combine the points to a single road going A-B-B-C. In optimal situation the roads go [A--road--B][B--road--C].
	If case the road goes [A--road--B][C--road--B], the ends of the roads are in the same place. We need to mirror the second road.
	\param roadPts1 The road segment where to append the second road.
	\param roadPts2 The second road segment.
	*/	
	static void CombineRoadPts(out array<vector> roadPts1, array<vector> roadPts2)
	{
		//Check if road1 end is closer to road2 start
		float d1 = vector.Distance(roadPts1[roadPts1.Count() - 1], roadPts2[0]);						//B-C
		float d2 = vector.Distance(roadPts1[roadPts1.Count() - 1], roadPts2[roadPts2.Count() - 1]);		//B-D
		if( d1 > d2 )
		{	
			SCR_DC_Log.Add("[SCR_DC_RoadHelper] Road to be mirrored.", LogLevel.DEBUG);			
			int j = roadPts2.Count() - 1;
			foreach(vector pt: roadPts2)
			{
				roadPts1.Insert(roadPts2[j]);
				j--;
			}
			SCR_DC_Log.Add("[SCR_DC_RoadHelper] Road to be mirrored: " + j, LogLevel.DEBUG);			
		}
		else
		{
			roadPts1.InsertAll(roadPts2);
		}		
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find the next road to position. Position is the end of the previous road.
	The road to be searched should not be the same one so we need two points to define the current road direction. We look at the two last points of road and create a direction vector from them.
	\param roadPosPrev, roadPosLast Last points of the road
	\param distanceToCheck How far the road may be.
	*/
	static BaseRoad FindNextRoad(vector roadPosPrev, vector roadPosLast, float distanceToCheck = 50)
	{	
		//Find the direction the road was heading. We look at the two last points of road and create a direction vector from them.
		vector direction = vector.Direction(roadPosPrev, roadPosLast);
		float angle = SCR_DC_Misc.VectorToAngle(direction);		
		
		vector pos = "0 0 0";
		//Use random angle to search from the position
		array<float> anglesToTest = {0, 180, 270, 0, 180, 270};
		int idx = Math.RandomInt(0, 2);

//		idx = 2;
		for(int i = 0; i < 2; i++)
		{				
			pos = SCR_DC_Misc.MovePosToAngle(roadPosLast, distanceToCheck, (angle + anglesToTest[idx+i]));
			SCR_DC_DebugHelper.AddDebugPos(pos, Color.GREEN, 2, "ROADTEST", 5);
			BaseRoad road = FindClosestRoad(pos, distanceToCheck);
			if(road)
				return road;
		}

		return null;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	DEBUG: Road segment testing
	*/
	static void TestRoad()
	{
		
/*		array<vector> routePts = {};
		CreateRoute(routePts, "2776 0 1623", "3165 0 2800");
		DebugDrawRoad(routePts);
*/		
		/*
		vector pos = "2650 0 1830";
		
		SCR_DC_DebugHelper.AddDebugPos(pos, Color.PINK, 2, "ROADTEST", 5);				
		
		SCR_DC_RoadPos roadPos;
		roadPos = FindClosestRoadposToPos(pos);
		SCR_DC_DebugHelper.AddDebugPos(roadPos.posOnRoad, Color.YELLOW, 3, "ROADTEST", 5);
		
		array<vector> roadPts = {};
		array<vector> roadPts1 = {};
		array<vector> roadPts2 = {};
		BaseRoad road = null;
		
		road = FindClosestRoad(pos, 200);
		FindRoadPts(roadPts, road);
		
		road = FindNextRoad(roadPts[roadPts.Count() - 2], roadPts[roadPts.Count() - 1]);		
		FindRoadPts(roadPts1, road);
		CombineRoadPts(roadPts, roadPts1);
		
		road = FindNextRoad(roadPts[roadPts.Count() - 2], roadPts[roadPts.Count() - 1]);
		FindRoadPts(roadPts1, road);
		CombineRoadPts(roadPts, roadPts1);
				
		road = FindNextRoad(roadPts[roadPts.Count() - 2], roadPts[roadPts.Count() - 1]);
		FindRoadPts(roadPts1, road);
		CombineRoadPts(roadPts, roadPts1);
		
		DebugDrawRoad(roadPts);*/
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	DEBUG: Create visible markers for road points.
	*/
	static void DebugDrawRoad(array<vector> roadPts)
	{
		float i = 0;
		foreach(vector roadPt: roadPts)
		{
			SCR_DC_DebugHelper.AddDebugPos(roadPt, Color.PINK, 2, "ROADTEST", 5 + i);
			i = i + 0.2;
		}
	}	
}