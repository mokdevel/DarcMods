# DarcMissions
A mission frame work for Arma Reforger. There are various missions premade and their behaviour can be modified by editing the configuration jsons.

## Installation
Just active the mod (and dependencies) and you're good to go.

## Vision
I wanted to create a simple drop-in mission package where there is minimal configuration needed to get it running. You can take the mod, play it in SP, MP or let it run on a dedicated server. Everything should be randomized so you should never know what is happening in the world. The behaviour of the mod should be close to vanilla and by doing that, you could be running this on top of your other game modes. I've been using this with [Escapists](https://reforger.armaplatform.com/workshop/5F16D7E4A1CBE075-Escapists) just to get a few additional surprises.

## Missions
- Hunter : AIs are hunting you and following you. They have some knowledge of where you are. You can outrun them or take them down. 
- Convoy : AIs are driving from a location A to B carrying valuable loot with them.
- Patrol : AIs are traveling either with a purpose to reach a location or to patrol areas.
- Crashsite : A helicopter carrying loot is damaged and is going to crash. Survivors will be protecting the wreck while waiting to be rescued.
- Occupation : A location is guarded by AIs with loot available. 

# MissionFrame cycle
```
----|------------|------------|------------|------------|------------|------------|----...
    |-------------------------|-------------------------|-------------------------|----...
    |------------------------------| missionActiveTime
                              |------------------------------| missionActiveTime
                                                        |------------------------------...
    |O----o----o----o----o----|O------o------o-----o----|O---o---o---o---o---|O----o---...
     ^                         ^                         ^
     mission is spawned        mission is spawned        mission is spawned 
     |----| missionCycleTime : Each mission has a cycle time it's running. It may differ between mission types
    |------------| missionFrameCycleTime : The cycle time to manage mission spawning, deletion etc...
    |-------------------------| missionDelayBetweeen : Time between mission spawns 
|---| missionStartDelay : Time waited before first mission is spawned
    |------------------------------| missionActiveTime : Mission is kept alive this amount of time.
                                     Note that if a player is within missionActiveDistance, the time will reset.      
```

