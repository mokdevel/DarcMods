//SDRC_RPLGMComp.c

//------------------------------------------------------------------------------------------------
/*!
RPL component to provide GM map information to clients.

The component is split accross multiple additional files:
- SDRC_RPLGMComp_NonValidArea.c
- SDRC_RPLGMComp_Symbols.c
- SDRC_RPLGMComp_MissionData.c

NOTE: This has been added to {}}Prefabs/Characters/DarcMissions/DefaultPlayerController.et
*/
[ComponentEditorProps(category: "GameScripted/DarcMissions", description: "DarcMissions GM Helper Component")]
class SDRC_RplGMCompClass : ScriptComponentClass { }
SDRC_RplGMCompClass g_RplGMCompClass;

//------------------------------------------------------------------------------------------------
class SDRC_RplGMComp : ScriptComponent
{
	private static SDRC_RplGMComp s_Instance;	
	
	//------------------------------------------------------------------------------------------------
    override void OnPostInit(IEntity owner)
    {
		//Store instance
		s_Instance = this;				
    }
 
	//------------------------------------------------------------------------------------------------
	/*!	
	Return instance to component
	*/
	static SDRC_RplGMComp GetInstance()
	{
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get instance
	//TBD: This might be exactly the same GetInstance() above
	static SDRC_RplGMComp FindInstance()
	{
		SDRC_RplGMEntity gmEnt = SDRC_RplGMEntity.GetInstance();
		
		if (!gmEnt)
		{
			return null;
		}
		
		return SDRC_RplGMComp.Cast(gmEnt.FindComponent(SDRC_RplGMComp));
	}
	
	//------------------------------------------------------------------------------------------------
	// Modded class references
	//
	// Functions in modded class to sort the stuff in files per topic. Reference are here for compilation reasons.
	//------------------------------------------------------------------------------------------------	
	
	void Sync_Symbols(int playerID) {}
 	
	//------------------------------------------------------------------------------------------------
}