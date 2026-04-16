//Helpers SDRC_VehicleListConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
// Types defined by default
/*
*/

//------------------------------------------------------------------------------------------------
class SDRC_VehicleListConfig : SDRC_ListConfig
{
	//------------------------------------------------------------------------------------------------
	override bool DoSave(ContainerSerializationSaveContext saveContext, Class T)
	{
		SDRC_VehicleListConfig data = SDRC_VehicleListConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------

	override void SetDefaults()
	{		
		super.SetDefaults();
		
		modList = {};
		//Vehicles
		lists.Insert(vehicleList00());			
		//Helicopters
		lists.Insert(vehicleList10());			
	}
			
	//Vehiclelist: VEHICLE_ALL
	SDRC_List vehicleList00()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"VEHICLE_WHEELED_ALL",
			{"Prefabs/Vehicles/Wheeled",
			},
			{"", 
			},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return lootList;
	}		
	
	//Vehiclelist: VEHICLE_HELICOPTER
	SDRC_List vehicleList10()
	{
		ref SDRC_List lootList = new SDRC_List();
		lootList.Set(
			"HELICOPTER",
			{"Prefabs/Vehicles/Helicopters",
			},
			{"", 
			},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return lootList;
	}		
}
