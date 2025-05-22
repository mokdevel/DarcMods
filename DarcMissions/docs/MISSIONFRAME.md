# Basics
Once the mod starts to run it will wait for ```missionStartDelay``` before the missions start to spawn. The missions have their own life cycle with a defined cycle time. Once a mission is completed and no players are near by, the missions will despawn. The missions are active a minimum of ```missionActiveTime```. The time is reset if there is a player within ```missionActiveDistance```. For more details, see [Mission Frame](#mission-frame).

## Mission position
Mission position is either defined or randomized. See [General parameters - pos](P_COMMON.md) and [Location parameters](P_LOCATIONS.md). Once a position is chosen, it's checked for validity. The position shall not be .. 
- .. too close to another mission
- .. too close to any player
- .. in water
- .. in [non valid area](NONVALIDAREAS.md)

In the case position is not usable, another try is made. Currently searching is limited to five tries before deciding that no position has been found. In this case, mission will not spawn.

## Dynamic vs Static mission
In the configuration you can define missions either as a dynamic mission (```missionTypeArrayDynamic```) or static mission (```missionTypeArrayStatic```). The missions are the same but static missions have priority in being spawned. Static missions are such that there should always be the given count of static missions running and if there are mission slots still available, these will be populated with dynamic missions. Note that both static and dynamic missions are counted towards the total count.

This enables you to have a count of certain types of missions always running. For example, you may want to have three patrols always roaming the map. To achieve this, you define the patrol mission three times in the ```missionTypeArrayStatic``` array. If you define multiple types of missions in the static list, the missions are picked at random. Having for example three patrol missions and one convoy mission in the list, makes sure that four static missions are running, but this may be any combination due to random picking. The patrol mission has a higher chance to be chosen.

## Mission end
Once all AIs have been eliminated, the mission is kept alive for ```missionActiveTimeToEnd``` seconds. The time is reset if there is a player within ```missionActiveDistance```. At the end of a mission, all spawned items will despawn (for example camps) but also vehicles. 

# Configuration
Configuration is done in .json files.

## Mission frame
Example: [dc_missionConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig.json)

The main configuration file for mission frame.
```
int version : See General parameters
string author : See General parameters
string comment : See General parameters
bool recreateConfigs : If set to true, all configs are written to disk. Should be run only first time.
string missionProfile : Directory specifying a certain conf for play. For example "Escapists" will result in configs under "profile\DarcMods_conf\Escapists\*"
int missionStartDelay : (seconds) Time to wait before spawning the first mission.
int missionDelayBetweeen : (seconds) Time delay between mission spawns.
int missionCount : Maximum amount of missions (both static and dynamic) to be active at the same time.
int staticTryLimit : How many static missions are tried (fail or success) before trying a dynamic one. To avoid constant static spawns especially if failed.
int missionFrameCycleTime : (seconds) The cycle time to manage mission spawning, deletion etc...
int missionActiveTime : (seconds) Time to keep the mission active.
int missionActiveTimeStatic : (seconds) Time to keep the static mission active (seconds). This typically is much longer than for dynamic.
int missionActiveDistance : The distance to a player to keep the mission active.
int missionActiveTimeToEnd : (seconds) Time to keep the mission active once all AI is dead. Used for both dynamic and static missions.
int missionHintTime : (seconds) Time to show mission hints to players. 0 disables ALL hints.
int minDistanceToMission : Distance to another mission. Two missions shall not be too close to each other.
int minDistanceToPlayer : Mission shall not spawn too close to a player.
array<string> enemyFactions : The array of factions to consider as enemies. "USSR" by default. You can define multiple factions and when enemies are chosen, the faction is chosen randomly per mission.
array<int> missionTypeArrayDynamic : List mission types that spawn randomly. (DC_EMissionType)
  0 = NONE       : Not used anywhere. The rest are names of the mission types.
  1 = HUNTER
  2 = OCCUPATION
  3 = CONVOY
  4 = CRASHSITE
  5 = PATROL
  6 = SQUATTER
array<int> missionTypeArrayStatic : List mission types that are always active. See missionTypeArrayDynamic for values.
```

## MissionFrame cycle
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
