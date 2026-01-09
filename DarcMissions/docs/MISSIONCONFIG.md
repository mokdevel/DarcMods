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
SDRC_EHintPosition missionHintPosition : Position of the hint/popup. 
  0 = UP_LEFT : Upper left corner 
  1 = DOWN_LEFT : Lower left corner 
int missionRandomPos : The radius to randomize the mission position. This avoids mission appearing always in same place.
int minDistanceToMission : Distance to another mission. Two missions shall not be too close to each other.
int minDistanceToPlayer : Mission shall not spawn too close to a player.
bool showStaticMissionMarker : Show/hide static mission markers. This will override any mission specific settings.
bool showDynamicMissionMarker : Show/hide dynamic mission markers. This will override any mission specific showMarker settings.
array<string> enemyFactions : The array of factions to consider as enemies. "USSR" by default. 
  You can define multiple factions and when enemies are chosen, the faction is chosen randomly per mission.
  Example: "enemyFactions": ["FIA", "USSR", "USSR"] - 33% missions are with FIA, 66% with USSR
array<int> missionLimit : Limits the amount of mission types that are spawned. The limit is total of static and dynamic missions. Requested missions e.g. GM spawned will not respect the limits by design.
  Below are default/example values. In the example only 2 Hunter missions can be active at the same time:
    -1, // 0 - NONE
    2,  // 1 - HUNTER
    5,  // 2 - OCCUPATION
    5,  // 3 - CONVOY
    2,  // 4 - CRASHSITE
    8,  // 5 - PATROL
    3,  // 6 - SQUATTERS
    8,  // 7 - ROADBLOCK
    2,  // 8 - HVTVIP
    2,  // 9 - HVTITEM
    2,  //10 - STASH
    2,  //11 - CHOPPER
array<int> missionDifficultyList : List of difficulties (0-4) from which the mission difficulty is chosen. 
  Random is the default for missions and the list contains mainly NORMAL difficulty values. This is the most probably selection.
 -1 = RANDOM
  0 = EASY
  1 = MODERATE
  2 = NORMAL
  3 = TOUGH
  4 = HARD
SDRC_MissionDifficulty missionDifficulty : Difficulty settings.
SDRC_MissionTypeConfig missionDynamic : Dynamic missions configurations.
SDRC_MissionTypeConfig missionStatic : Static missions configurations.
```
### Factions
The supported factions are listed below
* ``US, USSR, FIA`` : Vanilla factions from Arma Reforger
* ``RHS_USAF, RHS_AFRF``: RHS - [MOD](https://reforger.armaplatform.com/workshop/595F2BF2F44836FB-RHS-StatusQuo)
* ``MEI`` : Middle East Insurgents - [MOD](https://reforger.armaplatform.com/workshop/64CEC8E005828E5D-MiddleEastInsurgents)
* ``UK`` : British Forces - [MOD](https://reforger.armaplatform.com/workshop/5AE50EC5B8D6F4AE-BritishForces)
* ``BALLIEN_BC_FACTION`` : Ballien Creatures - [MOD](https://reforger.armaplatform.com/workshop/6534B9CEB6D12EE7-BallienCreatures)
* ``BACON_622120A5448725E3_FACTION`` : Bacon Zombies - [MOD](https://reforger.armaplatform.com/workshop/622120A5448725E3-BaconZombies)
* ``PLASTICBANDIT`` : Bandit Faction - [MOD](https://reforger.armaplatform.com/workshop/66C4458756B32594-BanditFaction)
* ``PLA`` : Chinese People's Liberation Army - [MOD](https://reforger.armaplatform.com/workshop/60A6507633AB1954-ChinesePLA)

Unsupported values
* ``RHS_ION``
* ``MEC``

### SDRC_MissionDifficulty
Difficulty setting that affects mission AI behaviour and loot reward. There are five different levels where ``RANDOM`` picks from the ``missionDifficultyList``.
```
-1 = RANDOM
 0 = EASY : Low amount of enemies, low amount of loot
 1 = MODERATE
 2 = NORMAL
 3 = TOUGH
 4 = HARD : A lot of dangerous enemies with high amount of loot.
```

<img src="https://github.com/mokdevel/DarcMods/blob/main/pics/icon_difficulty.png" width=30% height=30%>

```
array<float> aiCountCoef = 			{0.50, 0.60, 1.01, 2.01, 3.01};
array<float> aiSkillCoef = 			{0.20, 0.60, 1.01, 1.30, 1.60};
array<float> aiPerceptionCoef = 	{0.20, 0.60, 1.01, 1.30, 1.60};
array<float> lootChanceCoef = 		{0.50, 0.60, 1.01, 1.10, 1.20};	
array<float> lootCountCoef = 		{0.50, 0.70, 1.01, 1.51, 2.01};	
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
 11 = Chopper
```
