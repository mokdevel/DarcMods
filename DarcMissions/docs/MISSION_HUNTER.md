## Mission : Hunter
Example: [dc_missionConfig_Hunter.json](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/ExampleConfigs/dc_missionConfig_Hunter.json)

AIs are hunting you and following you. They will receive regularly information about where the closest player. The location is known to them with an error of ```rndDistanceToPlayer```. You can outrun them by going further than ```maxDistanceToPlayer```.

Note on GM requested mission: The location is where the mission was dropped.

See [Common and General parameters](https://github.com/mokdevel/DarcMods/blob/main/DarcMissions/docs/P_COMMON.md)
```
int version : See Common parameters
string author : See Common parameters
int missionCycleTime : See Common parameters
bool showMarker : See Common parameters
bool showHint : See Common parameters
bool showMessage : See Common parameters
int minDistanceToPlayer : Hunter group minimum distance to player for spawn
int maxDistanceToPlayer : ..max distance
int rndDistanceToPlayer : The error on the location where AI thinks you are. (0..rndDistanceToPlayer).
array<int> hunterList : The indexes of hunters.
array<SDRC_Hunter> hunters : List of hunters
```

## SDRC_Hunter
```
SDRC_MissionConfigGeneral general
array<int> groupCount : See General parameters
array<string> groupTypes : See General parameters
int aiSkill : See General parameters
float aiPerception : See General parameters
```
