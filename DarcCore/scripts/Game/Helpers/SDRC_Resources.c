//Helpers SDRC_Resources

//------------------------------------------------------------------------------------------------
/*!
Functions to find resources (for example cities) from game.
*/

sealed class SDRC_Resources
{
	private static ref array<string> m_resourceNames = {};
						
	//------------------------------------------------------------------------------------------------
	/*!
	Get items for the various lists like lootList. The items are the resource name of a prefab
	\param itemList The itemList to fill with loot items
	\param mod From which mod to search for items. Example: "$ArmaReforger:Prefabs/Weapons"
	\param lootList 
	*/	
	static void GetList(out array<string> list, string mod, SDRC_List lootList)
	{
		foreach (string modDir : lootList.modDir)
		{
			SearchResourcesFilter filter = new SearchResourcesFilter();
//			filter.rootPath = mod + lootList.modDir;
			filter.rootPath = mod + modDir;
			filter.fileExtensions = {"et"};
			filter.searchStr = {""};
			filter.recursive = true;

			m_resourceNames = {};
			ResourceDatabase.SearchResources(filter, GetResourcesCallback);		
			
			IncludeFilter(m_resourceNames, lootList.include);
			ExcludeFilter(m_resourceNames, lootList.exclude);
			
			list.InsertAll(m_resourceNames);
		}
		
//		list.Debug();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Remove the items from array that match the filters
	*/	
	static void ExcludeFilter(out array<string> resourceNames, array<string> filters)
	{	
		foreach (string filter : filters)
		{
			filter.ToLower();
			for (int i = 0; i < resourceNames.Count(); i++)		
			{
				string resourceName = resourceNames[i];
				resourceName.ToLower();
				
				if (resourceName.Contains(filter))
				{
					resourceNames.Remove(i);
					i--;
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Include only the items in the array that match the filters. Remove others.
	*/	
	static void IncludeFilter(out array<string> resourceNames, array<string> filters)
	{
		for (int i = 0; i < resourceNames.Count(); i++)		
		{
			bool ToBeRemoved = true;
			foreach (string filter : filters)
			{ 
				filter.ToLower();
				string resourceName = resourceNames[i];
				resourceName.ToLower();
				
				if (resourceName.Contains(filter))
				{
					ToBeRemoved = false;
					break;
				}
			}
		
			if (ToBeRemoved)
			{
				resourceNames.Remove(i);
				i--;
			}
		}
	}
		
	//------------------------------------------------------------------------------------------------
	/*!
	Call back filter for ResourceDatabase.SearchResources
	*/		
	static private bool GetResourcesCallback(ResourceName resourceName, string exactPath = "")
	{
		SDRC_Log.Add("[SDRC_Resources:GetResourcesFilter] Found: " + resourceName + " at " + exactPath, LogLevel.SPAM);
		m_resourceNames.Insert(resourceName);		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find resource faction .. if any
	
	TBD: Add functionality to recognize AI and group faction
	*/		
	static string GetResourceFaction(ResourceName resourceName)
	{
		FactionKey factionKey = string.Empty;
		
		Resource res = Resource.Load(resourceName);
		
		if (!res || !res.IsValid())				
		{
			return factionKey;
		}
		
		IEntityComponentSource factionComponentSource = SCR_BaseContainerTools.FindComponentSource(res, FactionAffiliationComponent);
		IEntityComponentSource factionControlComponentSource = SCR_BaseContainerTools.FindComponentSource(res, SCR_FactionAffiliationComponent);
		
		//Find the faction, if any
		if (factionComponentSource)
		{
			factionComponentSource.Get("faction affiliation", factionKey);
		}
		else if (factionControlComponentSource)
		{
			factionControlComponentSource.Get("m_DefaultFaction", factionKey);
		}
		
		//SDRC_Log.Add("[SDRC_Resources:GetResourceFaction] Faction: " + factionKey, LogLevel.DEBUG);
		
		return factionKey;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find vehicle type
	*/		
	static EVehicleType GetResourceVehicleType(ResourceName resourceName)
	{
		EVehicleType vehicleType = EVehicleType.VEHICLE;
		
		Resource res = Resource.Load(resourceName);
		
		if (!res || !res.IsValid())				
		{
			return vehicleType;
		}
		
		BaseContainer baseContainer = res.GetResource().ToBaseContainer();
		if (!baseContainer)
		{
			return null;
		}
		
		baseContainer.Get("m_eVehicleType", vehicleType);
		//SDRC_Log.Add("[SDRC_Resources:GetResourceVehicleType] Vehicle type: " + vehicleType, LogLevel.DEBUG);
		
		return vehicleType;
	}		
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find component within an entity source
	
	Original found in: SCR_DestructionIndicesAssignTool
	*/		
	static IEntityComponentSource FindComponent(IEntitySource source, string componentType)
	{
		for (int i = source.GetComponentCount() - 1; i >= 0; i--)
		{
			if (source.GetComponent(i).GetClassName() == componentType)
				return source.GetComponent(i);
		}

		return null;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find traits
	*/		
	static bool HasResourceTrait(ResourceName resourceName, EEditableEntityLabel trait)
	{
		Resource res = Resource.Load(resourceName);
		
		IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(res);
		if (!entitySource)
		{
			return false;
		}
		
		IEntityComponentSource editableComponentSource = SDRC_Resources.FindComponent(entitySource, "SCR_EditableVehicleComponent");
		if (!editableComponentSource)
		{
			return false;
		}
		
		BaseContainer container = editableComponentSource.GetObject("m_UIInfo");
		if (!container)
		{			
			return false;
		}
		
		array<EEditableEntityLabel> labels = {};
		container.Get("m_aAutoLabels", labels);
		foreach (EEditableEntityLabel label : labels)
		{
			if (label == trait)
			{
				return true;
			}
		}

		container.Get("m_aAuthoredLabels", labels);
		foreach (EEditableEntityLabel label : labels)
		{
			if (label == trait)
			{
				return true;
			}
		}
		
		return false;
	}		
}

/*
	//ResourceName resourceName = "{5678893357C6FC10}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HE_Patrol.et";
	ResourceName resourceName = "{2A5CFC83306263A4}Prefabs/Props/Industrial/CargoContainers/CargoContainer_01/CargoContainer_02_20ft_ENP.et";
	
	Resource res = Resource.Load(resourceName);
	
	BaseContainer childContainer;
	BaseContainer baseContainer = res.GetResource().ToBaseContainer();
	if (!baseContainer)
	{
		Print("No basecontainer");
		return;
	}
	
	BaseContainerList baseContainerList = baseContainer.GetObjectArray("components");
	if (!baseContainerList)
	{
		Print("No baseContainerList");
		return;
	}
	
	for (int i, count = baseContainerList.Count(); i < count; i++)
	{
		childContainer = baseContainerList.Get(i);
		if (!childContainer)
			continue;
	
		if (childContainer.GetClassName() == "SCR_DestructionMultiPhaseComponent")
		{
			Print("FOUND!");
			baseContainer = childContainer;
			break;
		}
	}
	
	IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(res);
	if (!entitySource)
	{
		Print("No entitySource");
		return;
	}
	
	IEntityComponentSource destructionComponent = SDRC_Resources.FindComponent(entitySource, "SCR_DestructionMultiPhaseComponent");
	
	// Setup first phase
	array<ref SCR_DamagePhaseData> damagePhases = {};
	destructionComponent.Get("m_aDamagePhases", damagePhases);
	int damagePhasesCount = damagePhases.Count();
	Print("Count: " + damagePhasesCount);

*/

/*
	ResourceName resourceName = "{5678893357C6FC10}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HE_Patrol.et";
	//ResourceName resourceName = "{2A5CFC83306263A4}Prefabs/Props/Industrial/CargoContainers/CargoContainer_01/CargoContainer_02_20ft_ENP.et";
	
	Resource res = Resource.Load(resourceName);
	
	BaseContainer childContainer;
	BaseContainer baseContainer = res.GetResource().ToBaseContainer();
	if (!baseContainer)
	{
		Print("No basecontainer");
		return;
	}
	
	BaseContainerList baseContainerList = baseContainer.GetObjectArray("components");
	if (!baseContainerList)
	{
		Print("No baseContainerList");
		return;
	}
	
	for (int i, count = baseContainerList.Count(); i < count; i++)
	{
		childContainer = baseContainerList.Get(i);
		if (!childContainer)
			continue;
	
		Print("BCL: " + childContainer.GetClassName());
		
		if (childContainer.GetClassName() == "SCR_DestructionMultiPhaseComponent")
		{
			Print("FOUND!");
			baseContainer = childContainer;
			break;
		}
	}
*/

/*
ResourceName resourceName = "{5678893357C6FC10}Prefabs/Vehicles/Helicopters/Mi8MT/Mi8MT_armed_gunship_HE_Patrol.et";
//ResourceName resourceName = "{2A5CFC83306263A4}Prefabs/Props/Industrial/CargoContainers/CargoContainer_01/CargoContainer_02_20ft_ENP.et";

Resource res = Resource.Load(resourceName);

IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(res);
if (!entitySource)
{
	Print("No entitySource");
	return;
}

IEntityComponentSource editableComponentSource = SDRC_Resources.FindComponent(entitySource, "SCR_EditableVehicleComponent");
if (!editableComponentSource)
{
	Print("No editableComponentSource");
	return;
}

BaseContainer container = editableComponentSource.GetObject("m_UIInfo");
if (!container)
{
	Print("No container");
	return;
}

// Setup first phase
array<EEditableEntityLabel> ll = {};
//container.Get("m_aAuthoredLabels", ll);
container.Get("m_aAutoLabels", ll);
int llCount = ll.Count();
Print("Labels: " + llCount); */