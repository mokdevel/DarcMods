//Mission SDRC_HvtItemConfig_Animals.c

//------------------------------------------------------------------------------------------------
/*!
*/

//------------------------------------------------------------------------------------------------
class SDRC_HvtItemConfig_Animals : SDRC_HvtItemConfig
{
	private const string DC_MISSIONCONFIG_FILE_HVTITEM_ANIMALS = "dc_missionConfig_HvtItem_Animals.json";
	private const int DC_MISSIONCONFIG_FILE_HVTITEM_ANIMALS_JSONVER = 2;
	
	//------------------------------------------------------------------------------------------------
	static string GetFileName()
	{		
		return DC_MISSIONCONFIG_FILE_HVTITEM_ANIMALS;
	}	

	//------------------------------------------------------------------------------------------------
	static int GetFileVersion()
	{		
		return DC_MISSIONCONFIG_FILE_HVTITEM_ANIMALS_JSONVER;
	}	
		
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_HvtItemConfig_Animals data = SDRC_HvtItemConfig_Animals.Cast(T);
		return saveContext.WriteValue("", data);
	}	
	
	//------------------------------------------------------------------------------------------------
	override void SetDefaults()
	{
		super.SetDefaults();
		missionType = SDRC_EMissionType.HVTITEM;
		
		//Default
		missionCycleTime = SDRC_MISSION_CYCLE_TIME_DEFAULT;
		missionList = {0};
		//Mission specific		
		//----------------------------------------------------
		subMissions.Clear();
		subMissions.Insert(HvtItem_A_0());				
	};
	
	//----------------------------------------------------
	SDRC_HvtItem HvtItem_A_0()
	{
		ref SDRC_HvtItem hvtItem = new SDRC_HvtItem();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Wolf near %l",
			"Help the farmers to get rid of the wolf. Look out for other hunters. ",
			"Wolf is not huffing and puffing anymore.",
			"Wolf still roams free as the nature intended.",);
		hvtItem.general.modList.Insert("$BARFrenchAnimals:");
		hvtItem.general.Set(
			0, "index 0: Kill wolf",
			{"0 0 0"}, 20,
			{},
			"any",
			{message},
			SDRC_EMissionWinCondition.HVT_DESTROY_ITEM,
			{},
			"DARC_MISSION", SDRC_EMissionIcon.GM_MISSION_HVTITEM_MAP,		
			{SDRC_EDifficulty.RANDOM},
			0
		);
		hvtItem.ai.Set(
			{1, 2},
			{"G_SNIPER", "G_SNIPER", "G_RECON", "G_RECON"},
			80, 1.0,
			{10, 200},
			SDRC_EWaypointGenerationType.RADIUS,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);		
		hvtItem.targetIdx = 1;
				
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", "WEAPON_RIFLE", 
				"UTIL_ATTACHMENT", "UTIL_OPTIC",
				"UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", "UTIL_AMMO", 
				"ITEM_MEDICAL", "ITEM_MEDICAL", "ITEM_MEDICAL",
				"ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL", "ITEM_GENERAL",
				"GEAR_HEADGEAR", "GEAR_VEST", "GEAR_HANDWEAR", "GEAR_UNIFORM", 
			};
		loot.Set(0.5, lootItems);
		hvtItem.loot = loot;
		
		// Loot Crate (first position)
		SDRC_Structure item_0 = new SDRC_Structure;
		item_0.Set(
		    "{4A9E0C3D18D5A1B8}Prefabs/Props/Crates/LootCrateWooden_01_blue.et",
		    "0 1 0"
		);
		hvtItem.campItems.Insert(item_0);
		
		// Concrete Pipe Short Pile
		SDRC_Structure item_1 = new SDRC_Structure;
		item_1.Set(
		    "{D2E246D3F0FFF5D2}PrefabsEditable/Animals/E_Wolf.et",
		    "3 1 3"
		);
		hvtItem.campItems.Insert(item_1);
		
		// Trash Bin
		SDRC_Structure item_2 = new SDRC_Structure;
		item_2.Set(
		    "{761D0D0A6993785C}PrefabsEditable/Zones/E_BAR_TerritoryMarker_Wolf.et",
		    "5 1 5"
		);
		hvtItem.campItems.Insert(item_2);

		return hvtItem;
	};			
}