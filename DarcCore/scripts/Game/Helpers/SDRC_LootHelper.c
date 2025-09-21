//Helpers SDRC_LootHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various loot related things
*/

const string DC_MISSIONCONFIG_FILE_LOOTLIST = "dc_lootList.json";
		
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
	private static ref SDRC_LootListJsonApi m_LootListJsonApi;
	private static ref SDRC_ListConfig m_Config;
	
	static void Setup()
	{
		SDRC_Log.Add("[SDRC_LootHelper:Setup] Preparing..", LogLevel.NORMAL);
		
		//Load loot config
		m_LootListJsonApi = new SDRC_LootListJsonApi(DC_MISSIONCONFIG_FILE_LOOTLIST);
		m_LootListJsonApi.Load();
		m_Config = m_LootListJsonApi.conf;
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
	static void SpawnItemsToStorage(IEntity storage, array<string> itemNames, float itemChance = 1.0)
	{
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
		
		foreach (string itemName: itemNames)
		{
			if (Math.RandomFloat(0, 1) < itemChance)
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
				if ((Math.RandomFloat(0, 1) < itemChance))
				{
					bool addToBox = false;
					
					//If it's defined as a list item, add to box
					if (itemName.Contains("WEAPON_"))
					{
						addToBox = true;
					}
					else 
					{ //If using original prefab name and it's not magazine nor ammo, add to box
						if ( !addToBox &&
						     (!resource.Contains("/Weapons/Magazines/")) && 
						     (!resource.Contains("/Weapons/Ammo/")) &&
						     (!resource.Contains("/Weapons/Attachments/")) &&
						     (!resource.Contains("/Weapons/Grenades/")) &&
						     (!resource.Contains("Prefabs/Items/"))
						   )
						{
							addToBox = true;
						}
					}
					
					//Add ammo to box 
					if (addToBox)
					{
						int magCount = Math.RandomFloat(0, 4);
						if (magCount > 0)
						{
							string magazine = SDRC_AmmoHelper.GetCompatibleMagazineForPrefab(resource);
						
							for (int i = 0; i < magCount; i++)
							{
								result = AddToStorage(storage, magazine);
								SDRC_Log.Add("[SDRC_LootHelper:SpawnItemsToStorage] Adding magazine " + magazine + ". Success: " + result, LogLevel.DEBUG);				
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

		ResourceName resourceName = m_Config.lists[lootIndex].items.GetRandomElement();
		SDRC_Log.Add("[SDRC_LootHelper:FindLootItem] Selected: (" + listName + ") " + resourceName, LogLevel.DEBUG);
		
		return resourceName;
	}

	//------------------------------------------------------------------------------------------------
	/*! 
	Give full loot list
	*/	
	static bool GetLootListItems(out array<string>items, string listName)
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
			SDRC_Log.Add("[SDRC_LootHelper:AddToStorage] storageManager not found", LogLevel.ERROR);
			return false;
		}
	}	
}
