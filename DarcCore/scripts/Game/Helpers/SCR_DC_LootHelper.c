//Helpers SCR_DC_LootHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various AI actions
*/

//------------------------------------------------------------------------------------------------

class SCR_DC_Loot : Managed
{
	//SCR_DC_Structure lootBox;
	IEntity box;
	float itemChance = 1.0;
	ref array<string> items = {};	
	
	void Set(float itemChance_, array<string> items_)
	{
		itemChance = itemChance_;
		items = items_;
/*		foreach(string item : items_)
		{
			items.Insert(item);
		}		*/
	}
}

sealed class SCR_DC_LootHelper
{
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
				ResourceName resource = itemName;
				bool result = AddToStorage(storage, resource);			
				SCR_DC_Log.Add("[SCR_DC_LootHelper:SpawnItemsToStorage] Adding item " + resource + ". Success: " + result, LogLevel.DEBUG);
			}
		}
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
