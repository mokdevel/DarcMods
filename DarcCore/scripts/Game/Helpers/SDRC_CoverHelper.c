//Helpers SDRC_CoverHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions related to buildings on map

NOTE: In order to use the caching, FillBuildingsCache has to be run at startup.
*/

class SDRC_CoverPos
{
	int tileX;
	int tileY;
	int coverId;
	vector pos;
}

class SDRC_CoverHelper
{
	const int COVER_COUNT = 30;					//Amount of covers to search
	
	private IEntity m_Building = null;
	private float m_BuildingSize = 0;
	
	private vector m_Pos = vector.Zero;
	private ref array<ref SDRC_CoverPos> m_aCovers = {};
	private bool m_Running = false;				//All good and necessary components were found
	private bool m_Ready = false;				//Cover finding is ready

	private AIPathfindingComponent m_PathFindindingComp;
	private ChimeraCoverManagerComponent m_CoverMgr;
	private ref CoverQueryProperties m_CoverQueryProps = new CoverQueryProperties();
	private NavmeshWorldComponent m_Navmesh;
	
	//------------------------------------------------------------------------------------------------
	void SDRC_CoverHelper(IEntity building)
	{
		m_Building = building;

		m_Pos = building.GetOrigin();
		vector sums = SDRC_SpawnHelper.FindEntitySize(building);
		m_BuildingSize = SDRC_Misc.FindMaxValue(sums);
		
		string resourceName = "{5B1996C05B1E51A4}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_AR.et";
		AIAgent aiAgent = SDRC_AIHelper.SpawnAIAgent(resourceName, m_Pos, "", true);
		
		string faction = SDRC_AIHelper.GetAIAgentFactionKey(aiAgent);
		SCR_AIGroup group = SDRC_AIHelper.GroupCreate(faction, aiAgent.GetOrigin());
		if (!group)
		{
			SDRC_Log.Add("[SDRC_CoverHelper] Could not spawn AIAgent.", LogLevel.ERROR);
			return;
		}	
		
		group.SetNewLeader(aiAgent);
		group.AddAgent(aiAgent);
		
		AIWorld aiWorld = GetGame().GetAIWorld();
		if (aiWorld)
			m_CoverMgr = ChimeraCoverManagerComponent.Cast(aiWorld.FindComponent(ChimeraCoverManagerComponent));
		
		IEntity entity = aiAgent.GetControlledEntity();
		if (entity)
			m_PathFindindingComp = AIPathfindingComponent.Cast(entity.FindComponent(AIPathfindingComponent));
		
		m_PathFindindingComp = AIPathfindingComponent.Cast(group.FindComponent(AIPathfindingComponent));		
		
		if (!m_CoverMgr || !m_PathFindindingComp)
		{
			SDRC_Log.Add("[SDRC_CoverHelper] Could not find managers needed for cover search.", LogLevel.ERROR);
			return;
		}
		
		m_Navmesh = m_PathFindindingComp.GetNavmeshComponent();
		if (!m_Navmesh)
		{
			SDRC_Log.Add("[SDRC_CoverHelper] Could not find navmesh!", LogLevel.ERROR);
			return;
		}				
		
		SetCoverQueryProps();
		
		m_Running = true;		
		SDRC_Log.Add("[SDRC_CoverHelper] Init successful.", LogLevel.DEBUG);
		FindBuildingCovers();
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsReady()
	{
		return m_Ready;
	}

	//------------------------------------------------------------------------------------------------
	bool IsRunning()
	{
		return m_Running;
	}
		
	//------------------------------------------------------------------------------------------------
	void FindBuildingCovers()
	{
		if (!m_Navmesh.IsTileLoaded(m_Pos))
		{
			m_Navmesh.LoadTileIn(m_Pos);
			SDRC_Log.Add("[SDRC_CoverHelper:FindBuildingCovers] Requested tile loading.", LogLevel.DEBUG);
			GetGame().GetCallqueue().CallLater(FindBuildingCovers, 1000, false);		
			return;
		}		
		
		//Tile was loaded. Find the covers
		vector coverPos, coverTallestPos;
		int tileX, tileY, coverId;
		
		for (int i = 0; i < COVER_COUNT; i++)
		{
			bool coverFound = m_CoverMgr.GetBestCover("Soldiers", m_PathFindindingComp, m_CoverQueryProps, coverPos, coverTallestPos, tileX, tileY, coverId);	
			Print("coverFound: " + coverFound);
//			Print("coverPos:" + coverPos);
			ref SDRC_CoverPos tmpCoverPos = new SDRC_CoverPos();
			tmpCoverPos.tileX = tileX;
			tmpCoverPos.tileY = tileY;
			tmpCoverPos.coverId = coverId;
			tmpCoverPos.pos = coverPos;
			m_aCovers.Insert(tmpCoverPos);

			m_CoverMgr.SetOccupiedCover(tileX, tileY, coverId, true);
			SDRC_DebugHelper.AddDebugSphere(coverPos, ARGB(50, 0, 128, 0), 0.25);
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	void FreeCovers()
	{
		foreach (SDRC_CoverPos coverPos : m_aCovers)
		{
			m_CoverMgr.SetOccupiedCover(coverPos.tileX, coverPos.tileY, coverPos.coverId, false);			
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void SetCoverQueryProps()
	{
		CoverQueryProperties m_CoverQueryProps = new CoverQueryProperties();
		m_CoverQueryProps.m_vNearestPolyHalfExtend = SCR_AIFindCover.NEAREST_POLY_HALF_EXTEND;
		m_CoverQueryProps.m_vSectorPos = m_Pos + "0 2 0";
		m_CoverQueryProps.m_vThreatPos = m_Pos + "50 2 50";
		m_CoverQueryProps.m_vSectorDir = vector.Direction(m_Pos, m_CoverQueryProps.m_vThreatPos);
		m_CoverQueryProps.m_vSectorDir.Normalize();
		float distance = vector.DistanceXZ(m_CoverQueryProps.m_vThreatPos, m_Pos);
		m_CoverQueryProps.m_fQuerySectorAngleCosMin = -1;
		m_CoverQueryProps.m_fSectorDistMin = 0;
		m_CoverQueryProps.m_fSectorDistMax = 20;
		m_CoverQueryProps.m_fCoverHeightMin = 0;
		m_CoverQueryProps.m_fCoverHeightMax = 2.0;
		m_CoverQueryProps.m_fCoverToThreatAngleCosMin = -1; //Full circle
		
		m_CoverQueryProps.m_fScoreWeightDirection = 0;
		m_CoverQueryProps.m_fScoreWeightDistance = 1.0;
		
		m_CoverQueryProps.m_fNmAreaCostScale = 1.0;	//SCR_AIFindCover.NAVMESH_AREA_COST_SCALE;
		
		m_CoverQueryProps.m_bCheckVisibility = false;
		m_CoverQueryProps.m_fVisibilityTraceFraction = 0.5;
		
		m_CoverQueryProps.m_bSelectHighestScore = false; // Lowest score, nearest cover
		m_CoverQueryProps.m_iMaxCoversToCheck = COVER_COUNT;
	}
}