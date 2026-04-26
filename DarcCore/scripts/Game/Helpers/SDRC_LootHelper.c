//Helpers SDRC_LootHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various loot related things
*/
		
//------------------------------------------------------------------------------------------------
class SDRC_Loot : Managed
{
	IEntity box = null;
	float itemChance = 1.0;
	ref array<string> items = {};
	
	void Set(float itemChance_, array<string> items_)
	{
		itemChance = itemChance_;
		items = items_;
	}
}

//------------------------------------------------------------------------------------------------
sealed class SDRC_LootHelper
{
	private const string DC_MISSIONCONFIG_FILE_LOOTLIST = "dc_lootList.json";
	private const int DC_MISSIONCONFIG_FILE_LOOTLIST_JSONVER = 2;
	
	private static ref SDRC_JsonApi2 m_JsonApi = null;
	private static ref SDRC_LootListConfig m_Config = new SDRC_LootListConfig();			
	
	static void Setup()
	{
		SDRC_Log.Add("[SDRC_LootHelper:Setup] Preparing..", LogLevel.NORMAL);
		
		//Load loot config
		m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_LOOTLIST);	
		m_JsonApi.Load(m_Config, SDRC_Config.Cast(m_Config), DC_MISSIONCONFIG_FILE_LOOTLIST_JSONVER, safeUpdate: true);		
		m_Config.Populate();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn a list of items to an entity storage. 
	Useful to fill for example a crate with items.
	\param storage The entity with to fill
	\param itemNames An array of resource names
	\param chance The percentage each item may be spawned. 1.0 = 100% so everything is spawned.
	*/
//	static void SpawnItemsToStorage(IEntity storage, array<string> itemNames, float itemChance = 1.0, SDRC_EDifficulty difficulty = SDRC_EDifficulty.IGNORE)
	static void SpawnItemsToStorage(IEntity storage, array<string> itemNames, float itemChance = 1.0)
	{
		SDRC_Log.Add("[SDRC_LootHelper:SpawnItemsToStorage] Storage: " + storage, LogLevel.SPAM);
		SDRC_Log.Add("[SDRC_LootHelper:SpawnItemsToStorage] Items: " + itemNames, LogLevel.SPAM);
		SDRC_Log.Add("[SDRC_LootHelper:SpawnItemsToStorage] Chance: " + itemChance, LogLevel.SPAM);
		
		if (!storage)
		{
			SDRC_Log.Add("[SDRC_LootHelper:SpawnItemsToStorage] Storage not available.", LogLevel.ERROR);
			return;
		}
		
		if (itemNames.IsEmpty())
		{
			SDRC_Log.Add("[SDRC_LootHelper:SpawnItemsToStorage] List of loot is empty.", LogLevel.ERROR);
			return;
		}
		
		foreach (string itemName : itemNames)
		{
			if (SDRC_Misc.RandomFloat(0, 1) < itemChance)
			{
				ResourceName resource = "";
				
				if (itemName[0] == "{")			//Manually defined prefabs are added
				{
					resource = itemName;
				}
				else
				{
					resource = FindLootItem(itemName);
				}
				
				bool result = AddToStorage(storage, resource);
				SDRC_Log.Add("[SDRC_LootHelper:SpawnItemsToStorage] Adding item " + resource + ". Success: " + result, LogLevel.DEBUG);
				
				//Shall we add ammo? Ammo is to be added with itemChance%
				if ((SDRC_Misc.RandomFloat(0, 1) < itemChance))
				{
					bool addToBox = false;
										
					//If it's defined as a WEAPON_ list item, add ammo to box
					if (itemName.Contains("WEAPON_"))
					{
						addToBox = true;
					}
					else 
					{ 	//If using original prefab name and it's not magazine, ammo nor an item, add to box
						if ( !addToBox &&
						     (!resource.Contains("/Weapons/Magazines/")) && 
						     (!resource.Contains("/Weapons/Ammo/")) &&
						     (!resource.Contains("/Weapons/Attachments/")) &&
						     (!resource.Contains("/Weapons/Grenades/")) &&
						     (!resource.Contains("Prefabs/Items/")) //&&			//Items
						     //(!resource.Contains("Prefabs/Characters/"))		//Clothing etc
						   )
						{
							addToBox = true;
						}
					}
					
					//Add ammo to box 
					if (addToBox)
					{
						int magCount = SDRC_Misc.RandomFloat(0, 6);
						if (magCount > 0)
						{
							string magazine = SDRC_AmmoHelper.GetCompatibleMagazineForPrefab(resource);
						
							for (int i = 0; i < magCount; i++)
							{
								result = AddToStorage(storage, magazine);
								if (magazine != "")
								{
									SDRC_Log.Add("[SDRC_LootHelper:SpawnItemsToStorage] Adding magazine " + magazine + ". Success: " + result, LogLevel.DEBUG);				
								}
							}
						}
					}
				}
			}
		}
	}		

	//------------------------------------------------------------------------------------------------
	/*! 
	Find the loot item
	*/	
	static ResourceName FindLootItem(string listName)
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
			SDRC_Log.Add("[SDRC_LootHelper:FindLootItem] No lootList with name: " + listName + ". Typo?", LogLevel.WARNING);
			return "";				
		}

		ResourceName resourceName = "";
		
		if (!m_Config.lists[lootIndex].items.IsEmpty())
		{
			resourceName = m_Config.lists[lootIndex].items.GetRandomElement();
			SDRC_Log.Add("[SDRC_LootHelper:FindLootItem] Selected: (" + listName + ") " + resourceName, LogLevel.DEBUG);
		}
		
		return resourceName;
	}

	//------------------------------------------------------------------------------------------------
	/*! 
	Give full loot list
	*/	
	static bool GetLootListItems(out array<string> items, string listName)
	{
		//Find the right list index		
		int lootIndex = SDRC_ListHelper.FindRightList(m_Config.lists, listName);
		
/*		int lootIndex = -1;
		for (int i = 0; i < m_Config.lists.Count(); i++)		
		{
			if (m_Config.lists[i].id == listName)
			{
				lootIndex = i;
				break;
			}
		}*/
		
		if (lootIndex == -1)
		{
			SDRC_Log.Add("[SDRC_LootHelper:FindLootItem] No lootList with name: " + listName + ". Typo?", LogLevel.WARNING);
			return false;				
		}

		SDRC_Log.Add("[SDRC_LootHelper:GetLootListItems] Found: " + listName, LogLevel.DEBUG);
				
		items.Copy(m_Config.lists[lootIndex].items);
		return true;
	}	

	//------------------------------------------------------------------------------------------------
	/*! 
	Try to add an item to a storage of an entity
	*/	
	static bool AddToStorage(IEntity entity, ResourceName item)
	{	
		//NOTE: The below Resource.Load will result in an error if the ResourceName is not available. For example from 
		//if (FileIO.FileExists("Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et")) ... 
				
		Resource resource = Resource.Load(item);
		if (!resource.IsValid())
			return null;		
		
		ScriptedInventoryStorageManagerComponent storageManager = ScriptedInventoryStorageManagerComponent.Cast(entity.FindComponent(ScriptedInventoryStorageManagerComponent));			
		if (storageManager)
		{				
			return storageManager.TrySpawnPrefabToStorage(item);
		}
		else
		{
			ResourceName res = entity.GetPrefabData().GetPrefabName();
			SDRC_Log.Add("[SDRC_LootHelper:AddToStorage] storageManager not found on: " + SDRC_Misc.GetSimpleEntityName(res), LogLevel.ERROR);
			return false;
		}
	}

}
