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
		FindRoadPts(roadPts1, road);
		road = FindNextRoad(roadPts1[roadPts1.Count() - 2], roadPts1[roadPts1.Count() - 1]);		
		FindRoadPts(roadPts2, road);
		CombineRoadPts(roadPts, roadPts1, roadPts2);
		
//		road = FindNextRoad(roadPts);
				
//		FindRoadPts(roadPts, road);
//		FindNextRoad(roadPts);
		
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
	static void CombineRoadPts(out array<vector> roadPts, array<vector> roadPts1, array<vector> roadPts2)
	{
		//Find out the first point of the road to continue. 
		//We want to combine the points to a single road going A-B-B-C. In optimal situation the roads go [A--road--B][B--road--C].
		//If case the road goes [A--road--B][C--road--B], the ends of the roads are in the same place. We need to mirror the second road.

		roadPts.Copy(roadPts1);
				
		//Check if road1 end is closer to road2 start
		float d1 = vector.Distance(roadPts1[roadPts1.Count() - 1], roadPts2[0]);						//B-C
		float d2 = vector.Distance(roadPts1[roadPts1.Count() - 1], roadPts2[roadPts2.Count() - 1]);		//B-D
		if( d1 > d2 )
		{	
			SCR_DC_Log.Add("[SCR_DC_RoadHelper] Road to be mirrored.", LogLevel.DEBUG);			
			int j = roadPts2.Count() - 1;
			foreach(vector pt: roadPts2)
			{
				roadPts.Insert(roadPts2[j]);
				j--;
			}
			SCR_DC_Log.Add("[SCR_DC_RoadHelper] Road to be mirrored: " + j, LogLevel.DEBUG);			
		}
		else
		{
			roadPts.InsertAll(roadPts2);
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
	static BaseRoad FindClosestRoad(vector pos, float maxDistanceToRoad)
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
//	static BaseRoad FindNextRoad(array<vector> roadPts)
	static BaseRoad FindNextRoad(vector roadPosPrev, vector roadPosLast)
	{	
		//Find the direction the road was heading. We look at the two last points of road and create a direction vector from them.
		vector direction = vector.Direction(roadPosPrev, roadPosLast);
		float angle = SCR_DC_Misc.VectorToAngle(direction);		
		
		float distanceToCheck = 50;
		vector pos = "0 0 0";
		array<float> anglesToTest = {0, 180, 270};		
		
		foreach(float angleToTest: anglesToTest)
		{		
			pos = SCR_DC_Misc.MovePosToAngle(roadPosLast, distanceToCheck, angle + angleToTest);
			SCR_DC_DebugHelper.AddDebugPos(pos, Color.GREEN, 2, "ROADTEST", 5);
			BaseRoad road = FindClosestRoad(pos, distanceToCheck);
			if(road)
				return road;
		}

		return null;
	}	
}