# DarcMissions
A mission frame work for Arma Reforger. There are various missions premade and their behaviour can be modified by editing the configuration jsons.

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
     ^                    ^
     mission is spawned   mission is spawned
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
For examples of configuration files see [ExampleConfigs](https://github.com/mokdevel/DarcMods/tree/main/DarcMissions/ExampleConfigs)
## MissionFrame 	
```
int version : Version number of the file.
string author : Author of the missions.
bool recreateConfigs : If set to true, all configs are written to disk. Should be run only first time.
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
array missionTypeArrayDynamic : (DC_EMissionType) List mission types that spawn randomly
array missionTypeArrayStatic : (DC_EMissionType) List mission types that are always active	
array nonValidAreas : (SCR_DC_NonValidArea) List of areas where missions shall not spawn. 
```
### nonValidAreas - SCR_DC_NonValidArea
This a list of areas where missions shall not spawn. For example a safe zone would be listed here.
```
string worldName : The world name this should be affecting. For example Arland or Eden.
vector pos : Center position of area
float radius : Radius 
string name : Your own name for the area. Not used by the mod.
```
## Common for all
```
int version : Version number of the file.
string author : Author of the missions.
int missionCycleTime : (seconds) The cycle time the mission states are run
bool showMarker : Show marker on map
bool showHint : Show players a hint with details about the missions
```
