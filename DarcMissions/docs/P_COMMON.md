# Common mission parameters
These parameters are in every mission.
* [Config](#SDRC_MissionConfig) : These are the basic settings for the mission.
* [General](#SDRC_MissionConfigGeneral) : Parameters for win, lose, messages, markers, difficult for a mission. 
* [AI](#SDRC_MissionConfigAi) : AI related parameters on count to spawn, type, difficulty and similar.
* [QRF] .. aka SecondWave .. TBD

## SDRC_MissionConfig
```
jsonVersion : Version of the json structure. Do not modify.
int version : Version number of the file for author use. Not used in game.
string author : Author of the file. Not used in game.
string comment : Generic comment to describe the file. Not used in game.
int missionCycleTime : (seconds) The cycle time the mission states are run
bool showMarker : Show mission marker on map. False will disable markers.
bool showHint : Show players a hint with details about the missions. False will disable hints.
bool showMessage : Show winmessage and losemessage to players. False will hide these.
bool disableArsenal : Disable arsenal for vehicles so that only defined loot items are found.
array<int> missionList : The list of mission suids.
  Example: "missionList":[0,0,1,1,2] - Missions 0 and 1 will spawn with 40% chance and 2 with 20%
array<string> missionFiles : The list of mission files to load. The are additional .json files that are of the same type.  
```
### Mission files
It is possible to define additional missions to be loaded from file. The filename can be with or without path.

Examples:

``"missionFiles": ["dc_missionConfig_HvtItem_010.json"]`` - The file will be loaded from the same directory as the mission jsons.
``"missionFiles": ["new/dc_missionConfig_HvtItem_010.json"]`` - The file will be loaded from a ``new`` directory under the the json mission directory. (NOTE: Untested!!)

The missions should define their unique ``subIdx`` to use. The list of ``missionList`` will be appended to already loaded list.

## SDRC_MissionConfigGeneral
See [Locations](./P_LOCATIONS.md)
```
int subIdx : Unique index for the sub mission. 
string comment : Generic comment to describe the mission. Not used in game.
array<vector> pos : Position(s) for mission. "0 0 0" used for random location chosen from locationTypes.
  [0-n] = The general position for a mission. This could be a mix a location positions and "0 0 0".
  [1] = The destination position for missions that need it - This is a specical case for missions Convoy and Patrol.
float size : The size (radius) of the empty space needed to decide on a mission position.
array<int> locationTypes : Location types as defined by EMapDescriptorType.
string posName : Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes. "" will leave the information empty.
string title : Title for the hint shown for players
string info : Details for the hint shown for players
DC_EMissionWinCondition winCondition =
  0 = UNUSED  
  1 = AI_KILL_ALL - Mission succeeds when 100% of AI is killed.
  2 = AI_KILL_75 - ..75%
  3 = AI_KILL_50 - ..50%
  4 = AI_KILL_RANDOM - ..a random % (30%-100%) of AI is killed
  5..9 = RESERVED FOR FUTURE
  10 = HVT_KILL_VIP - Specific for HVT_VIP mission. Mission is over when VIP is killed.
  20 = HVT_DESTROY_ITEM - Specific for HVT_ITEM mission. Mission is over when ITEM is destroyed.
  21..29 = RESERVED FOR FUTURE
  30 = FIND_IN_15 - Mission needs to be reached in 15 minutes
  31 = FIND_IN_30 - Mission needs to be reached in 30 minutes
  32 = FIND_IN_45 - Mission needs to be reached in 45 minutes
  33 = FIND_IN_60 - Mission needs to be reached in 60 minutes
string winMessage : Message to show when mission is completed
string loseMessage : Message to show when mission fails.
array<string> faction : Faction for the mission. Setting as empty or "", works as the default to select from the enemyFactions.
string markerType : The marker type to use. Default is "DARC_MISSION".
  For Arma Reforger default ones, you can use "PLACED_MILITARY". Other mods may extend this.
  Leaving markerType empty will hide the icon. Also from GM view.
int markerIcon : The index of the icon of defined markerType
array<SDRC_EMissionDifficulty> difficulty : Difficulty for specific mission
  0 = EASY
  1 = MODERATE
  2 = NORMAL
  3 = TOUGH 
  4 = HARD
int xp = 0 : Experience given - not supported currently
```

## SDRC_MissionConfigAi
```
array<int> count : (min, max) Amount of AI groups of characters to spawn.
array<string> types : The prefab names of AI groups or characters. The AI is randomly picked from this list.
  Example : {
            "{ADB43E67E3766CE7}Prefabs/Characters/Factions/OPFOR/USSR_Army/Spetsnaz/Character_USSR_SF_Sharpshooter.et",
            "{8E29E7581DE832CC}Prefabs/Groups/OPFOR/KLMK/Group_USSR_MedicalSection_KLMK.et",
			"G_LIGHT", "C_OFFICER"
            }
int skill : Skill for AI (0-100). See SCR_AICombatComponent and EaiSkill.
  0 = NONE
  10 = NOOB
  20 = ROOKIE
  50 = REGULAR
  70 = VETERAN
  80 = EXPERT
  100 = CYLON
float perception : How quickly AI reacts to danger. See SCR_AICombatComponent for details on perception factors.
array<int> waypointRange : See below
SDRC_EWaypointGenerationType waypointGenType : See below
SDRC_EWaypointMoveType waypointMoveType : See below
```

## SDRC_MissionConfigQrf
The setup for QRF details for a sub-mission.

See: [Difficulty](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/MISSIONCONFIG.md#sdrc_missiondifficulty)
```
array<int> subIdx : subIdx from which to choose
  Example: "subIdx":[0,0,1,1,2] - QRF 0 and 1 will be chosen 40% chance and 2 with 20%
SDRC_EMissionSuccess activation : Which success activates the check for QRF
  SDRC_EMissionSuccess.WIN : Mission was won. Safe to use for all missions.
  SDRC_EMissionSuccess.LOSE : Mission was lost. Can be selectively be used with some types of missions.
  SDRC_EMissionSuccess.WIN_OR_LOSE : Mission either won or lost. Can be selectively be used with some types of missions.
float chance : The chance for the sub-mission to get a QRF. Note that difficulty will modify this value.
array<int> delay : (seconds) Delay min-max before spawning QRF,
```

## Waypoint parameters
The mission AI can be given general rules on how to create waypoints for their movement. You need to define the waypoint generation rule and the movement rule and the rest is created automatically.

```waypointRange``` is used as the range for example for RADIUS. The area with waypoints will be a value between min and max. If you have multiple groups, they most likely will have different values and different areas to roam.
```
array<int> waypointRange : (min, max) The random radius for waypoint creation.
DC_EWaypointGenerationType waypointGenType : See Waypoint parameters
  0 = NONE        : Not used anywhere. The rest are names of the mission types.
  1 = RANDOM      : Use one of these randomly: LOITER (1), SCATTERED (2), RADIUS (4), 
  2 = SCATTERED   : Completely random waypoints without any logic. 
  3 = RADIUS      : AI follow a path that is close to a circle with a radius. There is some additional randomization to avoid a perfect circle.
  4 = ROUTE       : AI follow a route from A to B. The route is created with waypoints along the road. Once the destination is reached, the AI will LOITER
  5 = LOITER      : Loitering is where AI hand around in the location. AR does not have this yet, so a DEFEND waypoint is created.   
 (6 = SLOTS       : !!DO NOT USE - untested!! AI goes from a slot to slot. NOTE: This will not work unless the map has slots (the S/M/L letters on map) defined. 
DC_EWaypointMoveType waypointMoveType : See Waypoint parameters
  0 = NONE        : Not used anywhere. The rest are names of the mission types.
  1 = RANDOM      : Pick a random one from MOVECYCLE (1), PATROLCYCLE (4), MOVE (1), PATROL (1).
  2 = MOVE        : Creates move waypoints. AI will LOITER once reaching the last waypoint.
  3 = PATROL      : Same as MOVE but with patrol speed.
  4 = MOVECYCLE   : Creates move waypoints in cycke. AI will restart the cycle once all waypoints are visited.
  5 = PATROLCYCLE : Same as MOVECYCLE but with patrol speed.
  6 = LOITER      : If LOITER is chosen for generation, LOITER will be enforced for the movetype too.
```
