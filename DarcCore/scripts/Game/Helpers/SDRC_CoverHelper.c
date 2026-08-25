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
	private float m_BuildingSize = 0;			//The max width
	private float m_BuildingHeight = 0;			//The height 
	
	private vector m_Pos = vector.Zero;
	private ref array<ref SDRC_CoverPos> m_aCovers = {};
	private bool m_Running = false;				//All good and necessary components were found
	private bool m_Ready = false;				//Cover finding is ready

	const int NAVMESH_LOAD_TRY_LIMIT = 10;
	private int m_iNavmeshLoadTries = 0;
	
	private AIPathfindingComponent m_PathFindindingComp;
	private ChimeraCoverManagerComponent m_CoverMgr;
	private ref CoverQueryProperties m_CoverQueryProps = new CoverQueryProperties();
	private NavmeshWorldComponent m_Navmesh;
	
	private AIAgent m_AiAgent = null;
	private SCR_AIGroup m_Group = null;
	
	//------------------------------------------------------------------------------------------------
	void SDRC_CoverHelper(IEntity building)
	{
		m_Building = building;

		m_Pos = building.GetOrigin();
		vector sums = SDRC_SpawnHelper.FindEntitySize(building);
		m_BuildingHeight = sums[1];
		sums[1] = 0;
		m_BuildingSize = SDRC_Misc.FindMaxValue(sums);
		
		string resourceName = "{5B1996C05B1E51A4}Prefabs/Characters/Factions/BLUFOR/US_Army/Character_US_AR.et";
		m_AiAgent = SDRC_AIHelper.SpawnAIAgent(resourceName, m_Pos, "", true);
		
		string faction = SDRC_AIHelper.GetAIAgentFactionKey(m_AiAgent);
		m_Group = SDRC_AIHelper.GroupCreate(faction, m_AiAgent.GetOrigin());
		if (!m_Group)
		{
			SDRC_Log.Add("[SDRC_CoverHelper] Could not spawn AIAgent.", LogLevel.ERROR);
			return;
		}	
		
		m_Group.SetNewLeader(m_AiAgent);
		m_Group.AddAgent(m_AiAgent);
		
		AIWorld aiWorld = GetGame().GetAIWorld();
		if (aiWorld)
			m_CoverMgr = ChimeraCoverManagerComponent.Cast(aiWorld.FindComponent(ChimeraCoverManagerComponent));
		
		IEntity entity = m_AiAgent.GetControlledEntity();
		if (entity)
			m_PathFindindingComp = AIPathfindingComponent.Cast(entity.FindComponent(AIPathfindingComponent));
		
		m_PathFindindingComp = AIPathfindingComponent.Cast(m_Group.FindComponent(AIPathfindingComponent));		
		
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
	vector GetPosition(int index = -1)
	{
		if (index == -1)
		{
			index = m_aCovers.GetRandomIndex();
		}
		
		if (index >= m_aCovers.Count())
		{
			SDRC_Log.Add("[SDRC_CoverHelper:GetPosition] Index out of bounds. Using random.", LogLevel.ERROR);
			index = m_aCovers.GetRandomIndex();
		}
		
		return m_aCovers[index].pos;
	}
	
	//------------------------------------------------------------------------------------------------
	void FindBuildingCovers()
	{
		if (!m_Navmesh.IsTileLoaded(m_Pos))
		{
			//Try only for a few times 
			if (m_iNavmeshLoadTries > NAVMESH_LOAD_TRY_LIMIT)
			{
				//Ok... we failed. 
				m_Ready = false;
				//And stop...
				return; 
			}
			
			if (m_iNavmeshLoadTries == 0)
			{			
				//Load tile
				m_Navmesh.LoadTileIn(m_Pos);
				//Load some neighboring tiles too
	/*			vector tileNeighbor = {m_BuildingSize, 0, m_BuildingSize};
				m_Navmesh.LoadTileIn(m_Pos + tileNeighbor);
				tileNeighbor = {-m_BuildingSize, 0, -m_BuildingSize};
				m_Navmesh.LoadTileIn(m_Pos + tileNeighbor);*/
				
				SDRC_Log.Add("[SDRC_CoverHelper:FindBuildingCovers] Requested tile loading.", LogLevel.DEBUG);
			}
			
			m_iNavmeshLoadTries++;

			//Once valid, continue			
			if (!m_Navmesh.IsTileValid(m_Pos))
			{		
				//Not yet, wait...
				GetGame().GetCallqueue().CallLater(FindBuildingCovers, 2000, false);		
				return;				
			}
		}		
		
		//Tile was loaded. Find the covers
		vector coverPos, coverTallestPos;
		int tileX, tileY, coverId;
		
		for (int i = 0; i < COVER_COUNT; i++)
		{
			vector heightAdjust = vector.Zero;
			heightAdjust[1] = Math.Round(SDRC_Misc.RandomFloat(0, (m_BuildingHeight - 5) / 2) * 2) + 1;
			m_CoverQueryProps.m_vSectorPos = m_Pos + heightAdjust;
			
			bool coverFound = m_CoverMgr.GetBestCover("Soldiers", m_PathFindindingComp, m_CoverQueryProps, coverPos, coverTallestPos, tileX, tileY, coverId);	
			if (coverFound)
			{
				ref SDRC_CoverPos tmpCoverPos = new SDRC_CoverPos();
				tmpCoverPos.tileX = tileX;
				tmpCoverPos.tileY = tileY;
				tmpCoverPos.coverId = coverId;
				tmpCoverPos.pos = coverPos;
				m_aCovers.Insert(tmpCoverPos);
	
				m_CoverMgr.SetOccupiedCover(tileX, tileY, coverId, true);
				SDRC_DebugHelper.AddDebugSphere(coverPos, ARGB(50, 0, 128, 0), 0.25);
			}
			else
			{
				int xx = 0;
			}
		}
		
		//All collected, free them
		FreeCovers();
		
		m_Ready = true;
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
		m_CoverQueryProps.m_fSectorDistMax = m_BuildingSize;
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