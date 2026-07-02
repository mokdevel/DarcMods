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
float missionActiveDistanceMul : Multiplier to modify distance on every cycle when in win/lose state. Value should less than 1.0.
float missionActiveTimeToEndMul : Multiplier to modify time on every cycle when in win/lose state. Value should less than 1.0.
int missionHintTime : (seconds) Time to show mission hints to players. 0 disables ALL hints.
SDRC_EHintPosition missionHintPosition : Position of the hint/popup. 
  0 = UP_LEFT : Upper left corner 
  1 = DOWN_LEFT : Lower left corner 
int missionRandomPos : The radius to randomize the mission position. This avoids mission appearing always in same place.
int minDistanceToMission : Distance to another mission. Two missions shall not be too close to each other.
int minDistanceToPlayer : Dynamic missions shall not spawn closer than this distance to any player.
int maxDistanceToPlayer : Dynamic missions shall not spawn further than this distance to any player. -1 disables this check
bool showStaticMissionMarker : Show/hide static mission markers. This will override any mission specific settings.
bool showDynamicMissionMarker : Show/hide dynamic mission markers. This will override any mission specific showMarker settings.
bool showMissionDifficulty : Show/hide mission difficulty on marker
bool showMissionTimeLeft : Show/hide mission time left on marker click
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
## More details
For more details on general topics, see:
* [Mission position](./MISSIONFRAME.md#mission-position)
* [Dynamic vs Static missions](./MISSIONFRAME.md#dynamic-vs-static-mission)
* [Requested missions](./MISSIONFRAME.md#requested-missions)
* [Mission count](./MISSIONFRAME.md#mission-count)
* [Mission end](./MISSIONFRAME.md#mission-end-win-or-lose)

## Factions
The mission faction is defined in ``enemyFactions`` or individually per sub-mission. The supported factions are listed below.
* ``US, USSR, FIA`` : Vanilla factions from Arma Reforger
* ``RHS_USAF, RHS_AFRF, RHS_ION``: RHS - [MOD](https://reforger.armaplatform.com/workshop/595F2BF2F44836FB-RHS-StatusQuo)
* ``MEI`` : Middle East Insurgents - [MOD](https://reforger.armaplatform.com/workshop/64CEC8E005828E5D-MiddleEastInsurgents)
* ``UK`` : British Forces - [MOD](https://reforger.armaplatform.com/workshop/5AE50EC5B8D6F4AE-BritishForces)
* ``BALLIEN_BC_FACTION`` : Ballien Creatures - [MOD](https://reforger.armaplatform.com/workshop/6534B9CEB6D12EE7-BallienCreatures)
* ``BACON_622120A5448725E3_FACTION`` : Bacon Zombies - [MOD](https://reforger.armaplatform.com/workshop/622120A5448725E3-BaconZombies)
* ``PLASTICBANDIT`` : Bandit Faction - [MOD](https://reforger.armaplatform.com/workshop/66C4458756B32594-BanditFaction)
* ``ChinesePLA`` : Chinese People's Liberation Army (PLA) - [MOD](https://reforger.armaplatform.com/workshop/60A6507633AB1954-ChinesePLA)
* ``JSDF_RAS`` : Japan Self-Defense Forces (JSDF) - [MOD](https://reforger.armaplatform.com/workshop/658809A83416F34D-JapanSelf-DefenseForces)
* ``Takistan`` : Takistani Army - [MOD](https://reforger.armaplatform.com/workshop/611A2A77548D2F97-TakistaniArmy)
* ``AFOA, DSF`` : Argentinian Infantry forces - [MOD](https://reforger.armaplatform.com/workshop/685771C973A5516B)
* Unsupported values
  * ``MEC``

You can define multiple factions and when enemies are chosen, the faction is chosen randomly per mission. 

Example: ``"enemyFactions": ["FIA", "USSR", "USSR"]`` - 33% missions are with FIA, 66% with USSR
### Freedom Fighters
If you're playing [Freedom Fighters](https://reforger.armaplatform.com/workshop/CAFEBEEFF0CACC1A-FreedomFighters) and have [DarcMissionsCompatFF](https://reforger.armaplatform.com/workshop/6698F8DD1B444BEB-DarcMissionsCompatFF) in use, it may override the ``enemyFactions`` setting. Please see ``setEnemyFactionAutomatically`` in [DarcMissionsCompatFF](https://github.com/mokdevel/DarcMods/tree/main/DarcMissionsCompatFF#parameters).

### Mission factions
Each sub-mission within the specific mission json, has a parameter ``faction``. This is the override faction for the mission. You can define multiple factions and when enemies are chosen, the faction is chosen randomly per sub-mission. Leaving it empty or ``""``, works as the default to select from the ``enemyFactions``.

Example: ``"enemyFactions": ["FIA", "USSR", "USSR", ""]`` - 25% missions are with FIA, 50% with USSR, 25% are picked from values defined in ``enemyFactions``.

## Difficulty
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

Difficulty is represented on the mission icon. The left icon is ``EASY`` and ``HARD`` is on the right. The information can be shown/hidden with ``showMissionDifficulty`` parameter.

### SDRC_MissionDifficulty
Difficulty affects various aspects of the missions. Each 'coef' has five values that refer to the different difficulty level. 
* ``aiCountCoef`` : The amount of AIs/AI groups for the mission. If mission would spawn 2, on ``EASY``, the amount would drop to 1 (2 * 0.5), but with ``HARD`` the amount would be 5 (2 * 2.3 = 4.6 .. rounded to 5). In most cases missions define the amount of groups to use and a group may include a high amount of AIs. Thus, you may have a lot to fight.
* ``aiSkillCoef`` : The multiplier used to modify the AI skills. This refers to ``EAISkill`` value. If the value reaches max value of 100 (CYLON), the AIs will be very good at hitting you.
* ``aiPerceptionCoef`` : The multiplier used to modify the AI perception. With a high value, AIs are better at spotting you. Unsure what are the actual limits, but from BI code one can see that with 0.4 the AI is threatened and are bad at finding enemies. 1.0 is the normal state and 2.5 is considered vigilant/alerted.
* ``lootChanceCoef`` : The multiplier used to modify the loot chance. For each item on the lootlist, the chance is tested.
* ``lootCountCoef`` : The multiplier used to modify the loot amount. When loot is spawned, the list provided is used. If this value increases the total, amount of items in the lootlist, additional items are added to the list from the original list.
* ``rewardCoef`` : The multiplier used to modify the reward amount. Reward refers to ``xp`` variable in missions. Currently only Freedom Fighters is providing reward
* ``qrfChance`` : Chance to get a QRF (Quick Reaction Force) to your location. This is the multiplier that that modified QRF chance. See: [QRF](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_COMMON.md#sdrc_missionconfigqrf).
```
array<float> aiCountCoef =      {0.50, 0.60, 1.0, 1.40, 1.80};
array<float> aiSkillCoef =      {0.20, 0.60, 1.0, 1.30, 1.60};
array<float> aiPerceptionCoef = {0.20, 0.60, 1.0, 1.30, 1.60};
array<float> lootChanceCoef =   {0.50, 0.60, 1.0, 1.10, 1.20};	
array<float> lootCountCoef =    {0.70, 0.90, 1.0, 1.50, 2.00}	
array<float> rewardCoef =       {0.50, 0.70, 1.0, 2.00, 3.00};	
array<float> qrfChance =        {0.80, 0.90, 1.0, 1.20, 1.30};
```

## Mission type
The below is valid for both static and dynamic missions

### SDRC_MissionTypeConfig
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
``missionTypeArray`` can be used to define the probability of a mission type. The more you have the same ``missionType`` in the array, the more likely it is to be picked for spawning.

For example: ``"missionTypeArray": [5,5,1,7,7]`` gives:
- a 40% chance to get a PATROL mission
- a 20% chance to get a HUNTER mission
- a 40% chance to get a ROADBLOCK mission
