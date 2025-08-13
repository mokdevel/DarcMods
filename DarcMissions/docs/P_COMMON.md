# Common mission parameters
## Common parameters
These parameters are in every mission.
```
int version : See General parameters
string author : See General parameters
int missionCycleTime : (seconds) The cycle time the mission states are run
string markerType : The marker type to use. Default is "DARC_MISSION".
  For Arma Reforger default ones, you can use "PLACED_MILITARY". Other mods may extend this.
int markerIdx : The index of the icon of defined markerType
bool showMarker : Show mission marker on map. False will disable markers.
bool showHint : Show players a hint with details about the missions. False will disable hints.
bool showMessage : Show winmessage and losemessage to players. False will hide these.
```

## SDRC_MissionConfigGeneral
```
string comment : Generic comment to describe the mission. Not used in game.
array<vector> pos : Position(s) for mission. "0 0 0" used for random location chosen from locationTypes.
  [0] = The general position for a mission
  [1] = The destination position for missions that need it - convoy, patrol
string posName : Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes. "" will leave the information empty.
string title : Title for the hint shown for players
string info : Details for the hint shown for players
DC_EMissionWinCondition winCondition =
  0 = UNUSED  
	1 = AI_KILL_ALL - Mission succeeds when 100% of AI is killed.
	2 = AI_KILL_75 - ..75%
	3 = AI_KILL_50 - ..50%
	4 = AI_KILL_RANDOM - ..a random % (30%-100%) of AI is killed
string winMessage : Message to show when mission is completed
string loseMessage : Message to show when mission fails.
int xp = 0 : Experience given - not supported currently
```

## General parameters
array<int> groupCount : (min, max) Amount of AI groups of characters to spawn.
array<string> groupTypes : The prefab names of AI groups or characters. The AI is randomly picked from this list.
  Example : {
            "{ADB43E67E3766CE7}Prefabs/Characters/Factions/OPFOR/USSR_Army/Spetsnaz/Character_USSR_SF_Sharpshooter.et",
            "{8E29E7581DE832CC}Prefabs/Groups/OPFOR/KLMK/Group_USSR_MedicalSection_KLMK.et"
            }
int aiSkill : Skill for AI (0-100). See SCR_AICombatComponent and EaiSkill.
  0 = NONE
  10 = NOOB
  20 = ROOKIE
  50 = REGULAR
  70 = VETERAN
  80 = EXPERT
  100 = CYLON
float aiPerception : How quickly AI reacts to danger. See SCR_AICombatComponent for details on perception factors.
bool disableArsenal : Disable arsenal for vehicles so that only defined loot items are found.
```
