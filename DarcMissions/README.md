# DarcMissions
A mission frame work for Arma Reforger. There are various missions premade and their behaviour can be modified by editing the configuration jsons.

## Vision
I wanted to create a simple drop-in mission package where there is minimal configuration needed to get it running. You can take the mod, play it in SP, MP or let it run on a dedicated server. Everything should be randomized so you should never know what is happening in the world. The behaviour of the mod should be close to vanilla and by doing that, you could be running this on top of your other game modes. I've been using this with [Escapists](https://reforger.armaplatform.com/workshop/5F16D7E4A1CBE075-Escapists) just to get a few additional surprises.

## Missions
- Hunter : AIs are hunting you and following you. They have some knowledge of where you are. You can outrun them or take them down. 
- Convoy : AIs are driving from a location A to B carrying valuable loot with them.
- Patrol : AIs are traveling either with a purpose to reach a location or to patrol areas.
- Crashsite : A helicopter carrying loot is damaged and is going to crash. Survivors will be protecting the wreck while waiting to be rescued.
- Occupation : A location is guarded by AIs with loot available. 

## Installation
Just active the mod (and dependencies) and you're good to go.

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

## Mission frame - dc_missionConfig.json
The main configuration file for mission frame.

Example: [dc_missionConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig.json)
```
int version : Version number of the file.
string author : Author of the missions.
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
array missionTypeArrayStatic : (DC_EMissionType) List mission types that are always active. See missionTypeArrayDynamic for values
```
## Non valid areas - dc_nonValidArea.json
This a list of areas where missions shall not spawn. For example a safe zone would be listed here. 

Example: [dc_nonValidArea.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_nonValidArea.json)
```
string worldName : The world name this should be affecting. For example Arland or Eden. If left empty, area will be valid for all maps. Note that Arland and GM_Arland are considered different maps.
vector pos : Center position of area
float radius : Radius 
string name : Your own name for the area. Not used by the mod.
```
# Mission configuration parameters
## Common for all
```
int version : Version number of the file.
string author : Author of the missions.
int missionCycleTime : (seconds) The cycle time the mission states are run
bool showMarker : Show marker on map
bool showHint : Show players a hint with details about the missions
```
## Hunter - dc_missionConfig_Hunter.json
AIs are hunting you and following you. They will receive regularly information about where the closest player. The location is known to them with an error of rndDistanceToPlayer. You can outrun them by going further than maxDistanceToPlayer.

Example: [dc_missionConfig_Hunter.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Hunter.json)
```
array<int> groupCount : Amount of AI groups of characters to spawn. (min, max) 
int minDistanceToPlayer : Hunter group minimum distance to player for spawn
int maxDistanceToPlayer : ..max distance
int rndDistanceToPlayer : The error on the location where AI thinks you are. (0..rndDistanceToPlayer). 
	
array string groupTypes : See General parameters
int AISkill : See General parameters
float AIperception : See General parameters
```

## Patrol - dc_missionConfig_Patrol.json

Example:
```
int patrolingTime : (seconds) Time to patrol. Once this time has passed and not players nearby, despawn mission.
int distanceToPlayer : If no players this close to any players and patrolingTime has passed, despawn mission.
array<int> patrolList : The indexes of patrols.
array<SCR_DC_Patrol> patrols : List of patrols
```
```
SCR_DC_Patrol
string comment : Generic comment to describe the mission. Not used in game.
vector posStart : Position for mission. "0 0 0" used for random location chosen from locationTypes.
vector posDestination : Destination for the patrol to go to
string locationName : Your name for the mission location (like "Harbor near city"). "any" uses location name found from locationTypes 
string title : Title for the hint shown for players
string info : Details for the hint shown for players
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
```
array<EMapDescriptorType> locationTypes = {};
```

## Waypoint parameters
```
array<int> waypointRange : See Waypoint parameters
DC_EWaypointGenerationType waypointGenType : See Waypoint parameters
DC_EWaypointMoveType waypointMoveType : See Waypoint parameters
```
