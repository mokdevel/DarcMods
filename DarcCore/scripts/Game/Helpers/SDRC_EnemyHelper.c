//Helpers SDRC_EnemyHelper.c

//------------------------------------------------------------------------------------------------
/*!
Functions for various enemy related things
*/

//------------------------------------------------------------------------------------------------
class SDRC_EnemyHelper
{
	//------------------------------------------------------------------------------------------------
	/*! 
	Select the proper enemy resourcename for spawning. 
	\param listName The enemyList to check. If a prefab "{xxx}.." is provided, that is returned.
	*/	
	static ResourceName SelectEnemy(string listName, string faction)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!baseGameMode) {SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] baseGameMode not found", LogLevel.ERROR); return "";}
		if (!baseGameMode.m_SDRC_Core) {SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] m_SDRC_Core not found", LogLevel.ERROR); return "";}
		
		return baseGameMode.m_SDRC_Core.m_EnemyListHelper.SelectEnemyFromList(listName, faction);
	}
	
	//------------------------------------------------------------------------------------------------
	/*! 
	Select faction for the enemy.
	\param faction The faction requested
	*/	
	static string SelectEnemyFaction(string faction = "")
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!baseGameMode) {SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemyFaction] baseGameMode not found", LogLevel.ERROR); return "";}
		if (!baseGameMode.m_SDRC_Core) {SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemyFaction] m_SDRC_Core not found", LogLevel.ERROR); return "";}
		
		return baseGameMode.m_SDRC_Core.m_EnemyListHelper.SelectEnemyFactionFromList(faction);
	}

	//------------------------------------------------------------------------------------------------
	/*! 
	Find the default enemy Faction
	*/	
	static Faction GetDefaultEnemyFaction()
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!baseGameMode) {SDRC_Log.Add("[SDRC_EnemyHelper:GetDefaultEnemyFaction] baseGameMode not found", LogLevel.ERROR); return null;}
		if (!baseGameMode.m_SDRC_Core) {SDRC_Log.Add("[SDRC_EnemyHelper:GetDefaultEnemyFaction] m_SDRC_Core not found", LogLevel.ERROR); return null;}
		
		return baseGameMode.m_SDRC_Core.m_EnemyListHelper.m_DefaultEnemyFaction;
	}

	//------------------------------------------------------------------------------------------------
	/*! 
	Find the default enemy Faction
	*/	
	static string GetDefaultEnemyFactionKey()
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!baseGameMode) {SDRC_Log.Add("[SDRC_EnemyHelper:GetDefaultEnemyFactionKey] baseGameMode not found", LogLevel.ERROR); return "";}
		if (!baseGameMode.m_SDRC_Core) {SDRC_Log.Add("[SDRC_EnemyHelper:GetDefaultEnemyFactionKey] m_SDRC_Core not found", LogLevel.ERROR); return "";}
		
		return baseGameMode.m_SDRC_Core.m_EnemyListHelper.m_sDefaultEnemyFactionKey;
	}	
			
	//------------------------------------------------------------------------------------------------
	/*!
	Get Faction with string name
	*/	
	static Faction GetFactionWithName(string name)
	{
		SCR_BaseGameMode baseGameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (!baseGameMode) {SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] baseGameMode not found", LogLevel.ERROR); return null;}
		if (!baseGameMode.m_SDRC_Core) {SDRC_Log.Add("[SDRC_EnemyHelper:SelectEnemy] m_SDRC_Core not found", LogLevel.ERROR); return null;}
		
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager)
		{			
			SDRC_Log.Add("[SDRC_EnemyHelper:GetFactionWithName] No faction manager found.", LogLevel.ERROR);
			return GetDefaultEnemyFaction();
		}
		
		Faction faction = factionManager.GetFactionByKey(name);
		if (!faction)
		{
			SDRC_Log.Add("[SDRC_EnemyHelper:GetFactionWithName] Using default faction: " + GetDefaultEnemyFactionKey(), LogLevel.WARNING);
			return GetDefaultEnemyFaction();
		}
		
		return faction;
	}	
}
