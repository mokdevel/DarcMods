//------------------------------------------------------------------------------------------------

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
	string title;							//Title for the hint shown for players
	string info;							//Details for the hint shown for players
	SDRC_EMissionWinCondition winCondition;	//Mission win condidition
	string winMessage;						//Message to show when mission is completed
	string loseMessage;						//Message to show when mission fails
#ifdef NEW_VERSION_WIP
	ref array<string> faction = {};			//Faction for the mission. Setting as empty, works as the default to select from the enemyFactions
#else
	string faction;							//Faction for the mission. Setting as empty, works as the default to select from the enemyFactions
#endif	
	string markerType;						//Marker type for the mission
	int markerIcon;							//Marker ID within markerType
#ifdef NEW_VERSION_WIP
	ref array<SDRC_EDifficulty> difficulty = {}; //Difficulty options for specific mission
#else	
	SDRC_EDifficulty difficulty;			//Difficulty for specific mission
#endif	
	int xp;									//Experience given	
#ifdef NEW_VERSION_WIP
	ref array<string> mods = {};					//List of mods needed for this mission
#endif	
	
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
					 string posName_ = SDRC_DEFAULT, string title_ = SDRC_DEFAULT, string info_ = SDRC_DEFAULT, 
					 SDRC_EMissionWinCondition winCondition_ = SDRC_EMissionWinCondition.DEFAULT, 
					 string winMessage_ = SDRC_DEFAULT, string loseMessage_ = SDRC_DEFAULT, 
					 string faction_ = "", 
					 string markerType_ = SDRC_DEFAULT, int markerIcon_ = -1, 
					 SDRC_EDifficulty difficulty_ = SDRC_EDifficulty.RANDOM, 
					 int xp_ = 0 
					 )
	{
		array<vector> pos_array = {pos_, "0 0 0"};
		Set(subIdx_, comment_, pos_array, size_, locationTypes_, posName_, title_, info_, winCondition_, winMessage_, loseMessage_, faction_, markerType_, markerIcon_, difficulty_, xp_);
	}

	void SetDefaults2(array<string> mods_)
	{
#ifdef NEW_VERSION_WIP
		Set2(mods_);
#endif	
	}
		
	//------------------------------------------------------------------------------------------------
	void Set(int subIdx_, string comment_, array<vector> pos_, float size_, array<EMapDescriptorType> locationTypes_, string posName_, string title_, string info_, SDRC_EMissionWinCondition winCondition_, string winMessage_, string loseMessage_, string faction_, string markerType_, int markerIcon_, SDRC_EDifficulty difficulty_, int xp_)
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
		title = title_;
		info = info_;
		winCondition = winCondition_;
		winMessage = winMessage_;
		loseMessage = loseMessage_;
#ifdef NEW_VERSION_WIP
		if (faction_ != "")								//TBD: The parameter will be an array so this needs to fixed to handle that
		{
			faction.Clear();		
			faction.Insert(faction_);
		}
#else
		faction = faction_;
#endif
		markerType = markerType_;
		markerIcon = markerIcon_;
#ifdef NEW_VERSION_WIP		
		if (difficulty_ != SDRC_EDifficulty.RANDOM)		//TBD: The parameter will be an array so this needs to fixed to handle that
		{
			difficulty.Clear();
		}
		difficulty.Insert(difficulty_);
#else		
		difficulty = difficulty_;
#endif		
		xp = xp_;
	}
	
	void Set2(array<string> mods_)
	{
#ifdef NEW_VERSION_WIP
		mods = mods_;
#endif	
	}	
}