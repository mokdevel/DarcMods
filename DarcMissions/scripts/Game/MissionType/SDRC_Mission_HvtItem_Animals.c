//Mission SDRC_HvtItemConfig_Animals.c

//------------------------------------------------------------------------------------------------
/*!
*/

//------------------------------------------------------------------------------------------------
class SDRC_HvtItemConfig_Animals : SDRC_HvtItemConfig
{
	private const string DC_MISSIONCONFIG_FILE_HVTITEM_ANIMALS = "dc_missionConfig_HvtItem_Animals.json";
	private const int DC_MISSIONCONFIG_FILE_HVTITEM_ANIMALS_JSONVER = DC_MISSIONCONFIG_FILE_HVTITEM_JSONVER;
	
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
		missionList = {0,0,1};
		//Mission specific		
		//----------------------------------------------------
		subMissions.Clear();
		subMissions.Insert(HvtItem_A_0());				
		subMissions.Insert(HvtItem_A_1());				
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
			{"0 0 0"}, 10,
			{
				EMapDescriptorType.MDT_NAME_TOWN,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL,
			},
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
		hvtItem.targetIdx.Insert(2);
				
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
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
			"{973BB83B14B97689}PrefabsEditable/Auto/Props/Civilian/LootE_SackUniversal_01.et",
		    "0 1 0"
		);
		hvtItem.campItems.Insert(item_0);
		
		// Territory
		SDRC_Structure item_1 = new SDRC_Structure;
		item_1.Set(
		    "{761D0D0A6993785C}PrefabsEditable/Zones/E_BAR_TerritoryMarker_Wolf.et",
		    "5 1 5"
		);
		hvtItem.campItems.Insert(item_1);

		// Wolves
		SDRC_Structure item_2 = new SDRC_Structure;
		item_2.Set(
		    "{D2E246D3F0FFF5D2}PrefabsEditable/Animals/E_Wolf.et",
		    "4 1 4"
		);
		hvtItem.campItems.Insert(item_2);		
		
		return hvtItem;
	};			
	
	//----------------------------------------------------
	SDRC_HvtItem HvtItem_A_1()
	{
		ref SDRC_HvtItem hvtItem = new SDRC_HvtItem();
		ref SDRC_MissionMessage message = new SDRC_MissionMessage();
		message.Set("Wolfpack close to %l",
			"A wolfpack is roaming. Get rid of them.",
			"The pack was dismantled.",
			"A Wolfpack doesn’t care about the rules; it only cares about the pack.",);
		hvtItem.general.modList.Insert("$BARFrenchAnimals:");
		hvtItem.general.Set(
			1, "index 1: Kill wolfpack",
			{"0 0 0"}, 10,
			{
				EMapDescriptorType.MDT_NAME_TOWN,
				EMapDescriptorType.MDT_NAME_VILLAGE,
				EMapDescriptorType.MDT_NAME_LOCAL,
				EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST, EMapDescriptorType.MDT_FOREST,
			},
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
			{"G_RECON", "G_LIGHT", "G_HEAVY"},
			80, 1.0,
			{10, 200},
			SDRC_EWaypointGenerationType.RADIUS,
			SDRC_EWaypointMoveType.PATROLCYCLE,
		);		
		hvtItem.targetIdx.Insert(2);
		hvtItem.targetIdx.Insert(3);
		hvtItem.targetIdx.Insert(4);
		hvtItem.targetIdx.Insert(5);
				
		ref SDRC_Loot loot = new SDRC_Loot();
		array<string> lootItems = {
				"WEAPON_RIFLE", 
				"WEAPON_HANDGUN", "WEAPON_HANDGUN", "WEAPON_HANDGUN", 
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
			"{973BB83B14B97689}PrefabsEditable/Auto/Props/Civilian/LootE_SackUniversal_01.et",
		    "0 1 0"
		);
		hvtItem.campItems.Insert(item_0);
		
		// Territory
		SDRC_Structure item_1 = new SDRC_Structure;
		item_1.Set(
		    "{761D0D0A6993785C}PrefabsEditable/Zones/E_BAR_TerritoryMarker_Wolf.et",
		    "5 1 5"
		);
		hvtItem.campItems.Insert(item_1);

		// Wolves
		SDRC_Structure item_2 = new SDRC_Structure;
		item_2.Set(
		    "{D2E246D3F0FFF5D2}PrefabsEditable/Animals/E_Wolf.et",
		    "4 1 4"
		);
		hvtItem.campItems.Insert(item_2);		
		
		SDRC_Structure item_3 = new SDRC_Structure;
		item_3.Set(
		    "{D2E246D3F0FFF5D2}PrefabsEditable/Animals/E_Wolf.et",
		    "8 1 8"
		);
		hvtItem.campItems.Insert(item_3);		

		SDRC_Structure item_4 = new SDRC_Structure;
		item_4.Set(
		    "{D2E246D3F0FFF5D2}PrefabsEditable/Animals/E_Wolf.et",
		    "4 1 8"
		);
		hvtItem.campItems.Insert(item_4);		

		SDRC_Structure item_5 = new SDRC_Structure;
		item_5.Set(
		    "{D2E246D3F0FFF5D2}PrefabsEditable/Animals/E_Wolf.et",
		    "8 1 4"
		);
		hvtItem.campItems.Insert(item_5);		
						
		return hvtItem;
	};	
}