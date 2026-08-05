//Helpers SDRC_VehicleListHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various vehicle list related things
*/

//------------------------------------------------------------------------------------------------
sealed class SDRC_VehicleListHelper
{
	private const string DC_MISSIONCONFIG_FILE_VEHICLELIST = "dc_vehicleList.json";
	private const int DC_MISSIONCONFIG_FILE_VEHICLELIST_JSONVER = 3;
	
	private static ref SDRC_JsonApi2 m_JsonApi = null;
	private static ref SDRC_VehicleListConfig m_Config = null;
	
	private static bool m_bIsReady = false;
	
	static bool Scan(int index)
	{
		if (!m_Config)
		{		
			SDRC_Log.Add("[SDRC_VehicleListHelper:Scan] Preparing..", LogLevel.NORMAL);
			
			//Load vehicle list config
			m_Config = new SDRC_VehicleListConfig();
			m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_VEHICLELIST);	
			m_JsonApi.Load(m_Config, SDRC_Config.Cast(m_Config), DC_MISSIONCONFIG_FILE_VEHICLELIST_JSONVER, safeUpdate: true);		
		}
		
		m_bIsReady = m_Config.Populate(index, false);
		
		if (m_bIsReady)
		{
			Sanitize();
			
			foreach (SDRC_List list : m_Config.lists)
			{
				SDRC_Log.Add("[SDRC_ListConfig:Populate] List: " + list.id + " (" + list.items.Count() + ")", LogLevel.DEBUG);	//NOTE: SDRC_ListConfig:Populate left here on purpose! 
				
				//Print a detailed list of items when debug level is ALL
				if (SDRC_Log.GetLogLevel() > DC_LogLevel.DEBUG)
				{				
					list.items.Debug();
				}
			}
			
			SDRC_Log.Add("[SDRC_VehicleListHelper:Scan] Done!", LogLevel.DEBUG);			
		}
		
		return m_bIsReady;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Checker to see if everything is ready.
	*/
	static bool IsReady()
	{
		return m_bIsReady;
	}
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Clean up the lists.
	*/	
	static void Sanitize()
	{		
		//Let's find the factions for the vehicles
		foreach (SDRC_List list : m_Config.lists)
		{
			SDRC_Log.Add("[SDRC_VehicleListHelper:Sanitize] LIST: " + list.id, LogLevel.SPAM);
			
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

				if (list.id.Contains("VEHICLE_TRACKED_ARMED"))
				{
					int x = 0;
				}
				
								
				string containerClass = SCR_BaseContainerTools.GetContainerClassName(res);
				SDRC_Log.Add("[SDRC_VehicleListHelper:Sanitize] Found: " + containerClass + " from " + SDRC_Misc.GetSimpleEntityName(item), LogLevel.SPAM);				
				
				if ( (containerClass != "Vehicle") && (containerClass != "Tank") )
				{
					doDelete = true;						
				}
				
				//All good so far
				if (!doDelete)
				{
					array<IEntityComponentSource> componentSources = {};				

					// --- Common for all ---
			
					//Armed vehicle
					if (list.id.Contains("_ARMED"))
					{
						if (!SDRC_Resources.HasResourceTrait(item, EEditableEntityLabel.TRAIT_ARMED))
						{
							doDelete = true;
						}
					}
					//Unarmed vehicle
					if ( (list.id.Contains("_UNARMED")) || (list.id.Contains("_TRANSPORT")) )
					{
						if (SDRC_Resources.HasResourceTrait(item, EEditableEntityLabel.TRAIT_ARMED))
						{
							doDelete = true;
						}
					}
					if (list.id.Contains("_ARMOR"))
					{
						if (!SDRC_Resources.HasResourceTrait(item, EEditableEntityLabel.TRAIT_ARMOR))
						{
							doDelete = true;
						}
					}						
					
					//Military vehicle
					if (list.id.Contains("_MILITARY"))
					{
						if (SDRC_Resources.HasResourceTrait(item, EEditableEntityLabel.FACTION_CIV))
						{
							doDelete = true;
						}
					}
					//Civilian vehicle
					if (list.id.Contains("_CIV"))
					{
						if (!SDRC_Resources.HasResourceTrait(item, EEditableEntityLabel.FACTION_CIV))
						{
							doDelete = true;
						}
					}					
				
					// --- Cars ---
					
					//Wheeled vehicle
					if (list.id.Contains("WHEELED"))
					{
						if (SCR_BaseContainerTools.FindComponentSourcesOfClass(entitySource, VehicleWheeledSimulation, true, componentSources) <= 0)
						{
							doDelete = true;	
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
					}
					else
					{
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
						
/*						if (list.id.Contains("TRANSPORT"))
						{
							string itemlow = item;
							itemlow.ToLower();
							if (!itemlow.Contains("transport"))
							{
								doDelete = true;
							}
						}					
						if (list.id.Contains("ARMED"))
						{
							string itemlow = item;
							itemlow.ToLower();
							if ( (!itemlow.Contains("_gunship")) && (!itemlow.Contains("_armed")) )
							{
								doDelete = true;
							}
						}*/
					}					
				}
				
				if (doDelete)
				{
					//Oops, something went wrong. Delete this item
					SDRC_Log.Add("[SDRC_VehicleListHelper:Sanitize] Removed from list: " + SDRC_Misc.GetSimpleEntityName(item), LogLevel.SPAM);
					list.items.RemoveOrdered(i);
					list.factions.RemoveOrdered(i);
					i--;
				}
				else
				{
					//Add faction
					SDRC_Log.Add("[SDRC_VehicleListHelper:Sanitize] Ok: " + SDRC_Misc.GetSimpleEntityName(item) + " (" + list.factions[i] + ")", LogLevel.SPAM);
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
			SDRC_Log.Add("[SDRC_VehicleListHelper:FindVehicleItem] No vehicleList with name: " + listName + ". Typo?", LogLevel.WARNING);
			return "";				
		}

		//Fix faction according to aka
		foreach (SDRC_Aka aka : m_Config.akas)
		{
			if (aka.names.Count() > 1)
			{
				if (faction == aka.names[0])
				{
					faction = aka.names[1];
				}
			}
		}
		
		//Find the proper vehicle		
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
						if (resourceName == "")
						{
							SDRC_Log.Add("[SDRC_VehicleListHelper:FindVehicleItem] No vehicle matching faction: " + faction + ". Selecting random one.", LogLevel.DEBUG);
						}
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
					SDRC_Log.Add("[SDRC_VehicleListHelper:FindVehicleItem] No vehicle matching faction: " + faction + ". Selecting random one.", LogLevel.DEBUG);
					faction = "";
				}
			}
			
			if (faction == "")
			{
				resourceName = m_Config.lists[vehicleIndex].items.GetRandomElement();
			}
			
			SDRC_Log.Add("[SDRC_VehicleListHelper:FindVehicleItem] Selected: (" + listName + ") " + resourceName, LogLevel.DEBUG);
		}
		else
		{
			SDRC_Log.Add("[SDRC_VehicleListHelper:FindVehicleItem] Vehicle list: " + m_Config.lists[vehicleIndex].id + " is empty!", LogLevel.WARNING);
		}
		
		return resourceName;
	}
}
