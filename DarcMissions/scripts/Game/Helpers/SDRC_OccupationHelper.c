//Helpers SDRC_OccupationHelper

//------------------------------------------------------------------------------------------------
/*!
Helper functions for spawning occupation class 
*/

//------------------------------------------------------------------------------------------------
class SDRC_Occupation : Managed
{
	//Occupation specific
	string comment;							//Generic comment to describe the mission. Not used in game.
	vector pos;								//Position for mission. "0 0 0" used for random location chosen from locationTypes.
	string posName;							//Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes 
	string title;							//Title for the hint shown for players
	string info;							//Details for the hint shown for players
	ref array<EMapDescriptorType> locationTypes = {};	
	ref array<int> groupCount = {};			//min, max
	ref array<int> waypointRange = {};		//min, max
	DC_EWaypointGenerationType waypointGenType;
	DC_EWaypointMoveType waypointMoveType;
	ref array<string> groupTypes = {};
	int aiSkill;
	float aiPerception;
	float emptySize = 7;					//The size (radius) of the empty space to found to decide on a mission position.
	
	//Optional settings
	ref SDRC_Loot loot = null;
	ref array<ref SDRC_Structure> campItems = {};
	
	void Set(string comment_, vector pos_, string posName_, string title_, string info_, array<EMapDescriptorType> locationTypes_, array<int> groupCount_, array<int> waypointRange_, DC_EWaypointGenerationType waypointGenType_, DC_EWaypointMoveType waypointMoveType_, array<string> groupTypes_, int aiSkill_, float aiPerception_, float emptySize_)
	{
		comment = comment_;
		pos = pos_;
		posName = posName_;
		title = title_;
		info = info_;
		locationTypes = locationTypes_;
		groupCount = groupCount_;
		waypointRange = waypointRange_;
		waypointGenType = waypointGenType_;
		waypointMoveType = waypointMoveType_;
		groupTypes = groupTypes_;
		aiSkill = aiSkill_;
		aiPerception = aiPerception_;		
		emptySize = emptySize_;
	}
}

sealed class SDRC_OccupationHelper
{
	//------------------------------------------------------------------------------------------------
	/*!
	Spawn camp items, AI and loot defined in a SDRC_Occupation structure
	\param mission Mission that is calling this
	\param idx Index of the item to spawn. This needs to be increased each run.
	\param occupation The SDRC_Occupation structure to use
	\param rotation The rotation of the items around Y axis
	\param disableArsenal Defines if vehicle arsenals are to be disabled
	\return true/false if the mission spawning is ready and mission can continue to next stage 
	*/
	static bool Spawn(SDRC_Mission mission, int idx, SDRC_Occupation occupation, float rotation = 0, bool disableArsenal = true)
	{
		IEntity entity;
		
		//Spawn entities one by one
		if ( (idx < occupation.campItems.Count()) && (occupation.campItems.Count() > 0) )
		{			
			entity = SDRC_SpawnHelper.SpawnStructures(occupation.campItems, mission.GetPos(), rotation, idx);
			
			if (entity != NULL)
			{ 
				mission.AddToEntityList(entity);
				//Disable arsenal
				string resourceName = occupation.campItems[idx].GetResource();
				SDRC_SpawnHelper.DisableVehicleArsenal(entity, resourceName, disableArsenal);				
			}
			else
			{
				SDRC_Log.Add("[SDRC_OccupationHelper:Spawn] Could not load: " + occupation.campItems[idx], LogLevel.ERROR);				
			}
			
			return false;
		}
		else
		{
			//Spawn mission AI 
			int groupCount = Math.RandomInt(occupation.groupCount[0], occupation.groupCount[1]);
			
			for (int i = 0; i < groupCount; i++)
			{
				SCR_AIGroup group = SDRC_MissionHelper.SpawnMissionAIGroup(occupation.groupTypes.GetRandomElement(), mission.GetPos(), mission.GetFaction());
				if (group)
				{
					SDRC_AIHelper.SetAIGroupSkill(group, occupation.aiSkill, occupation.aiPerception);					
					mission.AddToGroupsList(group);
					
					int minRange = occupation.waypointRange[0];
					int maxRange = occupation.waypointRange[1];
					
					//If there are more than one group and loot, spawn one to protect the loot. 
					//For the first group, waypointRange is ignored.
					if ((occupation.loot) && i == 0)
					{
						minRange = 5;
						maxRange = 30;					
					}
					
					SDRC_WPHelper.CreateMissionAIWaypoints(group, occupation.waypointGenType, mission.GetPos(), "0 0 0", occupation.waypointMoveType, minRange, maxRange);
//					SDRC_WPHelper.CreateMissionAIWaypoints(group, DC_EWaypointGenerationType.LOITER, GetPos(), "0 0 0", DC_EWaypointMoveType.LOITER, occupation.waypointRange[0], occupation.waypointRange[1]);
				}
				SDRC_Log.Add("[SDRC_OccupationHelper:Spawn] AI groups spawned: " + groupCount, LogLevel.DEBUG);								
			}
			
			//Put loot
			if (occupation.loot)			
			{
				occupation.loot.box = mission.GetFromEntityList(0);
				SDRC_LootHelper.SpawnItemsToStorage(occupation.loot.box, occupation.loot.items, occupation.loot.itemChance);
				SDRC_Log.Add("[SDRC_OccupationHelper:Spawn] Loot added.", LogLevel.DEBUG);								
			}

			return true;			
		}		
	}*/
}