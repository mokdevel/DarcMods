//SDRC_RplPlayerComp.c

//------------------------------------------------------------------------------------------------
/*!
RPL component to provide GM map information to clients.

NOTE: This has been added to {}}Prefabs/Characters/DarcMissions/DefaultPlayerController.et
*/
[ComponentEditorProps(category: "GameScripted/DarcMissions", description: "DarcMissions Player Helper Component")]
class SDRC_RplPlayerCompClass : ScriptComponentClass { }
SDRC_RplPlayerCompClass g_RplPlayerCompClass;

//------------------------------------------------------------------------------------------------
class SDRC_RplPlayerComp : ScriptComponent
{
	private static SDRC_RplPlayerComp s_Instance;	
		 
    override void OnPostInit(IEntity owner)
    {
		s_Instance = this;				
    }
 
	//------------------------------------------------------------------------------------------------
	//! Get instance owned by local player
	static SDRC_RplPlayerComp FindLocalInstance()
	{
		PlayerController playerController = GetGame().GetPlayerController();
		
		if (!playerController)
		{
			return null;
		}
		
		return SDRC_RplPlayerComp.Cast(playerController.FindComponent(SDRC_RplPlayerComp));
	}	
	
	//------------------------------------------------------------------------------------------------
	//!	Return instance to component
	static SDRC_RplPlayerComp GetInstance()
	{
		return s_Instance;
	}
}