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
class SDRC_LootHelper
{
	private const string DC_MISSIONCONFIG_FILE_LOOTLIST = "dc_lootList.json";
	private const int DC_MISSIONCONFIG_FILE_LOOTLIST_JSONVER = 3;
	
	private ref SDRC_JsonApi2 m_JsonApi = null;
	private ref SDRC_LootListConfig m_Config = null;
	
	private static bool m_bIsReady = false;
	
	bool Scan(int index)
	{		
		if (!m_Config)
		{		
			SDRC_Log.Add("[SDRC_LootHelper:Scan] Preparing..", LogLevel.NORMAL);
			//Load loot config
			m_Config = new SDRC_LootListConfig();
			m_JsonApi = new SDRC_JsonApi2(DC_MISSIONCONFIG_FILE_LOOTLIST);	
			m_JsonApi.Load(m_Config, SDRC_Config.Cast(m_Config), DC_MISSIONCONFIG_FILE_LOOTLIST_JSONVER, safeUpdate: true);		
		}
		
		m_bIsReady = m_Config.Populate(index, false);
		
		if (m_bIsReady)
		{
			SDRC_Log.Add("[SDRC_LootHelper:Scan] Done!", LogLevel.DEBUG);
		}
		
		return m_bIsReady;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Checker to see if everything is ready.
	*/
	bool IsReady()
	{
		return m_bIsReady;
	}

	//------------------------------------------------------------------------------------------------
	/*! 
	Find the loot item
	*/	
	ResourceName FindLootItem(string listName)
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
	bool GetLootListItems(out array<string> items, string listName)
	{
		//Find the right list index		
		int lootIndex = SDRC_ListHelper.FindListIndex(m_Config.lists, listName);
		
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
	Get requested ammo counts
	*/	
	int GetAmmoCountLow()
	{
		return m_Config.ammoCount[0];
	}
	
	int GetAmmoCountHigh()
	{
		return m_Config.ammoCount[1];
	}	
	
	int GetRandomAmmoCount()
	{
		return SDRC_Misc.RandomFloat(m_Config.ammoCount[0], m_Config.ammoCount[1]);
	}
	
}
