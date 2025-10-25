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
  Tested values: US, USSR, FIA, UK, RHS_USAF, RHS_AFRF, MEI, BALLIEN_BC_FACTION, BACON_622120A5448725E3_FACTION
  Unsupported values: RHS_ION, MEC
  You can define multiple factions and when enemies are chosen, the faction is chosen randomly per mission.
  Example: "enemyFactions": ["FIA", "USSR"]
SDRC_MissionDifficulty missionDifficulty : Difficulty settings.
SDRC_MissionTypeConfig missionDynamic : Dynamic missions configurations.
SDRC_MissionTypeConfig missionStatic : Static missions configurations.
```

### SDRC_MissionDifficulty
Difficulty setting that affects mission AI behaviour and loot reward. Coef values in the array refer to SDRC_EMissionDifficulty values: {``EASY``, ``MODERATE``, ``NORMAL``, ``TOUGH``, ``HARD``}
```
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
