//Helpers SDRC_PlayerHelper.c

//------------------------------------------------------------------------------------------------
/*!
Helper functions to handle players
*/

class SDRC_PlayerPos : Managed
{
	vector pos;
	float distance;
	IEntity player;
}

sealed class SDRC_PlayerHelper
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
	static IEntity GetPlayerClosestToPos(vector positionToCheck, int distanceAway = 0, int maxDistance = 999999999)
	{
		array<int> players = {};
		GetGame().GetPlayerManager().GetPlayers(players);
		
		float closestDistance = maxDistance;
		IEntity closestPlayer = null;
		foreach (int i, int playerId : players)
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
	Check if any player is close to given position. Returns true is player between minimumRadius and radiusToCheck
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
		
		foreach (int i, int playerId : players)
		{
			IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			if (player)
			{
				float distance = vector.DistanceXZ(player.GetOrigin(), positionToCheck);
			
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
	Get players near a position with distance to it. 
	*/	
	static void GetPlayersClosestToPosition(out array<ref SDRC_PlayerPos> playerPosArray, vector positionToCheck, int radiusToCheck = -1)
	{
		array<int> players = {};
		
		GetGame().GetPlayerManager().GetPlayers(players);
		
		foreach (int playerId : players)
		{
			IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			if (player)
			{
				if (!IsAlive(player))
				{
					continue;
				}
				
				float distance = vector.DistanceXZ(player.GetOrigin(), positionToCheck);
				
				if (distance > radiusToCheck)
				{
					distance = -1;	//If outside of area to check, return -1 and we don't care about this player
				}
				
				if (distance > -1)
				{
					ref SDRC_PlayerPos playerPos = new SDRC_PlayerPos();
					
					playerPos.pos = player.GetOrigin();
					playerPos.distance = distance;
					playerPos.player = player;

					int idx = 0;
					
					foreach (int i, SDRC_PlayerPos pPos : playerPosArray)
					{
						if (playerPos.distance < pPos.distance)
						{
							idx = i;
							break;
						}
						idx = i + 1;
					}
					playerPosArray.InsertAt(playerPos, idx);
				}
			}			
		}
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

	static Faction GetPlayerFaction(IEntity playerEntity)
	{
		SCR_ChimeraCharacter character = SCR_ChimeraCharacter.Cast(playerEntity);
		if (!character)
		{	
			return null;		
		}
		
	    Faction faction = character.GetFaction();
		
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
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if player is alive
	*/	
	static bool IsAlive(IEntity playerEntity)
	{
		SCR_DamageManagerComponent damageManager = SCR_DamageManagerComponent.GetDamageManager(playerEntity);
		
		if (damageManager)
		{
			return damageManager.GetState() != EDamageState.DESTROYED;
		}
		else
		{
			return true;
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if player is in game master mode (not ArmaVision)
	*/	
	static bool IsInGMmode()
	{
		SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.GetInstance();
		if (editorManager && editorManager.IsOpened() && !editorManager.IsLimited())
		{
			return true;
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
	Check if interface in GM mode is visible
	*/	
	static bool IsGMInterfaceVisible()
	{
		//Is interface visible
		SCR_MenuEditorComponent menuManager = SCR_MenuEditorComponent.Cast(SCR_MenuEditorComponent.GetInstance(SCR_MenuEditorComponent, true));
		if (!menuManager)
		{
			return false;
		}
		
		if (!menuManager.IsVisible())
		{
			return false;
		}
		
		return true;
	}	
	
	//------------------------------------------------------------------------------------------------
	/*!
	Displays a message in chat
	*/	
	static void ShowChatMessage(string message)
	{
		PlayerController playerComponent = GetGame().GetPlayerController();
		if (!playerComponent)
			return;
		
		SCR_ChatComponent chatComponent = SCR_ChatComponent.Cast(playerComponent.FindComponent(SCR_ChatComponent));
		if (!chatComponent)
			return;
		
		chatComponent.ShowMessage(message);
	}		
}

