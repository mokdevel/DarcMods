  /* ------------------------------------------------------------------------------------------------ /
 / CRX-EAI Modded Class ( #CRX-EAIModdedClass )                                                      /
/ ------------------------------------------------------------------------------------------------ */
/*
//------------------------------------------------------------------------------------------------
modded class SCR_AIWorld : AIWorld
{
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_SUBGROUPS, false);
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_FIRETEAMS, false);
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_PRINT_DEBUG, false);
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_DEBUG_COVERS, false);
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_SHOW_DEBUG_SHAPES, false);
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_SELECT_FIXED_AGENT, false);
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_SHOW_SUPPRESS_DEBUG, false);
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_SHOW_SECTOR_THREAT_FILTER, false);
		
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_DEBUG_CATEGORY, EAIDebugCategory.NONE);
		
// ///////////////////////////////////////////////////////////////////////////////////////////////
// #ifdef CRX_DEBUG
		
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_PRINT_DEBUG, true);
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_SHOW_DEBUG_SHAPES, true);
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_SELECT_FIXED_AGENT, true);
		// DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_DEBUG_CATEGORY, EAIDebugCategory.CRX);
		DiagMenu.SetValue(SCR_DebugMenuID.DEBUGUI_AI_DEBUG_CATEGORY, EAIDebugCategory.BEHAVIOR);
		
// #endif
// ///////////////////////////////////////////////////////////////////////////////////////////////
	
	}
}
*/