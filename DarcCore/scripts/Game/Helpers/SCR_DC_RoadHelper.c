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
		BaseRoad road = null;
		
		road = FindClosestRoad(pos, 200);
		FindRoadPts(roadPts, road);
		road = FindNextRoad(roadPts);
		
		FindRoadPts(roadPts, road);
		road = FindNextRoad(roadPts);
				
		FindRoadPts(roadPts, road);
//		FindNextRoad(roadPts);
	}
	
	//------------------------------------------------------------------------------------------------
	static void FindRoadPts(out array<vector> roadPts, BaseRoad road)
	{
		roadPts = {};
		
		if(road)
		{
			road.GetPoints(roadPts);
			
			float i = 0;
			foreach(vector roadPt: roadPts)
			{
				SCR_DC_DebugHelper.AddDebugPos(roadPt, Color.PINK, 2, "ROADTEST", 5 + i);
				i = i + 0.2;
			}
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
	static BaseRoad FindNextRoad(array<vector> roadPts)
	{		
		vector direction = vector.Direction(roadPts[roadPts.Count() - 2], roadPts[roadPts.Count() - 1]);
		float angle = SCR_DC_Misc.VectorToAngle(direction);		
		
		float distanceToCheck = 50;
		vector pos = "0 0 0";
		array<float> anglesToTest = {0, 180, 270};		
		
		foreach(float angleToTest: anglesToTest)
		{		
			pos = SCR_DC_Misc.MovePosToAngle(roadPts[roadPts.Count() - 1], distanceToCheck, angle + angleToTest);
			SCR_DC_DebugHelper.AddDebugPos(pos, Color.GREEN, 2, "ROADTEST", 5);
			BaseRoad road = FindClosestRoad(pos, distanceToCheck);
			if(road)
				return road;
		}

		return null;
	}	
}