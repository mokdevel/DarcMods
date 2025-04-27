//Helpers SCR_DC_PlayerHelper.c

//------------------------------------------------------------------------------------------------
/*!
Helper functions to handle players
*/

sealed class SCR_DC_PlayerHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Find the count of players in game
	*/
	static int PlayerCount()
	{
		array<int> players = {};
		GetGame().GetPlayerManager().GetPlayers(players);
		
		return players.Count();
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Find the closest player to a position
	Original code from: HunterKiller mod by Rabid Squirrel	
	*/	
	static IEntity PlayerGetClosestToPos(vector positionToCheck, int distanceAway = 0, int maxDistance = 999999999)
	{
		array<int> players = {};
		GetGame().GetPlayerManager().GetPlayers(players);
		
		float closestDistance = maxDistance;
		IEntity closestPlayer = null;
		foreach (int i, int playerId: players)
		{
			IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			if (player)
			{
				float distance = vector.Distance(player.GetOrigin(), positionToCheck);
			
				if ((distance < closestDistance) && (distance >= distanceAway))
				{
					closestDistance = distance;
					closestPlayer = player;
				}
			}
		}
		
		return closestPlayer;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Check is any player is close to given position. Returns true is player between minimumRadius and radiusToCheck
	\param positionToCheck 
	\param radiusToCheck The radius with within one single player shall be
	\param minimumRadius The minimum radius how far the found player shall be from positionToCheck
	
	*-----)+++++++++++++)  + = the area that returns true
	^     ^             ^
	|     |             radiusToCheck
	|     minimumRadius
	position to check	
	*/	
	static bool IsAnyPlayerCloseToPos(vector positionToCheck, int radiusToCheck = 1000, int minimumRadius = 0)
	{
		array<int> players = {};
		GetGame().GetPlayerManager().GetPlayers(players);
		
		foreach (int i, int playerId: players)
		{
			IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			if (player)
			{
				float distance = vector.Distance(player.GetOrigin(), positionToCheck);
			
				if (distance < radiusToCheck)
				{
					if (distance > minimumRadius)
					{
						return true;
						break;
					}
				}
				
			}
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get player faction
	*/	
	static Faction GetPlayerFaction(int player)
	{
 		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		if (!factionManager)
		{
			return null;
		}
		Faction faction = factionManager.GetPlayerFaction(player);
		
		return faction;
	}

	//------------------------------------------------------------------------------------------------
	/*!
	Get player factionKey
	*/	
	static FactionKey GetPlayerFactionKey(int player)
	{
		Faction faction = GetPlayerFaction(player);
		FactionKey factionKey = "";
		
		if (faction)
		{
			factionKey = faction.GetFactionKey();
		}
		
		return factionKey;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Get player name
	*/	
	static string GetPlayerName(int playerId)
	{
		string playerName = "";
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (playerManager)
		{
			playerName = playerManager.GetPlayerName(playerId);		
		}		
		return playerName;
	}	
}

