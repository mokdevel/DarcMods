# Common mission parameters
These parameters are in every mission.

## SDRC_MissionConfig
```
int version : Version id of the file
string author : Author of the file
int missionCycleTime : (seconds) The cycle time the mission states are run
bool showMarker : Show mission marker on map. False will disable markers.
bool showHint : Show players a hint with details about the missions. False will disable hints.
bool showMessage : Show winmessage and losemessage to players. False will hide these.
bool disableArsenal : Disable arsenal for vehicles so that only defined loot items are found.
array<int> missionList : The list of mission suids.
array<string> missionFiles : The list of mission files to load. The are additional .json files that are of the same type.  
```

## SDRC_MissionConfigGeneral
```
int subIdx : Unique index for the sub mission. 
string comment : Generic comment to describe the mission. Not used in game.
array<vector> pos : Position(s) for mission. "0 0 0" used for random location chosen from locationTypes.
  [0-n] = The general position for a mission. This could be a mix a location positions and "0 0 0".
  [1] = The destination position for missions that need it - This is a specical case for missions Convoy and Patrol.
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
string faction : Faction for the mission. Setting as empty, works as the default to select from the enemyFactions.
string markerType : The marker type to use. Default is "DARC_MISSION".
  For Arma Reforger default ones, you can use "PLACED_MILITARY". Other mods may extend this.
  Leaving markerType empty will hide the icon. Also from GM view.
int markerIcon : The index of the icon of defined markerType
SDRC_EMissionDifficulty difficulty : Difficulty for specific mission
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

## SDRC_MissionConfigSecondWave 

... TBD ...

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
