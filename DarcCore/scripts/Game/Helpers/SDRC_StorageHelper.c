//Helpers SDRC_StorageHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various storage related things
*/

//------------------------------------------------------------------------------------------------
class SDRC_StorageHelper
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
		
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!baseGameMode) {SDRC_Log.Add("[SDRC_LootHelper:SpawnItemsToStorage] baseGameMode not found", LogLevel.ERROR); return;}
		if (!baseGameMode.m_SDRC_Core) {SDRC_Log.Add("[SDRC_LootHelper:SpawnItemsToStorage] m_SDRC_Core not found", LogLevel.ERROR); return;}		
		
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
					resource = baseGameMode.m_SDRC_Core.m_LootHelper.FindLootItem(itemName);
				}

				int itemCount = 1;

				if ( (itemName.Contains("UTIL_MAGAZINE")) || (itemName.Contains("UTIL_AMMO")) )
				{
					itemCount = baseGameMode.m_SDRC_Core.m_LootHelper.GetRandomAmmoCount();
				}
				
				for (int i = 0; i < itemCount; i++)
				{
					bool result = AddToStorage(storage, resource);
					SDRC_Log.Add("[SDRC_LootHelper:SpawnItemsToStorage] Adding item " + resource + ". Success: " + result, LogLevel.DEBUG);
				}				
				
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
						int ammoCount = baseGameMode.m_SDRC_Core.m_LootHelper.GetRandomAmmoCount();
						
						if (ammoCount > 0)
						{
							//Find the right magazine for added weapon
							string magazine = SDRC_AmmoHelper.GetCompatibleMagazineForPrefab(resource);
						
							for (int i = 0; i < ammoCount; i++)
							{
								bool result = AddToStorage(storage, magazine);
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
	Try to add an item to a storage of an entity
	*/	
	static bool AddToStorage(IEntity entity, ResourceName item)
	{	
		//NOTE: The below Resource.Load will result in an error if the ResourceName is not available. For example from 
		//if (FileIO.FileExists("Prefabs/Items/Medicine/SalineBag_01/SalineBag_US_01.et")) ... 
				
		SDRC_Log.Add("[SDRC_StorageHelper:AddToStorage] Adding to: " + entity, LogLevel.SPAM);
		
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
			SDRC_Log.Add("[SDRC_StorageHelper:AddToStorage] storageManager not found on: " + SDRC_Misc.GetSimpleEntityName(res), LogLevel.ERROR);
			return false;
		}
	}

}
