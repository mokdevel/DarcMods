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
		m_Config.Populate(false);
		
		Sanitize();
		
		if (SDRC_Log.GetLogLevel() > DC_LogLevel.NORMAL)
		{
			foreach (SDRC_List list : m_Config.lists)
			{
				SDRC_Log.Add("[SDRC_ListConfig:Populate] List: " + list.id + " (" + list.items.Count() + ")", LogLevel.DEBUG);	//NOTE: SDRC_ListConfig:Populate left here on purpose! 
				list.items.Debug();
			}
		}
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
			SDRC_Log.Add("[SDRC_VehicleListHelper:Sanitize] " + list.id, LogLevel.DEBUG);
			
			//foreach (string item : list.items)
			for (int i = 0; i < list.items.Count(); i++)
			{			
				bool doDelete = false;
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
					
					// --- Cars ---
					
					//Wheeled vehicle
					if (list.id.Contains("WHEELED"))
					{
						if (SCR_BaseContainerTools.FindComponentSourcesOfClass(entitySource, VehicleWheeledSimulation, true, componentSources) <= 0)
						{
							doDelete = true;	
						}
					}
					//Military vehicle
					if (list.id.Contains("MILITARY"))
					{
						if (list.factions[i] == "CIV")
						{
							doDelete = true;
						}
					}
					//Civilian vehicle
					if (list.id.Contains("CIV"))
					{
						if (list.factions[i] != "CIV")
						{
							doDelete = true;
						}
					}
					//Armed vehicle
					if (list.id.Contains("ARMED"))
					{
						if (SDRC_Resources.GetResourceVehicleType(item) != EVehicleType.APC)
						{
							doDelete = true;
						}
					}								
					//Car
					if (list.id.Contains("CAR"))
					{
						if (SDRC_Resources.GetResourceVehicleType(item) != EVehicleType.CAR)
						{
							doDelete = true;
						}
					}								
					//Truck
					if (list.id.Contains("TRUCK"))
					{
						if ( (SDRC_Resources.GetResourceVehicleType(item) != EVehicleType.TRUCK) 
						  && (SDRC_Resources.GetResourceVehicleType(item) != EVehicleType.COMM_TRUCK) 
						  && (SDRC_Resources.GetResourceVehicleType(item) != EVehicleType.FUEL_TRUCK) 
						  && (SDRC_Resources.GetResourceVehicleType(item) != EVehicleType.SUPPLY_TRUCK) 
						)  
						{
							doDelete = true;
						}
					}								
					
					// --- Helicopters ---
							
					if (list.id.Contains("CHOPPER"))
					{
						if (SCR_BaseContainerTools.FindComponentSourcesOfClass(entitySource, SDRC_ChopperComp, true, componentSources) <= 0)
						{
							doDelete = true;
						}
					}					
					
					if (list.id.Contains("HELICOPTER"))
					{
						if (SCR_BaseContainerTools.FindComponentSourcesOfClass(entitySource, VehicleHelicopterSimulation, true, componentSources) <= 0)
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
					list.factions.RemoveOrdered(i);
					i--;
				}
				else
				{
					//Add faction
					SDRC_Log.Add("[SDRC_VehicleListHelper:Sanitize] Ok: " + SDRC_Misc.GetSimpleEntityName(item) + " (" + list.factions[i] + ")", LogLevel.DEBUG);
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	/*! 
	Find the the right vehicle
	*/	
	static ResourceName FindVehicleItem(string listName, string faction = "")
	{
		int vehicleIndex = -1;
		for (int i = 0; i < m_Config.lists.Count(); i++)		
		{
			if (m_Config.lists[i].id == listName)
			{
				vehicleIndex = i;
				break;
			}
		}
		
		if (vehicleIndex == -1)
		{
			SDRC_Log.Add("[SDRC_LootHelper:FindVehicleItem] No vehicleList with name: " + listName + ". Typo?", LogLevel.WARNING);
			return "";				
		}

		ResourceName resourceName = "";
		
		if (!m_Config.lists[vehicleIndex].items.IsEmpty())
		{
			//If faction defined, try to find a faction suitable vehicle
			if (faction != "")
			{
				int idx = SDRC_Misc.RandomInt(0, m_Config.lists[vehicleIndex].items.Count() - 1);
				for (int i = 0; i < m_Config.lists[vehicleIndex].items.Count(); i++)
				{
					if (m_Config.lists[vehicleIndex].factions[idx] == faction)
					{
						resourceName = m_Config.lists[vehicleIndex].items[idx];
						break;
					}
					
					idx++;
					if (idx == m_Config.lists[vehicleIndex].items.Count())
					{
						idx = 0;
					}
				}
				
				if (resourceName == "")
				{
					SDRC_Log.Add("[SDRC_LootHelper:FindVehicleItem] No vehicle matching faction: " + faction + ". Selecting random one.", LogLevel.DEBUG);
					faction = "";
				}
			}
			
			if (faction == "")
			{
				resourceName = m_Config.lists[vehicleIndex].items.GetRandomElement();
			}
			
			SDRC_Log.Add("[SDRC_LootHelper:FindVehicleItem] Selected: (" + listName + ") " + resourceName, LogLevel.DEBUG);
		}
		
		return resourceName;
	}
}
