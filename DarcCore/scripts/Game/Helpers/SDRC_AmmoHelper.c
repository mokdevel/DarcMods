//Helpers SDRC_AmmoHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various ammo/magazine related things
NOTE: SDRC_LootHelper needs to initialized before using this. UTIL_MAGAZINES list has to be available. 
*/

//------------------------------------------------------------------------------------------------
sealed class SDRC_AmmoHelper
{
	private static InventoryStorageManagerComponent m_invManager;// = new InventoryStorageManagerComponent();
	private static IEntity m_ammoHelperBox;
	
	//------------------------------------------------------------------------------------------------
	static void Setup()
	{
		//Put a random box to put all our magazines available.
		//This is needed for the predicates to work.
		IEntity m_ammoHelperBox = SDRC_SpawnHelper.SpawnItem("122 1 144", "{86B51DAF731A4C87}Prefabs/Props/Military/SupplyBox/SupplyCrate/LootSupplyCrate_Base.et", emptyPosRadius: -1);		
		
		if (m_ammoHelperBox)
		{	
			m_invManager = InventoryStorageManagerComponent.Cast(m_ammoHelperBox.FindComponent(InventoryStorageManagerComponent));		
			array<string> itemNames = {};
			SDRC_LootHelper.GetLootListItems(itemNames, "UTIL_MAGAZINES");
			SDRC_LootHelper.SpawnItemsToStorage(m_ammoHelperBox, itemNames);
		}
		else
		{
			SDRC_Log.Add("[SDRC_AmmoHelper:Setup] Could not spawn box for ammohelper.", LogLevel.WARNING);
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Find a random compatible magazine for a given weapon
	*/
	static string GetCompatibleMagazineForPrefab(string weaponPrefab)
	{
		array<string>magazineList = {};
		
		IEntity wpn = SDRC_SpawnHelper.SpawnItem("0 0 0", weaponPrefab, emptyPosRadius: -1);		
		
		if (wpn)
		{		
			FindCompatibleMagazineList(magazineList, wpn);
			
			//Delete the unnecessary weapon that we used for finding the magazine
			GetGame().GetCallqueue().CallLater(SDRC_SpawnHelper.DespawnItem, 10000, false, wpn);
			
			return magazineList.GetRandomElement();
		}
		
		return "";
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find a random compatible magazine for a given weapon
	*/
	static string GetCompatibleMagazine(IEntity wpn)
	{
		array<string>magazineList = {};
		FindCompatibleMagazineList(magazineList, wpn);
		return magazineList.GetRandomElement();			
	}	
		
	//------------------------------------------------------------------------------------------------
	/*!
	Returns a list of compatible magazines for a given weapon
	TBD: Cache results
	*/
	static void FindCompatibleMagazineList(out array<string>magazineList, IEntity weapon)	
	{		
		BaseMuzzleComponent muzzle;
		BaseWeaponComponent weaponComp;
		array<BaseMuzzleComponent> muzzles;	
		weaponComp = BaseWeaponComponent.Cast(weapon.FindComponent(BaseWeaponComponent));
		SCR_MagazinePredicate m_pMagazineSearchPredicate = new SCR_MagazinePredicate(); // Predicate for searching for magazines
		
		if (!weaponComp)
			return;
		
		muzzle = weaponComp.GetCurrentMuzzle();
		ResourceName res = weapon.GetPrefabData().GetPrefabName();
		
		if (!m_invManager)
		{
			SDRC_Log.Add("[SDRC_AmmoHelper:FindCompatibleMagazineList] Could not populate the list for: " + res, LogLevel.WARNING);
			return;
		}
		
		int magazineCount = 0;
		
		if (muzzle)
		{
			// WEAPON WITH MUZZLE -> NON-GRENADE
			
			// Find compatible magazines for this magazine well
			BaseMagazineWell magWell = muzzle.GetMagazineWell();
			
			if (magWell)
			{
				m_pMagazineSearchPredicate.magWellType = magWell.Type();
				array<IEntity> magEntities = new array<IEntity>;
				m_invManager.FindItems(magEntities, m_pMagazineSearchPredicate);
				magazineCount = magazineList.Count();
				
				foreach (IEntity mag : magEntities)				
				{
					ResourceName magRes = mag.GetPrefabData().GetPrefabName();
					magazineList.Insert(magRes);
				}
				
				SDRC_Log.Add("[SDRC_AmmoHelper:FindCompatibleMagazineList] Found " + magazineCount + " compatible magazines for: " + res, LogLevel.WARNING);				
			}
			else
			{
				// No magazine well, weapon is probably not reloadable
				SDRC_Log.Add("[SDRC_AmmoHelper:FindCompatibleMagazineList] No magazineWell found for: " + res, LogLevel.WARNING);
			}
		}
		else
		{
			SDRC_Log.Add("[SDRC_AmmoHelper:FindCompatibleMagazineList] Only magazines currently supported." + res, LogLevel.WARNING);
			// WEAPON WITHOUT MUZZLE -> GRENADE
/*			
			// Find compatible weapons (same grenades for instance)
			m_pPrefabDataPredicate.prefabData = weapon.GetOwner().GetPrefabData();
			array<IEntity> sameWeapons = new array<IEntity>;
			m_invManager.FindItems(sameWeapons, m_pPrefabDataPredicate);
			
			// Remaining 'mags' count text
			magazineCount = sameWeapons.Count();*/
		}		
		
//		bool isGrenade = m_WeaponState.m_bIsExplosive;
	}			

	//------------------------------------------------------------------------------------------------
	/*!
	Returns the magazineWell defined for a magazine
	*/
	static BaseMagazineWell GetMagazineInfo(IEntity mag)
	{		
		BaseMagazineComponent magazine;
		BaseMagazineWell magazineWell;
		magazine = BaseMagazineComponent.Cast(mag.FindComponent(BaseMagazineComponent));
		
		if (magazine)
		{
			magazineWell = magazine.GetMagazineWell();
			SDRC_Log.Add("[SDRC_MissionHelper:GetMagazineInfo] magazineWell: " + magazineWell, LogLevel.SPAM);		
			return magazineWell;
		}
		
		return null;
	}
}
