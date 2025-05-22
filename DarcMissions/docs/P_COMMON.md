# Common mission parameters
## Common parameters
These parameters are in every mission.
```
int version : See General parameters
string author : See General parameters
int missionCycleTime : (seconds) The cycle time the mission states are run
bool showMarker : Show mission marker on map. False will disable markers.
bool showHint : Show players a hint with details about the missions. False will disable hints.
```

## General parameters
```
int version : Version number of the file.
string author : Author of the missions.
string comment : Generic comment to describe the mission. Not used in game.
vector pos : Position for mission. "0 0 0" used for random location chosen from locationTypes.
string posName : Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes. "" will leave the information empty.
string title : Title for the hint shown for players
string info : Details for the hint shown for players
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