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
		
		Sanitize();
	}
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Remove all non-vehicles. Add faction to the list items.
	*/	
	static void Sanitize()
	{		
		//Let's find the factions for the vehicles
		foreach (SDRC_List list : m_Config.lists)
		{
			SDRC_Log.Add("[SDRC_VehicleListHelper:Sanitize] Setting factions for: " + list.id, LogLevel.DEBUG);
			
			//foreach (string item : list.items)
			for (int i = 0; i < list.items.Count() - 1; i++)
			{			
				bool doDelete = false;
				//FactionKey factionKey = string.Empty;
				string item = list.items[i];				
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
				
				/*BaseContainer baseContainer = res.GetResource().ToBaseContainer();
				if (!baseContainer)
				{
					doDelete = true;
				}*/
				
				string containerClass = SCR_BaseContainerTools.GetContainerClassName(res);
				SDRC_Log.Add("[SDRC_VehicleListHelper:Sanitize] Found: " + containerClass + " from " + SDRC_Misc.GetSimpleEntityName(item), LogLevel.SPAM);				
				
				if (containerClass != "Vehicle")
				{
					doDelete = true;						
				}
				
				//All good so far
				if (!doDelete)
				{
					array<IEntityComponentSource> componentSources = {};				
/*					IEntityComponentSource factionComponentSource = SCR_BaseContainerTools.FindComponentSource(res, FactionAffiliationComponent);
					IEntityComponentSource factionControlComponentSource = SCR_BaseContainerTools.FindComponentSource(res, SCR_FactionAffiliationComponent);
					
					//Find the faction, if any
					if (factionComponentSource)
					{
						factionComponentSource.Get("faction affiliation", factionKey);
					}
					else if (factionControlComponentSource)
					{
						factionControlComponentSource.Get("m_DefaultFaction", factionKey);
					}*/
					
					//factionKey = SDRC_Resources.GetResourceFaction(item);
					//SDRC_Log.Add("[SDRC_VehicleListHelper:Sanitize] Faction: " + factionKey, LogLevel.SPAM);
					
					if (list.id.Contains("WHEELED"))
					{
						if (SCR_BaseContainerTools.FindComponentSourcesOfClass(entitySource, VehicleWheeledSimulation, true, componentSources) > 0)
						{
							SDRC_Log.Add("[SDRC_VehicleListHelper:Sanitize] Found: VehicleWheeledSimulation from " + SDRC_Misc.GetSimpleEntityName(item), LogLevel.SPAM);
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
							SDRC_Log.Add("[SDRC_VehicleListHelper:Sanitize] Found: VehicleHelicopterSimulation from " + SDRC_Misc.GetSimpleEntityName(item), LogLevel.SPAM);
						}
						else
						{
							doDelete = true;	
						}
					}
				}
				
				if (doDelete)
				{
					//Oops, something went wrong. Delete this item
					SDRC_Log.Add("[SDRC_VehicleListHelper:Sanitize] Removed from list: " + SDRC_Misc.GetSimpleEntityName(item), LogLevel.DEBUG);
					list.items.RemoveOrdered(i);
					i--;
					if (i < 0)					
					{
						i = 0;
					}
				}
				else
				{
					//Add faction
					//list.factions[i] = factionKey;
					SDRC_Log.Add("[SDRC_VehicleListHelper:Sanitize] Ok: " + SDRC_Misc.GetSimpleEntityName(item) + " (" + list.factions[i] + ")", LogLevel.DEBUG);
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
