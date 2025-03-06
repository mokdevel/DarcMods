//Helpers SCR_DC_RoadHelper.c

//------------------------------------------------------------------------------------------------
/*!
Includes various functions for road handling. 
*/

sealed class SCR_DC_RoadHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Road segment testing
	*/
	static void TestRoad()
	{
		vector pos = "2650 0 1830";
		
		SCR_DC_DebugHelper.AddDebugPos(pos, Color.PINK, 2, "ROADTEST", 5);				
		
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
		
		DebugDrawRoad(roadPts);
	}
	
	//------------------------------------------------------------------------------------------------
	static void DebugDrawRoad(array<vector> roadPts)
	{
		float i = 0;
		foreach(vector roadPt: roadPts)
		{
			SCR_DC_DebugHelper.AddDebugPos(roadPt, Color.PINK, 2, "ROADTEST", 5 + i);
			i = i + 0.2;
		}
	}
	
	//------------------------------------------------------------------------------------------------
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
	
/*	//------------------------------------------------------------------------------------------------
	vector FindClosestRoadToPos(vector pos, float maxDistanceToRoad)
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
	}*/
		
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
	Find the closest road to position.
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
		//Use random angle to search from the 
		array<float> anglesToTest = {0, 180, 270, 0, 180, 270};
		int idx = Math.RandomInt(0, 2);

		idx = 2;
		for(int i = 0; i < 3; i++)
		{				
			pos = SCR_DC_Misc.MovePosToAngle(roadPosLast, distanceToCheck, (angle + anglesToTest[idx+i]));
			SCR_DC_DebugHelper.AddDebugPos(pos, Color.GREEN, 2, "ROADTEST", 5);
		}		
				
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
}