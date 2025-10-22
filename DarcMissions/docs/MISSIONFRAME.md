# Basics
Once the mod starts to run it will wait for ```missionStartDelay``` before the missions start to spawn. The missions have their own life cycle with a defined cycle time. Once a mission is completed and no players are near by, the missions will despawn. The missions are active a minimum of ```missionActiveTime```. The time is reset if there is a player within ```missionActiveDistance```. For more details, see [Mission Frame](#mission-frame).

## Mission position
Mission position is either defined or randomized. See [General parameters - pos](P_COMMON.md) and [Location parameters](P_LOCATIONS.md). Once a position is chosen, it's checked for validity. The position shall not be .. 
- .. too close to another mission
- .. too close to any player
- .. in water
- .. in [non valid area](NONVALIDAREAS.md)

In the case position is not usable, another try is made. Currently searching is limited to five tries before deciding that no position has been found. In this case, mission will not spawn.

GM requested missions generally follow the same rule. Depends slightly on the mission requested.

## Dynamic vs Static mission
In the configuration you can define missions either as a dynamic mission (```missionDynamic```) or static mission (```missionStatic```). The missions are the same but spawning is different. 

* Static missions will spawn immediately when the server starts. Once all static missions are spawned, no new ones will be spawned during the session. If a static mission fails to start, it will not be tried again. For example, if you define 30 missions as static, on a small map there may not be locations that fit the missions criteria. You may end up with 20 missions running. For static missions ``activeTime`` should be long. NOTE: No popup will be provided to players for static mission spawns. Marker visibility can be defined with ``showStaticMissionMarker``.

* Dynamic missions will spawn on steady pace. The ``activeTime`` should be relatively short so that the missions despawn and new ones are brought to players. Note that no dynamic missions, including GM requested ones, will spawn if static missions are still spawning.

* GM spawned missions are dynamic missions. You can spawn missions as a GM even if you have reached the maximum count of dynamic missions. GM requested missions are spawned with a higher priority compared to normal dynamic missions. If you set both static and dynamic missions to zero, only GM spawned missions will spawn.

## Mission end (win or lose)
Each mission has a win condition. The normal case is to eliminate a certain amount of AI to reach the goal. Once the win condition has been reached, the mission is kept alive for ```missionActiveTimeToEnd``` seconds. The time is reset if there is a player within ```missionActiveDistance```. At the end of a mission, all spawned items will despawn (for example camps) but also vehicles. 
Available win conditions:
```
  0 = NONE 			  : Unused
  1 = AI_KILL_ALL     : Kill all AIs to win
  2 = AI_KILL_75      : 75%
  3 = AI_KILL_50      : 50%
  4 = AI_KILL_RANDOM  : 30%-100% of AIs to kill. This is set randomly when mission starts.
  5-9: RESERVED       : Reserved for future use
  10 = HVT_KILL_VIP   : Kill the target VIP - shall be used only with HVTVIP mission.
```

# Configuration
Configuration is done in .json files.

## Mission frame
Example: [dc_missionConfig.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig.json)

The main configuration file for mission frame.
```
int version : See General parameters
string author : See General parameters
string comment : See General parameters
bool recreateConfigs : If set to true, all configs are written to disk. Should be run only first time.	float difficultyAiCountCoefMul;
int missionStartDelay : (seconds) Time to wait before spawning the first mission.
int missionFrameCycleTime : (seconds) The cycle time to manage mission spawning, deletion etc...
int missionActiveDistance : The distance to a player to keep the mission active.
int missionActiveTimeToEnd : (seconds) Time to keep the mission active once all AI is dead. Used for both dynamic and static missions. 
  NOTE: Patrol and Hunter has a hardcoded 20 seconds defined.
float missionActiveDistanceMul : <TBD: more documentation>
float missionActiveTimeToEndMul : <TBD: more documentation>
int missionHintTime : (seconds) Time to show mission hints to players. 0 disables ALL hints.
int missionRandomPos : The radius to randomize the mission position. This avoids mission appearing always in same place.
int minDistanceToMission : Distance to another mission. Two missions shall not be too close to each other.
int minDistanceToPlayer : Mission shall not spawn too close to a player.
bool showStaticMissionMarker : Show/hide static mission markers. 
array<string> enemyFactions : The array of factions to consider as enemies. "USSR" by default. 
  Tested values: US, USSR, FIA, RHS_USAF, RHS_AFRF, MEI, BALLIEN_BC_FACTION, BACON_622120A5448725E3_FACTION
  Unsupported values: RHS_ION, MEC
  You can define multiple factions and when enemies are chosen, the faction is chosen randomly per mission. Example: "enemyFactions": ["FIA", "USSR"]
SDRC_MissionDifficulty missionDifficulty : Difficulty settings.
SDRC_MissionTypeConfig missionDynamic : Dynamic missions configurations.
SDRC_MissionTypeConfig missionStatic : Static missions configurations.
```

### SDRC_MissionDifficulty
Coef values in the array refer to SDRC_EMissionDifficulty values: {``EASY``, ``MODERATE``, ``NORMAL``, ``TOUGH``, ``HARD``}
```
float difficultyAiSkillCoefMul : Difficulty setting that affects all missions. <TBD: more documentation>
float difficultyAiPerceptionCoefMul : Difficulty setting that affects all missions. <TBD: more documentation>
float difficultyLootChanceCoefMul : Difficulty setting that affects all missions. <TBD: more documentation>
float difficultyLootCountCoefMul : Difficulty setting that affects all missions. <TBD: more documentation>
array<float> aiCountCoef =      {0.5, 0.6, 1.0, 2.0, 3.0};
array<float> aiSkillCoef =      {0.2, 0.6, 1.0, 1.3, 1.6};
array<float> aiPerceptionCoef = {0.2, 0.6, 1.0, 1.3, 1.6};
array<float> lootChanceCoef = 	{0.5, 0.6, 1.0, 1.2, 1.4};	
array<float> lootCountCoef =    {1.0, 1.0, 1.0, 1.0, 1.0};	
```

### SDRC_MissionTypeConfig
The below is valid for both static and dynamic missions
```
int count : Count of missions.
  0 = No missions will spawn
  1-n = Max amount of missions. 
  -1 = Missions amount depends on the size of the map and countMul. The
    Mission count = (mapsize in meters / 1000) * countMul.
    Example: 12km x 12km map with countMul 0.5 -> 12*0.5 = 6 missions
float countMul : Multiplier for mission count. Only used if count = -1
int activeTime : (seconds) Time to keep the mission active. For static this should be high.
int delayBetween : (seconds) Time delay between mission spawns.
array<int> missionTypeArray : List mission types that spawn randomly. (DC_EMissionType)
  0 = NONE       : Not used anywhere. The rest are names of the mission types.
  1 = HUNTER
  2 = OCCUPATION
  3 = CONVOY
  4 = CRASHSITE
  5 = PATROL
  6 = SQUATTER
  7 = ROADBLOCK
  8 = HVTVIP
  9 = HVTITEM
 10 = STASH
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
