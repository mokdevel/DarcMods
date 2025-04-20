//Helpers SCR_DC_LootHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various loot related things
*/

//------------------------------------------------------------------------------------------------
class SCR_DC_Loot : Managed
{
	IEntity box;
	float itemChance = 1.0;
	ref array<string> items = {};
	
	void Set(float itemChance_, array<string> items_)
	{
		itemChance = itemChance_;
		items = items_;
	}
}

//------------------------------------------------------------------------------------------------
sealed class SCR_DC_LootHelper
{
	private static ref SCR_DC_LootListJsonApi m_LootListJsonApi = new SCR_DC_LootListJsonApi();	
	private static ref SCR_DC_LootListConfig m_Config;
	
	static void Setup()
	{
		//Load loot config
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
		foreach(string itemName: itemNames)
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
				SCR_DC_Log.Add("[SCR_DC_LootHelper:SpawnItemsToStorage] Adding item " + resource + ". Success: " + result, LogLevel.DEBUG);
			}
		}
	}		

	//------------------------------------------------------------------------------------------------
	/*! 
	Find the loot item
	*/	
	static ResourceName FindLootItem(string lootListName)
	{
		int lootIndex = -1;
		for (int i = 0; i < m_Config.lootLists.Count(); i++)		
		{
			if (m_Config.lootLists[i].lootListName == lootListName)
			{
				lootIndex = i;
				break;
			}
		}
		
		if (lootIndex == -1)
		{
			SCR_DC_Log.Add("[SCR_DC_LootHelper:FindLootItem] No lootList with name: " + lootListName + ". Typo?", LogLevel.WARNING);
			return "";				
		}

		SCR_DC_Log.Add("[SCR_DC_LootHelper:SpawnItemsToStorage] Using lootlist: " + lootListName, LogLevel.DEBUG);
		ResourceName resourceName = m_Config.lootLists[lootIndex].itemList.GetRandomElement();
		return resourceName;
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
		if(storageManager)
		{				
			return storageManager.TrySpawnPrefabToStorage(item);
		}
		else
		{
			SCR_DC_Log.Add("[SCR_DC_LootHelper:AddToStorage] storageManager not found", LogLevel.ERROR);
			return false;
		}
	}	
}
