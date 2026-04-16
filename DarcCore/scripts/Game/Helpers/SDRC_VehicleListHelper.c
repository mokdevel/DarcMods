//Helpers SDRC_VehicleListHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various vehicle list related things
*/

//------------------------------------------------------------------------------------------------
sealed class SDRC_VehicleListHelper
{
	private const string DC_MISSIONCONFIG_FILE_VEHICLELIST = "dc_vehicleList.json";
	private const int DC_MISSIONCONFIG_FILE_VEHICLELIST_JSONVER = 1;
	
	private static ref SDRC_JsonApi2 m_JsonApi = null;
	private static ref SDRC_VehicleListConfig m_Config = new SDRC_VehicleListConfig();			
	
	static void Setup()
	{
		SDRC_Log.Add("[SDRC_VehicleListHelper:Setup] Preparing..", LogLevel.NORMAL);
		
		//Load loot config
		m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_VEHICLELIST);	
		m_JsonApi.Load(m_Config, SDRC_Config.Cast(m_Config), DC_MISSIONCONFIG_FILE_VEHICLELIST_JSONVER);		
		m_Config.Populate();
		
		//Let's find the factions for the vehicles
		foreach (SDRC_List list : m_Config.lists)
		{
			SDRC_Log.Add("[SDRC_VehicleListHelper:Setup] Setting factions for: " + list.id, LogLevel.DEBUG);
			
			foreach (string item : list.items)
			{			
				bool doDelete = false;
				
				Resource res = Resource.Load(item);
				
				if (!res || !res.IsValid())				
				{
					doDelete = true;
				}
				
				IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(res);
				if (!entitySource)
				{
					doDelete = true;
				}
				
				BaseContainer baseContainer = res.GetResource().ToBaseContainer();
				if (!baseContainer)
				{
					doDelete = true;
				}				
				
				//All good so far				
				if (!doDelete)
				{
					array<IEntityComponentSource> componentSources = {};				
					string containerClass = SCR_BaseContainerTools.GetContainerClassName(res);
					SDRC_Log.Add("[SDRC_VehicleListHelper:Setup] Found: " + containerClass + " from " + SDRC_Misc.GetSimpleEntityName(item), LogLevel.DEBUG);				
					
					if (containerClass != "Vehicle")
					{
						doDelete = true;
					}
										
					//IEntityComponentSource componentSource = SCR_EditableFactionComponentClass.GetEditableEntitySource(res);
					/*IEntityComponentSource componentSource = SCR_BaseContainerTools.FindComponentSource(res, SCR_VehicleFactionAffiliationComponent
					if (!componentSource)
					{
						
					}*/
					
					IEntityComponentSource factionComponentSource = SCR_BaseContainerTools.FindComponentSource(res, FactionAffiliationComponent);
					IEntityComponentSource factionControlComponentSource = SCR_BaseContainerTools.FindComponentSource(res, SCR_FactionAffiliationComponent);
					FactionKey factionKey = string.Empty;
					if (factionComponentSource)
					{
						factionComponentSource.Get("faction affiliation", factionKey);
					}
					else if (factionControlComponentSource)
					{
						factionControlComponentSource.Get("m_DefaultFaction", factionKey);
					}
					
					SDRC_Log.Add("[SDRC_VehicleListHelper:Setup] Faction: " + factionKey, LogLevel.DEBUG);
					
					if (list.id.Contains("WHEELED"))
					{
						if (SCR_BaseContainerTools.FindComponentSourcesOfClass(entitySource, VehicleWheeledSimulation, true, componentSources) > 0)
						{
							SDRC_Log.Add("[SDRC_VehicleListHelper:Setup] Found: VehicleWheeledSimulation from " + SDRC_Misc.GetSimpleEntityName(item), LogLevel.DEBUG);
						}
						else
						{
							doDelete = true;	
						}
					}
					
					if (list.id.Contains("HELICOPTER"))
					{
						if (SCR_BaseContainerTools.FindComponentSourcesOfClass(entitySource, VehicleHelicopterSimulation, true, componentSources) > 0)
						{
							SDRC_Log.Add("[SDRC_VehicleListHelper:Setup] Found: VehicleHelicopterSimulation from " + SDRC_Misc.GetSimpleEntityName(item), LogLevel.DEBUG);
						}
						else
						{
							doDelete = true;	
						}
					}
				}
				
				if (doDelete)
				{
					//Oops, something went wrong. 
					//Delete this item
					continue;
				}
			}
		}
		
	}

	//------------------------------------------------------------------------------------------------
	/*! 
	Find the the right vehicle
	*/	
	static ResourceName FindVehicleItem(string listName)
	{
		int lootIndex = -1;
		for (int i = 0; i < m_Config.lists.Count(); i++)		
		{
			if (m_Config.lists[i].id == listName)
			{
				lootIndex = i;
				break;
			}
		}
		
		if (lootIndex == -1)
		{
			SDRC_Log.Add("[SDRC_LootHelper:FindVehicleItem] No vehicleList with name: " + listName + ". Typo?", LogLevel.WARNING);
			return "";				
		}

		ResourceName resourceName = "";
		
		if (!m_Config.lists[lootIndex].items.IsEmpty())
		{
			resourceName = m_Config.lists[lootIndex].items.GetRandomElement();
			SDRC_Log.Add("[SDRC_LootHelper:FindVehicleItem] Selected: (" + listName + ") " + resourceName, LogLevel.DEBUG);
		}
		
		return resourceName;
	}
}
