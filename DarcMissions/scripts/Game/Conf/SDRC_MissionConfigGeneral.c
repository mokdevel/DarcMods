//------------------------------------------------------------------------------------------------

class SDRC_MissionMessage : Managed
{
	string title;					//Title for the hint shown for players
	string info;					//Details for the hint shown for players
	string win;						//Message to show when mission is completed
	string lose;					//Message to show when mission fails
	
	void Set (string title_, string info_, string win_, string lose_)
	{
		title = title_;
		info = info_;
		win = win_;
		lose = lose_;
	}
}	

//------------------------------------------------------------------------------------------------
// GENERAL CONFIG
//------------------------------------------------------------------------------------------------
class SDRC_MissionConfigGeneral : Managed
{
	int subIdx;								//Unique index for the sub mission. 
	string comment;							//Generic comment to describe the mission. Not used in game.
	ref array<vector> pos = {};				//Positions for mission. "0 0 0" used for random location chosen from locationTypes. First is mission position, second is destination for missions that need it.
	float size;								//The size (radius) of the empty space needed to decide on a mission position.
	ref array<EMapDescriptorType> locationTypes = {};
	string posName;							//Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes 	
	ref array<ref SDRC_MissionMessage> messages = {};
	SDRC_EMissionWinCondition winCondition;	//Mission win condidition
	ref array<string> faction = {};			//Faction for the mission. Setting as empty, works as the default to select from the enemyFactions
	string markerType;						//Marker type for the mission
	int markerIcon;							//Marker ID within markerType
	ref array<SDRC_EDifficulty> difficulty = {}; //Difficulty options for specific mission
	int xp;									//Experience given	
	ref array<string> modList = {};			//List of mods needed for this mission
	
	//------------------------------------------------------------------------------------------------
	void SDRC_MissionConfigGeneral()
	{
		SetDefaults();
		SetDefaults2({});
	}
	
	//------------------------------------------------------------------------------------------------
	void SetDefaults(int subIdx_ = -1, string comment_ = SDRC_DEFAULT, 
					 vector pos_ = "0 0 0", float size_ = 5,
					 array<EMapDescriptorType> locationTypes_ = null,
					 string posName_ = SDRC_DEFAULT, 
					 array<ref SDRC_MissionMessage> messages_ = null,
					 SDRC_EMissionWinCondition winCondition_ = SDRC_EMissionWinCondition.DEFAULT, 
					 array<string> faction_ = null,
					 string markerType_ = SDRC_DEFAULT, int markerIcon_ = -1, 
					 array<SDRC_EDifficulty> difficulty_ = null,
					 int xp_ = 0 
					 )
	{
		array<vector> pos_array = {pos_, "0 0 0"};
		Set(subIdx_, comment_, pos_array, size_, locationTypes_, posName_, messages_, winCondition_, faction_, markerType_, markerIcon_, difficulty_, xp_);
	}
		
	void SetDefaults2(array<string> modList_)
	{
		Set2(modList_);
	}
		
	//------------------------------------------------------------------------------------------------
	void Set(int subIdx_, string comment_, array<vector> pos_, float size_, array<EMapDescriptorType> locationTypes_, string posName_, array<ref SDRC_MissionMessage> messages_, SDRC_EMissionWinCondition winCondition_, array<string> faction_, string markerType_, int markerIcon_, array<SDRC_EDifficulty> difficulty_, int xp_)
	{
		subIdx = subIdx_;
		comment = comment_;
		pos = pos_;
		size = size_;
		if (locationTypes_)
		{
			locationTypes = locationTypes_;
		}
		posName = posName_;
		
		if (messages_)
		{
			foreach (SDRC_MissionMessage message : messages_)
			{
				messages.Insert(message);
			}
		}
		
		winCondition = winCondition_;
		
		if ( (!faction_) || (faction_.IsEmpty()) )
		{
			//Do nothing
		}
		else
		{
			faction = faction_;
		}
		
/*		if (!faction_)
		{
			faction.Insert("");
		}
		else if (faction_.IsEmpty())
		{
			faction.Insert("");
		}
		else
		{
			faction = faction_;
		}*/

		markerType = markerType_;
		markerIcon = markerIcon_;

		if (!difficulty_)
		{
			difficulty.Insert(SDRC_EDifficulty.RANDOM);
		}
		else if (difficulty_.IsEmpty())
		{
			difficulty_.Insert(SDRC_EDifficulty.RANDOM);
		}
		else
		{
			difficulty = difficulty_;
		}
		xp = xp_;
	}
	
	void Set2(array<string> modList_)
	{
		modList = modList_;
	}	
}