# Thanks
- The work is inspired by [Defent's Mission System (DMS)](https://github.com/Defent/DMS_Exile) in Arma 3.
- [HunterKiller mod by Rabid Squirrel](https://reforger.armaplatform.com/workshop/597324ECFC025225-HunterKiller) for the initial kick to get into coding.

# Configuration parameters
For examples of configuration files see [ExampleConfigs](https://github.com/mokdevel/DarcMods/tree/main/DarcMissions/ExampleConfigs).

The configuration files will be under your ```profile\DarcMods_conf\*```.

* dc_missionConfig.json ([link](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/README.md#dc_missionconfigjson---missionframe)) :  The main mission frame configuration. 
* dc_nonValidArea.json : 
* dc_lootList.json : Definitions of loot for missions. 
* dc_coreConfig.json ([link](https://github.com/mokdevel/DarcMods/blob/main/DarcCore/README.md#dc_coreconfigjson)) : Configuration file for core.
  
Mission specific files
* dc_missionConfig_Hunter.json
* dc_missionConfig_Patrol.json
* dc_missionConfig_Occupation.json
* dc_missionConfig_Convoy.json
* dc_missionConfig_Crashsite.json

## Dynamic vs Static mission
In the configuration you can define missions either as a dynamic mission (missionTypeArrayDynamic) or static mission (missionTypeArrayStatic). The missions are the same but static missions have priority in being spawned. Static missions are such that there should always be the given count of static missions running and if there are mission slots still available, these will be populated with dynamic missions. Note that both static and dynamic missions are counted towards the total count.

This enables you to have a count of certain types of missions always running. For example, you may want to have three patrols always roaming the map. To achieve this, you define the patrol mission three times in the missionTypeArrayStatic array. If you define multiple types of missions in the static list, the missions are picked at random. Having for example three patrol missions and one convoy mission in the list, makes sure that four static missions are running, but this may be any combination due to random picking. The patrol mission has a higher chance to be chosen.

## Mission frame - dc_missionConfig.json
The main configuration file for mission frame.

Example: [dc_missionConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig.json)
```
int version : See General parameters
string author : See General parameters
bool recreateConfigs : If set to true, all configs are written to disk. Should be run only first time.
string missionProfile : Directory specifying a certain conf for play. For example "Escapists" will result in configs under ```profile\DarcMods_conf\Escapists\*```
int missionStartDelay : (seconds) Time to wait before spawning the first mission.
int missionDelayBetweeen : (seconds) Time delay between mission spawns.
int missionCount : Maximum amount of missions (both static and dynamic) to be active at the same time. 
int missionFrameCycleTime : (seconds) The cycle time to manage mission spawning, deletion etc...
int missionActiveTime : (seconds) Time to keep the mission active.
int missionActiveTimeStatic : (seconds) Time to keep the static mission active (seconds). This typically is much longer than for dynamic.
int missionActiveDistance : The distance to a player to keep the mission active.
int missionHintTime : (seconds) Time to show mission hints to players. 0 disables ALL hints.
int minDistanceToMission : Distance to another mission. Two missions shall not be too close to each other.
int minDistanceToPlayer : Mission shall not spawn too close to a player.
array<int> missionTypeArrayDynamic : List mission types that spawn randomly. (DC_EMissionType)
  0 = NONE       : Not used anywhere. The rest are names of the mission types.
  1 = HUNTER
  2 = OCCUPATION
  3 = CONVOY
  4 = CRASHSITE
  5 = PATROL
array<int> missionTypeArrayStatic : List mission types that are always active. See missionTypeArrayDynamic for values.
```
## Non valid areas - dc_nonValidArea.json
This a list of areas where missions shall not spawn. For example a safe zone would be listed here. 

Example: [dc_nonValidArea.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_nonValidArea.json)
```
int version : See General parameters
string author : See General parameters
array<SCR_DC_NonValidArea> nonValidAreas : List of areas where missions shall not spawn.
```
### SCR_DC_NonValidArea
```
string worldName : The world name this should be affecting. For example Arland or Eden. If left empty, area will be valid for all maps. Note that Arland and GM_Arland are considered different maps.
vector pos : Center position of area
float radius : Radius from the center where the missions shall not spawn.
string name : Your own name for the area. Not used by the mod.
```
# Mission configuration parameters
## Common for all
```
int version : See General parameters
string author : See General parameters
int missionCycleTime : (seconds) The cycle time the mission states are run
bool showMarker : Show mission marker on map
bool showHint : Show players a hint with details about the missions
```
## Hunter - dc_missionConfig_Hunter.json
AIs are hunting you and following you. They will receive regularly information about where the closest player. The location is known to them with an error of rndDistanceToPlayer. You can outrun them by going further than maxDistanceToPlayer.

Example: [dc_missionConfig_Hunter.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Hunter.json)
```
string title : See General parameters
string info : See General parameters
int minDistanceToPlayer : Hunter group minimum distance to player for spawn
int maxDistanceToPlayer : ..max distance
int rndDistanceToPlayer : The error on the location where AI thinks you are. (0..rndDistanceToPlayer). 
array<int> groupCount : See General parameters
array string groupTypes : See General parameters
int AISkill : See General parameters
float AIperception : See General parameters
```

## Patrol - dc_missionConfig_Patrol.json

Example: [dc_missionConfig_Patrol.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Patrol.json)
```
int patrolingTime : (seconds) Time to patrol. Once this time has passed and not players nearby, despawn mission.
int distanceToPlayer : If no players this close to any players and patrolingTime has passed, despawn mission.
array<int> patrolList : The indexes of patrols.
array<SCR_DC_Patrol> patrols : List of patrols
```
### SCR_DC_Patrol
```
string comment : See General parameters
vector posStart : See General parameters - pos
vector posDestination : Destination for the patrol to go to
string locationName : See General parameters - posName
string title : See General parameters
string info : See General parameters
array<EMapDescriptorType> locationTypes : See Location parameters
array<int> groupCount : See General parameters
array<int> waypointRange : See Waypoint parameters
DC_EWaypointGenerationType waypointGenType : See Waypoint parameters
DC_EWaypointMoveType waypointMoveType : See Waypoint parameters
array string groupTypes : See General parameters
int AISkill : See General parameters
float AIperception : See General parameters
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
array<int> groupCount : Amount of AI groups of characters to spawn. (min, max) 
array string groupTypes : The prefab names of AI groups or characters. The AI is randomly picked from this list.
  Example : {
            "{ADB43E67E3766CE7}Prefabs/Characters/Factions/OPFOR/USSR_Army/Spetsnaz/Character_USSR_SF_Sharpshooter.et",
            "{8E29E7581DE832CC}Prefabs/Groups/OPFOR/KLMK/Group_USSR_MedicalSection_KLMK.et"
            }
int AISkill : Skill for AI (0-100). See SCR_AICombatComponent and EAISkill.
  0 = NONE
  10 = NOOB
  20 = ROOKIE
  50 = REGULAR
  70 = VETERAN
  80 = EXPERT
  100 = CYLON
float AIperception : How quickly AI reacts to danger. See SCR_AICombatComponent for details on perception factors.
```

## Location parametes
A mission location can be defined manually at certain coordinates or as "0 0 0". In the latter case, the mission position is chosen from the map from locations defined in locationTypes. The different types are defined as [EMapDescriptorType](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/group__Map.html#ga18c4f596069370b50b7f842cf36d5686) 
```
array<int> locationTypes : Location types as defined by EMapDescriptorType.
```
```
MDT_NAME_GENERIC 	
MDT_NAME_CITY 	
MDT_NAME_VILLAGE 	
MDT_NAME_TOWN 	
MDT_NAME_SETTLEMENT 	
MDT_NAME_HILL 	
MDT_NAME_LOCAL 	
MDT_NAME_ISLAND 	
MDT_NAME_WATER_MINOR 	
MDT_NAME_WATER_MAJOR 	
MDT_NAME_SEA_MINOR 	
MDT_NAME_SEA_MAJOR 	
MDT_NAME_RIDGE 	
MDT_NAME_VALLEY
```

## Waypoint parameters
```
array<int> waypointRange : See Waypoint parameters
DC_EWaypointGenerationType waypointGenType : See Waypoint parameters
  NONE        : Not used anywhere. The rest are names of the mission types.
  RANDOM      : Use one of these randomly: SCATTERED (1), RADIUS (3), LOITER (1)
  SCATTERED   : Completely random waypoints without any logic
  RADIUS      : AI follow a path that is close to a circle with a radius. There is some additional randomization to avoid a perfect circle.
  ROUTE       : AI follow a route from A to B. The route is created with waypoints along the road.
  LOITER      : Currently makes a DEFEND waypoint for the location
  SLOTS       : AI goes from a slot to slot. NOTE: This will not work unless the map has slots (the S/M/L letters on map) defined.
DC_EWaypointMoveType waypointMoveType : See Waypoint parameters
  NONE        : Not used anywhere. The rest are names of the mission types.
  RANDOM      : Pick a random one from MOVECYCLE (1) and PATROLCYCLE (3).
  MOVE        : Creates move waypoints. AI will stop once waypoints are visited.
  PATROL      : Same as MOVE but with patrol speed.
  MOVECYCLE   : Creates move waypoints in cycke. AI will restart the cycle once all waypoints are visited.
  PATROLCYCLE : Same as MOVECYCLE but with patrol speed.
  LOITER      : If LOITER is chosen for generation, LOITER will be anforced for the movetype too.
```
