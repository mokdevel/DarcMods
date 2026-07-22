//Helpers SDRC_VehicleListConfig.c

// For readable jsons, use https://jsonformatter.org
// NOTE: View .json in Notepad++ - press Ctrl+Alt+Shift+J , convert to readable format - press Ctrl+Alt+Shift+M

//------------------------------------------------------------------------------------------------
// Types defined by default
/*
VEHICLE_WHEELED_ALL
VEHICLE_WHEELED_MILITARY_ALL
VEHICLE_WHEELED_CIVILIAN_ALL
VEHICLE_WHEELED_ARMED
VEHICLE_WHEELED_UNARMED
VEHICLE_WHEELED_ARMED
VEHICLE_WHEELED_CIVILIAN_TRUCK
VEHICLE_WHEELED_MILITARY_TRUCK
VEHICLE_WHEELED_CIVILIAN_CAR
VEHICLE_WHEELED_MILITARY_CAR

VEHICLE_HELICOPTER_ALL
VEHICLE_CHOPPER_ALL
VEHICLE_CHOPPER_TRANSPORT
VEHICLE_CHOPPER_ARMED
*/

//------------------------------------------------------------------------------------------------
class SDRC_VehicleListConfig : SDRC_ListConfig
{
	//------------------------------------------------------------------------------------------------
	override bool DoSave(SaveContainerContext saveContext, Class T)
	{
		SDRC_VehicleListConfig data = SDRC_VehicleListConfig.Cast(T);
		return saveContext.WriteValue("", data);
	}		

	//------------------------------------------------------------------------------------------------

	override void SetDefaults()
	{		
		super.SetDefaults();
		
		m_modList = {};
		//Vehicles
		m_lists.Insert(vehicleList00());
		m_lists.Insert(vehicleList01());
		m_lists.Insert(vehicleList02());
		m_lists.Insert(vehicleList03());
		m_lists.Insert(vehicleList04());
		m_lists.Insert(vehicleList05());
		m_lists.Insert(vehicleList06());
		m_lists.Insert(vehicleList07());
		m_lists.Insert(vehicleList08());
		m_lists.Insert(vehicleList09());
		//Helicopters
		m_lists.Insert(vehicleList30());
		m_lists.Insert(vehicleList31());
		m_lists.Insert(vehicleList32());
		m_lists.Insert(vehicleList33());

		#ifndef SDRC_RELEASE
			SDRC_Aka akaTest = new SDRC_Aka();
			akaTest.names = {"USSR", "FIA"};
			m_akas.Insert(akaTest);	
		#endif
				
		SDRC_Aka aka00 = new SDRC_Aka();
		aka00.names = {"RHS_USAF", "US"};
		m_akas.Insert(aka00);	
		
		SDRC_Aka aka01 = new SDRC_Aka();
		aka01.names = {"RHS_AFRF", "USSR"};
		m_akas.Insert(aka01);		
		
		SDRC_Aka aka02 = new SDRC_Aka();
		aka02.names = {"MEI", "USSR"};
		m_akas.Insert(aka02);		

		SDRC_Aka aka03 = new SDRC_Aka();
		aka03.names = {"Takistan", "USSR"};
		m_akas.Insert(aka03);
		
		SDRC_Aka aka04 = new SDRC_Aka();
		aka04.names = {"ChinesePLA", "USSR"};
		m_akas.Insert(aka04);
		
		SDRC_Aka aka05 = new SDRC_Aka();
		aka05.names = {"JSDF_RAS", "US"};
		m_akas.Insert(aka05);
		
		SDRC_Aka aka06 = new SDRC_Aka();
		aka06.names = {"UK", "US"};
		m_akas.Insert(aka06);		
	}
			
	//Vehiclelist: VEHICLE_WHEELED_ALL
	SDRC_List vehicleList00()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_WHEELED_ALL",
			{"Prefabs/Vehicles/Wheeled",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return vehicleList;
	}		
	
	//Vehiclelist: VEHICLE_WHEELED_MILITARY_ALL
	SDRC_List vehicleList01()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_WHEELED_MILITARY_ALL",
			{"Prefabs/Vehicles/Wheeled",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return vehicleList;
	}		
	
	//Vehiclelist: VEHICLE_WHEELED_CIVILIAN_ALL
	SDRC_List vehicleList02()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_WHEELED_CIVILIAN_ALL",
			{"Prefabs/Vehicles/Wheeled",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return vehicleList;
	}		
	
	//Vehiclelist: VEHICLE_WHEELED_ARMED
	SDRC_List vehicleList03()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_WHEELED_ARMED",
			{"Prefabs/Vehicles/Wheeled",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return vehicleList;
	}		
	
	//Vehiclelist: VEHICLE_WHEELED_ARMED
	SDRC_List vehicleList04()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_WHEELED_UNARMED",
			{"Prefabs/Vehicles/Wheeled",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return vehicleList;
	}	
	
	//Vehiclelist: VEHICLE_WHEELED_ARMOR
	SDRC_List vehicleList05()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_WHEELED_ARMOR",
			{"Prefabs/Vehicles/Wheeled",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return vehicleList;
	}		
	
	//Vehiclelist: VEHICLE_WHEELED_CIVILIAN_TRUCK
	SDRC_List vehicleList06()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_WHEELED_CIVILIAN_TRUCK",
			{"Prefabs/Vehicles/Wheeled",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return vehicleList;
	}		
	
	//Vehiclelist: VEHICLE_WHEELED_MILITARY_TRUCK
	SDRC_List vehicleList07()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_WHEELED_MILITARY_TRUCK",
			{"Prefabs/Vehicles/Wheeled",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return vehicleList;
	}		
	
	//Vehiclelist: VEHICLE_WHEELED_CIVILIAN_CAR
	SDRC_List vehicleList08()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_WHEELED_CIVILIAN_CAR",
			{"Prefabs/Vehicles/Wheeled",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return vehicleList;
	}		
	
	//Vehiclelist: VEHICLE_WHEELED_MILITARY_CAR
	SDRC_List vehicleList09()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_WHEELED_MILITARY_CAR",
			{"Prefabs/Vehicles/Wheeled",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return vehicleList;
	}	
	
	//Vehiclelist: VEHICLE_HELICOPTER_ALL
	SDRC_List vehicleList30()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_HELICOPTER_ALL",
			{"Prefabs/Vehicles/Helicopters",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			"_patrol", 								//Do not include AI choppers
			},
			{}
		);
		return vehicleList;
	}
	
	//Vehiclelist: VEHICLE_CHOPPER_ALL
	SDRC_List vehicleList31()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_CHOPPER_ALL",
			{"Prefabs/Vehicles/Helicopters",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return vehicleList;
	}
	
	//Vehiclelist: VEHICLE_CHOPPER_TRANSPORT
	SDRC_List vehicleList32()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_CHOPPER_TRANSPORT",
			{"Prefabs/Vehicles/Helicopters",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return vehicleList;
	}
	
	//Vehiclelist: VEHICLE_CHOPPER_ARMED
	SDRC_List vehicleList33()
	{
		ref SDRC_List vehicleList = new SDRC_List();
		vehicleList.Set(
			"VEHICLE_CHOPPER_ARMED",
			{"Prefabs/Vehicles/Helicopters",
			},
			{},
			{"_Base", "_Sample", "Tutorial", "_Conflict", "_randomized", 
			"_bench", "_gun_mount", "_roof",
			"Dst", "Lights", "Probes", "VehParts", 
			},
			{}
		);
		return vehicleList;
	}		
}
